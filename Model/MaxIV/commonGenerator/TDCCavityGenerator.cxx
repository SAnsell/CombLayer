/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/TDCCavityGenerator.cxx
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

#include "TDCCavityGenerator.h"

namespace setVariable
{

TDCCavityGenerator::TDCCavityGenerator() :
  length(312.363),radius(6.0),innerRadius(1.15),
  wallThick(1.5),
  nCells(89),wallMat("Copper"),
  couplerThick(6.0),
  couplerWidth(21.2)
  /*!
    Constructor and defaults
  */
{}

TDCCavityGenerator::~TDCCavityGenerator()
 /*!
   Destructor
 */
{}

void
TDCCavityGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("TDCCavityGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"NCells",nCells);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"CouplerThick",couplerThick);
  Control.addVariable(keyName+"CouplerWidth",couplerWidth);

  return;

}


}  // namespace setVariable
