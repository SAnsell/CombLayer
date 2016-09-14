/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKIvariables.cxx
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
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "RotaryHoleGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
LOKIvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for loki
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("LOKIvariables[F]","LOKIvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::JawGenerator JawGen;
  setVariable::BladeGenerator BGen;
  setVariable::RotaryHoleGenerator RotGen;
  
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);
  
  Control.addVariable("lokiAxisXYAngle",1.0);   // rotation 
  Control.addVariable("lokiAxisZAngle",0.0);

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"lokiBA",350.0,3.0,3.0,3.0,3.0,5700.0,0.0);

  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"lokiPipeB",2.0,46.0);
  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateBender(Control,"lokiBB",44.0, 3.0,3.0,3.0,3.0,5700.0,0.0);


  CGen.setMainRadius(25.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"lokiChopperA",10.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.5,0.5});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiDBladeA",0.0,10.0,22.50);

 
  // Guide from First chopper to 7.5m
  PipeGen.generatePipe(Control,"lokiPipeC",2.0,140.0);
  FGen.generateRectangle(Control,"lokiFC",136.0,3.0,3.0);

  // Bender [reverse bend]
  PipeGen.generatePipe(Control,"lokiPipeD",2.0,216.0);
  FGen.generateBender(Control,"lokiBD",210.0,3.0,3.0,3.0,3.0,3700.0,180.0);

  // Disk chopper 9.7m
  CGen.setMainRadius(25.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"lokiChopperB",10.0,10.0,4.55);

  // Single Blade chopper
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeB",0.0,10.0,22.50);

  // Guide to third chpper 
  PipeGen.generatePipe(Control,"lokiPipeE",6.0,105.0);
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"lokiFE",101.0,3.0,3.0);

  // Disk chopper 9.7m
  CGen.setMainRadius(25.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"lokiChopperC",10.0,10.0,4.55);

  // Single Blade chopper
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeC",0.0,10.0,22.50);

    // Guide to bunker wall / Grid collimator 
  PipeGen.generatePipe(Control,"lokiPipeF",6.0,35.0);
  FGen.generateRectangle(Control,"lokiFF",31.0,3.0,3.0);

  PipeGen.setPipe(5.0,0.5);
  PipeGen.setWindow(-1.0,0.3);
  PipeGen.setFlange(-1.0,1.0);

  PipeGen.generatePipe(Control,"lokiPipeCollA",6.0,135.0);
  PipeGen.generatePipe(Control,"lokiPipeCollAX",6.0,135.0);
  PipeGen.generatePipe(Control,"lokiPipeCollB",6.0,135.0);
  PipeGen.generatePipe(Control,"lokiPipeCollC",6.0,135.0);
  // Small :: Grid Collimator A:
  RotGen.setMain(25.0,1.0);
  RotGen.setWall(1.0,"Aluminium");
  RotGen.addHole("Circle",   3.0,0.0,    0.0, 15.0);
  RotGen.addHole("Rectangle",5.0,5.0,  120.0, 15.0);
  RotGen.addHole("Circle",   4.0,0.0,  240.0, 15.0);
  RotGen.generatePinHole(Control,"lokiGridA",5.0,-15.0,180.0);
  RotGen.generatePinHole(Control,"lokiGridB",2.0,-15.0,180.0);
  RotGen.generatePinHole(Control,"lokiGridC",2.0,-15.0,180.0);
  RotGen.generatePinHole(Control,"lokiGridD",2.0,-15.0,180.0);

  RotGen.setMain(30.0,300.0);
  RotGen.resetHoles();
  RotGen.addHole("Rectangle",   8.0,8.0,    0.0, 17.50);
  RotGen.addHole("Rectangle",   8.0,8.0,  120.0, 17.50);
  RotGen.addHole("Rectangle",   8.0,8.0,  240.0, 17.50);
  RotGen.generatePinHole(Control,"lokiCollA",1.0,-17.5,180.0);
  RotGen.generatePinHole(Control,"lokiCollB",5.0,-17.5,180.0);
  RotGen.generatePinHole(Control,"lokiCollC",5.0,-17.5,180.0);

  Control.addVariable("lokiCBoxBYStep",2.0);
  Control.addVariable("lokiCBoxBHeight",80.0);
  Control.addVariable("lokiCBoxBWidth",80.0);
  Control.addVariable("lokiCBoxBDepth",12.0);   // THIS is half BYStep 
  Control.addVariable("lokiCBoxBDefMat","Void");

  // first guide in loki collimator
  FGen.generateRectangle(Control,"lokiFCA0",296.0,3.0,3.0);

  // HUT:
  Control.addVariable("lokiCaveYStep",0.0);
  Control.addVariable("lokiCaveVoidFront",60.0);
  Control.addVariable("lokiCaveVoidHeight",300.0);
  Control.addVariable("lokiCaveVoidDepth",183.0);
  Control.addVariable("lokiCaveVoidWidth",400.0);
  Control.addVariable("lokiCaveVoidLength",1600.0);


  Control.addVariable("lokiCaveFeFront",25.0);
  Control.addVariable("lokiCaveFeLeftWall",15.0);
  Control.addVariable("lokiCaveFeRightWall",15.0);
  Control.addVariable("lokiCaveFeRoof",15.0);
  Control.addVariable("lokiCaveFeFloor",15.0);
  Control.addVariable("lokiCaveFeBack",15.0);

  Control.addVariable("lokiCaveConcFront",35.0);
  Control.addVariable("lokiCaveConcLeftWall",35.0);
  Control.addVariable("lokiCaveConcRightWall",35.0);
  Control.addVariable("lokiCaveConcRoof",35.0);
  Control.addVariable("lokiCaveConcFloor",50.0);
  Control.addVariable("lokiCaveConcBack",35.0);

  Control.addVariable("lokiCaveFeMat","Stainless304");
  Control.addVariable("lokiCaveConcMat","Concrete");

    // Vacumm tank
  Control.addVariable("lokiVTankXStep",0.0);       
  Control.addVariable("lokiVTankYStep",25.0);
  Control.addVariable("lokiVTankZStep",0.0);       
  Control.addVariable("lokiVTankXYAngle",0.0);       
  Control.addVariable("lokiVTankZAngle",0.0);
  
  Control.addVariable("lokiVTankRadius",144.0);
  Control.addVariable("lokiVTankLength",1048.0);
  Control.addVariable("lokiVTankSideThick",5.0);   // NOT CORRECT
  Control.addVariable("lokiVTankBackThick",5.0);   // NOT CORRECT
  Control.addVariable("lokiVTankFrontThick",1.0);  // NOT CORRECT

  Control.addVariable("lokiVTankWindowInsetLen",106.0);  // NOT CORRECT
  Control.addVariable("lokiVTankWindowThick",1.0);  // NOT CORRECT
  Control.addVariable("lokiVTankWindowRadius",8.0);  // NOT CORRECT

  Control.addVariable("lokiVTankWindowMat","SiCrystal");  // NOT CORRECT
  Control.addVariable("lokiVTankWallMat","Stainless304");

  return;
}

}  // NAMESPACE setVariable
