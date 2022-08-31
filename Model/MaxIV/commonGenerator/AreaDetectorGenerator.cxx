/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/AreaDetectorGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "AreaDetectorGenerator.h"

namespace setVariable
{


AreaDetectorGenerator::AreaDetectorGenerator() :
  length(7.0),width(35.0),height(35.0),
  screenThick(0.1),screenDepth(0.4),
  wallThick(0.5),
  voidMat("Void"),
  detMat("B4C"),screenMat("Diamond"),
  wallMat("Aluminium")
  /*!
    Constructor and defaults
  */
{}

AreaDetectorGenerator::~AreaDetectorGenerator()
 /*!
   Destructor
 */
{}

void
AreaDetectorGenerator::generateDetector(FuncDataBase& Control,
					const std::string& keyName,
					const double yStep) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Distance from front of flight tube
  */
{
  ELog::RegMethod RegA("AreaDetectorGenerator","generateAreaDetector");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"ScreenThick",screenThick);
  Control.addVariable(keyName+"ScreenDepth",screenDepth);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"OuterMat",voidMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"DetMat",detMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"ScreenMat",screenMat);
  
  
  return;

}

}  // NAMESPACE setVariable
