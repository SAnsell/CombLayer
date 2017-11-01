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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
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
#include "essVariables.h"

namespace setVariable
{

void
EssButterflyModerator(FuncDataBase& Control)
  /*!
    Create all the Conic moderator option variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssButterflyModerator");

  const std::vector<std::string> TB = {"Top", "Low"};
  const std::vector<std::string> LR = {"Left", "Right"};
  
  // TOP MODERATOR

  Control.addVariable("TopFlyXStep",0.0);  
  Control.addVariable("TopFlyYStep",0.0);  
  Control.addVariable("TopFlyZStep",0.0);

  Control.addVariable("TopFlyXYAngle",90.0);
  Control.addVariable("TopFlyZAngle",0.0);
  Control.addVariable("TopFlyTotalHeight",4.9); // master: 5.5
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
  Control.addVariable("TopFlyLeftLobeModTemp",20.0);

  Control.addVariable("TopFlyLeftLobeNLayers",4);

  Control.addVariable("TopFlyLeftLobeHeight1",0.3);
  Control.addVariable("TopFlyLeftLobeDepth1",0.3);
  Control.addVariable("TopFlyLeftLobeThick1",0.3);
  Control.addVariable("TopFlyLeftLobeMat1","Aluminium");
  Control.addVariable("TopFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("TopFlyLeftLobeThick2",0.5);
  Control.addVariable("TopFlyLeftLobeMat2","Void");

  Control.addVariable("TopFlyLeftLobeHeight2",0.5);
  Control.addVariable("TopFlyLeftLobeDepth2",0.5);

  Control.addVariable("TopFlyLeftLobeThick3",0.3);
  Control.addVariable("TopFlyLeftLobeMat3","Aluminium");

  Control.addVariable("TopFlyLeftLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be; master: 0.3
  Control.addVariable("TopFlyLeftLobeDepth3",0.0); // master: 0.3

  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("TopFlyFlowGuideWallTemp",20.0);
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
  
  Control.addVariable("TopFlyRightLobeXStep",-1.0);  
  Control.addVariable("TopFlyRightLobeYStep",0.0);  

  Control.addVariable("TopFlyRightLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("TopFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("TopFlyRightLobeRadius1",5.0);
  Control.addVariable("TopFlyRightLobeRadius2",2.506);
  Control.addVariable("TopFlyRightLobeRadius3",2.506);

  Control.addVariable("TopFlyRightLobeModMat","HPARA");
  Control.addVariable("TopFlyRightLobeModTemp",20.0);

  Control.addVariable("TopFlyRightLobeNLayers",4);

  Control.addVariable("TopFlyRightLobeHeight1",0.3);
  Control.addVariable("TopFlyRightLobeDepth1",0.3);
  Control.addVariable("TopFlyRightLobeThick1",0.3);
  Control.addVariable("TopFlyRightLobeMat1","Aluminium20K");  // 20K
  Control.addVariable("TopFlyRightLobeTemp1",20.0);
  
  Control.addVariable("TopFlyRightLobeThick2",0.5);
  Control.addVariable("TopFlyRightLobeMat2","Void");

  Control.addVariable("TopFlyRightLobeHeight2",0.5);
  Control.addVariable("TopFlyRightLobeDepth2",0.5);

  Control.addVariable("TopFlyRightLobeThick3",0.3);
  Control.addVariable("TopFlyRightLobeMat3","Aluminium");

  Control.addVariable("TopFlyRightLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be
  Control.addVariable("TopFlyRightLobeDepth3",0.0);

  for (const std::string& tb : TB)
    {
    for (const std::string& lr : LR)
      {
	Control.addVariable(tb+"Fly"+lr+"WaterWidth",15.76);
	Control.addVariable(tb+"Fly"+lr+"WaterWallThick",0.347);
	Control.addVariable(tb+"Fly"+lr+"WaterCutAngle",30.0);
	Control.addVariable(tb+"Fly"+lr+"WaterCutWidth",10.562);
	Control.addVariable(tb+"Fly"+lr+"WaterModMat","H2O");
	Control.addVariable(tb+"Fly"+lr+"WaterWallMat","Aluminium");
	Control.addVariable(tb+"Fly"+lr+"WaterModTemp",300.0);
	// Julich drawing #212-000473 (bottom right drawing)
	// https://plone.esss.lu.se/docs/neutronics/engineering/drawings/moderator/bf2-and-toppremod-drawings/view
	Control.addVariable(tb+"Fly"+lr+"WaterMidWallThick",0.2);
	Control.addVariable(tb+"Fly"+lr+"WaterMidWallLength",3.0);
      }
    Control.addVariable(tb+"FlyMidWaterCutLayer",3);
    Control.addVariable(tb+"FlyMidWaterMidYStep",4.635);
    Control.addVariable(tb+"FlyMidWaterMidAngle",90);
    Control.addVariable(tb+"FlyMidWaterLength",11.4);
    Control.addVariable(tb+"FlyMidWaterCornerRadius",0.5);
    Control.addVariable(tb+"FlyMidWaterBaseThick",0.2);
    Control.addVariable(tb+"FlyMidWaterTopThick",0.2);
    
    Control.addVariable(tb+"FlyMidWaterWallThick",0.2);
    Control.addVariable(tb+"FlyMidWaterModMat","H2O");
    Control.addVariable(tb+"FlyMidWaterWallMat","Aluminium");
    Control.addVariable(tb+"FlyMidWaterModTemp",300.0);
    }
  
  // Low fly
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
  Control.addVariable("TopCakeXStep",0.0);  
  Control.addVariable("TopCakeYStep",0.0);  
  Control.addVariable("TopCakeZStep",0.0);
  Control.addVariable("TopCakeZangle",0.0);
  Control.addVariable("TopCakeXYangle",90.0);
  Control.addVariable("TopCakeWallMat","Aluminium");
  Control.addVariable("TopCakeWallDepth",0.0);
  Control.addVariable("TopCakeWallHeight",0.3);

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

  Control.addVariable("TopCakeMidH2Height3",0.0);
  Control.addVariable("TopCakeMidH2Depth3",0.0);
  Control.addVariable("TopCakeMidH2Thick3",0.3);
  Control.addVariable("TopCakeMidH2Mat3","Aluminium");

  Control.addParse<double>("TopCakeTotalHeight",
			   "TopCakeWallHeight+TopCakeWallDepth+TopCakeMidH2Height0+TopCakeMidH2Depth0+TopCakeMidH2Height1+TopCakeMidH2Depth1+TopCakeMidH2Height2+TopCakeMidH2Depth2");
  Control.addParse<double>("TopCakeMidH2ZStep",
			   "-(TopCakeMidH2Height0+TopCakeMidH2Depth0+TopCakeMidH2Height1+TopCakeMidH2Depth1+TopCakeMidH2Depth2+TopCakeMidH2Height2+TopCakeMidH2Depth3+TopCakeMidH2Height3)/2.0-0.15");

  for ( const std::string& s : LR)
    {
      Control.addVariable("TopCake"+s+"WaterWidth",30);  
      Control.addVariable("TopCake"+s+"WaterWallThick",0.347);
      Control.addVariable("TopCake"+s+"WaterCutAngle",30.0);
      Control.addVariable("TopCake"+s+"WaterCutWidth",6);
      Control.addVariable("TopCake"+s+"WaterModMat","H2O");
      Control.addVariable("TopCake"+s+"WaterWallMat","Aluminium");
      Control.addVariable("TopCake"+s+"WaterModTemp",300.0);
      Control.addVariable("TopCake"+s+"WaterMidWallThick",0.0);
    }

  // onion cooling
  Control.addVariable("TopCakeMidH2FlowGuideType","Onion");
  Control.addVariable("TopCakeMidH2OnionCoolingXStep", 0);
  Control.addVariable("TopCakeMidH2OnionCoolingYStep", 0);
  Control.addVariable("TopCakeMidH2OnionCoolingZStep", 0.0);
  Control.addVariable("TopCakeMidH2OnionCoolingXYangle",0.0); 
  Control.addVariable("TopCakeMidH2OnionCoolingZangle",0.0);
  Control.addVariable("TopCakeMidH2OnionCoolingHeight", 3); // should be same as moderator height
  Control.addVariable("TopCakeMidH2OnionCoolingWallThick", 0.3); // in addition to Be
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
  Control.addVariable("TopBoxXStep",0.0);
  Control.addVariable("TopBoxYStep",0.0);
  Control.addVariable("TopBoxZStep",0.0);
  Control.addVariable("TopBoxZangle",0.0);
  Control.addVariable("TopBoxXYangle",90.0);
  Control.addVariable("TopBoxWallMat","Aluminium");
  Control.addVariable("TopBoxWallDepth",0.0);
  Control.addVariable("TopBoxWallHeight",0.3);

  Control.addVariable("TopBoxMidH2NLayers",4);

  Control.addVariable("TopBoxMidH2Length0",10.0);
  Control.addVariable("TopBoxMidH2Width0",10);
  Control.addVariable("TopBoxMidH2Height0",1.5);
  Control.addParse<double>("TopBoxMidH2Depth0", "TopBoxMidH2Height0");
  Control.addVariable("TopBoxMidH2Mat0","HPARA");
  Control.addVariable("TopBoxMidH2Temp0",20.0);

  Control.addVariable("TopBoxMidH2Length1",0.3);
  Control.addParse<double>("TopBoxMidH2Width1","TopBoxMidH2Length1");
  Control.addParse<double>("TopBoxMidH2Height1","TopBoxMidH2Length1");
  Control.addParse<double>("TopBoxMidH2Depth1", "TopBoxMidH2Height1");
  Control.addVariable("TopBoxMidH2Mat1","Aluminium20K");
  Control.addVariable("TopBoxMidH2Temp1",20.0);

  Control.addVariable("TopBoxMidH2Length2",0.5);
  Control.addParse<double>("TopBoxMidH2Width2","TopBoxMidH2Length2");
  Control.addParse<double>("TopBoxMidH2Height2","TopBoxMidH2Length2");
  Control.addParse<double>("TopBoxMidH2Depth2", "TopBoxMidH2Height2");
  Control.addVariable("TopBoxMidH2Mat2","Void");

  Control.addVariable("TopBoxMidH2Length3",0.3);
  Control.addParse<double>("TopBoxMidH2Width3","TopBoxMidH2Length3");
  Control.addVariable("TopBoxMidH2Height3",0.0);
  Control.addParse<double>("TopBoxMidH2Depth3", "TopBoxMidH2Height3");
  Control.addVariable("TopBoxMidH2Mat3","Aluminium");

  Control.addParse<double>("TopBoxTotalHeight",
			   "TopBoxWallHeight+TopBoxWallDepth+TopBoxMidH2Height0+TopBoxMidH2Depth0+TopBoxMidH2Height1+TopBoxMidH2Depth1+TopBoxMidH2Height2+TopBoxMidH2Depth2");
  Control.addParse<double>("TopBoxMidH2ZStep",
			   "-(TopBoxMidH2Height0+TopBoxMidH2Depth0+TopBoxMidH2Height1+TopBoxMidH2Depth1+TopBoxMidH2Depth2+TopBoxMidH2Height2+TopBoxMidH2Depth3+TopBoxMidH2Height3)/2.0-0.15");

  for ( const std::string& s : LR)
    {
      Control.addVariable("TopBox"+s+"WaterWidth",30);  
      Control.addVariable("TopBox"+s+"WaterWallThick",0.347);
      Control.addVariable("TopBox"+s+"WaterCutAngle",30.0);
      Control.addVariable("TopBox"+s+"WaterCutWidth",6);
      Control.addVariable("TopBox"+s+"WaterModMat","Be5H2O");
      Control.addVariable("TopBox"+s+"WaterWallMat","Aluminium");
      Control.addVariable("TopBox"+s+"WaterModTemp",300.0);
      Control.addVariable("TopBox"+s+"WaterMidWallThick",0.0);
      Control.addVariable("TopBox"+s+"WaterPreThick",3);
      Control.addVariable("TopBox"+s+"WaterPreMat","H2O");
      Control.addVariable("TopBox"+s+"WaterPreTemp",300.0);
   }
  Control.addVariable("TopBoxMidH2FlowGuideType", "None");
  ////////////////////////////////////////////////////////////////////////
  
  Control.addVariable("TopPreModNLayers",2);
  
  Control.addVariable("TopPreModThick0",0.3);
  Control.addVariable("TopPreModThick1",2.85);
  Control.addVariable("TopPreModThick2",0.3);
  
  Control.addVariable("TopPreModRadius0x0",30.3);
  Control.addVariable("TopPreModRadius0x1",30.6);
  Control.addVariable("TopPreModMat0x0","Aluminium");
  Control.addVariable("TopPreModMat0x1","Void");
  Control.addVariable("TopPreModMat0x2","Stainless304");

  Control.addVariable("TopPreModRadius1x0",30.0);
  Control.addVariable("TopPreModRadius1x1",30.3);
  Control.addVariable("TopPreModRadius1x2",30.6);
  Control.addVariable("TopPreModMat1x0","H2O");
  Control.addVariable("TopPreModMat1x1","Aluminium");
  Control.addVariable("TopPreModMat1x2","Void"); 
  Control.addVariable("TopPreModMat1x3","Stainless304");

  Control.addVariable("TopPreModRadius2x0",30.0);
  Control.addVariable("TopPreModRadius2x1",30.3);
  Control.addVariable("TopPreModRadius2x2",30.6);
  Control.addVariable("TopPreModMat2x0","Empty");
  Control.addVariable("TopPreModMat2x1","Aluminium");
  Control.addVariable("TopPreModMat2x2","Void");
  Control.addVariable("TopPreModMat2x3","Stainless304");

  Control.copyVarSet("TopPreMod","LowPreMod");
  Control.addVariable("TopCapModNLayers",2);
  
  Control.addVariable("TopCapModThick0",0.3);
  Control.addVariable("TopCapModRadius0x0",30.0);
  Control.addVariable("TopCapModRadius0x1",32.3);
  Control.addVariable("TopCapModRadius0x2",32.6);
  Control.addVariable("TopCapModMat0x0","H2O");
  Control.addVariable("TopCapModMat0x1","Aluminium");
  Control.addVariable("TopCapModMat0x2","Void");
  Control.addVariable("TopCapModMat0x3","Stainless304");

  Control.addVariable("TopCapModThick1",0.7);
  Control.addVariable("TopCapModRadius1x0",32.0);
  Control.addVariable("TopCapModRadius1x1",32.3);
  Control.addVariable("TopCapModRadius1x2",32.6);
  Control.addVariable("TopCapModMat1x0","H2O");
  Control.addVariable("TopCapModMat1x1","Aluminium");
  Control.addVariable("TopCapModMat1x2","Void");
  Control.addVariable("TopCapModMat1x3","Stainless304");

  Control.addVariable("TopCapModThick2",0.3);
  Control.addVariable("TopCapModRadius2x0",32.3);
  Control.addVariable("TopCapModRadius2x1",32.6);
  Control.addVariable("TopCapModMat2x0","Aluminium");
  Control.addVariable("TopCapModMat2x1","Void");
  Control.addVariable("TopCapModMat2x2","Stainless304");
  Control.copyVarSet("TopCapMod","LowCapMod");


  Control.addVariable("TopLeftPreWingInnerHeight",2.0);
  Control.addVariable("TopLeftPreWingOuterHeight",2.5);
  Control.addVariable("TopLeftPreWingInnerDepth",2.0);
  Control.addVariable("TopLeftPreWingOuterDepth",2.5);
  Control.addVariable("TopLeftPreWingInnerRadius",10.0);
  Control.addVariable("TopLeftPreWingOuterRadius",38.0);
  Control.addVariable("TopLeftPreWingInnerYCut",8.0);
  Control.addVariable("TopLeftPreWingWallThick",0.3);
  Control.addVariable("TopLeftPreWingMat","H2O");
  Control.addVariable("TopLeftPreWingWallMat","Aluminium");

  Control.addVariable("TopLeftPreWingNLayers",4);    // RADII!!!!!
  Control.addVariable("TopLeftPreWingLayerRadius1",30.0);
  Control.addVariable("TopLeftPreWingInnerMat1","Aluminium");
  Control.addVariable("TopLeftPreWingSurfMat1","Aluminium");

  Control.addVariable("TopLeftPreWingLayerRadius2",30.3);
  Control.addVariable("TopLeftPreWingInnerMat2","Void");
  Control.addVariable("TopLeftPreWingSurfMat2","Void");
  
  Control.addVariable("TopLeftPreWingLayerRadius3",30.6);
  Control.addVariable("TopLeftPreWingInnerMat3","Stainless304");
  Control.addVariable("TopLeftPreWingSurfMat3","Stainless304");
  
  Control.addVariable("TopRightPreWingXYAngle",180.0);
  Control.addVariable("TopRightPreWingInnerHeight",1.8);
  Control.addVariable("TopRightPreWingOuterHeight",2.5);
  Control.addVariable("TopRightPreWingInnerDepth",1.8);
  Control.addVariable("TopRightPreWingOuterDepth",2.5);
  Control.addVariable("TopRightPreWingInnerRadius",10.0);
  Control.addVariable("TopRightPreWingOuterRadius",38.0);
  Control.addVariable("TopRightPreWingInnerYCut",8.0);
  Control.addVariable("TopRightPreWingWallThick",0.3);
  Control.addVariable("TopRightPreWingMat","H2O");
  Control.addVariable("TopRightPreWingWallMat","Aluminium");

  Control.addVariable("TopRightPreWingNLayers",4);
  Control.addVariable("TopRightPreWingLayerRadius1",30.0);
  Control.addVariable("TopRightPreWingInnerMat1","Aluminium");
  Control.addVariable("TopRightPreWingSurfMat1","Aluminium");

  Control.addVariable("TopRightPreWingLayerRadius2",30.3);
  Control.addVariable("TopRightPreWingInnerMat2","Void");
  Control.addVariable("TopRightPreWingSurfMat2","Void");
  
  Control.addVariable("TopRightPreWingLayerRadius3",30.6);
  Control.addVariable("TopRightPreWingInnerMat3","Stainless304");
  Control.addVariable("TopRightPreWingSurfMat3","Stainless304");

  return;
}

} // NAMESPACE setVariable
