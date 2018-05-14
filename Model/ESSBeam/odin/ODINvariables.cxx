/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODINvariables.cxx
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
#include "LayerGenerator.h"
#include "PitGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PipeGenerator.h"
#include "BladeGenerator.h"
#include "TwinBaseGenerator.h"
#include "TwinGenerator.h"
#include "JawGenerator.h"
#include "RotaryHoleGenerator.h"
#include "variableSetup.h"

namespace setVariable
{

void
ODINvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("ODINvariables[F]","ODINvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::LayerGenerator LGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PitGenerator PGen;
  setVariable::BladeGenerator BGen;
  setVariable::TwinGenerator TGen;
  setVariable::JawGenerator JGen;
  setVariable::RotaryHoleGenerator RotGen;

  LGen.setWall(30.0,{25.0,35.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setRoof(30.0,{25.0,35.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setFloor(30.0,{25.0,35.0}, {4,4}, {"CastIron","Concrete"});
 
  PipeGen.setPipe(12.0,1.0);
  PipeGen.setWindow(13.0,0.3);
  PipeGen.setFlange(16.0,1.0);

  // VACUUM PIPE in Gamma shield
  Control.addVariable("odinStopPoint",0);

  FGen.setLayer(1,0.8,"Copper");
  FGen.setLayer(2,0.3,"Void");
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"odinFA",350.0, 4.38,3.70,  3.3,5.26 );

  PipeGen.generatePipe(Control,"odinPipeB",8.0,46.0);
  FGen.setLayer(1,0.8,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"odinFB",44.0, 1.806,1.60, 5.24,6.78);

  // Gamma shutter to first Chopper
  PipeGen.generatePipe(Control,"odinPipeC",2.0,40.0);
  FGen.generateTaper(Control,"odinFC",37.0, 1.606,1.5, 6.78,6.91);

  //CGen.setMainRadius(26.0);
  //  CGen.setFrame(60.0,60.0);

  CGen.setMainRadius(37.0);
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"odinChopperAA",10.0,8.0,6.0);
  CGen.generateChopper(Control,"odinChopperAB",40.0,8.0,6.0);

  // Single Blade chopper x4
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinDiskAA",0.0,25.0,35.0);
  BGen.generateBlades(Control,"odinDiskAB",0.0,25.0,35.0);

  // First Chopper pair to T0
  PipeGen.generatePipe(Control,"odinPipeD",2.0,110.0);
  FGen.generateTaper(Control,"odinFD",107.0, 5.50,3.5, 4.0,4.0);

  CGen.setMainRadius(33.0);
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"odinChopperB",18.0,30.0,26.0);

  // T0 Chopper disk B
  BGen.setThick({10.0,10.0});
  BGen.setGap(5.0);
  BGen.setInnerThick({10.0,10.0});
  BGen.setMaterials("Inconnel","Tungsten");
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinT0Disk",0.0,20.0,30.0);

  CGen.setMainRadius(45.0);
  CGen.setFrame(100.0,100.0);
  CGen.generateChopper(Control,"odinChopperFOC1",10.0,10.0,6);

  // FOC5 single disk chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC1Blade",0.0,30.0,40.0);
  
  PipeGen.generatePipe(Control,"odinPipeE",2.0,310.0);
  FGen.generateTaper(Control,"odinFE",306.0,7.0,10.0,3.0,3.0);
  
  CGen.setMainRadius(55.0);
  CGen.setFrame(125.0,125.0);
  CGen.generateChopper(Control,"odinChopperFOC2",10.0,10.0,6);

  // FOC2 single disk chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC2Blade",0.0,40.0,50.0);

  
  PipeGen.generatePipe(Control,"odinPipeF",2.0,449.0);
  FGen.generateTaper(Control,"odinFF",445.0,7.0,10.0,3.0,3.0);

  // FOC3 single disk chopper
  CGen.setMainRadius(65.0);
  CGen.setFrame(165.0,165.0);
  CGen.generateChopper(Control,"odinChopperFOC3",9.0,10.0,6);

  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC3Blade",0.0,50.0,60.0);

  PipeGen.generatePipe(Control,"odinPipeG",2.0,634.0);
  FGen.generateTaper(Control,"odinFG",630.0,7.0,10.0,3.0,3.0);

  // FOC4 single disk chopper
  CGen.setMainRadius(65.0);
  CGen.setFrame(165.0,165.0);
  CGen.generateChopper(Control,"odinChopperFOC4",9.0,10.0,6);

  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC4Blade",0.0,50.0,60.0);

  PipeGen.generatePipe(Control,"odinPipeH",2.0,140.0);
  FGen.generateTaper(Control,"odinFH",136.0,7.0,10.0,3.0,3.0);

    // BEAM INSERT:
  Control.addVariable("odinBInsertHeight",20.0);
  Control.addVariable("odinBInsertWidth",28.0);
  Control.addVariable("odinBInsertTopWall",1.0);
  Control.addVariable("odinBInsertLowWall",1.0);
  Control.addVariable("odinBInsertLeftWall",1.0);
  Control.addVariable("odinBInsertRightWall",1.0);
  Control.addVariable("odinBInsertWallMat","CastIron");       

  // BEAM INSERT:
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"odinFWall",346.0,4.5,4.5);

  // Pit on exit:
  PGen.setFeLayer(30.0);
  PGen.setConcLayer(60.0);
  PGen.generatePit(Control,"odinOutPitA",0.0,150.0,480.0,210.0,40.0);

  Control.addVariable("odinOutACutShape","Circle");
  Control.addVariable("odinOutACutRadius",5.0);
  Control.addVariable("odinOutBCutShape","Circle");
  Control.addVariable("odinOutBCutRadius",5.0);

  LGen.generateLayer(Control,"odinShieldA",200.0,1);

  // FOC5 single disk chopper
  CGen.setMainRadius(100.0);
  CGen.setFrame(215.0,215.0);
  CGen.generateChopper(Control,"odinChopOutFOC5",46.0,10.0,6.55);

  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC5Blade",0.0,70.0,90.0);
  
  PipeGen.setPipe(16.0,1.0);
  PipeGen.setWindow(17.5,0.3);
  PipeGen.setFlange(20.0,1.0);
  PipeGen.generatePipe(Control,"odinPipeOutA",2.2,197.0);
  FGen.generateTaper(Control,"odinOutFA",194.0,4.0,4.0,20.0,16.0);

  Control.addVariable("odinOutBCutFrontShape","Circle");
  Control.addVariable("odinOutBCutFrontRadius",5.0);
  
  Control.addVariable("odinOutBCutBackShape","Circle");
  Control.addVariable("odinOutBCutBackRadius",5.0);

  LGen.generateLayer(Control,"odinShieldB",2000.0,4);  
  PipeGen.generatePipe(Control,"odinPipeOutB",164.0,1880.0);
  FGen.generateTaper(Control,"odinOutFB",1876.0,4.0,4.0,20.0,16.0);

  // HUT:
  Control.addVariable("odinCaveYStep",0.0);
  Control.addVariable("odinCaveVoidHeight",300.0);
  Control.addVariable("odinCaveVoidDepth",183.0);
  Control.addVariable("odinCaveVoidWidth",600.0);
  Control.addVariable("odinCaveVoidLength",1686.0);
  Control.addVariable("odinCaveVoidNoseWidth",293.0);
  Control.addVariable("odinCaveVoidNoseLen",183.0);

  Control.addVariable("odinCaveFeLeftWall",15.0);
  Control.addVariable("odinCaveFeRightWall",15.0);
  Control.addVariable("odinCaveFeRoof",15.0);
  Control.addVariable("odinCaveFeFloor",15.0);
  Control.addVariable("odinCaveFeNoseFront",25.0);
  Control.addVariable("odinCaveFeNoseSide",25.0);
  Control.addVariable("odinCaveFeBack",15.0);

  Control.addVariable("odinCaveConcLeftWall",35.0);
  Control.addVariable("odinCaveConcRightWall",35.0);
  Control.addVariable("odinCaveConcRoof",35.0);
  Control.addVariable("odinCaveConcFloor",50.0);
  Control.addVariable("odinCaveConcNoseFront",35.0);
  Control.addVariable("odinCaveConcNoseSide",35.0);
  Control.addVariable("odinCaveConcBack",35.0);

  Control.addVariable("odinCaveFeMat","CastIron");
  Control.addVariable("odinCaveConcMat","Concrete");
   
  // WALL for Hut
  Control.addVariable("odinCaveWallYStep",6.0);
  Control.addVariable("odinCaveWallThick",45.0);
  Control.addVariable("odinCaveWallXGap",6.0);
  Control.addVariable("odinCaveWallZGap",6.0);
  Control.addVariable("odinCaveWallMat","CastIron");

  // Beam port through front of cave
  Control.addVariable("odinCaveCutShape","Circle");
  Control.addVariable("odinCaveCutRadius",5.0);
  // Beam port through midlayer of cave
  Control.addVariable("odinCaveMidCutShape","Circle");
  Control.addVariable("odinCaveMidCutRadius",8.0);

  
  PipeGen.generatePipe(Control,"odinPipeCaveA",8.0,46.0);
  FGen.generateTaper(Control,"odinCaveFA",42.0,4.0,4.0,10.0,6.0);
  
  // PIN COLLIMATOR SYSTEM
  Control.addVariable("odinPinLength",50.0);
  Control.addVariable("odinPinRadius",60.0);
  Control.addVariable("odinPinYStep",30.0);
  // Collimator A:
  RotGen.setMain(30.0,10.0);
  RotGen.addHole("Circle", 3.0, 0.0,   0.0, 20.0);
  RotGen.addHole("Hexagon",5.0, 0.0, 120.0, 15.0);
  RotGen.addHole("Circle", 4.0, 0.0, 240.0, 20.0);
  RotGen.generatePinHole(Control,"odinPinCollA",-23.0,20.0,0.0);

  // Collimator B:
  RotGen.setMain(30.0,10.0);
  RotGen.generatePinHole(Control,"odinPinCollB",13.0,20.0,0.0);

  //
  // JAWS
  //
  JGen.generateJaws(Control,"odinPinJaw",0.0);


  Control.addVariable("odinBeamStopYStep",1200.0);
  
  Control.addVariable("odinBeamStopWidth",360.0);
  Control.addVariable("odinBeamStopDepth",180.0);
  Control.addVariable("odinBeamStopHeight",180.0);
  Control.addVariable("odinBeamStopLength",350.0);

  Control.addVariable("odinBeamStopFeRadius",25.0);
  Control.addVariable("odinBeamStopFeLength",72.0);
  
  Control.addVariable("odinBeamStopOuterRadius",75.0);
  Control.addVariable("odinBeamStopOuterFeRadius",20.0);
  Control.addVariable("odinBeamStopOuterFeStep",25.0);
  Control.addVariable("odinBeamStopOuterCut",50.0);

  Control.addVariable("odinBeamStopInnerRadius",50.0);
  Control.addVariable("odinBeamStopInnerFeRadius",20.0);
  Control.addVariable("odinBeamStopInnerFeStep",35.0);
  Control.addVariable("odinBeamStopInnerCut",70.0);
  
  Control.addVariable("odinBeamStopFeMat","CastIron");
  Control.addVariable("odinBeamStopConcMat","Concrete");

  return;
}

}  // NAMESPACE setVariable
