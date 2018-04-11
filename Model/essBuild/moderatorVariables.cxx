/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/moderatorVariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <numeric>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h" 
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "essVariables.h"

namespace setVariable
{

void
EssButterflyModerator(mainSystem::inputParam& IParam,FuncDataBase& Control)
  /*!
    Create all the Conic moderator option variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssButterflyModerator");

  // Top Butterfly: variables for BF2

  Control.addVariable("TopFlyXStep",0.0);  
  Control.addVariable("TopFlyYStep",0.0);  
  Control.addVariable("TopFlyZStep",0.0);

  Control.addVariable("TopFlyXYAngle",90.0);
  Control.addVariable("TopFlyZAngle",0.0);
  Control.addVariable("TopFlyTotalHeight",5.5);
  Control.addVariable("TopFlyWallMat","Aluminium");

  Control.addVariable("TopFlyLeftLobeXStep",1.0);  
  Control.addVariable("TopFlyLeftLobeYStep",0.0);  

  Control.addVariable("TopFlyLeftLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("TopFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("TopFlyLeftLobeRadius1",5.0);
  Control.addVariable("TopFlyLeftLobeRadius2",2.506);
  Control.addVariable("TopFlyLeftLobeRadius3",2.506);

  Control.addVariable("TopFlyLeftLobeModMat","HPARA");
  Control.addVariable("TopFlyLeftLobeHomogenisedModMat","HPARA_Al2.9");
  Control.addVariable("TopFlyLeftLobeModTemp",20.0);

  Control.addVariable("TopFlyLeftLobeNLayers",4);

  Control.addVariable("TopFlyLeftLobeHeight1",0.3);
  Control.addVariable("TopFlyLeftLobeDepth1",0.3);
  Control.addVariable("TopFlyLeftLobeThick1",0.3);
  Control.addVariable("TopFlyLeftLobeMat1","Aluminium20K");
  Control.addVariable("TopFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("TopFlyLeftLobeThick2",0.5);
  Control.addVariable("TopFlyLeftLobeMat2","Void");

  Control.addVariable("TopFlyLeftLobeHeight2",0.5);
  Control.addVariable("TopFlyLeftLobeDepth2",0.5);

  Control.addVariable("TopFlyLeftLobeThick3",0.3);
  Control.addVariable("TopFlyLeftLobeMat3","Aluminium");

  Control.addVariable("TopFlyLeftLobeHeight3",0.6);
  Control.addVariable("TopFlyLeftLobeDepth3",0.3);

  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("TopFlyFlowGuideWallTemp",20.0);
  Control.addVariable("TopFlyFlowGuideWallThick",0.2);

  // BF1 - straight and curved blades
  Control.addVariable("TopFlyFlowGuideBaseOffset",-10.7);
  Control.addVariable("TopFlyFlowGuideLen1L",3.0);
  Control.addVariable("TopFlyFlowGuideLen1R",8);
  Control.addVariable("TopFlyFlowGuideAngle1",90);
  Control.addVariable("TopFlyFlowGuideRadius1",1);
  Control.addVariable("TopFlyFlowGuideLen1Foot",2.3);
  Control.addVariable("TopFlyFlowGuideDist12",3);
  Control.addVariable("TopFlyFlowGuideLen2L",5.0);
  Control.addVariable("TopFlyFlowGuideLen2R",7);
  Control.addVariable("TopFlyFlowGuideAngle2",135);
  Control.addVariable("TopFlyFlowGuideRadius2",1);
  Control.addVariable("TopFlyFlowGuideLen2Foot",4);
  Control.addVariable("TopFlyFlowGuideDist23",3);
  Control.addVariable("TopFlyFlowGuideLen3L",2.0);
  Control.addVariable("TopFlyFlowGuideLen3R",4.0);
  Control.addVariable("TopFlyFlowGuideAngle3",90);
  Control.addVariable("TopFlyFlowGuideDist14",5);
  Control.addVariable("TopFlyFlowGuideLen4L",8.0);
  Control.addVariable("TopFlyFlowGuideLen4R",-5.0);
  Control.addVariable("TopFlyFlowGuideAngle4",135+90);

  // BF2 - McDonald
  Control.addVariable("TopFlyFlowGuideBaseThick",0.3);
  Control.addVariable("TopFlyFlowGuideBaseLen",6.5);
  Control.addVariable("TopFlyFlowGuideBaseOffset",-12);
  Control.addVariable("TopFlyFlowGuideAngle",17);
  Control.addVariable("TopFlyFlowGuideSQOffsetY",4.0);
  Control.addVariable("TopFlyFlowGuideSQSideA",1.0);
  Control.addVariable("TopFlyFlowGuideSQSideE",-1.0);
  Control.addVariable("TopFlyFlowGuideSQSideF",-0.005);
  Control.addVariable("TopFlyFlowGuideSQCenterA",1.0);
  Control.addVariable("TopFlyFlowGuideSQCenterE",-0.6);
  Control.addVariable("TopFlyFlowGuideSQCenterF",-0.005);

  Control.addVariable("TopFlyLeftWaterWidth",15.76);
  Control.addVariable("TopFlyLeftWaterWallThick",0.347);
  Control.addVariable("TopFlyLeftWaterCutAngle",30.0);
  Control.addVariable("TopFlyLeftWaterCutWidth",10.562);
  Control.addVariable("TopFlyLeftWaterModMat","H2O");
  Control.addVariable("TopFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("TopFlyLeftWaterModTemp",300.0);
  // Julich drawing #212-000473 (bottom right drawing)
  // https://plone.esss.lu.se/docs/neutronics/engineering/drawings/moderator/bf2-and-toppremod-drawings/view
  Control.addVariable("TopFlyLeftWaterMidWallThick",0.2);
  Control.addVariable("TopFlyLeftWaterMidWallLength",3.0);

  // Top fly right-side components
  Control.copyVarSet("TopFlyLeft","TopFlyRight");
  Control.addVariable("TopFlyRightLobeXStep",-1.0);

  Control.addVariable("TopFlyMidWaterCutLayer",3);
  Control.addVariable("TopFlyMidWaterMidYStep",4.635);
  Control.addVariable("TopFlyMidWaterMidAngle",90);
  Control.addVariable("TopFlyMidWaterLength",11.4);
  Control.addVariable("TopFlyMidWaterCornerRadius",0.5);
  Control.addVariable("TopFlyMidWaterBaseThick",0.3);
  Control.addVariable("TopFlyMidWaterTopThick",0.6);
  
  Control.addVariable("TopFlyMidWaterWallThick",0.2);
  Control.addVariable("TopFlyMidWaterModMat","H2O");
  Control.addVariable("TopFlyMidWaterWallMat","Aluminium");
  Control.addVariable("TopFlyMidWaterModTemp",300.0);

  // Low Butterfly
  Control.copyVarSet("TopFly","LowFly");
  Control.addVariable("LowFlyTotalHeight",8.1-0.4);
  Control.addVariable("LowFlyZAngle", 180);
  Control.addVariable("LowFlyLeftLobeHeight1",0.4);
  Control.addVariable("LowFlyLeftLobeThick1",0.4);
  Control.addVariable("LowFlyLeftLobeThick3",0.4);
  Control.addVariable("LowFlyRightLobeHeight1",0.4);
  Control.addVariable("LowFlyRightLobeThick1",0.4);
  Control.addVariable("LowFlyRightLobeThick3",0.4);
  Control.addVariable("LowFlyFlowGuideBaseThick",0.4);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium20K");

  // Pancake
  Control.addVariable("TopCakeXYAngle",90.0);
  Control.addVariable("TopCakeWallMat","Aluminium");

  Control.addVariable("TopCakeMidH2NLayers",4);
  
  Control.addVariable("TopCakeMidH2Height0",1.5);
  Control.addParse<double>("TopCakeMidH2Depth0", "TopCakeMidH2Height0");
  Control.addVariable("TopCakeMidH2Thick0",10);
  Control.addVariable("TopCakeMidH2Mat0","HPARA");
  Control.addVariable("TopCakeMidH2Temp0",20.0);

  Control.addVariable("TopCakeMidH2Height1",0.3);
  Control.addParse<double>("TopCakeMidH2Depth1","TopCakeMidH2Height1");
  Control.addParse<double>("TopCakeMidH2Thick1","TopCakeMidH2Height1");
  Control.addVariable("TopCakeMidH2Mat1","Aluminium20K");
  Control.addVariable("TopCakeMidH2Temp1",20.0);

  Control.addVariable("TopCakeMidH2Height2",0.5);
  Control.addParse<double>("TopCakeMidH2Depth2","TopCakeMidH2Height2");
  Control.addParse<double>("TopCakeMidH2Thick2","TopCakeMidH2Height2");
  Control.addVariable("TopCakeMidH2Mat2","Void");

  Control.addVariable("TopCakeMidH2Height3",0.3);
  Control.addParse<double>("TopCakeMidH2Depth3","TopCakeMidH2Height3");
  Control.addParse<double>("TopCakeMidH2Thick3","TopCakeMidH2Height3");
  Control.addVariable("TopCakeMidH2Mat3","Aluminium");

  Control.addParse<double>("TopCakeTotalHeight",
			   "TopCakeMidH2Height0+TopCakeMidH2Depth0+TopCakeMidH2Height1+TopCakeMidH2Depth1+TopCakeMidH2Height2+TopCakeMidH2Depth2+TopCakeMidH2Height3+TopCakeMidH2Depth3");
  Control.addParse<double>("TopCakeMidH2ZStep","-TopCakeTotalHeight/2.0");

  Control.addVariable("TopCakeLeftWaterWidth",30);
  Control.addVariable("TopCakeLeftWaterWallThick",0.347);
  Control.addVariable("TopCakeLeftWaterCutAngle",30.0);
  Control.addVariable("TopCakeLeftWaterCutWidth",6);
  Control.addVariable("TopCakeLeftWaterModMat","H2O");
  Control.addVariable("TopCakeLeftWaterWallMat","Aluminium");
  Control.addVariable("TopCakeLeftWaterModTemp",300.0);
  Control.addVariable("TopCakeLeftWaterMidWallThick",0.0);
  Control.copyVarSet("TopCakeLeftWater", "TopCakeRightWater");

  // onion cooling
  Control.addVariable("TopCakeMidH2FlowGuideType","Onion");
  Control.addParse<double>("TopCakeMidH2OnionCoolingHeight",
			   "TopCakeMidH2Height0+TopCakeMidH2Depth0");
  Control.addVariable("TopCakeMidH2OnionCoolingWallThick", 0.3);
  Control.addVariable("TopCakeMidH2OnionCoolingWallMat",   "Aluminium20K");
  Control.addVariable("TopCakeMidH2OnionCoolingWallTemp",   20.0);
  Control.addVariable("TopCakeMidH2OnionCoolingNRings", 2);
  Control.addVariable("TopCakeMidH2OnionCoolingRadius1", 4);
  Control.addVariable("TopCakeMidH2OnionCoolingGateWidth1", 1);
  Control.addVariable("TopCakeMidH2OnionCoolingGateLength1", 2);
  Control.addVariable("TopCakeMidH2OnionCoolingRadius2", 8);
  Control.addVariable("TopCakeMidH2OnionCoolingGateWidth2", 2);
  Control.addVariable("TopCakeMidH2OnionCoolingGateLength2", 1.5);

  // Box moderator
  Control.addVariable("TopBoxXYAngle",90.0);
  Control.addVariable("TopBoxWallMat","Aluminium");

  Control.addVariable("TopBoxMidH2NLayers",4);

  Control.addVariable("TopBoxMidH2Length0",10.0);
  Control.addVariable("TopBoxMidH2Width0",10.0);
  Control.addVariable("TopBoxMidH2Height0",1.5);
  Control.addParse<double>("TopBoxMidH2Depth0", "TopBoxMidH2Height0");
  Control.addVariable("TopBoxMidH2Mat0","HPARA");
  Control.addVariable("TopBoxMidH2Temp0",20.0);

  Control.addVariable("TopBoxMidH2Length1",0.3);
  Control.addParse<double>("TopBoxMidH2Width1", "TopBoxMidH2Length1");
  Control.addParse<double>("TopBoxMidH2Height1","TopBoxMidH2Length1");
  Control.addParse<double>("TopBoxMidH2Depth1", "TopBoxMidH2Height1");
  Control.addVariable("TopBoxMidH2Mat1","Aluminium20K");
  Control.addVariable("TopBoxMidH2Temp1",20.0);

  Control.addVariable("TopBoxMidH2Length2",0.5);
  Control.addParse<double>("TopBoxMidH2Width2", "TopBoxMidH2Length2");
  Control.addParse<double>("TopBoxMidH2Height2","TopBoxMidH2Length2");
  Control.addParse<double>("TopBoxMidH2Depth2", "TopBoxMidH2Height2");
  Control.addVariable("TopBoxMidH2Mat2","Void");

  Control.addVariable("TopBoxMidH2Length3",0.3);
  Control.addParse<double>("TopBoxMidH2Width3", "TopBoxMidH2Length3");
  Control.addParse<double>("TopBoxMidH2Height3","TopBoxMidH2Length3");
  Control.addParse<double>("TopBoxMidH2Depth3", "TopBoxMidH2Height3");
  Control.addVariable("TopBoxMidH2Mat3","Aluminium");

  Control.addParse<double>("TopBoxTotalHeight",
			   "TopBoxMidH2Height0+TopBoxMidH2Depth0+TopBoxMidH2Height1+TopBoxMidH2Depth1+TopBoxMidH2Height2+TopBoxMidH2Depth2+TopBoxMidH2Height3+TopBoxMidH2Depth3");
  Control.addParse<double>("TopBoxMidH2ZStep", "-TopBoxTotalHeight/2.0");

  Control.addVariable("TopBoxLeftWaterWidth",30);  
  Control.addVariable("TopBoxLeftWaterWallThick",0.347);
  Control.addVariable("TopBoxLeftWaterCutAngle",30.0);
  Control.addVariable("TopBoxLeftWaterCutWidth",6);
  Control.addVariable("TopBoxLeftWaterModMat","Be5H2O");
  Control.addVariable("TopBoxLeftWaterWallMat","Aluminium");
  Control.addVariable("TopBoxLeftWaterModTemp",300.0);
  Control.addVariable("TopBoxLeftWaterMidWallThick",0.0);
  Control.addVariable("TopBoxLeftWaterPreThick",3);
  Control.addVariable("TopBoxLeftWaterPreMat","H2O");
  Control.addVariable("TopBoxLeftWaterPreTemp",300.0);
  Control.copyVarSet("TopBoxLeft", "TopBoxRight");

  Control.addVariable("TopBoxMidH2FlowGuideType", "None");
  ////////////////////////////////////////////////////////////////////////
  
  Control.addVariable("TopPreModNLayers",2);
  
  Control.addVariable("TopPreModThick0",0.3);
  Control.addVariable("TopPreModThick1",3.0);
  Control.addVariable("TopPreModThick2",0.3);
  
  Control.addVariable("TopPreModRadius0x0",30.3);
  Control.addVariable("TopPreModRadius0x1",30.6);
  Control.addVariable("TopPreModMat0x0","Aluminium");
  Control.addVariable("TopPreModMat0x1","Void");
  Control.addVariable("TopPreModMat0x2","SS316L");

  Control.addVariable("TopPreModRadius1x0",30.0);
  Control.addVariable("TopPreModRadius1x1",30.3);
  Control.addVariable("TopPreModRadius1x2",30.6);
  Control.addVariable("TopPreModMat1x0","H2O_7Al");
  Control.addVariable("TopPreModMat1x1","Aluminium");
  Control.addVariable("TopPreModMat1x2","Void"); 
  Control.addVariable("TopPreModMat1x3","SS316L");

  Control.addVariable("TopPreModRadius2x0",30.0);
  Control.addVariable("TopPreModRadius2x1",30.3);
  Control.addVariable("TopPreModRadius2x2",30.6);
  Control.addVariable("TopPreModMat2x0","Empty");
  Control.addVariable("TopPreModMat2x1","Aluminium");
  Control.addVariable("TopPreModMat2x2","Void");
  Control.addVariable("TopPreModMat2x3","SS316L");

  Control.copyVarSet("TopPreMod","LowPreMod");
  Control.addVariable("TopCapModNLayers",2);
  
  Control.addVariable("TopCapModThick0",0.3);
  Control.addVariable("TopCapModRadius0x0",30.0);
  Control.addVariable("TopCapModRadius0x1",32.3);
  Control.addVariable("TopCapModRadius0x2",32.6);
  Control.addVariable("TopCapModMat0x0","H2O_7Al");
  Control.addVariable("TopCapModMat0x1","Aluminium");
  Control.addVariable("TopCapModMat0x2","Void");
  Control.addVariable("TopCapModMat0x3","SS316L");

  Control.addVariable("TopCapModThick1",0.7);
  Control.addVariable("TopCapModRadius1x0",32.0);
  Control.addVariable("TopCapModRadius1x1",32.3);
  Control.addVariable("TopCapModRadius1x2",32.6);
  Control.addVariable("TopCapModMat1x0","H2O_7Al");
  Control.addVariable("TopCapModMat1x1","Aluminium");
  Control.addVariable("TopCapModMat1x2","Void");
  Control.addVariable("TopCapModMat1x3","SS316L");

  Control.addVariable("TopCapModThick2",0.3);
  Control.addVariable("TopCapModRadius2x0",32.3);
  Control.addVariable("TopCapModRadius2x1",32.6);
  Control.addVariable("TopCapModMat2x0","Aluminium");
  Control.addVariable("TopCapModMat2x1","Void");
  Control.addVariable("TopCapModMat2x2","SS316L");
  Control.copyVarSet("TopCapMod","LowCapMod");


  Control.addVariable("TopLeftPreWingInnerHeight",2.0);
  Control.addVariable("TopLeftPreWingOuterHeight",2.5);
  Control.addVariable("TopLeftPreWingInnerDepth",2.0);
  Control.addVariable("TopLeftPreWingOuterDepth",2.5);
  Control.addVariable("TopLeftPreWingInnerRadius",19.5);
  Control.addVariable("TopLeftPreWingOuterRadius",38.0);
  Control.addVariable("TopLeftPreWingInnerYCut",8.0);
  Control.addVariable("TopLeftPreWingWallThick",0.3);
  Control.addVariable("TopLeftPreWingMat","H2O_7Al");
  Control.addVariable("TopLeftPreWingWallMat","Aluminium");

  Control.addVariable("TopLeftPreWingNLayers",4);    // RADII!!!!!
  Control.addVariable("TopLeftPreWingLayerRadius1",30.0);
  Control.addVariable("TopLeftPreWingInnerMat1","Aluminium");
  Control.addVariable("TopLeftPreWingSurfMat1","Aluminium");

  Control.addVariable("TopLeftPreWingLayerRadius2",30.3);
  Control.addVariable("TopLeftPreWingInnerMat2","Void");
  Control.addVariable("TopLeftPreWingSurfMat2","Void");
  
  Control.addVariable("TopLeftPreWingLayerRadius3",30.6);
  Control.addVariable("TopLeftPreWingInnerMat3","SS316L");
  Control.addVariable("TopLeftPreWingSurfMat3","SS316L");
  
  Control.copyVarSet("TopLeftPreWing", "TopRightPreWing");
  Control.addVariable("TopRightPreWingXYAngle",180.0);

  // if lowMod is None then BeRefLowVoidThick must be set to 0.0
  const std::string lowMod=IParam.getValue<std::string>("lowMod");
  const std::string topMod=IParam.getValue<std::string>("topMod");

  std::set<std::string> allowedTopModTypes={"BF1","BF2","Pancake","Box","None"};
  std::set<std::string> allowedLowModTypes={"BF1","BF2","None"};
  std::ostringstream streamTop, streamLow;
  
  std::copy(allowedTopModTypes.begin(), allowedTopModTypes.end(),
	    std::ostream_iterator<std::string>(streamTop, " "));
  if (allowedTopModTypes.find(topMod)==allowedTopModTypes.end())
    throw ColErr::InvalidLine(topMod,
			      "Wrong top moderator type. Supported types: " + streamTop.str());

  std::copy(allowedLowModTypes.begin(), allowedLowModTypes.end(),
	    std::ostream_iterator<std::string>(streamLow, " "));
  if (allowedLowModTypes.find(lowMod)==allowedLowModTypes.end())
    throw ColErr::InvalidLine(lowMod,
			      "Wrong low moderator type. Supported types: " + streamLow.str());


  if ((topMod=="BF1") || (lowMod=="BF1"))
    {
      std::vector<std::string> TLfly, TLpipe;

      if (topMod=="BF1")
	{
	  IParam.setValue("topMod", std::string("Butterfly"));
	  Control.addVariable("TopFlyType", 1);
	  IParam.setValue("topPipe", std::string("supply"));
	  TLfly.push_back("TopFly");
	  TLpipe.push_back("T");
	}

      if (lowMod=="BF1")
	{
	  IParam.setValue("lowMod", std::string("Butterfly"));
	  Control.addVariable("LowFlyType", 1);
	  IParam.setValue("lowPipe", std::string("supply"));
	  TLfly.push_back("LowFly");
	  TLpipe.push_back("L");
	}

      // These variables are as in the model received from Luca 22 Feb 2017
      // there is no engieering drawings for BF1 yet, so this email is
      // the only reference
      std::string s;
      for (std::string s : TLfly) {
	Control.setVariable(s+"LeftLobeCorner1",Geometry::Vec3D(0,0.6,0));
	Control.setVariable(s+"LeftLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	Control.setVariable(s+"LeftLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));

	Control.setVariable(s+"RightLobeCorner1",Geometry::Vec3D(0,0.6,0));
	Control.setVariable(s+"RightLobeCorner2",Geometry::Vec3D(-14.4,-12.95,0));
	Control.setVariable(s+"RightLobeCorner3",Geometry::Vec3D(14.4,-12.95,0));

	Control.setVariable(s+"LeftLobeRadius1",1.0);
	Control.setVariable(s+"RightLobeRadius1",1.0);

	Control.setVariable(s+"LeftLobeXStep",-0.85);
	Control.setVariable(s+"RightLobeXStep",0.85);

	Control.setVariable(s+"LeftWaterCutWidth",9.6);
	Control.setVariable(s+"RightWaterCutWidth",9.6);

	Control.setVariable(s+"MidWaterLength",9.9);
	Control.setVariable(s+"MidWaterMidYStep",6.3);
      }

      // flow guides
      for (std::string strmod : TLfly) {
	s = strmod + "FlowGuide";
	Control.addVariable(s+"BaseOffset",-10.5);
	Control.setVariable(s+"Len1L",1.2);
	Control.setVariable(s+"Len1R",8);
	Control.setVariable(s+"Angle1",90);
	Control.setVariable(s+"Radius1",1);
	Control.setVariable(s+"Len1Foot",2.3);
	Control.setVariable(s+"Dist12",3);
	Control.setVariable(s+"Len2L",5.0);
	Control.setVariable(s+"Len2R",8);
	Control.setVariable(s+"Angle2",135);
	Control.setVariable(s+"Radius2",1);
	Control.setVariable(s+"Len2Foot",4);
	Control.setVariable(s+"Dist23",3);
	Control.setVariable(s+"Len3L",2.0);
	Control.setVariable(s+"Len3R",4.0);
	Control.setVariable(s+"Angle3",90);
	Control.setVariable(s+"Dist14",5);
	Control.setVariable(s+"Len4L",8.0);
	Control.setVariable(s+"Len4R",-5.0);
	Control.setVariable(s+"Angle4",135+90);
      }

      for (std::string strpipe : TLpipe) {
	s = strpipe + "Supply";
	Control.setVariable(s+"RightAlNSegIn", 2);
	Control.setVariable(s+"RightAlPPt0", Geometry::Vec3D(0,0,0));
	Control.setVariable(s+"RightAlPPt1", Geometry::Vec3D(0,26,0));
	Control.setVariable(s+"RightAlPPt2", Geometry::Vec3D(0,26,40));

	Control.setVariable(s+"LeftAlNSegIn", 2);
	Control.setVariable(s+"LeftAlPPt0", Geometry::Vec3D(0,0,0));
	Control.setVariable(s+"LeftAlPPt1", Geometry::Vec3D(0,25.3,0));
	Control.setVariable(s+"LeftAlPPt2", Geometry::Vec3D(0,25.3,40));
      }
    }

  if ((topMod=="BF2") || (lowMod=="BF2"))
    {
      std::vector<std::string> TLfly, TLpipe;
	
      if (topMod=="BF2")
	{
	  IParam.setValue("topMod", std::string("Butterfly"));
	  Control.addVariable("TopFlyType", 2);
	  IParam.setValue("topPipe", std::string("supply,return"));
	  TLfly.push_back("TopFly");
	  TLpipe.push_back("T");
	}

      if (lowMod=="BF2")
	{
	  IParam.setValue("lowMod", std::string("Butterfly"));
	  Control.addVariable("LowFlyType", 2);
	  IParam.setValue("lowPipe", std::string("supply,return"));
	  TLfly.push_back("LowFly");
	  TLpipe.push_back("L");
	}

      // variables are already set in the same function above
    }
  
  if ((topMod=="Pancake") || (lowMod=="Pancake"))
    {
      std::vector<std::string> TLpipe;
      if (topMod=="Pancake")
	{
	  IParam.setValue("topMod", topMod);
	  IParam.setValue("topPipe", std::string("PancakeSupply"));
	  TLpipe.push_back("T");
	}

      if (lowMod=="Pancake")
	{
	  IParam.setValue("lowMod", lowMod);
	  IParam.setValue("lowPipe", std::string("PancakeSupply"));
	  TLpipe.push_back("T");
	  ELog::EM << "Low Pancake moderator variables are missing" << ELog::endCrit;
	}

      // straighten the pipes
      for (std::string strpipe : TLpipe)
	{
	  const std::string s(strpipe + "Supply");
	  Control.setVariable(s+"RightAlNSegIn", 2);
	  Control.setVariable(s+"RightAlPPt0", Geometry::Vec3D(0,0,0));
	  Control.setVariable(s+"RightAlPPt1", Geometry::Vec3D(0,30,0));
	  Control.setVariable(s+"RightAlPPt2", Geometry::Vec3D(0,30,40));
	  Control.setVariable(s+"LeftAlNSegIn", 2);
	  Control.setVariable(s+"LeftAlPPt0", Geometry::Vec3D(0,0,0));
	  Control.setVariable(s+"LeftAlPPt1", Geometry::Vec3D(0,30,0));
	  Control.setVariable(s+"LeftAlPPt2", Geometry::Vec3D(0,30,40));
	}
      
      Control.addVariable("TopCapWingTiltRadius", 10+0.3+0.5+0.3);
      Control.addVariable("TopPreWingTiltRadius", 10+0.3+0.5+0.3);
      Control.addVariable("TopCapWingThick", 1.1);
      Control.addVariable("TopCapWingTiltAngle", 1.7);
      Control.addVariable("TopPreWingThick", 0.8);
      Control.addVariable("TopPreWingTiltAngle", 1.5);
    }
  
  if ((topMod=="Box") || (lowMod=="Box"))
    {
      std::vector<std::string> TLpipe;
      if (topMod=="Box")
	{
	  IParam.setValue("topMod", topMod);
	  IParam.setValue("topPipe", std::string("BoxSupply"));
	  TLpipe.push_back("T");
	}

      if (lowMod=="Box")
	{
	  IParam.setValue("lowMod", lowMod);
	  IParam.setValue("lowPipe", std::string("BoxSupply"));
	  TLpipe.push_back("T");
	  ELog::EM << "Low Box moderator variables are missing" << ELog::endCrit;
	}

      // straighten the pipes
      for (std::string strpipe : TLpipe)
	{
	  const std::string s(strpipe + "Supply");
	  Control.setVariable(s+"RightAlNSegIn", 2);
	  Control.setVariable(s+"RightAlPPt0", Geometry::Vec3D(0,0,0));
	  Control.setVariable(s+"RightAlPPt1", Geometry::Vec3D(0,40,0));
	  Control.setVariable(s+"RightAlPPt2", Geometry::Vec3D(0,40,40));
	  Control.setVariable(s+"LeftAlNSegIn", 2);
	  Control.setVariable(s+"LeftAlPPt0", Geometry::Vec3D(0,0,0));
	  Control.setVariable(s+"LeftAlPPt1", Geometry::Vec3D(0,40,0));
	  Control.setVariable(s+"LeftAlPPt2", Geometry::Vec3D(0,40,40));
	}

      Control.addVariable("TopCapWingTiltRadius", 10+0.3+0.5+0.3);
      Control.addVariable("TopPreWingTiltRadius", 10+0.3+0.5+0.3);
      Control.addVariable("TopCapWingThick", 1.1);
      Control.addVariable("TopCapWingTiltAngle", 1.7);
      Control.addVariable("TopPreWingThick", 0.8);
      Control.addVariable("TopPreWingTiltAngle", 1.5);
    }

  if (lowMod=="None") // single
    {
      Control.setVariable("BeRefLowVoidThick", 0);
      Control.setVariable("BeRefLowRefMat", std::string("SS316L"));
      Control.setVariable("BeRefLowWallMat", std::string("AluminiumBe"));
      Control.addVariable("BeRefLowInnerStructureActive", 1);
      // The 'depth' array numbers are from LZ drawing 2017-10-10
      // and discussions with Marc 2017-10-11
      // https://plone.esss.lu.se/docs/neutronics/engineering/drawings/reflector/water-layers-in-lower-be-reflector/view
      std::vector<double> depth({3.0,5.1,3.0,0.9,3.0,3.0,3.0,9.0,3.0,9.0,3.0,12.7});
      const double BeRefDepth(std::accumulate(depth.begin(), depth.end(), 0.0));
      const double Ztop(7.8);// z-coordinate of LowBeRef upper plane
      Control.setVariable("BeRefDepth", BeRefDepth+Ztop);
      Control.addVariable("BulkDepth1", BeRefDepth+Ztop+1.0); // a bit bigger to make some clearance
      Control.addVariable("BeRefVoidCylRadius", 15.0);
      Control.addVariable("BeRefVoidCylDepth", std::accumulate(depth.begin(),
							       depth.begin()+3,
							       0.0));

      const size_t nLayers(depth.size());
      Control.setVariable("BeRefLowInnerStructureNLayers", nLayers);

      // 40% water fraction based on email from Marc 13 Oct 2017
      for (size_t i=0; i<nLayers; i++)
	{
	  Control.addVariable("BeRefLowInnerStructureBaseLen" + std::to_string(i+1),
		   depth[i]/BeRefDepth);
	  Control.addVariable("BeRefLowInnerStructureMat" + std::to_string(i),
			      (i%2) ? std::string("SS316L") :
			      std::string("SS316L_40H2O"));
	}
    }

  IParam.setValue("matDB", std::string("neutronics"));
  IParam.setValue("physModel", std::string("BD"));

  if (IParam.flag("bunkerSimple"))
    {
      // simplify the bunkers
      IParam.setValue("bunker", std::string("noPillar"));

      std::vector<std::string> bunkerName({"A","B","C","D"});
      for (const std::string n: bunkerName)
	{
	  Control.setVariable(n+"BunkerWallNBasic", 1);
	  Control.setVariable(n+"BunkerRoofNBasicVert", 1);
	  Control.setVariable(n+"BunkerNSectors", 1);
	  Control.setVariable(n+"BunkerRoofMat0", std::string("Void"));
	  Control.setVariable(n+"BunkerWallMat0", std::string("Void"));
	  Control.setVariable(n+"BunkerWallMat", std::string("Void"));
	}

      // simplify the curtain
      Control.setVariable("CurtainNBaseLayers", 1);
      Control.setVariable("CurtainNMidLayers", 1);
      Control.setVariable("CurtainNTopLayers", 1);
      Control.setVariable("CurtainWallMat", std::string("Void"));
    }
  // sdef
  // Control.setVariable("sdefEnergy", 2000.0);
  // Control.setVariable("sdefWidth", 14);
  // Control.setVariable("sdefHeight", 3.2);
  // Control.setVariable("sdefYPos", -500);
  // Control.setVariable("sdefPDF", std::string("uniform"));

  return;
}

} // NAMESPACE setVariable
