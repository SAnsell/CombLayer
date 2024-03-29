/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CooledScreenGenerator.cxx
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
#include "CooledScreenGenerator.h"

namespace setVariable
{

CooledScreenGenerator::CooledScreenGenerator() :
  ScreenGenerator(),

  screenAngle(45.0),copperWidth(3.0),
  copperHeight(4.0),copperThick(0.8),

  innerRadius(1.0),screenThick(0.8),

  copperMat("Stainless304L"),
  screenMat("SiO2")
  
  /*!
    Constructor and defaults
  */
{}

CooledScreenGenerator::~CooledScreenGenerator()
 /*!
   Destructor
 */
{}


void
CooledScreenGenerator::generateScreen(FuncDataBase& Control,
				   const std::string& keyName,
				   const bool inBeam) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param inBeam :: is item in beam
  */
{
  ELog::RegMethod RegA("CooledScreenGenerator","generate");

  ScreenGenerator::generateScreen(Control,keyName,inBeam);
  
  Control.addVariable(keyName+"ScreenAngle",screenAngle);
  Control.addVariable(keyName+"CopperWidth",copperWidth);
  Control.addVariable(keyName+"CopperHeight",copperHeight);
  Control.addVariable(keyName+"CopperThick",copperThick);

  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"ScreenThick",screenThick);

  Control.addVariable(keyName+"CopperMat",copperMat);
  Control.addVariable(keyName+"ScreenMat",screenMat);
 
 return;

}

}  // namespace setVariable
