/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/freia/FREIAvariables.cxx
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
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"

namespace setVariable
{
   
void
FREIAvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("FREIAvariables[F]","FREIAvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::JawGenerator JawGen;
  setVariable::BladeGenerator BGen;
  
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,50.0,"Concrete");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  PipeGen.setCF<CF250>();
  PipeGen.setNoWindow();

  //  setVariable::ShieldGenerator SGen;
  // extent of beamline
  Control.addVariable("freiaStopPoint",0);
  Control.addVariable("freiaAxisXYAngle",0.0);   // rotation
  Control.addVariable("freiaAxisZAngle",0.0);   // down slope

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.4,"Void");
  FGen.setYOffset(0.0);
  // was 10.593 to 17.566 
  FGen.generateBender(Control,"freiaBA",350.0,4.0,4.0, 13.8,21.24,
                      7000.0,90.0);

  // Pipe in gamma shield
  FGen.setLayer(1,0.5,"Aluminium");
  PipeGen.generatePipe(Control,"freiaPipeB",40.0);
  Control.addVariable("freiaPipeBYStep",8.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"freiaBB",36.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  // Pipe in to first ch
  PipeGen.generatePipe(Control,"freiaPipeC",46.0);
  Control.addVariable("freiaPipeCYStep",4.0);
  Control.addVariable("freiaYStep",6.0);   // step + flange  
  FGen.generateBender(Control,"freiaBC",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperA",12.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaDBlade",0.0,25.0,50.0);
  

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperB",30.0,46.0,40.0);
  
  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(36.1);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaWFMBlade",0.0,40.0,75.0);

  PipeGen.setPipe(14.0,0.5);
  PipeGen.generatePipe(Control,"freiaPipeD",125.0);
  Control.addVariable("freiaPipeDYStep",2.0);
  FGen.generateBender(Control,"freiaBD",121.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperC",12.0,12.0,8.0);

    // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95},{30.0});
  BGen.addPhase({275},{30.0});
  BGen.generateBlades(Control,"freiaFOC1Blade",0.0,40.0,75.0);

  PipeGen.generatePipe(Control,"freiaPipeE",132.0);
  Control.addVariable("freiaPipeEYStep",2.0);
  FGen.generateBender(Control,"freiaBE",128.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperD",12.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaWBC2Blade",0.0,25.0,50.0);

  // Double Blade chopper

  PipeGen.generatePipe(Control,"freiaPipeF",102.0);
  Control.addVariable("freiaPipeFYStep",2.0);
  FGen.generateBender(Control,"freiaBF",98.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);


  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperE",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaFOC2Blade",0.0,40.0,75.0);


  // BEAM INSERT:
  Control.addVariable("freiaBInsertHeight",20.0);
  Control.addVariable("freiaBInsertWidth",28.0);
  Control.addVariable("freiaBInsertTopWall",1.0);
  Control.addVariable("freiaBInsertLowWall",1.0);
  Control.addVariable("freiaBInsertLeftWall",1.0);
  Control.addVariable("freiaBInsertRightWall",1.0);
  Control.addVariable("freiaBInsertWallMat","Stainless304");       

  // Guide in wall
  FGen.generateTaper(Control,"freiaFWall",346.0,6.0,6.0,6.0,6.0);
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"freiaPipeWall",348.0);
  Control.addVariable("freiaPipeWallYStep",4.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperOutA",18.0,10.0,4.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaWBC3Blade",0.0,25.0,50.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperOutB",12.0,12.0,8.0);

  // FOC chopper
  BGen.setThick({0.2});
  BGen.setGap(1.0);
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"freiaFOC3Blade",0.0,40.0,75.0);


  PipeGen.generatePipe(Control,"freiaPipeOutA",450.0);
  Control.addVariable("freiaPipeOutAYStep",30.0);
  FGen.generateTaper(Control,"freiaOutFA",442.0,4.0,4.0,20.0,16.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"freiaOutPitA",0.0,40.0,190.0,170.0,30.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"freiaJawPit",485.0,40.0,150.0,120.0,30.0);

  Control.addVariable("freiaOutACutShape","Circle");
  Control.addVariable("freiaOutACutRadius",5.0);
  
  SGen.generateShield(Control,"freiaShieldA",470.0,40.0,40.0,60.0,4,8);
  JawGen.generateJaws(Control,"freiaCaveJaws",0.0);

  Control.addVariable("freiaOutBCutBackShape","Circle");
  Control.addVariable("freiaOutBCutBackRadius",5.0);

  Control.addVariable("freiaOutBCutFrontShape","Circle");
  Control.addVariable("freiaOutBCutFrontRadius",5.0);

  
  return;
}
 
}  // NAMESPACE setVariable
