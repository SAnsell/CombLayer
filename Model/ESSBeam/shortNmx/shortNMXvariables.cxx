/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/nmx/NMXvariables.cxx
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
#include "variableSetup.h"
#include "essVariables.h"
#include "FocusGenerator.h"
#include "PipeGenerator.h"

namespace setVariable
{

void
shortNMXvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for nmx
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("shortNMXvariables[F]","shortNMXvariables");

  setVariable::FocusGenerator FGen;
  setVariable::PipeGenerator PipeGen;

  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);

  Control.addVariable("shortNMXAxisZAngle",0.0);

  
  // Pipe in gamma shield
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.setLayer(2,0.4,"Void");
  PipeGen.generatePipe(Control,"shortNMXPipeB",8.0,46.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"shortNMXFB",44.0,6.0,6.0,2.0,2.0);

  // Pipe to wall
  PipeGen.generatePipe(Control,"shortNMXPipeC",2.0,495.0);
  FGen.generateTaper(Control,"shortNMXFC",440.0,10.0,1.0,10.0,1.0);

  return;
}

}  // NAMESPACE setVariable
