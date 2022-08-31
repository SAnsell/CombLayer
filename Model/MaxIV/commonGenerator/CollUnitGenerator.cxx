/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CollUnitGenerator.cxx
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

#include "CollUnitGenerator.h"

namespace setVariable
{

CollUnitGenerator::CollUnitGenerator() :
  tubeRadius(0.25),tubeWall(0.2),
  tubeYStep(1.0),tubeTopGap(0.6),
  tubeMainGap(1.2),tubeTopLength(7.0),
  tubeDownLength(2.0),
  plateThick(1.0),plateLength(10.0),
  plateWidth(5.0),nHoles(5),
  holeGap(0.8),holeRadius(0.2),

  voidMat("Void"),plateMat("Stainless304L"),
  pipeMat("Copper"),waterMat("H2O")
  /*!
    Constructor and defaults
  */
{}

CollUnitGenerator::~CollUnitGenerator()
 /*!
   Destructor
 */
{}


void
CollUnitGenerator::generateScreen(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("CollUnitGenerator","generateScreen");

  Control.addVariable(keyName+"TubeRadius",tubeRadius);
  Control.addVariable(keyName+"TubeWall",tubeWall);
  Control.addVariable(keyName+"TubeYStep",tubeYStep);
  Control.addVariable(keyName+"TubeTopGap",tubeTopGap);
  Control.addVariable(keyName+"TubeMainGap",tubeMainGap);
  Control.addVariable(keyName+"TubeTopLength",tubeTopLength);
  Control.addVariable(keyName+"TubeDownLength",tubeDownLength);


  Control.addVariable(keyName+"PlateThick",plateThick);
  Control.addVariable(keyName+"PlateLength",plateLength);
  Control.addVariable(keyName+"PlateWidth",plateWidth);

  Control.addVariable(keyName+"NHoles",nHoles);
  Control.addVariable(keyName+"HoleGap",holeGap);
  Control.addVariable(keyName+"HoleRadius",holeRadius);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PlateMat",plateMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
  Control.addVariable(keyName+"WaterMat",waterMat);

  return;
}

}  // namespace setVariable
