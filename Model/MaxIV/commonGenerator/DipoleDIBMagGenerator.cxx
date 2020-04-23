/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/DipoleDIBMagGenerator.cxx
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

#include "DipoleDIBMagGenerator.h"

namespace setVariable
{

DipoleDIBMagGenerator::DipoleDIBMagGenerator() :
  magOffset(6.0),magHeight(2.0),magWidth(19.3),
  magLength(70.71),magInnerWidth(6.58),
  frameHeight(19.0),
  frameWidth(28.4),
  frameLength(55.0),
  voidMat("Void"),coilMat("Copper"),frameMat("Iron")
  /*!
    Constructor and defaults
  */
{}

DipoleDIBMagGenerator::~DipoleDIBMagGenerator()
 /*!
   Destructor
 */
{}

void
DipoleDIBMagGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("DipoleDIBMagGenerator","generate");

  Control.addVariable(keyName+"MagOffset",magOffset);
  Control.addVariable(keyName+"MagHeight",magHeight);
  Control.addVariable(keyName+"MagWidth",magWidth);
  Control.addVariable(keyName+"MagLength",magLength);
  Control.addVariable(keyName+"MagInnerWidth",magInnerWidth);
  Control.addVariable(keyName+"FrameHeight",frameHeight);
  Control.addVariable(keyName+"FrameWidth",frameWidth);
  Control.addVariable(keyName+"FrameLength",frameLength);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"FrameMat",frameMat);

  return;

}


}  // xraySystem setVariable
