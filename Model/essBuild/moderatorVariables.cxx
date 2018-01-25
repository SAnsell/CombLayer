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

  Control.addVariable("LowFlyXStep",0.0);  
  Control.addVariable("LowFlyYStep",0.0);  
  Control.addVariable("LowFlyZStep",0.0);
  Control.addVariable("LowFlyXYAngle",90.0);
  Control.addVariable("LowFlyZAngle",180.0);
  Control.addVariable("LowFlyTotalHeight",7.6);
  
  Control.addVariable("LowFlyLeftLobeXStep",1.0);  
  Control.addVariable("LowFlyLeftLobeYStep",0.0);  

  Control.addVariable("LowFlyLeftLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("LowFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("LowFlyLeftLobeRadius1",5.0);
  Control.addVariable("LowFlyLeftLobeRadius2",2.506);
  Control.addVariable("LowFlyLeftLobeRadius3",2.506);

  Control.addVariable("LowFlyLeftLobeModMat","ParaOrtho%99.5");  
  Control.addVariable("LowFlyLeftLobeModTemp",20.0);

  Control.addVariable("LowFlyLeftLobeNLayers",4);

  Control.addVariable("LowFlyLeftLobeHeight1",0.3);
  Control.addVariable("LowFlyLeftLobeDepth1",0.3);
  Control.addVariable("LowFlyLeftLobeThick1",0.3);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium20K");
  Control.addVariable("LowFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("LowFlyLeftLobeHeight2",0.5);
  Control.addVariable("LowFlyLeftLobeDepth2",0.5);
  Control.addVariable("LowFlyLeftLobeThick2",0.5);
  Control.addVariable("LowFlyLeftLobeMat2","Void");

  Control.addVariable("LowFlyLeftLobeHeight3",0.0);
  Control.addVariable("LowFlyLeftLobeDepth3",0.0);  
  Control.addVariable("LowFlyLeftLobeThick3",0.3);
  Control.addVariable("LowFlyLeftLobeMat3","Aluminium");


  Control.addVariable("LowFlyFlowGuideBaseThick",0.2);
  Control.addVariable("LowFlyFlowGuideBaseLen",8.5);
  Control.addVariable("LowFlyFlowGuideArmThick",0.2);
  Control.addVariable("LowFlyFlowGuideArmLen",8.0);
  Control.addVariable("LowFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("LowFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("LowFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("LowFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("LowFlyFlowGuideWallTemp",20.0);
  
  Control.addVariable("LowFlyRightLobeXStep",-1.0);  
  Control.addVariable("LowFlyRightLobeYStep",0.0);  

  Control.addVariable("LowFlyRightLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("LowFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("LowFlyRightLobeRadius1",5.0);
  Control.addVariable("LowFlyRightLobeRadius2",2.506);
  Control.addVariable("LowFlyRightLobeRadius3",2.506);


  Control.addVariable("LowFlyRightLobeModMat","ParaOrtho%99.5");
  Control.addVariable("LowFlyRightLobeModTemp",20.0);

  Control.addVariable("LowFlyRightLobeNLayers",4);
  Control.addVariable("LowFlyRightLobeThick1",0.3);
  Control.addVariable("LowFlyRightLobeMat1","Aluminium");

  Control.addVariable("LowFlyRightLobeHeight1",0.3);
  Control.addVariable("LowFlyRightLobeDepth1",0.3);
  Control.addVariable("LowFlyRightLobeTemp1",20.0);
  
  Control.addVariable("LowFlyRightLobeThick2",0.5);
  Control.addVariable("LowFlyRightLobeMat2","Void");

  Control.addVariable("LowFlyRightLobeHeight2",0.5);
  Control.addVariable("LowFlyRightLobeDepth2",0.5);

  Control.addVariable("LowFlyRightLobeThick3",0.3);
  Control.addVariable("LowFlyRightLobeMat3","Aluminium");

  Control.addVariable("LowFlyRightLobeHeight3",0.0);
  Control.addVariable("LowFlyRightLobeDepth3",0.0);

  Control.addVariable("LowFlyMidWaterCutLayer",3);
  Control.addVariable("LowFlyMidWaterMidYStep",4.635);
  Control.addVariable("LowFlyMidWaterMidAngle",90);
  Control.addVariable("LowFlyMidWaterLength",10.98);

  Control.addVariable("LowFlyMidWaterWallThick",0.2);
  Control.addVariable("LowFlyMidWaterModMat","H2O");
  Control.addVariable("LowFlyMidWaterWallMat","Aluminium");
  Control.addVariable("LowFlyMidWaterModTemp",300.0);

  Control.addVariable("LowFlyLeftWaterWidth",15.76);
  Control.addVariable("LowFlyLeftWaterWallThick",0.347);
  Control.addVariable("LowFlyLeftWaterCutAngle",30.0);
  Control.addVariable("LowFlyLeftWaterCutWidth",10.56);
  
  Control.addVariable("LowFlyLeftWaterModMat","H2O");
  Control.addVariable("LowFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("LowFlyLeftWaterModTemp",300.0);


  Control.addVariable("LowFlyRightWaterWidth",15.76);
  Control.addVariable("LowFlyRightWaterWallThick",0.347);
  Control.addVariable("LowFlyRightWaterCutAngle",30.0);
  Control.addVariable("LowFlyRightWaterCutWidth",10.56);
  Control.addVariable("LowFlyRightWaterModMat","H2O");
  Control.addVariable("LowFlyRightWaterWallMat","Aluminium");
  Control.addVariable("LowFlyRightWaterModTemp",300.0);
  // TOP MODERATOR

  Control.addVariable("TopFlyXStep",0.0);  
  Control.addVariable("TopFlyYStep",0.0);  
  Control.addVariable("TopFlyZStep",0.0);
  Control.addVariable("TopFlyXYAngle",90.0);
  Control.addVariable("TopFlyZAngle",0.0);
  Control.addVariable("TopFlyTotalHeight",5.5);
  
  Control.addVariable("TopFlyLeftLobeXStep",1.0);  
  Control.addVariable("TopFlyLeftLobeYStep",0.0);  

  Control.addVariable("TopFlyLeftLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("TopFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("TopFlyLeftLobeRadius1",5.0);
  Control.addVariable("TopFlyLeftLobeRadius2",2.506);
  Control.addVariable("TopFlyLeftLobeRadius3",2.506);

  Control.addVariable("TopFlyLeftLobeModMat","ParaOrtho%99.5");  // LH05ortho
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

  Control.addVariable("TopFlyLeftLobeHeight3",0.60);
  Control.addVariable("TopFlyLeftLobeDepth3",0.30);

  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium");
  Control.addVariable("TopFlyFlowGuideBaseThick",0.2);
  Control.addVariable("TopFlyFlowGuideBaseLen",8.5);
  Control.addVariable("TopFlyFlowGuideArmThick",0.2);
  Control.addVariable("TopFlyFlowGuideArmLen",8.0);
  Control.addVariable("TopFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("TopFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("TopFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium");
  Control.addVariable("TopFlyFlowGuideWallTemp",20.0);
  
  Control.addVariable("TopFlyRightLobeXStep",-1.0);  
  Control.addVariable("TopFlyRightLobeYStep",0.0);  

  Control.addVariable("TopFlyRightLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("TopFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("TopFlyRightLobeRadius1",5.0);
  Control.addVariable("TopFlyRightLobeRadius2",2.506);
  Control.addVariable("TopFlyRightLobeRadius3",2.506);

  Control.addVariable("TopFlyRightLobeModMat","ParaOrtho%99.5");
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
  Control.addVariable("TopFlyRightLobeHeight3",0.3);
  Control.addVariable("TopFlyRightLobeDepth3",0.3);
  Control.addVariable("TopFlyRightLobeMat3","Aluminium");

  Control.addVariable("TopFlyMidWaterCutLayer",3);
  Control.addVariable("TopFlyMidWaterMidYStep",4.635);
  Control.addVariable("TopFlyMidWaterMidAngle",90.0);
  Control.addVariable("TopFlyMidWaterLength",10.98);
 
  Control.addVariable("TopFlyMidWaterWallThick",0.2);
  Control.addVariable("TopFlyMidWaterTopThick",0.2);
  Control.addVariable("TopFlyMidWaterBaseThick",0.2);
  Control.addVariable("TopFlyMidWaterCornerRadius",2.0);
  Control.addVariable("TopFlyMidWaterModMat","H2O");
  Control.addVariable("TopFlyMidWaterWallMat","Aluminium");
  Control.addVariable("TopFlyMidWaterModTemp",300.0);

  Control.addVariable("TopFlyLeftWaterWidth",15.76);  
  Control.addVariable("TopFlyLeftWaterWallThick",0.347);
  Control.addVariable("TopFlyLeftWaterCutAngle",30.0);
  Control.addVariable("TopFlyLeftWaterCutWidth",10.562);
  Control.addVariable("TopFlyLeftWaterModMat","H2O");
  Control.addVariable("TopFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("TopFlyLeftWaterModTemp",300.0);

  Control.addVariable("TopFlyRightWaterWidth",15.76);
  Control.addVariable("TopFlyRightWaterWallThick",0.347);
  Control.addVariable("TopFlyRightWaterCutAngle",30.0);
  Control.addVariable("TopFlyRightWaterCutWidth",10.56);
  Control.addVariable("TopFlyRightWaterModMat","H2O");
  Control.addVariable("TopFlyRightWaterWallMat","Aluminium");
  Control.addVariable("TopFlyRightWaterModTemp",300.0);

  Control.addVariable("LowPreModNLayers",4);
  Control.addVariable("LowPreModHeight0",1.5);
  Control.addVariable("LowPreModDepth0",1.5);
  Control.addVariable("LowPreModRadius0",30.0);
  Control.addVariable("LowPreModMat0","H2O");
  Control.addVariable("LowPreModHeight1",0);
  Control.addVariable("LowPreModDepth1",0);
  Control.addVariable("LowPreModRadius1",30.3);
  Control.addVariable("LowPreModMat1","Aluminium");
  Control.addVariable("LowPreModHeight2",0);
  Control.addVariable("LowPreModDepth2",0);
  //  Control.Parse("BeRefRadius");
  Control.addVariable("LowPreModRadius2",-0.3);
  //  Control.Parse("BeRefRadius-LowPreModRadius0-LowPreModRadius1");
  //  Control.addVariable("LowPreModRadius2");
  Control.addVariable("LowPreModMat2","Iron10H2O");

  Control.addVariable("LowPreModHeight3",0.3);
  Control.addVariable("LowPreModDepth3",0.3);
  Control.addVariable("LowPreModRadius3",0.0);
  Control.addVariable("LowPreModMat3","Aluminium");

  Control.addVariable("LowPreModFlowGuideWallThick", 0.3);
  Control.addVariable("LowPreModFlowGuideWallMat","Aluminium");
  Control.addVariable("LowPreModFlowGuideNBaffles", 9);
  Control.addVariable("LowPreModFlowGuideGapWidth", 3);

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


  Control.addVariable("TopCapModNLayers",4);
  
  Control.addVariable("TopCapModThick0",0.2);
  Control.addVariable("TopCapModMat0x0","Void");

  Control.addVariable("TopCapModThick1",0.3);
  Control.addVariable("TopCapModRadius1x0",32.3);
  Control.addVariable("TopCapModRadius1x1",32.6);
  Control.addVariable("TopCapModMat1x0","Aluminium");
  Control.addVariable("TopCapModMat1x1","Void"); 
  Control.addVariable("TopCapModMat1x2","Stainless304");

  Control.addVariable("TopCapModThick2",0.7);
  Control.addVariable("TopCapModRadius2x0",32.0);
  Control.addVariable("TopCapModRadius2x1",32.3);
  Control.addVariable("TopCapModRadius2x2",32.6);
  Control.addVariable("TopCapModMat2x0","H2O");
  Control.addVariable("TopCapModMat2x1","Aluminium");
  Control.addVariable("TopCapModMat2x2","Void");
  Control.addVariable("TopCapModMat2x3","Stainless304");

  Control.addVariable("TopCapModThick3",0.3);
  Control.addVariable("TopCapModRadius3x0",32.3);
  Control.addVariable("TopCapModRadius3x1",32.6);
  Control.addVariable("TopCapModMat3x0","Aluminium");
  Control.addVariable("TopCapModMat3x1","Void"); 
  Control.addVariable("TopCapModMat3x2","Stainless304");
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

  Control.copyVarSet("TopLeftPreWing", "TopRightPreWing");
  Control.addVariable("TopRightPreWingXYAngle",180.0);

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

  return;
}

} // NAMESPACE setVariable
