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
  slitLength(0.2),
  slitWidth(2.5),
  slitHeight(4.5),
  slitSupportLength(0.5),
  slitSupportWidth(3.5),
  slitSupportHeight(5.1),
  chamberLength(6.0),
  chamberWidth(6.4),
  chamberHeight(6.0),
  wallThick(0.15),
  portRadius(1.95),
  frontPortLength(8.75-1.15),
  backPortLength(8.75-1.15),
  leftPortLength(10.5+1.9),
  rightPortLength(10.5+1.9),
  bottomPortLength(8.2),
  topPortLength(12.5),
  innerFlangeRadius(4.0),
  innerFlangeThick(1.7),
  outerFlangeRadius(3.5),
  outerFlangeThick(1.2),
  outerFlangeCapThick(1.2),
  leftFlangeCapWindowThick(0.2),
  leftFlangeCapWindowMat("SiO2"),
  slitsMat("Tungsten"),
  slitSupportMat("Aluminium"),
  wallMat("Stainless304"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{
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
  Control.addVariable(keyName+"SlitSupportLength",slitSupportLength);
  Control.addVariable(keyName+"SlitSupportWidth",slitSupportWidth);
  Control.addVariable(keyName+"SlitSupportHeight",slitSupportHeight);
  Control.addVariable(keyName+"ChamberLength",chamberLength);
  Control.addVariable(keyName+"ChamberWidth",chamberWidth);
  Control.addVariable(keyName+"ChamberHeight",chamberHeight);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"FrontPortLength",frontPortLength);
  Control.addVariable(keyName+"BackPortLength",backPortLength);
  Control.addVariable(keyName+"LeftPortLength",leftPortLength);
  Control.addVariable(keyName+"RightPortLength",rightPortLength);
  Control.addVariable(keyName+"BottomPortLength",bottomPortLength);
  Control.addVariable(keyName+"TopPortLength",topPortLength);
  Control.addVariable(keyName+"InnerFlangeRadius",innerFlangeRadius);
  Control.addVariable(keyName+"InnerFlangeThick",innerFlangeThick);
  Control.addVariable(keyName+"OuterFlangeRadius",outerFlangeRadius);
  Control.addVariable(keyName+"OuterFlangeThickness",outerFlangeThick);
  Control.addVariable(keyName+"OuterFlangeCapThickness",outerFlangeCapThick);
  Control.addVariable(keyName+"LeftFlangeCapWindowThick",leftFlangeCapWindowThick);
  Control.addVariable(keyName+"LeftFlangeCapWindowMat",leftFlangeCapWindowMat);
  Control.addVariable(keyName+"SlitsMat",slitsMat);
  Control.addVariable(keyName+"SlitSupportMat",slitSupportMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}


}  // namespace setVariable
