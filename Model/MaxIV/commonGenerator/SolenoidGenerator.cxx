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
  length(20.0),frameWidth(39),frameThick(2.5),
  frameRadius(19.5),
  coilThick(3.0),coilRadius(17),
  coilGap(0.3),
  penRadius(3.8),
  frameMat("Iron"),coilMat("Copper"), // TODO: maybe StbTCABL?
  voidMat("Void"),
  nCoils(4)
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
  Control.addVariable(keyName+"FrameThick",frameThick);
  Control.addVariable(keyName+"FrameRadius",frameRadius);
  Control.addVariable(keyName+"CoilThick",coilThick);
  Control.addVariable(keyName+"CoilRadius",coilRadius);
  Control.addVariable(keyName+"CoilGap",coilGap);
  Control.addVariable(keyName+"PenetraionRadius",penRadius);
  Control.addVariable(keyName+"FrameMat",frameMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"NCoils",nCoils);

  return;

}


}  // namespace setVariable
