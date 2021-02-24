/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/SPFCameraShieldGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "SPFCameraShieldGenerator.h"

namespace setVariable
{

SPFCameraShieldGenerator::SPFCameraShieldGenerator() :
  length(40.0),width(20.0),height(25.0),
  wallThick(5.0),
  roofLength(10.0),
  roofAngle(30.0),
  roofXShift(7.0),
  roofYShift(10.0),
  mat("Lead")
  /*!
    Constructor and defaults
  */
{}

SPFCameraShieldGenerator::~SPFCameraShieldGenerator()
 /*!
   Destructor
 */
{}

void
SPFCameraShieldGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("SPFCameraShieldGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"RoofLength",roofLength);
  Control.addVariable(keyName+"RoofAngle",roofAngle);
  Control.addVariable(keyName+"RoofXShift",roofXShift);
  Control.addVariable(keyName+"RoofYShift",roofYShift);
  Control.addVariable(keyName+"Mat",mat);

  return;

}


}  // namespace setVariable
