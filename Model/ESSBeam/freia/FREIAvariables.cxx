/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/freia/FREIAvariables.cxx
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
  ELog::RegMethod RegA("FREIAvariables[F]","generatePipe");
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
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");
  
  //  setVariable::ShieldGenerator SGen;
  // extent of beamline
  Control.addVariable("freiaStopPoint",0);
  Control.addVariable("freiaAxisXYAngle",0.0);   // rotation
  Control.addVariable("freiaAxisZAngle",2.0);   // rotation 

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"freiaBA",350.0,4.0,4.0,10.593,17.566,
                      7000.0,0.0);
  
  generatePipe(Control,"freiaPipeB",46.0,12.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"freiaBB",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  generatePipe(Control,"freiaPipeC",46.0,12.0);
  Control.addVariable("freiaYStep",6.0);   // step + flange  
  FGen.generateBender(Control,"freiaBC",44.0,4.0,4.0,17.566,18.347,
                      7000.0,0.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperA",12.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("freiaDBladeXStep",0.0);
  Control.addVariable("freiaDBladeYStep",0.0);
  Control.addVariable("freiaDBladeZStep",0.0);
  Control.addVariable("freiaDBladeXYangle",0.0);
  Control.addVariable("freiaDBladeZangle",0.0);

  Control.addVariable("freiaDBladeGap",1.0);
  Control.addVariable("freiaDBladeInnerRadius",25.0);
  Control.addVariable("freiaDBladeOuterRadius",50.0);
  Control.addVariable("freiaDBladeNDisk",2);

  Control.addVariable("freiaDBlade0Thick",0.2);
  Control.addVariable("freiaDBlade1Thick",0.2);
  Control.addVariable("freiaDBladeInnerMat","Inconnel");
  Control.addVariable("freiaDBladeOuterMat","B4C");
  
  Control.addVariable("freiaDBladeNBlades",2);
  Control.addVariable("freiaDBlade0PhaseAngle0",95.0);
  Control.addVariable("freiaDBlade0OpenAngle0",30.0);
  Control.addVariable("freiaDBlade1PhaseAngle0",95.0);
  Control.addVariable("freiaDBlade1OpenAngle0",30.0);

  Control.addVariable("freiaDBlade0PhaseAngle1",275.0);
  Control.addVariable("freiaDBlade0OpenAngle1",30.0);
  Control.addVariable("freiaDBlade1PhaseAngle1",275.0);
  Control.addVariable("freiaDBlade1OpenAngle1",30.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperB",30.0,46.0,40.0);
  
  // Double Blade chopper
  Control.addVariable("freiaWFMBladeXStep",0.0);
  Control.addVariable("freiaWFMBladeYStep",0.0);
  Control.addVariable("freiaWFMBladeZStep",0.0);
  Control.addVariable("freiaWFMBladeXYangle",0.0);
  Control.addVariable("freiaWFMBladeZangle",0.0);

  Control.addVariable("freiaWFMBladeGap",36.1);  // 36
  Control.addVariable("freiaWFMBladeInnerRadius",40.0);
  Control.addVariable("freiaWFMBladeOuterRadius",75.0);
  Control.addVariable("freiaWFMBladeNDisk",2);

  Control.addVariable("freiaWFMBlade0Thick",0.2);
  Control.addVariable("freiaWFMBlade1Thick",0.2);
  Control.addVariable("freiaWFMBladeInnerMat","Inconnel");
  Control.addVariable("freiaWFMBladeOuterMat","B4C");
  
  Control.addVariable("freiaWFMBladeNBlades",2);
  Control.addVariable("freiaWFMBlade0PhaseAngle0",95.0);
  Control.addVariable("freiaWFMBlade0OpenAngle0",30.0);
  Control.addVariable("freiaWFMBlade1PhaseAngle0",95.0);
  Control.addVariable("freiaWFMBlade1OpenAngle0",30.0);

  Control.addVariable("freiaWFMBlade0PhaseAngle1",275.0);
  Control.addVariable("freiaWFMBlade0OpenAngle1",30.0);
  Control.addVariable("freiaWFMBlade1PhaseAngle1",275.0);
  Control.addVariable("freiaWFMBlade1OpenAngle1",30.0);

  generatePipe(Control,"freiaPipeD",125.0,12.0);
  FGen.generateBender(Control,"freiaBD",121.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperC",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("freiaFOC1BladeXStep",0.0);
  Control.addVariable("freiaFOC1BladeYStep",0.0);
  Control.addVariable("freiaFOC1BladeZStep",0.0);
  Control.addVariable("freiaFOC1BladeXYangle",0.0);
  Control.addVariable("freiaFOC1BladeZangle",0.0);

  Control.addVariable("freiaFOC1BladeGap",36.1);  // 36
  Control.addVariable("freiaFOC1BladeInnerRadius",40.0);
  Control.addVariable("freiaFOC1BladeOuterRadius",75.0);
  Control.addVariable("freiaFOC1BladeNDisk",1);

  Control.addVariable("freiaFOC1Blade0Thick",0.2);
  Control.addVariable("freiaFOC1Blade1Thick",0.2);
  Control.addVariable("freiaFOC1BladeInnerMat","Inconnel");
  Control.addVariable("freiaFOC1BladeOuterMat","B4C");
  
  Control.addVariable("freiaFOC1BladeNBlades",2);
  Control.addVariable("freiaFOC1Blade0PhaseAngle0",95.0);
  Control.addVariable("freiaFOC1Blade0OpenAngle0",30.0);
  Control.addVariable("freiaFOC1Blade0PhaseAngle1",275.0);
  Control.addVariable("freiaFOC1Blade0OpenAngle1",30.0);

  generatePipe(Control,"freiaPipeE",132.0,12.0);
  FGen.generateBender(Control,"freiaBE",128.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperD",12.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("freiaWBC2BladeXStep",0.0);
  Control.addVariable("freiaWBC2BladeYStep",0.0);
  Control.addVariable("freiaWBC2BladeZStep",0.0);
  Control.addVariable("freiaWBC2BladeXYangle",0.0);
  Control.addVariable("freiaWBC2BladeZangle",0.0);

  Control.addVariable("freiaWBC2BladeGap",1.0);
  Control.addVariable("freiaWBC2BladeInnerRadius",25.0);
  Control.addVariable("freiaWBC2BladeOuterRadius",50.0);
  Control.addVariable("freiaWBC2BladeNDisk",2);

  Control.addVariable("freiaWBC2Blade0Thick",0.2);
  Control.addVariable("freiaWBC2Blade1Thick",0.2);
  Control.addVariable("freiaWBC2BladeInnerMat","Inconnel");
  Control.addVariable("freiaWBC2BladeOuterMat","B4C");
  
  Control.addVariable("freiaWBC2BladeNBlades",2);
  Control.addVariable("freiaWBC2Blade0PhaseAngle0",95.0);
  Control.addVariable("freiaWBC2Blade0OpenAngle0",30.0);
  Control.addVariable("freiaWBC2Blade1PhaseAngle0",95.0);
  Control.addVariable("freiaWBC2Blade1OpenAngle0",30.0);

  Control.addVariable("freiaWBC2Blade0PhaseAngle1",275.0);
  Control.addVariable("freiaWBC2Blade0OpenAngle1",30.0);
  Control.addVariable("freiaWBC2Blade1PhaseAngle1",275.0);
  Control.addVariable("freiaWBC2Blade1OpenAngle1",30.0);

  generatePipe(Control,"freiaPipeF",102.0,12.0);
  FGen.generateBender(Control,"freiaBF",98.0,4.0,4.0,20.0,16.0,
                      7000.0,180.0);


  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperE",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("freiaFOC2BladeXStep",0.0);
  Control.addVariable("freiaFOC2BladeYStep",0.0);
  Control.addVariable("freiaFOC2BladeZStep",0.0);
  Control.addVariable("freiaFOC2BladeXYangle",0.0);
  Control.addVariable("freiaFOC2BladeZangle",0.0);

  Control.addVariable("freiaFOC2BladeGap",36.1);  // 36
  Control.addVariable("freiaFOC2BladeInnerRadius",40.0);
  Control.addVariable("freiaFOC2BladeOuterRadius",75.0);
  Control.addVariable("freiaFOC2BladeNDisk",1);

  Control.addVariable("freiaFOC2Blade0Thick",0.2);
  Control.addVariable("freiaFOC2Blade1Thick",0.2);
  Control.addVariable("freiaFOC2BladeInnerMat","Inconnel");
  Control.addVariable("freiaFOC2BladeOuterMat","B4C");
  
  Control.addVariable("freiaFOC2BladeNBlades",2);
  Control.addVariable("freiaFOC2Blade0PhaseAngle0",95.0);
  Control.addVariable("freiaFOC2Blade0OpenAngle0",30.0);
  Control.addVariable("freiaFOC2Blade0PhaseAngle1",275.0);
  Control.addVariable("freiaFOC2Blade0OpenAngle1",30.0);

  // BEAM INSERT:
  Control.addVariable("freiaBInsertHeight",20.0);
  Control.addVariable("freiaBInsertWidth",28.0);
  Control.addVariable("freiaBInsertTopWall",1.0);
  Control.addVariable("freiaBInsertLowWall",1.0);
  Control.addVariable("freiaBInsertLeftWall",1.0);
  Control.addVariable("freiaBInsertRightWall",1.0);
  Control.addVariable("freiaBInsertWallMat","Stainless304");       

  // Guide in wall
  FGen.generateTaper(Control,"freiaFWall",308.0,6.0,6.0,6.0,6.0);

  CGen.setMainRadius(56.0);
  CGen.setFrame(120.0,120.0);
  CGen.generateChopper(Control,"freiaChopperOutA",12.0,10.0,4.55);

  // Double Blade chopper
  Control.addVariable("freiaWBC3BladeXStep",0.0);
  Control.addVariable("freiaWBC3BladeYStep",0.0);
  Control.addVariable("freiaWBC3BladeZStep",0.0);
  Control.addVariable("freiaWBC3BladeXYangle",0.0);
  Control.addVariable("freiaWBC3BladeZangle",0.0);

  Control.addVariable("freiaWBC3BladeGap",1.0);
  Control.addVariable("freiaWBC3BladeInnerRadius",25.0);
  Control.addVariable("freiaWBC3BladeOuterRadius",50.0);
  Control.addVariable("freiaWBC3BladeNDisk",2);

  Control.addVariable("freiaWBC3Blade0Thick",0.2);
  Control.addVariable("freiaWBC3Blade1Thick",0.2);
  Control.addVariable("freiaWBC3BladeInnerMat","Inconnel");
  Control.addVariable("freiaWBC3BladeOuterMat","B4C");
  
  Control.addVariable("freiaWBC3BladeNBlades",2);
  Control.addVariable("freiaWBC3Blade0PhaseAngle0",95.0);
  Control.addVariable("freiaWBC3Blade0OpenAngle0",30.0);
  Control.addVariable("freiaWBC3Blade1PhaseAngle0",95.0);
  Control.addVariable("freiaWBC3Blade1OpenAngle0",30.0);

  Control.addVariable("freiaWBC3Blade0PhaseAngle1",275.0);
  Control.addVariable("freiaWBC3Blade0OpenAngle1",30.0);
  Control.addVariable("freiaWBC3Blade1PhaseAngle1",275.0);
  Control.addVariable("freiaWBC3Blade1OpenAngle1",30.0);

  CGen.setMainRadius(81.0);
  CGen.setFrame(175.0,175.0);
  CGen.generateChopper(Control,"freiaChopperOutB",12.0,12.0,8.0);

  // FOC chopper
  Control.addVariable("freiaFOC3BladeXStep",0.0);
  Control.addVariable("freiaFOC3BladeYStep",0.0);
  Control.addVariable("freiaFOC3BladeZStep",0.0);
  Control.addVariable("freiaFOC3BladeXYangle",0.0);
  Control.addVariable("freiaFOC3BladeZangle",0.0);

  Control.addVariable("freiaFOC3BladeGap",36.1);  // 36
  Control.addVariable("freiaFOC3BladeInnerRadius",40.0);
  Control.addVariable("freiaFOC3BladeOuterRadius",75.0);
  Control.addVariable("freiaFOC3BladeNDisk",1);

  Control.addVariable("freiaFOC3Blade0Thick",0.2);
  Control.addVariable("freiaFOC3Blade1Thick",0.2);
  Control.addVariable("freiaFOC3BladeInnerMat","Inconnel");
  Control.addVariable("freiaFOC3BladeOuterMat","B4C");
  
  Control.addVariable("freiaFOC3BladeNBlades",2);
  Control.addVariable("freiaFOC3Blade0PhaseAngle0",95.0);
  Control.addVariable("freiaFOC3Blade0OpenAngle0",30.0);
  Control.addVariable("freiaFOC3Blade0PhaseAngle1",275.0);
  Control.addVariable("freiaFOC3Blade0OpenAngle1",30.0);

  generatePipe(Control,"freiaPipeOutA",470.0,12.0);
  FGen.generateTaper(Control,"freiaOutFA",462.0,4.0,4.0,20.0,16.0);

  PGen.generatePit(Control,"freiaOutPitA",50.0,50.0,50.0,50.0,0,0);
  Control.addVariable("frieaJawPitYStep",-60.5);

  Control.addVariable("freiaJawPitVoidHeight",167.0);
  Control.addVariable("freiaJawPitVoidDepth",36.0);
  Control.addVariable("freiaJawPitVoidWidth",246.0);
  Control.addVariable("freiaJawPitVoidLength",105.0);
  
  Control.addVariable("freiaJawPitFeHeight",70.0);
  Control.addVariable("freiaJawPitFeDepth",60.0);
  Control.addVariable("freiaJawPitFeWidth",60.0);
  Control.addVariable("freiaJawPitFeFront",45.0);
  Control.addVariable("freiaJawPitFeBack",70.0);
  Control.addVariable("freiaJawPitFeMat","Stainless304");
  
  Control.addVariable("freiaJawPitConcHeight",50.0);
  Control.addVariable("freiaJawPitConcDepth",50.0);
  Control.addVariable("freiaJawPitConcWidth",50.0);
  Control.addVariable("freiaJawPitConcFront",50.0);
  Control.addVariable("freiaJawPitConcBack",50.0);
  Control.addVariable("freiaJawPitConcMat","Concrete");

  Control.addVariable("freiaJawPitColletHeight",15.0);
  Control.addVariable("freiaJawPitColletDepth",15.0);
  Control.addVariable("freiaJawPitColletWidth",40.0);
  Control.addVariable("freiaJawPitColletLength",5.0);
  Control.addVariable("freiaJawPitColletMat","Tungsten");

  SGen.generateShield(Control,"freiaShieldA",570.0,40.0,40.0,40.0,4,8);
  return;
}
 
}  // NAMESPACE setVariable
