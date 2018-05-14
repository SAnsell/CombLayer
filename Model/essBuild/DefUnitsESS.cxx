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
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Main : Everything that works"<<ELog::endDiag;
	  ELog::EM<<"  Full : Beamline on every port"<<ELog::endDiag;
	  ELog::EM<<"  PortsOnly [lower/upper] : Nothing beyond beamport "
		  <<ELog::endDiag;
	  ELog::EM<<"  Single  beamLine : Single beamline [for BL devel] "
		  <<ELog::endDiag;
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
setESSNeutronics(defaultConfig& A, const std::string& modType,
		 const std::string& single)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param modType :: Moderator type: either BF1 or BF2
    \param single :: if "single" then only top moderator is built;
                     if not specified then both moderators are built.
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSNeutronics");


  if (modType=="BF1")
    {
      A.setVar("LowFlyType", modType);
      A.setVar("TopFlyType", modType);

      A.setOption("lowMod", "Butterfly");
      A.setOption("topMod", "Butterfly");
      A.setOption("topPipe", "supply");

      // These variables are as in the model received from Luca 22 Feb 2017
      // there is no engieering drawings for BF1 yet, so this email is
      // the only reference
      const std::vector<std::string> TLfly = {"TopFly", "LowFly"};
      for (const std::string& modName : TLfly)
	{
	  A.setVar(modName+"LeftLobeCorner1",Geometry::Vec3D(0,0.6,0));
	  A.setVar(modName+"LeftLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	  A.setVar(modName+"LeftLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));
	  
	  A.setVar(modName+"RightLobeCorner1",Geometry::Vec3D(0,0.6,0));
	  A.setVar(modName+"RightLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	  A.setVar(modName+"RightLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));
	  
	  A.setVar(modName+"LeftLobeRadius1",1.0);
	  A.setVar(modName+"RightLobeRadius1",1.0);
	  
	  A.setVar(modName+"LeftLobeXStep",-0.85);
	  A.setVar(modName+"RightLobeXStep",0.85);
	  
	  A.setVar(modName+"LeftWaterCutWidth",9.6);
	  A.setVar(modName+"RightWaterCutWidth",9.6);
	  
	  A.setVar(modName+"MidWaterLength",9.9);
	  A.setVar(modName+"MidWaterMidYStep",6.3);
	}

      // flow guides
      for (const std::string& modName : TLfly)
	{
	  A.setVar(modName+"FlowGuideWallMat","Aluminium20K");
	  A.setVar(modName+"FlowGuideWallTemp",20.0);
	  A.setVar(modName+"FlowGuideBaseThick",0.3);
	  A.setVar(modName+"FlowGuideBaseLen",6.5);
	  A.setVar(modName+"FlowGuideBaseOffset",-11);
	  A.setVar(modName+"FlowGuideAngle",23);
	  A.setVar(modName+"FlowGuideSQOffsetY",3.0);
	  A.setVar(modName+"FlowGuideSQSideA",1.0);
	  A.setVar(modName+"FlowGuideSQSideE",-1.0);
	  A.setVar(modName+"FlowGuideSQSideF",-0.005);
	  A.setVar(modName+"FlowGuideSQCenterA",1.0);
	  A.setVar(modName+"FlowGuideSQCenterE",-0.6);
	  A.setVar(modName+"FlowGuideSQCenterF",-0.005);
	}
      
      // pipes
      A.setOption("topPipe", "supply");
      A.setOption("lowPipe", "supply");

      const std::vector<std::string> TLpipe = {"TSupply", "LSupply"};
      for (const std::string& pipeName : TLpipe)
	{
	  A.setVar(pipeName+"RightAlNSegIn", 1);
	  A.setVar(pipeName+"RightAlPPt0", Geometry::Vec3D(0,0,0));
	  A.setVar(pipeName+"RightAlPPt1", Geometry::Vec3D(0,15,0));
	  // should be inside twister
	  A.setVar(pipeName+"RightInvarPPt1", Geometry::Vec3D(0,8.3,0));
	  A.setVar(pipeName+"RightInvarPPt2", Geometry::Vec3D(0,8.3,30));
	  A.setVar(pipeName+"LeftAlNSegIn", 1);
	  A.setVar(pipeName+"LeftAlPPt0", Geometry::Vec3D(0,0,0));
	  A.setVar(pipeName+"LeftAlPPt1", Geometry::Vec3D(0,15,0));
	  // should be inside twister
	  A.setVar(pipeName+"LeftInvarPPt1", Geometry::Vec3D(0,8.3,0));
	  A.setVar(pipeName+"LeftInvarPPt2", Geometry::Vec3D(0,8.3,30));
	}
    }
  else if (modType=="BF2")
    {
      A.setOption("lowMod", "Butterfly");
      A.setOption("topMod", "Butterfly");

      A.setVar("LowFlyType", modType);
      A.setVar("TopFlyType", modType);

      // variables are set in moderatorVariables
      // build pipes
      A.setOption("topPipe", "supply,return");
      A.setOption("lowPipe", "supply,return");
    }
  else if (modType=="Pancake")
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
      
      A.setVar("TopCapWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopPreWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopCapWingThick", 1.1);
      A.setVar("TopCapWingTiltAngle", 1.7);
      A.setVar("TopPreWingThick", 0.8);
      A.setVar("TopPreWingTiltAngle", 1.5);
    }
  else if (modType=="Box")
    {
      A.setOption("lowMod", "Box");
      A.setOption("topMod", "Box");

      A.setOption("topPipe", "BoxSupply");
      // straighten the pipes
      A.setVar("TSupplyRightAlNSegIn", 2);
      A.setVar("TSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyRightAlPPt1", Geometry::Vec3D(0,40,0));
      A.setVar("TSupplyRightAlPPt2", Geometry::Vec3D(0,40,40));
      A.setVar("TSupplyLeftAlNSegIn", 2);
      A.setVar("TSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyLeftAlPPt1", Geometry::Vec3D(0,40,0));
      A.setVar("TSupplyLeftAlPPt2", Geometry::Vec3D(0,40,40));

      A.setVar("LSupplyRightAlNSegIn", 2);
      A.setVar("LSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyRightAlPPt1", Geometry::Vec3D(0,40,0));
      A.setVar("LSupplyRightAlPPt2", Geometry::Vec3D(0,40,40));
      A.setVar("LSupplyLeftAlNSegIn", 2);
      A.setVar("LSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyLeftAlPPt1", Geometry::Vec3D(0,40,0));
      A.setVar("LSupplyLeftAlPPt2", Geometry::Vec3D(0,40,40));
      
      A.setVar("TopCapWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopPreWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopCapWingThick", 1.1);
      A.setVar("TopCapWingTiltAngle", 1.7);
      A.setVar("TopPreWingThick", 0.8);
      A.setVar("TopPreWingTiltAngle", 1.5);
    }
  else throw ColErr::InContainerError<std::string>
	 (modType,"modType :: BF1, BF2, Pancake or Box in : defaultConfig");
  
  if (single=="single")
    {
      A.setOption("lowMod", "None");

      A.setVar("BeRefLowVoidThick", 0);
      A.setVar("BeRefLowRefMat", "SS316L");
      A.setVar("BeRefLowWallMat", "AluminiumBe");
      A.setVar("BeRefLowInnerStructureActive", 1);
      // The following numbers are from LZ drawing 2017-10-10
      // and discussions with Marc 2017-10-11
      const double BeRefDepth(57.7); // 30+51+30+9+30+30+30+90+30+90+30+127
      const double Ztop(7.8);// z-coordinate of LowBeRef upper plane
      A.setVar("BeRefDepth", BeRefDepth+Ztop);
      A.setVar("BulkDepth1", BeRefDepth+Ztop+1.0); // a bit bigger to make some clearance

      A.setVar("BeRefVoidCylRadius", 15.0);
      A.setVar("BeRefVoidCylDepth", 3.0+5.1+3.0); // first 3 InnerStructure layers

      A.setVar("BeRefLowInnerStructureNLayers", 12);
      A.setVar("BeRefLowInnerStructureBaseLen1", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen2", 5.1/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen3", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen4", 0.9/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen5", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen6", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen7", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen8", 9.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen9", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen10", 9.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen11", 3.0/BeRefDepth);
      A.setVar("BeRefLowInnerStructureBaseLen12", 12.7/BeRefDepth);
      A.setVar("BeRefLowInnerStructureMat0", "H2O");
      A.setVar("BeRefLowInnerStructureMat1", "SS316L_40H2O"); // email from Marc 13 Oct 2017
      A.setVar("BeRefLowInnerStructureMat2", "H2O");
      A.setVar("BeRefLowInnerStructureMat3", "SS316L_40H2O");
      A.setVar("BeRefLowInnerStructureMat4", "H2O");
      A.setVar("BeRefLowInnerStructureMat5", "SS316L_40H2O");
      A.setVar("BeRefLowInnerStructureMat6", "H2O");
      A.setVar("BeRefLowInnerStructureMat7", "SS316L_40H2O");
      A.setVar("BeRefLowInnerStructureMat8", "H2O");
      A.setVar("BeRefLowInnerStructureMat9", "SS316L_40H2O");
      A.setVar("BeRefLowInnerStructureMat10", "H2O");
      A.setVar("BeRefLowInnerStructureMat11", "SS316L_40H2O");
      A.setVar("BeRefLowInnerStructureMat12", "H2O");
      // Since there is no bunkers, we have to 
      // prolong collimators until they are emerged into ShutterBay,
      // otherwise a neutron crosses imp=0 cell
      A.setVar("F5DefaultLength", 440.0);
    }
  else if (!single.empty())
    throw ColErr::InContainerError<std::string>
      (single,"defaultConfig neutronic single: single type error");


  A.setOption("matDB", "neutronics");
  A.setOption("physModel", "BD");
    
  // sdef
  A.setVar("sdefEnergy", 2000.0);
  A.setVar("sdefWidth", 7.0);
  A.setVar("sdefHeight", 1.6);
  A.setVar("sdefYPos", -500);   
  A.setVar("sdefPDF", "uniform");
  
  return;
}

  
void
setESSFull(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSFull");

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
      {"ODIN","G1BLineTop20"}        // was low / now high
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
     {"HEIMDAL","G1BLineTop18"},       // S2
     {"SLEIPNIR","G1BLineLow13"},      // N9
     {"ANNI","G2BLineTop4"},           // E4
     {"SURFSCATTER","G2BLineTop8"},    // E8
     {"SKADI","G2BLineTop5"}           // E5
    };     
  const std::map<std::string,std::string> beamDef=
    {{"NMX","G1BLineTop1"},               // W1
     {"BEER","G1BLineTop2"},              // W2
     {"CSPEC","G1BLineTop3"},             // W3
     {"BIFROST","G1BLineTop4"},           // W4
     

     {"MIRACLES","G1BLineTop5"},   // W5
     {"MAGIC","G1BLineTop6"},      // W6     
     {"TREX","G1BLineTop7"},       // W7
     {"HEIMDAL","G1BLineTop8"},    // W8 
     
     {"LOKI","G1BLineTop15"},      // N7
     {"FREIA","G1BLineTop17"},     // N5


     {"NNBAR","G1BLineTop11"},      // TEST PORT
     {"TESTBEAM","G1BLineTop11"},   // TEST PORT
     
     {"ODIN","G2BLineTop20"},      // MOVED from Lower to top: S2
     {"DREAM","G2BLineTop19"},     // S3

     {"SKADI","G2BLineTop3"},      // E3
     
     
     {"VOR","G2BLineTop11"},       // S10/S11  [CHANGED TO FIT]
     {"VESPA","G2BLineTop7"},      // E7


     {"ESTIA","G2BLineTop2"},     // E2
     
     {"SIMPLE","G1BLineTop17"},

     {"SHORTDREAM","G1BLineTop17"},
     {"SHORTNMX","G1BLineTop13"},
     {"SHORTODIN","G1BLineLow4"}
     

    };     
  const std::set<std::string> beamFilled=
    {"BEER","BIFROST","CSPEC","DREAM","FREIA","HEIMDAL","LOKI",
     "MAGIC","MIRACLES","NMX","NNBAR","ODIN","TESTBEAM",
     "TREX","VESPA","VOR","SHORTNMX","SHORTDREAM","SKADI","ESTIA"};

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
      {"ODIN","G2BLineTop20"}
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
