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

      // These variables are as in the model received from Luca 22 Feb 2017
      // there is no engieering drawings for BF1 yet, so this email is
      // the only reference
      const std::vector<std::string> TLfly = {"TopFly", "LowFly"};
      std::string s;
      for (std::string s : TLfly) {
	A.setVar(s+"LeftLobeCorner1",Geometry::Vec3D(0,0.6,0));
	A.setVar(s+"LeftLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	A.setVar(s+"LeftLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));
	
	A.setVar(s+"RightLobeCorner1",Geometry::Vec3D(0,0.6,0));
	A.setVar(s+"RightLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	A.setVar(s+"RightLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));
	
	A.setVar(s+"LeftLobeRadius1",1.0);
	A.setVar(s+"RightLobeRadius1",1.0);
	
	A.setVar(s+"LeftLobeXStep",-0.85);
	A.setVar(s+"RightLobeXStep",0.85);
	
	A.setVar(s+"LeftWaterCutWidth",9.6);
	A.setVar(s+"RightWaterCutWidth",9.6);
	
	A.setVar(s+"MidWaterLength",9.9);
	A.setVar(s+"MidWaterMidYStep",6.3);
      }

      // flow guides
      for (std::string strmod : TLfly) {
	s = strmod + "FlowGuide";
	A.setVar(s+"WallMat","Aluminium20K");
	A.setVar(s+"WallTemp",20.0);
	A.setVar(s+"BaseThick",0.3);
	A.setVar(s+"BaseLen",6.5);
	A.setVar(s+"BaseOffset",-11);
	A.setVar(s+"Angle",23);
	A.setVar(s+"SQOffsetY",3.0);
	A.setVar(s+"SQSideA",1.0);
	A.setVar(s+"SQSideE",-1.0);
	A.setVar(s+"SQSideF",-0.005);
	A.setVar(s+"SQCenterA",1.0);
	A.setVar(s+"SQCenterE",-0.6);
	A.setVar(s+"SQCenterF",-0.005);
      }

      // These variables (until "moderator end") are copied from the BF2
      // section of moderatorVarialbes to be sure that the BF1 geometry
      // does not change if we change the BF2 variables:

      A.setVar("TopFlyXStep",0.0);
      A.setVar("TopFlyYStep",0.0);
      A.setVar("TopFlyZStep",0.0);
      A.setVar("TopFlyXYangle",90.0);
      A.setVar("TopFlyZangle",0.0);
      A.setVar("TopFlyTotalHeight",4.9);
      A.setVar("TopFlyWallMat","Aluminium");
      A.setVar("TopFlyWallDepth",0.0);
      A.setVar("TopFlyWallHeight",0.3);
  
      A.setVar("TopFlyLeftLobeYStep",0.0);
  
      A.setVar("TopFlyLeftLobeRadius2",2.506);
      A.setVar("TopFlyLeftLobeRadius3",2.506);

      A.setVar("TopFlyLeftLobeModMat","HPARA");
      A.setVar("TopFlyLeftLobeModTemp",20.0);

      A.setVar("TopFlyLeftLobeNLayers",4);
      A.setVar("TopFlyLeftLobeThick1",0.3);
      A.setVar("TopFlyLeftLobeMat1","Aluminium20K");

      A.setVar("TopFlyLeftLobeHeight1",0.3);
      A.setVar("TopFlyLeftLobeDepth1",0.3);
      A.setVar("TopFlyLeftLobeTemp1",20.0);
  
      A.setVar("TopFlyLeftLobeThick2",0.5);
      A.setVar("TopFlyLeftLobeMat2","Void");

      A.setVar("TopFlyLeftLobeHeight2",0.5);
      A.setVar("TopFlyLeftLobeDepth2",0.5);

      A.setVar("TopFlyLeftLobeThick3",0.3);
      A.setVar("TopFlyLeftLobeMat3","Aluminium");

      A.setVar("TopFlyLeftLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be
      A.setVar("TopFlyLeftLobeDepth3",0.0);
  
      A.setVar("TopFlyRightLobeYStep",0.0);

      A.setVar("TopFlyRightLobeRadius2",2.506);
      A.setVar("TopFlyRightLobeRadius3",2.506);

      A.setVar("TopFlyRightLobeModMat","HPARA");
      A.setVar("TopFlyRightLobeModTemp",20.0);

      A.setVar("TopFlyRightLobeNLayers",4);
      A.setVar("TopFlyRightLobeThick1",0.3);
      A.setVar("TopFlyRightLobeMat1","Aluminium20K");

      A.setVar("TopFlyRightLobeHeight1",0.3);
      A.setVar("TopFlyRightLobeDepth1",0.3);
      A.setVar("TopFlyRightLobeTemp1",20.0);
  
      A.setVar("TopFlyRightLobeThick2",0.5);
      A.setVar("TopFlyRightLobeMat2","Void");

      A.setVar("TopFlyRightLobeHeight2",0.5);
      A.setVar("TopFlyRightLobeDepth2",0.5);

      A.setVar("TopFlyRightLobeThick3",0.3);
      A.setVar("TopFlyRightLobeMat3","Aluminium");

      A.setVar("TopFlyRightLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be
      A.setVar("TopFlyRightLobeDepth3",0.0);

      A.setVar("TopFlyMidWaterCutLayer",3);
      A.setVar("TopFlyMidWaterMidAngle",90);
      A.setVar("TopFlyMidWaterEdgeRadius",0.5);

      A.setVar("TopFlyMidWaterWallThick",0.2);
      A.setVar("TopFlyMidWaterModMat","H2O");
      A.setVar("TopFlyMidWaterWallMat","Aluminium");
      A.setVar("TopFlyMidWaterModTemp",300.0);

      A.setVar("TopFlyLeftWaterWidth",15.76);  
      A.setVar("TopFlyLeftWaterWallThick",0.347);
      A.setVar("TopFlyLeftWaterCutAngle",30.0);
      A.setVar("TopFlyLeftWaterModMat","H2O");
      A.setVar("TopFlyLeftWaterWallMat","Aluminium");
      A.setVar("TopFlyLeftWaterModTemp",300.0);

      A.setVar("TopFlyRightWaterWidth",15.76);
      A.setVar("TopFlyRightWaterWallThick",0.347);
      A.setVar("TopFlyRightWaterCutAngle",30.0);
      A.setVar("TopFlyRightWaterModMat","H2O");
      A.setVar("TopFlyRightWaterWallMat","Aluminium");
      A.setVar("TopFlyRightWaterModTemp",300.0);
      // moderator end

      // pipes
      A.setOption("topPipe", "supply");
      A.setOption("lowPipe", "supply");

      const std::vector<std::string> TLpipe = {"T", "L"};
      for (std::string strpipe : TLpipe) {
	s = strpipe + "Supply";
	A.setVar(s+"RightAlNSegIn", 1);
	A.setVar(s+"RightAlPPt0", Geometry::Vec3D(0,0,0));
	A.setVar(s+"RightAlPPt1", Geometry::Vec3D(0,15,0));
	A.setVar(s+"RightInvarPPt1", Geometry::Vec3D(0,8.3,0)); // should be inside twister
	A.setVar(s+"RightInvarPPt2", Geometry::Vec3D(0,8.3,30));
	A.setVar(s+"LeftAlNSegIn", 1);
	A.setVar(s+"LeftAlPPt0", Geometry::Vec3D(0,0,0));
	A.setVar(s+"LeftAlPPt1", Geometry::Vec3D(0,15,0));
	A.setVar(s+"LeftInvarPPt1", Geometry::Vec3D(0,8.3,0)); // should be inside twister
	A.setVar(s+"LeftInvarPPt2", Geometry::Vec3D(0,8.3,30));
      }
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
      
      A.setVar("TopCapWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopPreWingTiltRadius", 10+0.3+0.5+0.3);
      A.setVar("TopCapWingThick", 1.1);
      A.setVar("TopCapWingTiltAngle", 1.7);
      A.setVar("TopPreWingThick", 0.8);
      A.setVar("TopPreWingTiltAngle", 1.5);
    } else if (modtype=="Box")
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
    } else throw ColErr::InvalidLine(modtype,
				     "Either BF1, BF2, Pancake or Box are supported in defaultConfig");

  if (single=="")
    {
      // pass
    } else if (single=="single")
    {
      A.setOption("lowMod", "None");

      A.setVar("BeRefLowVoidThick", 0);
      A.setVar("BeRefLowRefMat", "SS316L");
      A.setVar("BeRefLowWallMat", "AluminiumBe");
      A.setVar("BeRefLowInnerStructureActive", 1);
      // The following numbers are from Yannick's email 31 Jan 2017:
      A.setVar("BeRefDepth", 50.3); // (157.5+17.5 + 120 + 30 + 50 +30 +20)/10.0+7.8
      A.setVar("BulkDepth1", 51); // a bit bigger to make some clearance
      A.setVar("BeRefLowInnerStructureNLayers", 7);
      A.setVar("BeRefLowInnerStructureBaseLen1", 0.0470589); // 2 cm
      A.setVar("BeRefLowInnerStructureBaseLen2", 0.0705882); // 3
      A.setVar("BeRefLowInnerStructureBaseLen3", 0.117647); // 5
      A.setVar("BeRefLowInnerStructureBaseLen4", 0.0705882); // 3
      A.setVar("BeRefLowInnerStructureBaseLen5", 0.282353); // 12
      A.setVar("BeRefLowInnerStructureBaseLen6", 0.0411765); // 1.75
      A.setVar("BeRefLowInnerStructureMat0", "SS316L");
      A.setVar("BeRefLowInnerStructureMat1", "H2O");
      A.setVar("BeRefLowInnerStructureMat2", "SS316L");
      A.setVar("BeRefLowInnerStructureMat3", "H2O");
      A.setVar("BeRefLowInnerStructureMat4", "SS316L");
      A.setVar("BeRefLowInnerStructureMat5", "H2O");
      A.setVar("BeRefLowInnerStructureMat6", "SS316L");
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
