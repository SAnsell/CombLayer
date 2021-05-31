/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/LocalShieldingGenerator.cxx
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

#include "LocalShieldingGenerator.h"

namespace setVariable
{

LocalShieldingGenerator::LocalShieldingGenerator() :
  length(10.0),width(60.0),height(40.0),
  midHoleWidth(0.0),midHoleHeight(0.0),
  cornerWidth(0.0),cornerHeight(0.0),
  zStep(0.0),mainMat("Lead"),
  cType("both"),
  opt("")
  /*!
    Constructor and defaults
  */
{}

LocalShieldingGenerator::~LocalShieldingGenerator()
 /*!
   Destructor
 */
{}

void
LocalShieldingGenerator::setSize(const double L, const double W, const double H)
/*!
  Wall size setter
  \param L :: total length
  \param W :: total width
  \param W :: total height
 */
{
  length = L;
  width  = W;
  height = H;

  return;
}

void
LocalShieldingGenerator::setMidHole(const double W, const double H)
/*!
  Mid hole size setter
  \param W :: width
  \param W :: height
 */
{
  midHoleWidth  = W;
  midHoleHeight = H;

  return;
}

void
LocalShieldingGenerator::setCorner(const double W, const double H, const std::string type)
/*!
  Corner size setter. The corners are not built if at least one of the dimensions is zero
  \param W :: width
  \param W :: height
 */
{
  cornerWidth  = W;
  cornerHeight = H;
  cType = type;

  return;
}

void
LocalShieldingGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("LocalShieldingGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"MidHoleWidth",midHoleWidth);
  Control.addVariable(keyName+"MidHoleHeight",midHoleHeight);
  Control.addVariable(keyName+"CornerHeight",cornerHeight);
  Control.addVariable(keyName+"CornerWidth",cornerWidth);
  Control.addVariable(keyName+"ZStep",zStep);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"CornerType",cType);
  Control.addVariable(keyName+"Option",opt);

  return;
}


}  // namespace setVariable
