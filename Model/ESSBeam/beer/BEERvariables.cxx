/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/beer/BEERvariables.cxx
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
#include "essVariables.h"
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"

namespace setVariable
{

 
void
generatePipe(FuncDataBase& Control,
	     const std::string& keyName,
	     const double length,
             const double radius)
  /*!
    Create general pipe
    \param Control :: Data Base for variables
    \param keyName :: main name
    \param length :: length of pipe
   */
{
  ELog::RegMethod RegA("BEERvariables[F]","generatePipe");
    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",2.0);   // step + flange
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",1.0);
  Control.addVariable(keyName+"FlangeRadius",radius+4.0);
  Control.addVariable(keyName+"FlangeLength",1.0);
  Control.addVariable(keyName+"FeMat","Stainless304");
  Control.addVariable(keyName+"WindowActive",3);
  Control.addVariable(keyName+"WindowRadius",radius+2.0);
  Control.addVariable(keyName+"WindowThick",0.5);
  Control.addVariable(keyName+"WindowMat","Silicon300K");

  return;
}


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
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");
  
  //  setVariable::ShieldGenerator SGen;
  // extent of beamline
  Control.addVariable("beerStopPoint",0);
  Control.addVariable("beerAxisXYAngle",0.0);   // rotation
  Control.addVariable("beerAxisZAngle",2.0);   // rotation 

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"beerBA",350.0,4.0,4.0,10.593,17.566,
                      7000.0,0.0);
  
  generatePipe(Control,"beerPipeB",46.0,12.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"beerBB",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  generatePipe(Control,"beerPipeC",46.0,12.0);
  Control.addVariable("beerYStep",6.0);   // step + flange  
  FGen.generateBender(Control,"beerBC",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperA",12.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("beerDBladeXStep",0.0);
  Control.addVariable("beerDBladeYStep",0.0);
  Control.addVariable("beerDBladeZStep",0.0);
  Control.addVariable("beerDBladeXYangle",0.0);
  Control.addVariable("beerDBladeZangle",0.0);

  Control.addVariable("beerDBladeGap",1.0);
  Control.addVariable("beerDBladeInnerRadius",25.0);
  Control.addVariable("beerDBladeOuterRadius",50.0);
  Control.addVariable("beerDBladeNDisk",2);

  Control.addVariable("beerDBlade0Thick",0.2);
  Control.addVariable("beerDBlade1Thick",0.2);
  Control.addVariable("beerDBladeInnerMat","Inconnel");
  Control.addVariable("beerDBladeOuterMat","B4C");
  
  Control.addVariable("beerDBladeNBlades",2);
  Control.addVariable("beerDBlade0PhaseAngle0",95.0);
  Control.addVariable("beerDBlade0OpenAngle0",30.0);
  Control.addVariable("beerDBlade1PhaseAngle0",95.0);
  Control.addVariable("beerDBlade1OpenAngle0",30.0);

  Control.addVariable("beerDBlade0PhaseAngle1",275.0);
  Control.addVariable("beerDBlade0OpenAngle1",30.0);
  Control.addVariable("beerDBlade1PhaseAngle1",275.0);
  Control.addVariable("beerDBlade1OpenAngle1",30.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperB",30.0,46.0,40.0);
  
  // Double Blade chopper
  Control.addVariable("beerWFMBladeXStep",0.0);
  Control.addVariable("beerWFMBladeYStep",0.0);
  Control.addVariable("beerWFMBladeZStep",0.0);
  Control.addVariable("beerWFMBladeXYangle",0.0);
  Control.addVariable("beerWFMBladeZangle",0.0);

  Control.addVariable("beerWFMBladeGap",36.1);  // 36
  Control.addVariable("beerWFMBladeInnerRadius",40.0);
  Control.addVariable("beerWFMBladeOuterRadius",75.0);
  Control.addVariable("beerWFMBladeNDisk",2);

  Control.addVariable("beerWFMBlade0Thick",0.2);
  Control.addVariable("beerWFMBlade1Thick",0.2);
  Control.addVariable("beerWFMBladeInnerMat","Inconnel");
  Control.addVariable("beerWFMBladeOuterMat","B4C");
  
  Control.addVariable("beerWFMBladeNBlades",2);
  Control.addVariable("beerWFMBlade0PhaseAngle0",95.0);
  Control.addVariable("beerWFMBlade0OpenAngle0",30.0);
  Control.addVariable("beerWFMBlade1PhaseAngle0",95.0);
  Control.addVariable("beerWFMBlade1OpenAngle0",30.0);

  Control.addVariable("beerWFMBlade0PhaseAngle1",275.0);
  Control.addVariable("beerWFMBlade0OpenAngle1",30.0);
  Control.addVariable("beerWFMBlade1PhaseAngle1",275.0);
  Control.addVariable("beerWFMBlade1OpenAngle1",30.0);

  generatePipe(Control,"beerPipeD",125.0,12.0);
  FGen.generateBender(Control,"beerBD",121.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperC",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("beerFOC1BladeXStep",0.0);
  Control.addVariable("beerFOC1BladeYStep",0.0);
  Control.addVariable("beerFOC1BladeZStep",0.0);
  Control.addVariable("beerFOC1BladeXYangle",0.0);
  Control.addVariable("beerFOC1BladeZangle",0.0);

  Control.addVariable("beerFOC1BladeGap",36.1);  // 36
  Control.addVariable("beerFOC1BladeInnerRadius",40.0);
  Control.addVariable("beerFOC1BladeOuterRadius",75.0);
  Control.addVariable("beerFOC1BladeNDisk",1);

  Control.addVariable("beerFOC1Blade0Thick",0.2);
  Control.addVariable("beerFOC1Blade1Thick",0.2);
  Control.addVariable("beerFOC1BladeInnerMat","Inconnel");
  Control.addVariable("beerFOC1BladeOuterMat","B4C");
  
  Control.addVariable("beerFOC1BladeNBlades",2);
  Control.addVariable("beerFOC1Blade0PhaseAngle0",95.0);
  Control.addVariable("beerFOC1Blade0OpenAngle0",30.0);
  Control.addVariable("beerFOC1Blade0PhaseAngle1",275.0);
  Control.addVariable("beerFOC1Blade0OpenAngle1",30.0);

  generatePipe(Control,"beerPipeE",132.0,12.0);
  FGen.generateBender(Control,"beerBE",128.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperD",12.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("beerWBC2BladeXStep",0.0);
  Control.addVariable("beerWBC2BladeYStep",0.0);
  Control.addVariable("beerWBC2BladeZStep",0.0);
  Control.addVariable("beerWBC2BladeXYangle",0.0);
  Control.addVariable("beerWBC2BladeZangle",0.0);

  Control.addVariable("beerWBC2BladeGap",1.0);
  Control.addVariable("beerWBC2BladeInnerRadius",25.0);
  Control.addVariable("beerWBC2BladeOuterRadius",50.0);
  Control.addVariable("beerWBC2BladeNDisk",2);

  Control.addVariable("beerWBC2Blade0Thick",0.2);
  Control.addVariable("beerWBC2Blade1Thick",0.2);
  Control.addVariable("beerWBC2BladeInnerMat","Inconnel");
  Control.addVariable("beerWBC2BladeOuterMat","B4C");
  
  Control.addVariable("beerWBC2BladeNBlades",2);
  Control.addVariable("beerWBC2Blade0PhaseAngle0",95.0);
  Control.addVariable("beerWBC2Blade0OpenAngle0",30.0);
  Control.addVariable("beerWBC2Blade1PhaseAngle0",95.0);
  Control.addVariable("beerWBC2Blade1OpenAngle0",30.0);

  Control.addVariable("beerWBC2Blade0PhaseAngle1",275.0);
  Control.addVariable("beerWBC2Blade0OpenAngle1",30.0);
  Control.addVariable("beerWBC2Blade1PhaseAngle1",275.0);
  Control.addVariable("beerWBC2Blade1OpenAngle1",30.0);

  generatePipe(Control,"beerPipeF",102.0,12.0);
  FGen.generateBender(Control,"beerBF",98.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);


  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperE",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("beerFOC2BladeXStep",0.0);
  Control.addVariable("beerFOC2BladeYStep",0.0);
  Control.addVariable("beerFOC2BladeZStep",0.0);
  Control.addVariable("beerFOC2BladeXYangle",0.0);
  Control.addVariable("beerFOC2BladeZangle",0.0);

  Control.addVariable("beerFOC2BladeGap",36.1);  // 36
  Control.addVariable("beerFOC2BladeInnerRadius",40.0);
  Control.addVariable("beerFOC2BladeOuterRadius",75.0);
  Control.addVariable("beerFOC2BladeNDisk",1);

  Control.addVariable("beerFOC2Blade0Thick",0.2);
  Control.addVariable("beerFOC2Blade1Thick",0.2);
  Control.addVariable("beerFOC2BladeInnerMat","Inconnel");
  Control.addVariable("beerFOC2BladeOuterMat","B4C");
  
  Control.addVariable("beerFOC2BladeNBlades",2);
  Control.addVariable("beerFOC2Blade0PhaseAngle0",95.0);
  Control.addVariable("beerFOC2Blade0OpenAngle0",30.0);
  Control.addVariable("beerFOC2Blade0PhaseAngle1",275.0);
  Control.addVariable("beerFOC2Blade0OpenAngle1",30.0);

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

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"beerChopperOutA",18.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("beerWBC3BladeXStep",0.0);
  Control.addVariable("beerWBC3BladeYStep",0.0);
  Control.addVariable("beerWBC3BladeZStep",0.0);
  Control.addVariable("beerWBC3BladeXYangle",0.0);
  Control.addVariable("beerWBC3BladeZangle",0.0);

  Control.addVariable("beerWBC3BladeGap",1.0);
  Control.addVariable("beerWBC3BladeInnerRadius",25.0);
  Control.addVariable("beerWBC3BladeOuterRadius",50.0);
  Control.addVariable("beerWBC3BladeNDisk",2);

  Control.addVariable("beerWBC3Blade0Thick",0.2);
  Control.addVariable("beerWBC3Blade1Thick",0.2);
  Control.addVariable("beerWBC3BladeInnerMat","Inconnel");
  Control.addVariable("beerWBC3BladeOuterMat","B4C");
  
  Control.addVariable("beerWBC3BladeNBlades",2);
  Control.addVariable("beerWBC3Blade0PhaseAngle0",95.0);
  Control.addVariable("beerWBC3Blade0OpenAngle0",30.0);
  Control.addVariable("beerWBC3Blade1PhaseAngle0",95.0);
  Control.addVariable("beerWBC3Blade1OpenAngle0",30.0);

  Control.addVariable("beerWBC3Blade0PhaseAngle1",275.0);
  Control.addVariable("beerWBC3Blade0OpenAngle1",30.0);
  Control.addVariable("beerWBC3Blade1PhaseAngle1",275.0);
  Control.addVariable("beerWBC3Blade1OpenAngle1",30.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"beerChopperOutB",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("beerFOC3BladeXStep",0.0);
  Control.addVariable("beerFOC3BladeYStep",0.0);
  Control.addVariable("beerFOC3BladeZStep",0.0);
  Control.addVariable("beerFOC3BladeXYangle",0.0);
  Control.addVariable("beerFOC3BladeZangle",0.0);

  Control.addVariable("beerFOC3BladeGap",36.1);  // 36
  Control.addVariable("beerFOC3BladeInnerRadius",40.0);
  Control.addVariable("beerFOC3BladeOuterRadius",75.0);
  Control.addVariable("beerFOC3BladeNDisk",1);

  Control.addVariable("beerFOC3Blade0Thick",0.2);
  Control.addVariable("beerFOC3Blade1Thick",0.2);
  Control.addVariable("beerFOC3BladeInnerMat","Inconnel");
  Control.addVariable("beerFOC3BladeOuterMat","B4C");
  
  Control.addVariable("beerFOC3BladeNBlades",2);
  Control.addVariable("beerFOC3Blade0PhaseAngle0",95.0);
  Control.addVariable("beerFOC3Blade0OpenAngle0",30.0);
  Control.addVariable("beerFOC3Blade0PhaseAngle1",275.0);
  Control.addVariable("beerFOC3Blade0OpenAngle1",30.0);

  generatePipe(Control,"beerPipeOutA",450.0,12.0);
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
