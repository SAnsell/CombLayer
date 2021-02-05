/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/DipoleExtractGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "DipoleExtractGenerator.h"

namespace setVariable
{

DipoleExtractGenerator::DipoleExtractGenerator() :
  length(118.5),width(2.5),
  height(2.7),wideHeight(0.51),
  wideWidth(5.5),exitLength(0.44),
  wallThick(0.1),edgeThick(1.4),
  voidMat("Void"),wallMat("Copper"),
  outerMat("Void")
  /*!
    Constructor and defaults
  */
{}
  
DipoleExtractGenerator::~DipoleExtractGenerator() 
 /*!
   Destructor
 */
{}

void
DipoleExtractGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("DipoleExtractGenerator","generatePipe");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"WideHeight",wideHeight);
  Control.addVariable(keyName+"WideWidth",wideWidth);
  Control.addVariable(keyName+"ExitLength",exitLength);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"EdgeThick",edgeThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"OuterMat",outerMat);

  return;
}

  
}  // NAMESPACE setVariable
