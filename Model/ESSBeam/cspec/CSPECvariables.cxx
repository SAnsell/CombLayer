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
#include "LayerGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "TwinBaseGenerator.h"
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
  setVariable::LayerGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::TwinGenerator TGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-2.0,1.0);

  // extent of beamline
  Control.addVariable("cspecStopPoint",0);

  FGen.setLayer(1,0.8,"Copper");
  FGen.setLayer(2,0.3,"Void");
  FGen.setYOffset(4.0);
  FGen.generateTaper(Control,"cspecFA",350.0, 9.1,10.6, 10.0,14.8);


  PipeGen.generatePipe(Control,"cspecPipeB",8.0,46.0);
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"cspecFB",44.0, 10.6,14.8);   

  PipeGen.generatePipe(Control,"cspecPipeC",4.0,1260.0);
  FGen.generateRectangle(Control,"cspecFC",1256.0, 10.6,14.8);   

  CGen.setMainRadius(38.122);   // diameter 70.0 internal
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"cspecChopperA",8.0,10.0,6.5);  

  // BW1 Blade
  BGen.setMaterials("Aluminium","B4C");
  BGen.setThick({0.5});
  BGen.addPhase({120},{35.74});
  BGen.generateBlades(Control,"cspecBWDiskA",0.0,20.0,35.0);

  PipeGen.generatePipe(Control,"cspecPipeD",4.0,510.0);
  FGen.generateBender(Control,"cspecBD",506.0, 3.0,3.0,3.0,3.0,20000.0,0.0);
    
  return;
}
 
}  // NAMESPACE setVariable
