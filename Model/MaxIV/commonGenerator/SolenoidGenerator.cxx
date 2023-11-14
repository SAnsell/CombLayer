/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/SolenoidGenerator.cxx
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

#include "SolenoidGenerator.h"

namespace setVariable
{

SolenoidGenerator::SolenoidGenerator() :
  length(10.0),frameWidth(5.0),spacerThick(15.0),frameThick(1.0),
  frameMat("Void"),coilMat("Stainless316L"),
  nCoils(4),
  nFrameFacets(8)
  /*!
    Constructor and defaults
  */
{}

SolenoidGenerator::~SolenoidGenerator()
 /*!
   Destructor
 */
{}

void
SolenoidGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("SolenoidGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FrameWidth",frameWidth);
  Control.addVariable(keyName+"SpacerThick",spacerThick);
  Control.addVariable(keyName+"FrameThick",frameThick);
  Control.addVariable(keyName+"FrameMat",frameMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"NCoils",nCoils);
  Control.addVariable(keyName+"NFrameFacets",nFrameFacets);

  return;

}


}  // namespace setVariable
