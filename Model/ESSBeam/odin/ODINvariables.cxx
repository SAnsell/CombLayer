/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODINvariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "ShieldGenerator.h"
#include "PitGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PipeGenerator.h"
#include "BladeGenerator.h"
#include "JawGenerator.h"
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
  setVariable::ShieldGenerator SGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PitGenerator PGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JGen;
  
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  PipeGen.setPipe(12.0,1.0);
  PipeGen.setWindow(13.0,0.3);
  PipeGen.setFlange(16.0,1.0);

  // VACUUM PIPE in Gamma shield
  Control.addVariable("odinStopPoint",0);
  
  PipeGen.generatePipe(Control,"odinPipeB",2.0,46.0);
  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"odinFB",44.0, 3.50,3.5, 4.0,4.0);

  CGen.setMainRadius(26.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"odinChopperA",15.0,21.0,15.55);

  // Quad Blade chopper
  BGen.setThick({0.5,0.5,0.5,0.5});
  BGen.setMaterials("Inconnel","B4C");
  BGen.setGap(3.0);
  BGen.addPhase({0,180},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinQBlade",0.0,10.0,22.50);

  Control.addVariable("odinGA0HeightStart",3.0);
  Control.addVariable("odinGA0HeightEnd",3.0);
  Control.addVariable("odinGA0WidthStart",7.0);
  Control.addVariable("odinGA0WidthEnd",10.0);

  PipeGen.generatePipe(Control,"odinPipeC",2.0,220.0);
  FGen.generateTaper(Control,"odinFC",216.0,7.0,10.0,3.0,3.0);

  CGen.setMainRadius(33.0);
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"odinChopperB",10.0,15.0,9.55);

  // T0 Chopper disk B
  BGen.setThick({3.4});
  BGen.setInnerThick({5.4});
  BGen.setMaterials("Inconnel","Tungsten");
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinT0Disk",2.0,20.0,30.0);


  PipeGen.generatePipe(Control,"odinPipeD",2.0,400.0);
  FGen.generateTaper(Control,"odinFD",396.0,7.0,10.0,3.0,3.0);

  PipeGen.generatePipe(Control,"odinPipeE",2.0,400.0);
  FGen.generateTaper(Control,"odinFE",396.0,7.0,10.0,3.0,3.0);
  
  PipeGen.generatePipe(Control,"odinPipeF",2.0,400.0);
  FGen.generateTaper(Control,"odinFF",396.0,7.0,10.0,3.0,3.0);

  PipeGen.generatePipe(Control,"odinPipeG",2.0,380.0);
  FGen.generateTaper(Control,"odinFG",376.0,7.0,10.0,3.0,3.0);


    // BEAM INSERT:
  Control.addVariable("odinBInsertHeight",20.0);
  Control.addVariable("odinBInsertWidth",28.0);
  Control.addVariable("odinBInsertTopWall",1.0);
  Control.addVariable("odinBInsertLowWall",1.0);
  Control.addVariable("odinBInsertLeftWall",1.0);
  Control.addVariable("odinBInsertRightWall",1.0);
  Control.addVariable("odinBInsertWallMat","Stainless304");       

  // BEAM INSERT:
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"odinFWall",318.0,4.5,4.5);

  // Pit on exit:
  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"odinOutPitA",0.0,40.0,220.0,210.0,40.0);

  Control.addVariable("odinOutACutShape","Circle");
  Control.addVariable("odinOutACutRadius",5.0);

  CGen.setMainRadius(100.0);
  CGen.setFrame(215.0,215.0);
  CGen.generateChopper(Control,"odinChopperOutA",18.0,20.0,14.55);

  // FOC5 single disk chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC5Blade",0.0,70.0,90.0);

  SGen.generateShield(Control,"odinShieldA",945.0,80.0,80.0,80.0,2,8);  
  PipeGen.generatePipe(Control,"odinPipeOutA",2.0,945.0);
  FGen.generateTaper(Control,"odinOutFA",941.0,4.0,4.0,20.0,16.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"odinOutPitB",975.0,40.0,220.0,210.0,40.0);
  
  CGen.setMainRadius(100.0);
  CGen.setFrame(215.0,215.0);
  CGen.generateChopper(Control,"odinChopperOutB",30.0,20.0,14.55);

  // FOC6 single disk chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"odinFOC6Blade",0.0,70.0,90.0);

  Control.addVariable("odinOutBCutFrontShape","Circle");
  Control.addVariable("odinOutBCutFrontRadius",5.0);
  
  Control.addVariable("odinOutBCutBackShape","Circle");
  Control.addVariable("odinOutBCutBackRadius",5.0);

  SGen.generateShield(Control,"odinShieldB",900.0,80.0,80.0,80.0,2,8);  
  PipeGen.generatePipe(Control,"odinPipeOutB",2.0,855.0);
  FGen.generateTaper(Control,"odinOutFB",847.0,4.0,4.0,20.0,16.0);
  
  // HUT:
  Control.addVariable("odinCaveYStep",900.0);
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

  Control.addVariable("odinCaveFeMat","Stainless304");
  Control.addVariable("odinCaveConcMat","Concrete");
   
  // WALL for Hut
  Control.addVariable("odinCaveWallYStep",6.0);
  Control.addVariable("odinCaveWallThick",45.0);
  Control.addVariable("odinCaveWallXGap",6.0);
  Control.addVariable("odinCaveWallZGap",6.0);
  Control.addVariable("odinCaveWallMat","Stainless304");

  // Beam port through front of cave
  Control.addVariable("odinCaveCutShape","Circle");
  Control.addVariable("odinCaveCutRadius",5.0);

  
  PipeGen.generatePipe(Control,"odinPipeCaveA",8.0,46.0);
  FGen.generateTaper(Control,"odinCaveFA",42.0,4.0,4.0,10.0,6.0);
  
  // PIN COLLIMATOR SYSTEM
  Control.addVariable("odinPinLength",50.0);
  Control.addVariable("odinPinRadius",60.0);
  Control.addVariable("odinPinYStep",20.0);
  // Collimator A:
  Control.addVariable("odinPinCollAYStep",1.0);
  Control.addVariable("odinPinCollARotDepth",20.0);
  Control.addVariable("odinPinCollARadius",30.0);
  Control.addVariable("odinPinCollAThick",10.0);

  Control.addVariable("odinPinCollAInnerWall",1.0);
  Control.addVariable("odinPinCollAInnerWallMat","Void");

  Control.addVariable("odinPinCollANLayers",0);
  Control.addVariable("odinPinCollAHoleIndex",0);
  Control.addVariable("odinPinCollAHoleAngOff",0.0);

  Control.addVariable("odinPinCollADefMat","Inconnel");
  // collimator holes:
  Control.addVariable("odinPinCollANHole",3);
  Control.addVariable("odinPinCollAHole0Shape",1);
  Control.addVariable("odinPinCollAHole0Radius",3.0);

  Control.addVariable("odinPinCollAHole0AngleCentre",0.0);
  Control.addVariable("odinPinCollAHole0AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole0RadialStep",20.0);

  Control.addVariable("odinPinCollAHole1Shape",3);
  Control.addVariable("odinPinCollAHole1Radius",5.0);
  Control.addVariable("odinPinCollAHole1AngleCentre",120.0);
  Control.addVariable("odinPinCollAHole1AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole1RadialStep",15.0);
  
  Control.addVariable("odinPinCollAHole2Shape",1);
  Control.addVariable("odinPinCollAHole2Radius",4.0);
  Control.addVariable("odinPinCollAHole2AngleCentre",240.0);
  Control.addVariable("odinPinCollAHole2AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole2RadialStep",20.0);

  // Collimator B:
  Control.addVariable("odinPinCollBYStep",40.0);
  Control.addVariable("odinPinCollBRotDepth",20.0);
  Control.addVariable("odinPinCollBRadius",30.0);
  Control.addVariable("odinPinCollBThick",10.0);

  Control.addVariable("odinPinCollBInnerWall",1.0);
  Control.addVariable("odinPinCollBInnerWallMat","Void");

  Control.addVariable("odinPinCollBNLayers",0);
  Control.addVariable("odinPinCollBHoleIndex",0);
  Control.addVariable("odinPinCollBHoleAngOff",0.0);

  Control.addVariable("odinPinCollBDefMat","Tungsten");
  // collimator holes:
  Control.addVariable("odinPinCollBNHole",3);
  Control.addVariable("odinPinCollBHole0Shape",1);
  Control.addVariable("odinPinCollBHole0Radius",3.0);

  Control.addVariable("odinPinCollBHole0AngleCentre",0.0);
  Control.addVariable("odinPinCollBHole0AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole0RadialStep",20.0);

  Control.addVariable("odinPinCollBHole1Shape",3);
  Control.addVariable("odinPinCollBHole1Radius",5.0);
  Control.addVariable("odinPinCollBHole1AngleCentre",120.0);
  Control.addVariable("odinPinCollBHole1AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole1RadialStep",15.0);
  
  Control.addVariable("odinPinCollBHole2Shape",1);
  Control.addVariable("odinPinCollBHole2Radius",4.0);
  Control.addVariable("odinPinCollBHole2AngleCentre",240.0);
  Control.addVariable("odinPinCollBHole2AngleOffset",0.0);
  Control.addVariable("odinPinCollBHole2RadialStep",20.0);

  //
  // JAWS
  //
  JGen.generateJaws(Control,"odinPinJaw",22.0);


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
  
  Control.addVariable("odinBeamStopFeMat","Stainless304");
  Control.addVariable("odinBeamStopConcMat","Concrete");

  return;
}

}  // NAMESPACE setVariable
