/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuild/exampleVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNEXAMPLE FOR A PARTICULAR PURPOSE.  See the
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

// #include "exampleVariables.h"

namespace setVariable
{

void
ExampleVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for EXAMPLE 
    \param Control :: Function data base to add constants too
    \param beamName :: Set of beamline names
  */
{
  ELog::RegMethod RegA("setVariable","exampleVariables");

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("sdefEnergy",3000.0);

  Control.addVariable("DPipeRadius",200.0);
  Control.addVariable("DPipeHeight",3.0);
  Control.addVariable("DPipeWidth",6.0);
  Control.addVariable("DPipeOuterWidth",20.0);
  Control.addVariable("DPipeOuterHeight",20.0);
  Control.addVariable("DPipeAngle",90.0);
  Control.addVariable("DPipeOuterMat","Stainless304");

  Control.addVariable("QuadAYStep",100.0);
  Control.addVariable("QuadBYStep",300.0);
  Control.addVariable("QuadCYStep",500.0);
  Control.addVariable("QuadDYStep",700.0);
  for(const std::string& AName : {"QuadA","QuadB","QuadC","QuadD"})
    {
      Control.addVariable(AName+"RadiusX",3.0);
      Control.addVariable(AName+"RadiusZ",1.5);
      Control.addVariable(AName+"Height",20.0);
      Control.addVariable(AName+"Width",20.0);
      Control.addVariable(AName+"Length",20.0);
      Control.addVariable(AName+"OuterMat","Stainless304");
    }
// FINAL:
  Control.resetActive();
  return;
}

  
}  // NAMESPACE setVariable
