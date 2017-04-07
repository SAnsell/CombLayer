/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/moderatorVariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
  Control.addVariable("LowFlyXYangle",90.0);
  Control.addVariable("LowFlyZangle",180.0);
  Control.addVariable("LowFlyTotalHeight",8.1); // 7.6
  Control.addVariable("LowFlyWallMat","Aluminium");
  Control.addVariable("LowFlyWallDepth",0.0);
  Control.addVariable("LowFlyWallHeight",0.3);
  
  Control.addVariable("LowFlyLeftLobeXStep",1.0);  
  Control.addVariable("LowFlyLeftLobeYStep",0.0);  

  Control.addVariable("LowFlyLeftLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("LowFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("LowFlyLeftLobeRadius1",5.0);
  Control.addVariable("LowFlyLeftLobeRadius2",2.506);
  Control.addVariable("LowFlyLeftLobeRadius3",2.506);

  Control.addVariable("LowFlyLeftLobeModMat","HPARA");  
  Control.addVariable("LowFlyLeftLobeModTemp",20.0);

  Control.addVariable("LowFlyLeftLobeNLayers",4);
  Control.addVariable("LowFlyLeftLobeThick1",0.4);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyLeftLobeHeight1",0.4);
  Control.addVariable("LowFlyLeftLobeDepth1",0.4);
  Control.addVariable("LowFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("LowFlyLeftLobeThick2",0.5);
  Control.addVariable("LowFlyLeftLobeMat2","Void");

  Control.addVariable("LowFlyLeftLobeHeight2",0.5);
  Control.addVariable("LowFlyLeftLobeDepth2",0.5);

  Control.addVariable("LowFlyLeftLobeThick3",0.3);
  Control.addVariable("LowFlyLeftLobeMat3","Aluminium");

  Control.addVariable("LowFlyLeftLobeHeight3",0.0);
  Control.addVariable("LowFlyLeftLobeDepth3",0.0);

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

  Control.addVariable("LowFlyRightLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("LowFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("LowFlyRightLobeRadius1",5.0);
  Control.addVariable("LowFlyRightLobeRadius2",2.506);
  Control.addVariable("LowFlyRightLobeRadius3",2.506);

  Control.addVariable("LowFlyRightLobeModMat","HPARA");
  Control.addVariable("LowFlyRightLobeModTemp",20.0);

  Control.addVariable("LowFlyRightLobeNLayers",4);
  Control.addVariable("LowFlyRightLobeThick1",0.4);
  Control.addVariable("LowFlyRightLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyRightLobeHeight1",0.4);
  Control.addVariable("LowFlyRightLobeDepth1",0.4);
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
  Control.addVariable("LowFlyMidWaterLength",11.4);
  Control.addVariable("LowFlyMidWaterEdgeRadius",0.5);

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
  Control.addVariable("TopFlyXYangle",90.0);
  Control.addVariable("TopFlyZangle",0.0);
  Control.addVariable("TopFlyTotalHeight",4.9);
  Control.addVariable("TopFlyWallMat","Aluminium");
  Control.addVariable("TopFlyWallDepth",0.0);
  Control.addVariable("TopFlyWallHeight",0.3);
  
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
  Control.addVariable("TopFlyLeftLobeThick1",0.3);
  Control.addVariable("TopFlyLeftLobeMat1","Aluminium20K");

  Control.addVariable("TopFlyLeftLobeHeight1",0.3);
  Control.addVariable("TopFlyLeftLobeDepth1",0.3);
  Control.addVariable("TopFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("TopFlyLeftLobeThick2",0.5);
  Control.addVariable("TopFlyLeftLobeMat2","Void");

  Control.addVariable("TopFlyLeftLobeHeight2",0.5);
  Control.addVariable("TopFlyLeftLobeDepth2",0.5);

  Control.addVariable("TopFlyLeftLobeThick3",0.3);
  Control.addVariable("TopFlyLeftLobeMat3","Aluminium");

  Control.addVariable("TopFlyLeftLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be
  Control.addVariable("TopFlyLeftLobeDepth3",0.0);

  Control.addVariable("TopFlyFlowGuideBaseThick",0.2);
  Control.addVariable("TopFlyFlowGuideBaseLen",8.5);
  Control.addVariable("TopFlyFlowGuideArmThick",0.2);
  Control.addVariable("TopFlyFlowGuideArmLen",8.0);
  Control.addVariable("TopFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("TopFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("TopFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("TopFlyFlowGuideWallTemp",20.0);
  
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
  Control.addVariable("TopFlyRightLobeThick1",0.3);
  Control.addVariable("TopFlyRightLobeMat1","Aluminium20K");

  Control.addVariable("TopFlyRightLobeHeight1",0.3);
  Control.addVariable("TopFlyRightLobeDepth1",0.3);
  Control.addVariable("TopFlyRightLobeTemp1",20.0);
  
  Control.addVariable("TopFlyRightLobeThick2",0.5);
  Control.addVariable("TopFlyRightLobeMat2","Void");

  Control.addVariable("TopFlyRightLobeHeight2",0.5);
  Control.addVariable("TopFlyRightLobeDepth2",0.5);

  Control.addVariable("TopFlyRightLobeThick3",0.3);
  Control.addVariable("TopFlyRightLobeMat3","Aluminium");

  Control.addVariable("TopFlyRightLobeHeight3",0.0); // KB: must be 0, otherwise 3 Al layers b/w H2 and Be
  Control.addVariable("TopFlyRightLobeDepth3",0.0);

  Control.addVariable("TopFlyMidWaterCutLayer",3);
  Control.addVariable("TopFlyMidWaterMidYStep",4.635);
  Control.addVariable("TopFlyMidWaterMidAngle",90);
  Control.addVariable("TopFlyMidWaterLength",11.4);
  Control.addVariable("TopFlyMidWaterEdgeRadius",0.5);

  Control.addVariable("TopFlyMidWaterWallThick",0.2);
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

  // Pancake
  Control.addVariable("TopCakeXStep",0.0);  
  Control.addVariable("TopCakeYStep",0.0);  
  Control.addVariable("TopCakeZStep",0.0);
  Control.addVariable("TopCakeZangle",0.0);
  Control.addVariable("TopCakeXYangle",90.0);
  Control.addVariable("TopCakeWallMat","Aluminium");
  Control.addVariable("TopCakeWallDepth",0.0);
  Control.addVariable("TopCakeWallHeight",0.3);
  
  
  Control.addVariable("TopCakeMidH2NLayers",3);
  
  Control.addVariable("TopCakeMidH2Height0",1.5);
  Control.addParse<double>("TopCakeMidH2Depth0", "TopCakeMidH2Height0");
  Control.addVariable("TopCakeMidH2Thick0",15);
  Control.addVariable("TopCakeMidH2Mat0","HPARA");

  Control.addVariable("TopCakeMidH2Height1",0.5);
  Control.addParse<double>("TopCakeMidH2Depth1","TopCakeMidH2Height1");
  Control.addParse<double>("TopCakeMidH2Thick1","TopCakeMidH2Height1");
  Control.addVariable("TopCakeMidH2Mat1","Void");

  Control.addVariable("TopCakeMidH2Height2",0.0);
  Control.addVariable("TopCakeMidH2Depth2",0.3);
  Control.addParse<double>("TopCakeMidH2Thick2","TopCakeMidH2Depth2");
  Control.addVariable("TopCakeMidH2Mat2","Aluminium20K");

  // Control.addVariable("TopCakeTotalHeight",3+0.5*2+0.3*2 );
  Control.addParse<double>("TopCakeTotalHeight","TopCakeMidH2Height0+TopCakeMidH2Depth0+TopCakeMidH2Height1+TopCakeMidH2Depth1+0.3*2" );
  Control.addParse<double>("TopCakeMidH2ZStep",
			   "-(TopCakeMidH2Height0+TopCakeMidH2Depth0+TopCakeMidH2Height1+TopCakeMidH2Depth1+2*TopCakeMidH2Depth2+TopCakeMidH2Height2)/2.0");

  
  Control.addVariable("TopCakeLeftWaterWidth",30);  
  Control.addVariable("TopCakeLeftWaterWallThick",0.347);
  Control.addVariable("TopCakeLeftWaterCutAngle",30.0);
  Control.addVariable("TopCakeLeftWaterCutWidth",11);
  Control.addVariable("TopCakeLeftWaterModMat","H2O");
  Control.addVariable("TopCakeLeftWaterWallMat","Aluminium");
  Control.addVariable("TopCakeLeftWaterModTemp",300.0);

  Control.addVariable("TopCakeRightWaterWidth",15.76);
  Control.addVariable("TopCakeRightWaterWallThick",0.347);
  Control.addVariable("TopCakeRightWaterCutAngle",30.0);
  Control.addVariable("TopCakeRightWaterCutWidth",10.56);
  Control.addVariable("TopCakeRightWaterModMat","H2O");
  Control.addVariable("TopCakeRightWaterWallMat","Aluminium");
  Control.addVariable("TopCakeRightWaterModTemp",300.0);

  
  Control.addVariable("LowPreModNLayers",4);
  Control.addVariable("LowPreModHeight0",1.5);
  Control.addVariable("LowPreModDepth0",1.5);
  Control.addVariable("LowPreModRadius0",30.0);
  Control.addVariable("LowPreModMat0","H2OAl47");
  Control.addVariable("LowPreModHeight1",0);
  Control.addVariable("LowPreModDepth1",0);
  Control.addVariable("LowPreModRadius1",0.3);
  Control.addVariable("LowPreModMat1","Aluminium");
  Control.addVariable("LowPreModHeight2",0);
  Control.addVariable("LowPreModDepth2",0);
  //  Control.Parse("BeRefRadius");
  Control.addVariable("LowPreModRadius2",7.7-3+0.3);
  //  Control.Parse("BeRefRadius-LowPreModRadius0-LowPreModRadius1");
  //  Control.addVariable("LowPreModRadius2");
  Control.addVariable("LowPreModMat2","Aluminium");

  Control.addVariable("LowPreModHeight3",0.3);
  Control.addVariable("LowPreModDepth3",0.3);
  Control.addVariable("LowPreModRadius3",0.0);
  Control.addVariable("LowPreModMat3","Aluminium");

  Control.addVariable("LowPreModTiltAngle",0.0);

  Control.addVariable("LowPreModFlowGuideWallThick", 0.3);
  Control.addVariable("LowPreModFlowGuideWallMat","Aluminium");
  Control.addVariable("LowPreModFlowGuideNBaffles", 9);
  Control.addVariable("LowPreModFlowGuideGapWidth", 3);

  Control.addVariable("TopPreModNLayers",4);
  Control.addVariable("TopPreModHeight0",1.5);
  Control.addVariable("TopPreModDepth0",1.5);
  Control.addVariable("TopPreModRadius0",30.0);
  Control.addVariable("TopPreModMat0","H2OAl47");
  Control.addVariable("TopPreModHeight1",0);
  Control.addVariable("TopPreModDepth1",0);
  Control.addVariable("TopPreModRadius1",0.3);
  Control.addVariable("TopPreModMat1","Aluminium");
  Control.addVariable("TopPreModHeight2",0);
  Control.addVariable("TopPreModDepth2",0);
  Control.addVariable("TopPreModRadius2", 7.7-3+0.3);
  Control.addVariable("TopPreModMat2","Aluminium");
  Control.addVariable("TopPreModHeight3",0.3);
  Control.addVariable("TopPreModDepth3",0.3);
  Control.addVariable("TopPreModRadius3", 0.0);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopPreModThick3");
  Control.addVariable("TopPreModMat3","Aluminium");

  Control.addVariable("TopPreModTiltAngle", 2);
  Control.addVariable("TopPreModTiltRadius", 32.17);
  
  Control.addVariable("TopPreModHeight4", 0.0);
  Control.addVariable("TopPreModDepth4",  0.0);
  Control.addVariable("TopPreModRadius4", 0.0);
  Control.addVariable("TopPreModMat4","Void");

  Control.Parse("LowPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowPreModFlowGuideNBaffles");
  Control.addVariable("TopPreModFlowGuideNBaffles");
  Control.Parse("LowPreModFlowGuideGapWidth");
  Control.addVariable("TopPreModFlowGuideGapWidth");

  Control.copyVarSet("LowPreModFlowGuide","TopPreModFlowGuide");

  Control.addVariable("LowCapModNLayers",3);
  Control.addVariable("LowCapModHeight0",0.5);
  Control.addVariable("LowCapModDepth0", 0.5);
  Control.addVariable("LowCapModRadius0",32);
  Control.addVariable("LowCapModMat0","H2OAl47");
  Control.addVariable("LowCapModHeight1",0.0);
  Control.addVariable("LowCapModDepth1",0.0);
  Control.addVariable("LowCapModThick1",0.3);
  Control.addVariable("LowCapModMat1","Aluminium");
  Control.addVariable("LowCapModRadius1",6-3+0.3);
  Control.addVariable("LowCapModHeight2",0.0);
  Control.addVariable("LowCapModDepth2",0.3);
  Control.addVariable("LowCapModRadius2",0);
  Control.addVariable("LowCapModMat2","Aluminium");

  Control.addVariable("LowCapModTiltAngle",0.0);

  Control.addVariable("LowCapModFlowGuideWallThick", 0.3);
  Control.addVariable("LowCapModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowCapModFlowGuideNBaffles", 9);
  Control.addVariable("LowCapModFlowGuideGapWidth", 3);


  Control.copyVarSet("LowCapModFlowGuide","TopCapModFlowGuide");

  Control.addVariable("TopCapModNLayers",3);
  Control.addVariable("TopCapModHeight0",0.5);
  Control.addVariable("TopCapModDepth0", 0.5);
  Control.addVariable("TopCapModRadius0",32);
  Control.addVariable("TopCapModMat0","H2OAl47");
  Control.addVariable("TopCapModHeight1",0.0);
  Control.addVariable("TopCapModDepth1",0.0);
  Control.addVariable("TopCapModThick1",0.3);
  Control.addVariable("TopCapModMat1","Aluminium");
  Control.addVariable("TopCapModRadius1",6-3+0.3);
  Control.addVariable("TopCapModHeight2",0.0);
  Control.addVariable("TopCapModDepth2",0.3);
  Control.addVariable("TopCapModRadius2",0.0);
  Control.addVariable("TopCapModMat2","Aluminium");
  Control.addVariable("TopCapModHeight3",0);
  Control.addVariable("TopCapModDepth3", 0.3);
  Control.addVariable("TopCapModRadius3",0.0);
  Control.addVariable("TopCapModMat3", "Aluminium");

  Control.addVariable("TopCapModTiltAngle", 0.0);
  Control.addVariable("TopCapModTiltRadius", 32.17);

    // wings
  Control.addVariable("TopPreWingMat", "H2OAl47");
  Control.addVariable("TopPreWingThick", 0.45); // ESS-0032315.3
  Control.addVariable("TopPreWingWallMat", "Aluminium");
  Control.addVariable("TopPreWingWallThick", 0.3);
  Control.addVariable("TopPreWingTiltAngle", 2.0);
  Control.addVariable("TopPreWingTiltRadius", 39.0/2); // =19.5 ESS-0032315.3

  Control.addVariable("TopCapWingMat", "H2OAl47");
  Control.addVariable("TopCapWingThick", 0.75); // ESS-0032315.3
  Control.addVariable("TopCapWingWallMat", "Aluminium");
  Control.addVariable("TopCapWingWallThick", 0.3);
  Control.addVariable("TopCapWingTiltAngle", 1.33); // ESS-0032315.3
  Control.addVariable("TopCapWingTiltRadius", 39.0/2); // = 19.5  ESS-0032315.3

  Control.addVariable("LowPreWingMat", "H2OAl47");
  Control.addVariable("LowPreWingThick", 0.55);  // ESS-0032315.3
  Control.addVariable("LowPreWingWallMat", "Aluminium");
  Control.addVariable("LowPreWingWallThick", 0.3);
  Control.addVariable("LowPreWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowPreWingTiltRadius", 39.0/2.0);  // = 19.5  ESS-0032315.3

  Control.addVariable("LowCapWingMat", "H2OAl47");
  Control.addVariable("LowCapWingThick", 0.85);  // ESS-0032315.3
  Control.addVariable("LowCapWingWallMat", "Aluminium");
  Control.addVariable("LowCapWingWallThick", 0.3);
  Control.addVariable("LowCapWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowCapWingTiltRadius", 39.0/2.0); // = 19.5  ESS-0032315.3


  return;
}

} // NAMESPACE setVariable
