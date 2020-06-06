/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/TWCavityGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "TWCavityGenerator.h"

namespace setVariable
{

TWCavityGenerator::TWCavityGenerator() :
  nCells(89),
  cellLength(2.833), // David Olsson 2020-06-05
  cellRadius(5.961), // David Olsson 2020-06-05
  irisRadius(1.16), // David Olsson 2020-06-05
  irisLength(0.5), // David Olsson 2020-06-05
  couplerLength(6.0),
  couplerWidth(21.2),
  wallThick(1.489), // David Olsson 2020-06-05
  wallMat("Copper")
  /*!
    Constructor and defaults
  */
{}

TWCavityGenerator::~TWCavityGenerator()
 /*!
   Destructor
 */
{}

void
TWCavityGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("TWCavityGenerator","generate");

  Control.addVariable(keyName+"NCells",nCells);
  Control.addVariable(keyName+"CellLength",cellLength);
  Control.addVariable(keyName+"CellRadius",cellRadius);
  Control.addVariable(keyName+"IrisLength",irisLength);
  Control.addVariable(keyName+"IrisRadius",irisRadius);
  Control.addVariable(keyName+"CouplerLength",couplerLength);
  Control.addVariable(keyName+"CouplerWidth",couplerWidth);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}


}  // namespace setVariable
