/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/beer/BEERvariables.cxx
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
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "PitGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
generatePinHole(FuncDataBase& Control)
  /*!
    Generate the pin-hole variables
    \param Control :: Database
   */
{
    // PIN COLLIMATOR SYSTEM
  Control.addVariable("beerCaveJawLength",25.0);
  Control.addVariable("beerCaveJawRadius",30.0);

  //
  // JAWS
  //
  // Jaw X
  Control.addVariable("beerCaveJawJawVertXStep",0.0);
  Control.addVariable("beerCaveJawJawVertYStep",-5.0);
  Control.addVariable("beerCaveJawJawVertZStep",0.0);
  Control.addVariable("beerCaveJawJawVertXYangle",180.0);
  Control.addVariable("beerCaveJawJawVertZangle",0.0);

  Control.addVariable("beerCaveJawJawVertZOpen",5.5);
  Control.addVariable("beerCaveJawJawVertZThick",2.0);
  Control.addVariable("beerCaveJawJawVertZCross",15.0);
  Control.addVariable("beerCaveJawJawVertZLen",8.0);

  Control.addVariable("beerCaveJawJawVertGap",0.5);  
  
  Control.addVariable("beerCaveJawJawVertXOpen",5.5);
  Control.addVariable("beerCaveJawJawVertXThick",2.0);
  Control.addVariable("beerCaveJawJawVertXCross",15.0);
  Control.addVariable("beerCaveJawJawVertXLen",8.0);  

  Control.addVariable("beerCaveJawJawVertXHeight",28.0);
  Control.addVariable("beerCaveJawJawVertYHeight",9.0);
  Control.addVariable("beerCaveJawJawVertZHeight",28.0);
  Control.addVariable("beerCaveJawJawVertWallThick",2.0);

  Control.addVariable("beerCaveJawJawVertxJawMat","Tungsten");
  Control.addVariable("beerCaveJawJawVertzJawMat","Tungsten");
  Control.addVariable("beerCaveJawJawVertWallMat","Aluminium");

  // Jaw XZ
  Control.addVariable("beerCaveJawJawDiagXStep",0.0);
  Control.addVariable("beerCaveJawJawDiagYStep",5.0);
  Control.addVariable("beerCaveJawJawDiagZStep",0.0);
  Control.addVariable("beerCaveJawJawDiagXAngle",0.0);
  Control.addVariable("beerCaveJawJawDiagYAngle",45.0);
  Control.addVariable("beerCaveJawJawDiagZAngle",0.0);

  Control.addVariable("beerCaveJawJawDiagZOpen",5.5);
  Control.addVariable("beerCaveJawJawDiagZThick",2.0);
  Control.addVariable("beerCaveJawJawDiagZCross",15.0);
  Control.addVariable("beerCaveJawJawDiagZLen",8.0);

  Control.addVariable("beerCaveJawJawDiagGap",0.5);  
  
  Control.addVariable("beerCaveJawJawDiagXOpen",5.5);
  Control.addVariable("beerCaveJawJawDiagXThick",2.0);
  Control.addVariable("beerCaveJawJawDiagXCross",15.0);
  Control.addVariable("beerCaveJawJawDiagXLen",8.0);  

  Control.addVariable("beerCaveJawJawDiagXHeight",28.0);
  Control.addVariable("beerCaveJawJawDiagYHeight",9.0);
  Control.addVariable("beerCaveJawJawDiagZHeight",28.0);
  Control.addVariable("beerCaveJawJawDiagWallThick",2.0);


  Control.addVariable("beerCaveJawJawDiagxJawMat","Tungsten");
  Control.addVariable("beerCaveJawJawDiagzJawMat","Tungsten");
  Control.addVariable("beerCaveJawJawDiagWallMat","Aluminium");
  return;
}
  
  
void
BEERvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BEERvariables[F]","BEERvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  PipeGen.setPipe(12.0,0.8);
  PipeGen.setWindow(-2.0,0.8);
  PipeGen.setFlange(-4.0,1.0);
  
  //  setVariable::ShieldGenerator SGen;
  // extent of beamline
  Control.addVariable("beerStopPoint",0);
  Control.addVariable("beerAxisXYAngle",0.9);   // rotation
  Control.addVariable("beerAxisZAngle",0.0);   // rotation 

  FGen.setLayer(1,0.5,"Copper");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"beerBA",350.0,4.0,4.0,10.593,17.566,
                      7000.0,0.0);
  
  PipeGen.generatePipe(Control,"beerPipeB",8.0,44.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"beerBB",42.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  PipeGen.generatePipe(Control,"beerPipeC",6.0,46.0);
  FGen.generateBender(Control,"beerBC",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperA",12.0,10.0,4.55);
  
  // Double Blade chopper
  BGen.setMaterials("Inconnel","B4C");
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerDBlade",0.0,25.0,50.0);
  
  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperB",30.0,46.0,40.0);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(36.1);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerWFMBlade",0.0,40.0,75.0);

  PipeGen.generatePipe(Control,"beerPipeD",4.0,125.0);
  FGen.generateBender(Control,"beerBD",121.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperC",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerFOC1Blade",0.0,40.0,75.0);
  
  PipeGen.generatePipe(Control,"beerPipeE",4.0,132.0);
  FGen.generateBender(Control,"beerBE",128.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperD",12.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerWBC2Blade",0.0,25.0,50.0);

  PipeGen.generatePipe(Control,"beerPipeF",4.0,102.0);
  FGen.generateBender(Control,"beerBF",98.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);


  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperE",12.0,12.0,8.0);

  // FOC chopper (second)
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerFOC2Blade",0.0,40.0,75.0);

  // BEAM INSERT:
  Control.addVariable("beerBInsertHeight",20.0);
  Control.addVariable("beerBInsertWidth",28.0);
  Control.addVariable("beerBInsertTopWall",1.0);
  Control.addVariable("beerBInsertLowWall",1.0);
  Control.addVariable("beerBInsertLeftWall",1.0);
  Control.addVariable("beerBInsertRightWall",1.0);
  Control.addVariable("beerBInsertWallMat","Stainless304");       

  // Guide in wall
  FGen.generateTaper(Control,"beerFWall",308.0,6.0,6.0,6.0,6.0);
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"beerPipeWall",4.0,348.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperOutA",18.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerWBC3Blade",0.0,25.0,50.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperOutB",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.setGap(36.1);
  BGen.generateBlades(Control,"beerFOC3Blade",0.0,40.0,75.0);


  PipeGen.generatePipe(Control,"beerPipeOutA",4.0,450.0);
  FGen.generateTaper(Control,"beerOutFA",442.0,4.0,4.0,20.0,16.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"beerOutPitA",0.0,40.0,170.0,150.0,30.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"beerJawPit",485.0,40.0,150.0,120.0,30.0);

  Control.addVariable("beerOutACutShape","Circle");
  Control.addVariable("beerOutACutRadius",5.0);
  
  SGen.generateShield(Control,"beerShieldA",470.0,40.0,40.0,40.0,4,8);
  generatePinHole(Control);
  return;
}
 
}  // NAMESPACE setVariable
