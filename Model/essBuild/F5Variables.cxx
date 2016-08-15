/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Konstantin Batkov
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

///\file F5Variables.cxx

namespace setVariable
{
void
F5Variables(FuncDataBase& Control)
/*!
  Create DUMMY variables for all F5 collimators
 */
{
    ELog::RegMethod RegA("f5Variables[F]","f5Variables");

    // some default values are inherited from F5:
    Control.addVariable("F5DefaultRadius", 1000);          // Radial location of all F5 tallies (valid with -f5-collimators argument)
    Control.addVariable("F5DefaultAlgorithm", "FocalPoints"); // Algorithm of all F5 tallies
    Control.addVariable("F5DefaultLength", 440.0);
    Control.addVariable("F5DefaultViewWidth", 6);

    std::string name;
    for (size_t i=5; i<=995; i+=5)
      {
	name=StrFunc::makeString("F", i);
	Control.addVariable(name+"Delta", 0.0);
	Control.addVariable(name+"LinkPoint", 0.0);
	// for manual collimtors (F5Algorithm is 'manual'):
	Control.addVariable(name+"FocalPoint", Geometry::Vec3D(0,0,13.7)); // z is not in use
	
	Control.addVariable(name+"XB", 11.0);
	Control.addVariable(name+"YB", -14.0);
	Control.addVariable(name+"ZB", 14.0);
	Control.addVariable(name+"XC", 11.0);
	Control.addVariable(name+"YC", -8.0);
	Control.addVariable(name+"ZC", 14.0);
	Control.addVariable(name+"ZG", 11.0);
	Control.addVariable(name+"X", 999.0);
	Control.addVariable(name+"Y", -18.0);
	Control.addVariable(name+"Z", 13.0);
      }
    
    return;
}
}
