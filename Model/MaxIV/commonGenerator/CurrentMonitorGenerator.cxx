/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/CurrentMonitorGenerator.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "CFFlanges.h"
#include "CurrentMonitorGenerator.h"

namespace setVariable
{

CurrentMonitorGenerator::CurrentMonitorGenerator() :
  length(10.0),frontRadius(CF63::innerRadius),midRadius(4.0),
  backRadius(5.5),
  outerRadius(7.6),
  wallThick(2.0),
  voidMat("Void"),wallMat("Stainless316L")
  /*!
    Constructor and defaults
  */
{}

CurrentMonitorGenerator::~CurrentMonitorGenerator()
 /*!
   Destructor
 */
{}

void
CurrentMonitorGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("CurrentMonitorGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FrontRadius",frontRadius);
  Control.addVariable(keyName+"MidRadius",midRadius);
  Control.addVariable(keyName+"BackRadius",backRadius);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}


}  // namespace setVariable
