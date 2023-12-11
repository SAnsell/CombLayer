/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/RFGunGenerator.cxx
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

#include "RFGunGenerator.h"

namespace setVariable
{

RFGunGenerator::RFGunGenerator() :
  length(9.7), // TODO
  cavityRadius(4.0),cavityLength(1.0),
  cavitySideWallThick(1.9), // TODO
  cavityOffset(4.0), // TODO
  irisRadius(1.5), // measured
  irisStretch(0.1),
  wallThick(1.0), // TODO
  frontFlangeThick(2.2), // TODO
  mainMat("Void"),wallMat("Stainless316L") // TODO
  /*!
    Constructor and defaults
  */
{}

RFGunGenerator::~RFGunGenerator()
 /*!
   Destructor
 */
{}

void
RFGunGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("RFGunGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"CavityRadius",cavityRadius);
  Control.addVariable(keyName+"CavityLength",cavityLength);
  Control.addVariable(keyName+"CavitySideWallThick",cavitySideWallThick);
  Control.addVariable(keyName+"CavityOffset",cavityOffset);
  Control.addVariable(keyName+"IrisRadius",irisRadius);
  Control.addVariable(keyName+"IrisStretch",irisStretch);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FrontFlangeThick",frontFlangeThick);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}


}  // namespace setVariable
