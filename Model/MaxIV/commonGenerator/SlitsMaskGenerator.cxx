/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/SlitsMaskGenerator.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "SlitsMaskGenerator.h"

namespace setVariable
{

SlitsMaskGenerator::SlitsMaskGenerator() :
  slitLength(0.2),slitWidth(3.0),slitHeight(4.5),
  wallThick(0.15),
  portRadius(1.95),
  frontPortLength(8.75),
  backPortLength(8.75),
  leftPortLength(10.5),
  rightPortLength(10.5),
  bottomPortLength(8.2),
  topPortLength(12.5),
  outerFlangeRadius(3.5),
  outerFlangeThick(1.2),
  outerFlangeCapThick(1.2),
  leftFlangeCapWindowMat("LeadGlass"),
  slitsMat("Tungsten"),
  wallMat("Stainless304"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{
  ELog::EM << "Slits width/height are arbitrary (big enough to cover the beam)" << ELog::endWarn;
}

SlitsMaskGenerator::~SlitsMaskGenerator()
 /*!
   Destructor
 */
{}

void
SlitsMaskGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("SlitsMaskGenerator","generate");

  Control.addVariable(keyName+"SlitLength",slitLength);
  Control.addVariable(keyName+"SlitWidth",slitWidth);
  Control.addVariable(keyName+"SlitHeight",slitHeight);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"FrontPortLength",frontPortLength);
  Control.addVariable(keyName+"BackPortLength",backPortLength);
  Control.addVariable(keyName+"LeftPortLength",leftPortLength);
  Control.addVariable(keyName+"RightPortLength",rightPortLength);
  Control.addVariable(keyName+"BottomPortLength",bottomPortLength);
  Control.addVariable(keyName+"TopPortLength",topPortLength);
  Control.addVariable(keyName+"OuterFlangeRadius",outerFlangeRadius);
  Control.addVariable(keyName+"OuterFlangeThickness",outerFlangeThick);
  Control.addVariable(keyName+"OuterFlangeCapThickness",outerFlangeCapThick);
  Control.addVariable(keyName+"LeftFlangeCapWindowMat",leftFlangeCapWindowMat);
  Control.addVariable(keyName+"SlitsMat",slitsMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}


}  // namespace setVariable
