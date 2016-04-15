/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/targetVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Konstantin Batkov
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
  Control.addVariable("LowFlyTotalHeight",8.1); // to center it at -15.3 as in ESS-0032315.3 (mind 4 mm cold Al thick)
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

  Control.addVariable("LowFlyLeftLobeModMat","HPARA"); // email from LZ 27 Nov 2015
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

  Control.addVariable("LowFlyRightLobeModMat","HPARA"); // email from LZ 27 Nov 2015
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
 

  Control.addVariable("LowFlyMidWaterWallThick",0.2);
  Control.addVariable("LowFlyMidWaterModMat","H2O");
  Control.addVariable("LowFlyMidWaterWallMat","Aluminium");
  Control.addVariable("LowFlyMidWaterModTemp",300.0);

  Control.addVariable("LowFlyLeftWaterWidth",10.6672);
  Control.addVariable("LowFlyLeftWaterWallThick",0.347);
  Control.addVariable("LowFlyLeftWaterSideWaterThick",2.2);
  Control.addVariable("LowFlyLeftWaterSideWaterMat","H2O");
  Control.addVariable("LowFlyLeftWaterSideWaterCutAngle",60.0);
  Control.addVariable("LowFlyLeftWaterSideWaterCutDist",0.0);
  Control.addVariable("LowFlyLeftWaterModMat","Void");
  Control.addVariable("LowFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("LowFlyLeftWaterModTemp",300.0);

  Control.addVariable("LowFlyRightWaterWidth",10.6672);
  Control.addVariable("LowFlyRightWaterWallThick",0.347);
  Control.addVariable("LowFlyRightWaterSideWaterThick",2.2);
  Control.addVariable("LowFlyRightWaterSideWaterMat","H2O");
  Control.addVariable("LowFlyRightWaterSideWaterCutAngle",60.0);
  Control.addVariable("LowFlyRightWaterSideWaterCutDist",0.0);
  Control.addVariable("LowFlyRightWaterModMat","Void");
  Control.addVariable("LowFlyRightWaterWallMat","Aluminium");
  Control.addVariable("LowFlyRightWaterModTemp",300.0);

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

  Control.addVariable("TopFlyLeftLobeModMat","HPARA"); // email from LZ 27 Nov 2015; actually it already contains 0.5% ortho-H (see material definition in essDBMaterials)
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

  Control.addVariable("TopFlyLeftLobeHeight3",0.0);
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

  Control.addVariable("TopFlyRightLobeModMat","HPARA"); // email from LZ 27 Nov 2015; actually it already contains 0.5% ortho-H (see material definition in essDBMaterials)
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

  Control.addVariable("TopFlyRightLobeHeight3",0.0);
  Control.addVariable("TopFlyRightLobeDepth3",0.0);


  Control.addVariable("TopFlyMidWaterCutLayer",3);
  Control.addVariable("TopFlyMidWaterMidYStep",4.635);
  Control.addVariable("TopFlyMidWaterMidAngle",90);
  Control.addVariable("TopFlyMidWaterLength",11.4);
 

  Control.addVariable("TopFlyMidWaterWallThick",0.2);
  Control.addVariable("TopFlyMidWaterModMat","H2O");
  Control.addVariable("TopFlyMidWaterWallMat","Aluminium");
  Control.addVariable("TopFlyMidWaterModTemp",300.0);

  Control.addVariable("TopFlyLeftWaterWidth",10.6672);
  Control.addVariable("TopFlyLeftWaterWallThick",0.347);
  Control.addVariable("TopFlyLeftWaterSideWaterThick",2.2);
  Control.addVariable("TopFlyLeftWaterSideWaterMat","H2O");
  Control.addVariable("TopFlyLeftWaterSideWaterCutAngle",60.0);
  Control.addVariable("TopFlyLeftWaterSideWaterCutDist",0.0);
  Control.addVariable("TopFlyLeftWaterModMat","Void");
  Control.addVariable("TopFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("TopFlyLeftWaterModTemp",300.0);

  Control.addVariable("TopFlyRightWaterWidth",10.6672);
  Control.addVariable("TopFlyRightWaterWallThick",0.347);
  Control.addVariable("TopFlyRightWaterSideWaterThick",2.2);
  Control.addVariable("TopFlyRightWaterSideWaterMat","H2O");
  Control.addVariable("TopFlyRightWaterSideWaterCutAngle",60.0);
  Control.addVariable("TopFlyRightWaterSideWaterCutDist",0.0);
  Control.addVariable("TopFlyRightWaterModMat","Void");
  Control.addVariable("TopFlyRightWaterWallMat","Aluminium");
  Control.addVariable("TopFlyRightWaterModTemp",300.0);

  Control.addVariable("LowPreModNLayers",4);
  Control.addVariable("LowPreModHeight0",1.5);
  Control.addVariable("LowPreModDepth0",1.5);
  Control.addVariable("LowPreModRadius0",30.0);
  Control.addVariable("LowPreModMat0","H2OAl47"); // email from LZ 15 Jan 2016
  Control.addVariable("LowPreModHeight1",0);
  Control.addVariable("LowPreModDepth1",0);
  Control.addVariable("LowPreModRadius1",0.3);
  Control.addVariable("LowPreModMat1","Aluminium");
  Control.addVariable("LowPreModHeight2",0);
  Control.addVariable("LowPreModDepth2",0);
  Control.Parse("BeRefRadius-LowPreModRadius0-LowPreModRadius1+BeRefWallThick-0.0"); // 0.0 is LowPreModRadius3
  Control.addVariable("LowPreModRadius2");
  Control.addVariable("LowPreModMat2","Iron10H2O");

  Control.addVariable("LowPreModHeight3",0.3);
  Control.addVariable("LowPreModDepth3",0.3);
  Control.addVariable("LowPreModRadius3", 0.0);
  Control.addVariable("LowPreModMat3","Aluminium");
  //  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowPreModTiltAngle", 0.0);

  Control.addVariable("LowPreModFlowGuideWallThick", 0.3);
  Control.addVariable("LowPreModFlowGuideWallMat","Aluminium");
  Control.addVariable("LowPreModFlowGuideNBaffles", 9);
  Control.addVariable("LowPreModFlowGuideGapWidth", 3);

  Control.addVariable("TopPreModNLayers",4);
  Control.addVariable("TopPreModHeight0",1.5);
  Control.addVariable("TopPreModDepth0",1.5);
  Control.addVariable("TopPreModRadius0",30.0);
  Control.addVariable("TopPreModMat0","H2OAl47"); // email from LZ 15 Jan 2016
  Control.addVariable("TopPreModHeight1",0);
  Control.addVariable("TopPreModDepth1",0);
  Control.addVariable("TopPreModRadius1",0.3);
  Control.addVariable("TopPreModMat1","Aluminium");
  Control.addVariable("TopPreModHeight2",0);
  Control.addVariable("TopPreModDepth2",0);
  Control.Parse("BeRefRadius-TopPreModRadius0-TopPreModRadius1+BeRefWallThick-0.0"); // 0.0 is TopPreModRadius3
  Control.addVariable("TopPreModRadius2");
  Control.addVariable("TopPreModMat2","Iron10H2O");
  Control.addVariable("TopPreModHeight3",0.3);
  Control.addVariable("TopPreModDepth3",0.3);
  Control.addVariable("TopPreModRadius3",0.0);
  Control.addVariable("TopPreModMat3","Aluminium");
  //  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopPreModTiltAngle", 2.0);
  Control.addVariable("TopPreModTiltRadius", 32.17);

  Control.addVariable("TopPreWingMat", "H2OAl47");
  Control.addVariable("TopPreWingThick", 0.45); // ESS-0032315.3
  Control.addVariable("TopPreWingWallMat", "Aluminium");
  Control.addVariable("TopPreWingWallThick", 0.3);
  Control.addVariable("TopPreWingTiltAngle", 2.0);
  Control.addVariable("TopPreWingTiltRadius", 39.0/2); // =19.5 ESS-0032315.3


  Control.addVariable("LowPreWingMat", "H2OAl47");
  Control.addVariable("LowPreWingThick", 0.55); // ESS-0032315.3
  Control.addVariable("LowPreWingWallMat", "Aluminium");
  Control.addVariable("LowPreWingWallThick", 0.3);
  Control.addVariable("LowPreWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowPreWingTiltRadius", 39.0/2.0);  // = 19.5  ESS-0032315.3


  Control.addVariable("TopCapWingMat", "H2OAl47");
  Control.addVariable("TopCapWingThick", 0.75); // ESS-0032315.3
  Control.addVariable("TopCapWingWallMat", "Aluminium");
  Control.addVariable("TopCapWingWallThick", 0.3);
  Control.addVariable("TopCapWingTiltAngle", 1.33); // ESS-0032315.3
  Control.addVariable("TopCapWingTiltRadius", 39.0/2); // = 19.5  ESS-0032315.3

  Control.addVariable("LowCapWingMat", "H2OAl47");
  Control.addVariable("LowCapWingThick", 0.85);  // ESS-0032315.3
  Control.addVariable("LowCapWingWallMat", "Aluminium");
  Control.addVariable("LowCapWingWallThick", 0.3);
  Control.addVariable("LowCapWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowCapWingTiltRadius", 39.0/2.0); // = 19.5  ESS-0032315.3

  Control.Parse("LowPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowPreModFlowGuideNBaffles");
  Control.addVariable("TopPreModFlowGuideNBaffles");
  Control.Parse("LowPreModFlowGuideGapWidth");
  Control.addVariable("TopPreModFlowGuideGapWidth");

  Control.addVariable("LowCapModNLayers",3);
  Control.addVariable("LowCapModHeight0",0.5);
  Control.addVariable("LowCapModDepth0", 0.5);
  Control.addVariable("LowCapModRadius0",35-3);
  Control.addVariable("LowCapModMat0","H2OAl47");
  Control.addVariable("LowCapModRadius1", 3.3);
  Control.addVariable("LowCapModHeight1",0.0);
  Control.addVariable("LowCapModDepth1",0.0);
  Control.Parse("BeRefWallThick+3");
  Control.addVariable("LowCapModRadius1");
  Control.addVariable("LowCapModMat1","SS316L");
  Control.addVariable("LowCapModHeight2",0.0);
  Control.addVariable("LowCapModDepth2",0.3);
  Control.addVariable("LowCapModRadius2",0.0);
  Control.addVariable("LowCapModMat2","Aluminium");
  Control.addVariable("LowCapModTiltAngle", 0.0);

  Control.addVariable("LowCapModFlowGuideWallThick", 0.3);
  Control.addVariable("LowCapModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowCapModFlowGuideNBaffles", 9);
  Control.addVariable("LowCapModFlowGuideGapWidth", 3);

  Control.addVariable("TopCapModNLayers",3);
  Control.addVariable("TopCapModHeight0",0.5);
  Control.addVariable("TopCapModDepth0", 0.5);
  Control.addVariable("TopCapModRadius0",35-3); // 3 is being used in TopCapModRadius1
  Control.addVariable("TopCapModMat0","H2OAl47");
  Control.addVariable("TopCapModHeight1",0.0);
  Control.addVariable("TopCapModDepth1",0.0);
  Control.Parse("BeRefWallThick+3");
  Control.addVariable("TopCapModRadius1");
  Control.addVariable("TopCapModMat1","SS316L");
  Control.addVariable("TopCapModHeight2",0.0);
  Control.addVariable("TopCapModDepth2",0.3);
  Control.addVariable("TopCapModRadius2", 0);
  Control.addVariable("TopCapModMat2","Aluminium");
  Control.addVariable("TopCapModTiltAngle", 0.0);
  Control.Parse("TopPreModTiltRadius");
  Control.addVariable("TopCapModTiltRadius");

  Control.Parse("LowCapModFlowGuideWallThick");
  Control.addVariable("TopCapModFlowGuideWallThick");
  Control.addVariable("TopCapModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowCapModFlowGuideNBaffles");
  Control.addVariable("TopCapModFlowGuideNBaffles");
  Control.Parse("LowCapModFlowGuideGapWidth");
  Control.addVariable("TopCapModFlowGuideGapWidth");

 
  return;
}

} // NAMESPACE setVariable
