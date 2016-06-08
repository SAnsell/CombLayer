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
  CGen.generateChopper(Control,"freiaChopperB",25.0, 52.0,40.0);

  // Double Blade chopper
  Control.addVariable("freiaWFMBladeXStep",0.0);
  Control.addVariable("freiaWFMBladeYStep",-2.0);
  Control.addVariable("freiaWFMBladeZStep",0.0);
  Control.addVariable("freiaWFMBladeXYangle",0.0);
  Control.addVariable("freiaWFMBladeZangle",0.0);

  Control.addVariable("freiaWFMBladeGap",36.1);
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

  
  return;
}
 
}  // NAMESPACE setVariable
