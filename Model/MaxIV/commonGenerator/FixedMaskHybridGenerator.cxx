/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/FixedMaskHybridGenerator.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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

#include "FixedMaskHybridGenerator.h"

// [1] S1-2-2AG00580.pdf

namespace setVariable
{

FixedMaskHybridGenerator::FixedMaskHybridGenerator() :
  length(40.0), // [1]
  radius(2.9), // [1]: diameter = 5.8, in the middle the max diameter is 6.0, but use conservative
  flangeLength(2.0),  // TODO: guess
  flangeRadius(7.7), // TODO: guess,
  outWidth(1.12), // TODO: guess
  outHeight(1.12), // TODO: guess,
  outAngle(0.7), // [1]
  mat("Copper"), // TODO: use GLIDCOP
  flangeMat("Stainless304L") // [1]: SUS304L
  /*!
    Constructor and defaults
  */
{}

FixedMaskHybridGenerator::~FixedMaskHybridGenerator()
 /*!
   Destructor
 */
{}

void
FixedMaskHybridGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("FixedMaskHybridGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"OutWidth",outWidth);
  Control.addVariable(keyName+"OutHeight",outHeight);
  Control.addVariable(keyName+"OutAngle",outAngle);
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;

}


}  // namespace setVariable
