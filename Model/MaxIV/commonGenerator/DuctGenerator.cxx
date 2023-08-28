/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/DuctGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "DuctGenerator.h"

namespace setVariable
{

DuctGenerator::DuctGenerator() :
  length(100.0),width(50.0),height(150.0),
  radius(0.0),voidMat(0)
  /*!
    Constructor and defaults
  */
{}

DuctGenerator::~DuctGenerator()
 /*!
   Destructor
 */
{}

void
DuctGenerator::generate(FuncDataBase& Control,
			const std::string& keyName,
			const double& zAngle,
			const double& xStep,
			const double& yStep,
			const double& zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("DuctGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"ZAngle",zAngle);
  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);

  return;

}


}  // namespace setVariable
