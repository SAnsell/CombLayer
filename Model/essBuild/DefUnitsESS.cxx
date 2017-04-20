/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DefUnitsESS.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "inputParam.h"
#include "support.h"
#include "stringCombine.h"
#include "defaultConfig.h"
#include "DefUnitsESS.h"

namespace mainSystem
{

void 
setDefUnits(FuncDataBase& Control,
	    inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
    \param IParam :: input system
  */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      
      std::vector<std::string> LItems=
	IParam.getObjectItems("defaultConfig",0);
      const std::string sndItem=(LItems.size()>1) ? LItems[1] : "";
      const std::string extraItem=(LItems.size()>2) ? LItems[2] : "";


      if (Key=="Main")
	setESS(A);
      else if (Key=="Full")
	setESSFull(A);
      else if (Key=="PortsOnly")
	setESSPortsOnly(A,sndItem,extraItem);
      else if (Key=="Single")
	setESSSingle(A,LItems);
      else if (Key=="neutronics")
	setESSNeutronics(A,sndItem,extraItem);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Main : Everything that works"<<ELog::endDiag;
	  ELog::EM<<"  Full : Beamline on every port"<<ELog::endDiag;
	  ELog::EM<<"  PortsOnly [lower/upper] : Nothing beyond beamport "
		  <<ELog::endDiag;
	  ELog::EM<<"  Single  beamLine : Single beamline [for BL devel] "
		  <<ELog::endDiag;
	  ELog::EM<<"  neutronics {BF1,BF2} [single]: configuration for neutronics calculations " << ELog::endDiag;
	  throw ColErr::ExitAbort("Iparam.defaultConfig");	  
	}
      else 
	{
	  ELog::EM<<"Unknown Default Key ::"<<Key<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");
	}
      A.process(Control,IParam);
    }
  return;
}

void
setESSFull(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      {"MAGIC","G1BLineTop6"},      // W6
      {"BIFROST","G1BLineTop4"},    // W4
      {"NMX","G1BLineTop1"},        // W1
      {"VOR","G2BLineTop10"},   // also 17
      {"LOKI","G1BLineTop17"},
      {"DREAM","G1BLineTop19"},
      // {"CSPEC","G4BLine3"},
       {"VESPA","G2BLineTop7"},
      {"FREIA","G1BLineTop15"},     // N5
      {"ODIN","G1BLineLow2"}
    };
  
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","FREIA","SHORTDREAM","SHORTDREAM2",
     "SHORTNMX","LOKI",
     "MAGIC","VESPA","VOR"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
	A.setVar(mc->second+"Filled",1);
      index++;
    }
  return;
}

void
setESSPortsOnly(defaultConfig& A,const std::string& lvl,
                const std::string& lvlExtra)
  /*!
    Default configuration for ESS for beamports only
    \param A :: Paramter for default config
    \param lvl :: level value (lower/uppper)
    \param lvlExtra :: extra level value (lower/uppper)
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  static const std::map<std::string,std::string>
    nameBay({
        {"lower","G1BLineLow"},
        {"upper","G1BLineTop"},
        {"farUpper","G2BLineTop"},
        {"farLower","G2BLineLow"}
      });
  
  A.setOption("lowMod","Butterfly");

  int defaultFlag(0);
  std::string GNum;
  std::map<std::string,std::string>::const_iterator mc=
    nameBay.find(lvlExtra);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum=mc->second+" ";
    }
  // and take default:
  mc=nameBay.find(lvl);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum+=mc->second;
    }
  if (!defaultFlag)
    {
      for(size_t i=0;i<21;i++)
        A.setVar("G1BLineTop"+StrFunc::makeString(i+1)+"Active",1);
      GNum="G1BLineTop";
    }
      
  ELog::EM<<"Port Only on sectors:"<<GNum<<ELog::endDiag;
  return;
}

void
setESSSingle(defaultConfig& A,
	     std::vector<std::string>& LItems)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param LItems :: single selection
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSSingle");

  A.setOption("lowMod","Butterfly");
  const std::map<std::string,std::string> beamDefNotSet=
    { 
     {"HEIMDAL","G1BLineLow8"},     // W8
     {"SLEIPNIR","G1BLineLow13"},   // N9
     {"ANNI","G2BLineTop3"},        // E3
     {"SURFSCATTER","G2BLineTop8"},        // E8
     {"SKADI","G2BLineTop5"}         // E5
    };     
  const std::map<std::string,std::string> beamDef=
    {{"NMX","G1BLineTop1"},        // W1
     {"BEER","G1BLineTop2"},       // W2
     {"CSPEC","G1BLineTop3"},      // W3
     {"BIFROST","G1BLineTop4"},    // W4
     {"MIRACLES","G1BLineTop5"},   // W5
     {"MAGIC","G1BLineTop6"},      // W6     
     {"TREX","G1BLineTop7"},       // W7

     {"LOKI","G1BLineTop15"},      // N7
     {"FREIA","G1BLineTop17"},     // N5


     {"ODIN","G2BLineLow20"},      // Lower S2
     {"DREAM","G2BLineTop19"},     // S3

     {"VOR","G2BLineTop11"},       // S10/S11  [CHANGED TO FIT]
     {"VESPA","G2BLineTop7"},     // E7

     {"ESTIA","G2BLineTop2"},     // E2
     
     {"SIMPLE","G1BLineTop17"},

     {"SHORTDREAM","G1BLineTop17"},
     {"SHORTNMX","G1BLineTop13"},
     {"SHORTODIN","G1BLineLow4"}
     

    };     
  const std::set<std::string> beamFilled=
    {"BEER","BIFROST","CSPEC","DREAM","FREIA","LOKI",
     "MAGIC","MIRACLES","NMX","TREX","VESPA",
     "VOR","SHORTNMX","SHORTDREAM"};

  size_t beamLineIndex(0);
  while(!LItems.empty())
    {
      bool portFlag=0;
      if (LItems.front()!="Single")
        {
          const std::string beamItem=LItems.front();
          const std::string portItem=(LItems.size()>1) ? LItems[1] : "";
      
          std::map<std::string,std::string>::const_iterator mc=
            beamDef.find(beamItem);	  
          portFlag=
	    (portItem.find("BLine")!=std::string::npos);
          const int filled =
            (beamFilled.find(beamItem)==beamFilled.end()) ? 0 : 1;
          
          if (mc!=beamDef.end())
            {
              if (!portFlag)
                {
                  A.setMultiOption("beamlines",beamLineIndex,
				   mc->second+" "+beamItem);
                  A.setVar(mc->second+"Active",1);
                  if (filled)
                    A.setVar(mc->second+"Filled",1);
                }
              else
                {
                  A.setMultiOption("beamlines",beamLineIndex,
				   portItem+" "+beamItem);
                  A.setVar(portItem+"Active",1);
                  if (filled)
                    A.setVar(portItem+"Filled",1);
                }
	      beamLineIndex++;
            }
          else
            throw ColErr::InContainerError<std::string>(beamItem,"BeamItem");
        }
      if (portFlag)
        LItems.erase(LItems.begin());
      
      LItems.erase(LItems.begin());
    }
  return;
}

void
setESSNeutronics(defaultConfig& A, const std::string& modtype, const std::string& single)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param modtype :: Moderator type: either BF1 or BF2
    \param single :: if "single" then only top moderator is built;
                     if not specified then both moderators are built.
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSNeutronics");

  A.setOption("mcnp", "10");

  size_t bfType=0;

  if (modtype=="BF1")
    {
      bfType = 1;
      A.setOption("lowMod", "Butterfly");
      A.setOption("topMod", "Butterfly");
      A.setVar("TopFlyLeftLobeXStep", -2.0);
      A.setVar("TopFlyRightLobeXStep", 2.0);
      A.setVar("TopFlyMidWaterMidYStep", 7.0);
      A.setOption("topPipe", "supply");

      A.setVar("LowFlyLeftLobeXStep", -2.0);
      A.setVar("LowFlyRightLobeXStep", 2.0);
      A.setVar("LowFlyMidWaterMidYStep", 7.0);
      A.setOption("lowPipe", "supply");

      // straighten the pipes
      A.setVar("TSupplyRightAlNSegIn", 2);
      A.setVar("TSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyRightAlPPt1", Geometry::Vec3D(0,26,0));
      A.setVar("TSupplyRightAlPPt2", Geometry::Vec3D(0,26,40));
      A.setVar("TSupplyLeftAlNSegIn", 2);
      A.setVar("TSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyLeftAlPPt1", Geometry::Vec3D(0,26,0));
      A.setVar("TSupplyLeftAlPPt2", Geometry::Vec3D(0,26,40));

      A.setVar("LSupplyRightAlNSegIn", 2);
      A.setVar("LSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyRightAlPPt1", Geometry::Vec3D(0,26,0));
      A.setVar("LSupplyRightAlPPt2", Geometry::Vec3D(0,26,40));
      A.setVar("LSupplyLeftAlNSegIn", 2);
      A.setVar("LSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyLeftAlPPt1", Geometry::Vec3D(0,26,0));
      A.setVar("LSupplyLeftAlPPt2", Geometry::Vec3D(0,26,40));
    } else if (modtype=="BF2")
    {
      bfType = 2;
      A.setOption("lowMod", "Butterfly");
      A.setOption("topMod", "Butterfly");
      // variables are set in moderatorVariables
      // build pipes
      A.setOption("topPipe", "supply,return");
      A.setOption("lowPipe", "supply,return");
    } else if (modtype=="Pancake")
    {
      A.setOption("lowMod", "Pancake");
      A.setOption("topMod", "Pancake");
      
      A.setOption("topPipe", "PancakeSupply");
      // straighten the pipes
      A.setVar("TSupplyRightAlNSegIn", 2);
      A.setVar("TSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyRightAlPPt1", Geometry::Vec3D(0,30,0));
      A.setVar("TSupplyRightAlPPt2", Geometry::Vec3D(0,30,40));
      A.setVar("TSupplyLeftAlNSegIn", 2);
      A.setVar("TSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyLeftAlPPt1", Geometry::Vec3D(0,30,0));
      A.setVar("TSupplyLeftAlPPt2", Geometry::Vec3D(0,30,40));

      A.setVar("LSupplyRightAlNSegIn", 2);
      A.setVar("LSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyRightAlPPt1", Geometry::Vec3D(0,30,0));
      A.setVar("LSupplyRightAlPPt2", Geometry::Vec3D(0,30,40));
      A.setVar("LSupplyLeftAlNSegIn", 2);
      A.setVar("LSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyLeftAlPPt1", Geometry::Vec3D(0,30,0));
      A.setVar("LSupplyLeftAlPPt2", Geometry::Vec3D(0,30,40));
      
    } else throw ColErr::InvalidLine(modtype,
				     "Either BF1, BF2 or Pancake are supported in defaultConfig");

  if (single=="")
    {
      // pass
    } else if (single=="single")
    {
      A.setOption("lowMod", "None");
      A.setVar("BeRefLowVoidThick", 0);
      A.setVar("BeRefLowRefMat", "Iron_10H2O");
      A.setVar("BeRefLowWallMat", "Iron_10H2O");
      A.setVar("BeRefLowInnerStructureActive", 0);
      // Since there is no bunkers, we have to 
      // prolong collimators until they are emerged into ShutterBay, otherwise a neutron crosses imp=0 cell
      A.setVar("F5DefaultLength", 500.0);
    } else
    throw ColErr::InvalidLine(single,"Either 'single' or nothing are supported in defaultConfig");

  A.setVar("LowFlyType", bfType);
  A.setVar("TopFlyType", bfType);

  A.setOption("matDB", "neutronics");
  A.setOption("physModel", "BD");
  
  // simplify the bunkers
  A.setOption("bunker", "noPillar");
  A.setVar("ABunkerWallNBasic", 1);
  A.setVar("ABunkerRoofNBasicVert", 1);
  A.setVar("ABunkerNSectors", 1);
  A.setVar("ABunkerRoofMat0", "Void");
  A.setVar("ABunkerWallMat0", "Void");
  A.setVar("ABunkerWallMat", "Void");

  A.setVar("BBunkerWallNBasic", 1);
  A.setVar("BBunkerRoofNBasicVert", 1);
  A.setVar("BBunkerNSectors", 1);
  A.setVar("BBunkerRoofMat0", "Void");
  A.setVar("BBunkerWallMat0", "Void");
  A.setVar("BBunkerWallMat", "Void");

  A.setVar("CBunkerWallNBasic", 1);
  A.setVar("CBunkerRoofNBasicVert", 1);
  A.setVar("CBunkerNSectors", 1);
  A.setVar("CBunkerRoofMat0", "Void");
  A.setVar("CBunkerWallMat0", "Void");
  A.setVar("CBunkerWallMat", "Void");

  A.setVar("DBunkerWallNBasic", 1);
  A.setVar("DBunkerRoofNBasicVert", 1);
  A.setVar("DBunkerNSectors", 1);
  A.setVar("DBunkerRoofMat0", "Void");
  A.setVar("DBunkerWallMat0", "Void");
  A.setVar("DBunkerWallMat", "Void");


  // simplify the curtain
  A.setVar("CurtainNBaseLayers", 1);
  A.setVar("CurtainNMidLayers", 1);
  A.setVar("CurtainNTopLayers", 1);
  A.setVar("CurtainWallMat", "Void");
  
  // sdef
  A.setVar("sdefEnergy", 2000.0);
  A.setVar("sdefWidth", 7.0);
  A.setVar("sdefHeight", 1.6);
  A.setVar("sdefYPos", -500);
  A.setVar("sdefPDF", "uniform");
  
  return;
}

void
setESS(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");


  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      {"NMX","G1BLineTop1"},
      {"DREAM","G1BLineTop19"},
      {"CSPEC","G1BLineTop3"},
      {"VOR","G1BLinetop7"},   // also 17  
      {"LOKI","G1BLineTop17"},
      {"ODIN","G2BLineLow2"}
    };     
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","VOR","LOKI"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
	A.setVar(mc->second+"Filled",1);
      index++;
    }
   
  return;
}

  
} // NAMESPACE mainSystem
