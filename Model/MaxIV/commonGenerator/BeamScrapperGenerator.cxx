/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/BeamScrapperGenerator.cxx
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

#include "ScreenGenerator.h"
#include "BeamScrapperGenerator.h"

namespace setVariable
{

BeamScrapperGenerator::BeamScrapperGenerator() :

  tubeRadius(0.1),tubeOffset(1.25),
  tubeWall(0.1),plateOffset(2.0),
  plateAngle(-15.0),plateLength(3.0),
  plateHeight(1.0),
  plateThick(1.0),tubeWidth(2.0),
  tubeHeight(4.0),inletZOffset(-1.0),

  voidMat("Void"),
  waterMat("H2O"),
  copperMat("Copper")  
  /*!
    Constructor and defaults
  */
{}

BeamScrapperGenerator::~BeamScrapperGenerator()
 /*!
   Destructor
 */
{}


void
BeamScrapperGenerator::generateScreen(FuncDataBase& Control,
				      const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("BeamScrapperGenerator","generate");

  Control.addVariable(keyName+"TubeRadius",tubeRadius);
  Control.addVariable(keyName+"TubeOffset",tubeOffset);
  Control.addVariable(keyName+"TubeWall",tubeWall);

  Control.addVariable(keyName+"PlateOffset",plateOffset);
  Control.addVariable(keyName+"PlateAngle",plateAngle);
  Control.addVariable(keyName+"PlateLength",plateLength);
  Control.addVariable(keyName+"PlateHeight",plateHeight);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"TubeWidth",tubeWidth);
  Control.addVariable(keyName+"TubeHeight",tubeHeight);

  Control.addVariable(keyName+"InletZOffset",inletZOffset);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WaterMat",waterMat);
  Control.addVariable(keyName+"CopperMat",copperMat);
  
 
 return;

}

}  // namespace setVariable
