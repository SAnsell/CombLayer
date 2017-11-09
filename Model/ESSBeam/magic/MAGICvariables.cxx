/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/magic/MAGICvariables.cxx
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
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"


namespace setVariable
{
  
void
MAGICvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("MAGICvariables[F]","MAGICvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,25.0,"CastIron");
  SGen.addRoof(1,25.0,"CastIron");
  SGen.addFloor(1,25.0,"Concrete");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("magicStopPoint",0);
  Control.addVariable("magicAxisXYAngle",0.3);   // rotation
  Control.addVariable("magicAxisZAngle",0.46);   // rotation
  Control.addVariable("magicAxisZStep",0.0);   // rotation

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.4,"Void");
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"magicFA",350.0,12.4,3.0 ,4.0,3.0);

  PipeGen.generatePipe(Control,"magicPipeB",8.0,44.0);

  // out to 6.18m
  PipeGen.generatePipe(Control,"magicPipeC",10.0,75.0);
  

  FGen.setLayer(1,0.5,"SiCrystal");
  FGen.clearYOffset();
  FGen.generateBender(Control,"magicBC",15.0, 3.0,3.0,3.0,3.0,
                      6500,0.0);

  CGen.setMainRadius(25.0);
  CGen.setFrame(65.0,65.0);
  CGen.generateChopper(Control,"magicChopperA",38.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"magicPSCBlade",0.0,18.0,22.5);


  // out from ChopA (6.5m) to 12m
  PipeGen.generatePipe(Control,"magicPipeD",3.0,650.0);

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"magicFD",646.0 ,2.7,4.08  ,2.4,4.06 );

  // out (12m to 19m)
  PipeGen.generatePipe(Control,"magicPipeE",3.0,600.0);
  FGen.generateTaper(Control,"magicFE",596.0 ,4.08,4.93, 4.06,4.79 );
  // out (19m to bunker wall)
  PipeGen.generatePipe(Control,"magicPipeF",3.0,490.0);
  FGen.generateTaper(Control,"magicFF",486.0 ,4.93,5.60,  4.79,5.50 );

    // BEAM INSERT:
  Control.addVariable("magicBInsertHeight",20.0);
  Control.addVariable("magicBInsertWidth",28.0);
  Control.addVariable("magicBInsertTopWall",1.0);
  Control.addVariable("magicBInsertLowWall",1.0);
  Control.addVariable("magicBInsertLeftWall",1.0);
  Control.addVariable("magicBInsertRightWall",1.0);
  Control.addVariable("magicBInsertWallMat","Stainless304");       
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"magicPipeWall",4.0,348.0);
  // Guide in wall
  FGen.generateTaper(Control,"magicFWall",346.0 ,5.60,5.89, 5.50,5.80 );
  
  // 28m to 38m
  SGen.generateShield(Control,"magicShieldA",1000.0,40.0,40.0,60.0,4,8);
  PipeGen.generatePipe(Control,"magicPipeOutA",5.0,994.0);
  FGen.clearYOffset();
  FGen.setYMainOffset(8.0);  
  FGen.generateTaper(Control,"magicOutFA",990.0, 5.89,6.65, 5.80,6.60 );

  // 38m to 48m
  FGen.setYMainOffset(4.0);  
  SGen.generateShield(Control,"magicShieldB",1000.0,40.0,40.0,60.0,4,8);
  PipeGen.generatePipe(Control,"magicPipeOutB",4.0,996.0);
  FGen.generateTaper(Control,"magicOutFB",992.0, 6.65,7.20, 6.60,7.18 );
 
  // 48m to 58m
  SGen.generateShield(Control,"magicShieldC",1000.0,40.0,40.0,60.0,4,8);
  PipeGen.generatePipe(Control,"magicPipeOutC",6.0,996.0);
  FGen.generateTaper(Control,"magicOutFC",992.0, 7.20,7.60, 7.18,7.58 );

  // 58m to 68m
  SGen.generateShield(Control,"magicShieldD",1000.0,40.0,40.0,60.0,4,8);
  PipeGen.generatePipe(Control,"magicPipeOutD",6.0,996.0);
  FGen.generateTaper(Control,"magicOutFD",992.0, 7.20,7.60, 7.18,7.58 );

  // 68m to 78m
  SGen.generateShield(Control,"magicShieldE",1000.0,40.0,40.0,60.0,4,8);
  PipeGen.generatePipe(Control,"magicPipeOutE",6.0,996.0);
  FGen.generateTaper(Control,"magicOutFE",992.0, 7.20,7.60, 7.18,7.58 );
  
  // 78 to 83m
  SGen.generateShield(Control,"magicShieldF",590.0,40.0,40.0,60.0,2,8);
  PipeGen.generatePipe(Control,"magicPipeOutF",6.0,586.0);
  FGen.generateTaper(Control,"magicOutFF",582.0, 7.20,7.60, 7.18,7.58 );

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"magicPolarizerPit",0.0,340.0,150.0,120.0,30.0);

  // 78 to 83m
  SGen.generateShield(Control,"magicShieldG",1000.0,40.0,40.0,60.0,4,8);
  Control.addVariable("magicShieldGYStep",300.0);
  PipeGen.generatePipe(Control,"magicPipeOutG",5.0,994.0);
  FGen.generateTaper(Control,"magicOutFG",990.0, 7.20,7.60, 7.18,7.58 );


  FGen.setYOffset(-160.0);
  FGen.generateRectangle(Control,"magicMCGuideA",150.0, 7.20,7.60);
  Control.addVariable("magicMCGuideAZAngle",-0.25);

  FGen.setYOffset(10.0);
  FGen.generateRectangle(Control,"magicMCGuideB",150.0, 7.20,7.60);
  Control.addVariable("magicMCGuideBZAngle",-0.125);
   
  Control.addVariable("magicMCANBlades",6);
  Control.addVariable("magicMCABladeThick",0.2);
  Control.addVariable("magicMCALength",148.0);
  Control.addParse<double>("magicMCAYStep","magicMCALength/2.0");
    
  Control.addVariable("magicMCABladeMat","SiCrystal");
  Control.addVariable("magicMCAVoidMat","Void");


  Control.addVariable("magicMCBNBlades",6);
  Control.addVariable("magicMCBBladeThick",0.2);
  Control.addVariable("magicMCBLength",125.0);
  Control.addParse<double>("magicMCBYStep","magicMCBLength/2.0");
    
  Control.addVariable("magicMCBBladeMat","SiCrystal");
  Control.addVariable("magicMCBVoidMat","Void");

  Control.addVariable("magicAppAInnerWidth",4.0);
  Control.addVariable("magicAppAInnerHeight",4.0);
  Control.addVariable("magicAppAWidth",12.0);
  Control.addVariable("magicAppAHeight",12.0);
  Control.addVariable("magicAppAThick",5.0);
  Control.addVariable("magicAppAYStep",7.0);
  Control.addVariable("magicAppADefMat","Tungsten");

  return;
}
 
}  // NAMESPACE setVariable
