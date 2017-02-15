/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/cspec/CSPECvariables.cxx
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
#include "TwinGenerator.h"
#include "essVariables.h"

namespace setVariable
{
  
void
CSPECvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("CSPECvariables[F]","CSPECvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::TwinGenerator TGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-2.0,1.0);

  // extent of beamline
  Control.addVariable("cspecStopPoint",0);

  FGen.setGuideMat("Copper");
  FGen.setThickness(0.8,0.3);
  FGen.setYOffset(4.0);
  FGen.generateTaper(Control,"cspecFA",350.0, 6.0,2.0 ,4.0,3.5);


  PipeGen.generatePipe(Control,"cspecPipeB",8.0,44.0);
  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"cspecFB",42.0, 1.88,2.06,4.6,4.5);   


  PipeGen.generatePipe(Control,"cspecPipeC",8.0,44.0);
  FGen.setThickness(0.8,0.3);
  FGen.clearYOffset();
  FGen.generateBender(Control,"cspecBC",400.0, 3.0,3.0,3.0,3.0,20000.0,0.0);

  CGen.setMainRadius(38.122);   // diameter 70.0 emali
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"cspecChopperA",55.0,9.0,3.55);  

    // Double Blade chopper
  BGen.setMaterials("Aluminium","Aluminium");
  BGen.setThick({2.0,2.0});
  BGen.setGap(1.0);
  BGen.addPhase({120},{120.0});
  BGen.addPhase({120},{120.0});
  BGen.generateBlades(Control,"cspecDBlade",0.95,22.5,33.5);

  
  return;
}
 
}  // NAMESPACE setVariable
