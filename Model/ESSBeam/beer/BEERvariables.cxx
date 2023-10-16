/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/beer/BEERvariables.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PipeGenerator.h"
#include "BladeGenerator.h"
#include "PitGenerator.h"

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
  Control.addVariable("beerCaveJawRadius",29.0);

  //
  // JAWS
  //
  // Jaw X
  Control.addVariable("beerCaveJawVertXStep",0.0);
  Control.addVariable("beerCaveJawVertYStep",-5.0);
  Control.addVariable("beerCaveJawVertZStep",0.0);
  Control.addVariable("beerCaveJawVertXYangle",180.0);
  Control.addVariable("beerCaveJawVertZangle",0.0);

  Control.addVariable("beerCaveJawVertZOpen",5.5);
  Control.addVariable("beerCaveJawVertZThick",2.0);
  Control.addVariable("beerCaveJawVertZCross",15.0);
  Control.addVariable("beerCaveJawVertZLen",8.0);

  Control.addVariable("beerCaveJawVertGap",0.5);  
  
  Control.addVariable("beerCaveJawVertXOpen",5.5);
  Control.addVariable("beerCaveJawVertXThick",2.0);
  Control.addVariable("beerCaveJawVertXCross",15.0);
  Control.addVariable("beerCaveJawVertXLen",8.0);  

  Control.addVariable("beerCaveJawVertXHeight",28.0);
  Control.addVariable("beerCaveJawVertYHeight",9.0);
  Control.addVariable("beerCaveJawVertZHeight",28.0);
  Control.addVariable("beerCaveJawVertWallThick",2.0);

  Control.addVariable("beerCaveJawVertxJawMat","Tungsten");
  Control.addVariable("beerCaveJawVertzJawMat","Tungsten");
  Control.addVariable("beerCaveJawVertWallMat","Aluminium");

  // Jaw XZ
  Control.addVariable("beerCaveJawDiagXStep",0.0);
  Control.addVariable("beerCaveJawDiagYStep",5.0);
  Control.addVariable("beerCaveJawDiagZStep",0.0);
  Control.addVariable("beerCaveJawDiagXAngle",0.0);
  Control.addVariable("beerCaveJawDiagYAngle",45.0);
  Control.addVariable("beerCaveJawDiagZAngle",0.0);

  Control.addVariable("beerCaveJawDiagZOpen",5.5);
  Control.addVariable("beerCaveJawDiagZThick",2.0);
  Control.addVariable("beerCaveJawDiagZCross",15.0);
  Control.addVariable("beerCaveJawDiagZLen",8.0);

  Control.addVariable("beerCaveJawDiagGap",0.5);  
  
  Control.addVariable("beerCaveJawDiagXOpen",5.5);
  Control.addVariable("beerCaveJawDiagXThick",2.0);
  Control.addVariable("beerCaveJawDiagXCross",15.0);
  Control.addVariable("beerCaveJawDiagXLen",8.0);  

  Control.addVariable("beerCaveJawDiagXHeight",28.0);
  Control.addVariable("beerCaveJawDiagYHeight",9.0);
  Control.addVariable("beerCaveJawDiagZHeight",28.0);
  Control.addVariable("beerCaveJawDiagWallThick",2.0);


  Control.addVariable("beerCaveJawDiagxJawMat","Tungsten");
  Control.addVariable("beerCaveJawDiagzJawMat","Tungsten");
  Control.addVariable("beerCaveJawDiagWallMat","Aluminium");
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

  PipeGen.setCF<CF250>();
  PipeGen.setNoWindow();
  
  //  setVariable::ShieldGenerator SGen;
  // extent of beamline
  Control.addVariable("beerStopPoint",0);
  Control.addVariable("beerAxisXYAngle",0.9);   // rotation
  Control.addVariable("beerAxisZAngle",0.0);   // rotation 

  FGen.setLayer(1,0.5,"Copper");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"beerBA",350.0,4.0,4.0,10.593,17.566,
                      7000.0,0.0);
  
  PipeGen.generatePipe(Control,"beerPipeB",44.0);
  Control.addVariable("beerPipeBYStep",8.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"beerBB",42.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  PipeGen.generatePipe(Control,"beerPipeC",46.0);
  Control.addVariable("beerPipeCYStep",6.0);
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

  PipeGen.setCF<CF300>();
  PipeGen.generatePipe(Control,"beerPipeD",125.0);
  Control.addVariable("beerPipeDYStep",4.0);
  FGen.generateBender(Control,"beerBD",121.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperC",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerFOC1Blade",0.0,40.0,75.0);
  
  PipeGen.generatePipe(Control,"beerPipeE",132.0);
  Control.addVariable("beerPipeEYStep",4.0);
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

  PipeGen.generatePipe(Control,"beerPipeF",102.0);
  Control.addVariable("beerPipeFYStep",4.0);
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
  FGen.generateTaper(Control,"beerFWall",355.0,6.0,6.0,6.0,6.0);
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"beerPipeWall",348.0);
  Control.addVariable("beerPipeWallYStep",4.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperOutA",18.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"beerWBC3Blade",0.0,25.0,50.0);

  CGen.setMainRadius(75.0);
  CGen.setFrame(165.0,165.0);
  CGen.generateChopper(Control,"beerChopperOutB",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.setGap(36.1);
  BGen.generateBlades(Control,"beerFOC3Blade",0.0,40.0,75.0);


  PipeGen.generatePipe(Control,"beerPipeOutA",450.0);
  Control.addVariable("beerPipeOutAYStep",4.0);
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
