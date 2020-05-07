/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/YagScreenGenerator.cxx
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

#include "YagScreenGenerator.h"

namespace setVariable
{

YagScreenGenerator::YagScreenGenerator() :
  jbLength(10.0),jbWidth(5.0),jbHeight(15.0),
  jbWallThick(0.15),
  jbMat("Aluminium"),
  ffLength(19.5),
  ffInnerRadius(1.5),
  ffWallThick(0.2),
  ffWallMat("Stainless304L"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

YagScreenGenerator::~YagScreenGenerator()
 /*!
   Destructor
 */
{}

void
YagScreenGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("YagScreenGenerator","generate");

  Control.addVariable(keyName+"JBLength",jbLength);
  Control.addVariable(keyName+"JBWidth",jbWidth);
  Control.addVariable(keyName+"JBHeight",jbHeight);
  Control.addVariable(keyName+"JBWallThick",jbWallThick);
  Control.addVariable(keyName+"FFLength",ffLength);
  Control.addVariable(keyName+"FFInnerRadius",ffInnerRadius);
  Control.addVariable(keyName+"FFWallThick",ffWallThick);
  Control.addVariable(keyName+"FFWallMat",ffWallMat);
  Control.addVariable(keyName+"JBMat",jbMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}


}  // tdcSystem setVariable
