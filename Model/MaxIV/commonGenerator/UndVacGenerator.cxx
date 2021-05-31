/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/UndVacGenerator.cxx
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
#include "CFFlanges.h"

#include "UndVacGenerator.h"

namespace setVariable
{

UndVacGenerator::UndVacGenerator() :
  nSegment(3),radius(12.25),segLength(163.0),
  wallThick(0.6),flangeRadius(15.0),flangeLength(2.7),
  portOutLength(6.755),portRadius(CF40::innerRadius),
  accessRadius(CF120::innerRadius),accessFlangeRadius(CF120::flangeRadius),
  accessFlangeLength(CF120::flangeLength),

  smallRadius(CF100::innerRadius),smallFlangeRadius(CF100::flangeRadius),
  smallFlangeLength(CF100::flangeLength),

  magGap(8.0),magRadius(CF40::innerRadius),
  magLength(2.5),magBellowLen(9.3), magFlangeRadius(CF40::flangeRadius),
  magFlangeLength(CF40::flangeLength),magSupportRadius(0.75),

  vacRadius(CF100::innerRadius),vacLength(8.0),vacHeight(8.0),
  vacFlangeRadius(CF100::flangeRadius),
  vacFlangeLength(CF100::flangeLength),

  preLength(16.5),

  voidMat("Void"),wallMat("Stainless304L"),supportMat("Nickel")
  /*!
    Constructor and defaults
  */
{}

UndVacGenerator::~UndVacGenerator()
 /*!
   Destructor
 */
{}


void
UndVacGenerator::generateUndVac(FuncDataBase& Control,
				const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("UndVacGenerator","generateUndVac");

  Control.addVariable(keyName+"NSegment",nSegment);
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"SegLength",segLength);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"PortOutLength",portOutLength);
  Control.addVariable(keyName+"PortRadius",portRadius);

  Control.addVariable(keyName+"AccessRadius",accessRadius);
  Control.addVariable(keyName+"AccessFlangeRadius",accessFlangeRadius);
  Control.addVariable(keyName+"AccessFlangeLength",accessFlangeLength);

  Control.addVariable(keyName+"SmallRadius",smallRadius);
  Control.addVariable(keyName+"SmallFlangeRadius",smallFlangeRadius);
  Control.addVariable(keyName+"SmallFlangeLength",smallFlangeLength);
  
  Control.addVariable(keyName+"MagGap",magGap);
  Control.addVariable(keyName+"MagRadius",magRadius);
  Control.addVariable(keyName+"MagLength",magLength);
  Control.addVariable(keyName+"MagBellowLen",magBellowLen);
  Control.addVariable(keyName+"MagFlangeRadius",magFlangeRadius);
  Control.addVariable(keyName+"MagFlangeLength",magFlangeLength);
  Control.addVariable(keyName+"MagSupportRadius",magSupportRadius);
  
  Control.addVariable(keyName+"VacRadius",vacRadius);
  Control.addVariable(keyName+"VacLength",vacLength);
  Control.addVariable(keyName+"VacHeight",vacHeight);
  Control.addVariable(keyName+"VacFlangeRadius",vacFlangeRadius);
  Control.addVariable(keyName+"VacFlangeLength",vacFlangeLength);

  Control.addVariable(keyName+"PreLength",preLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"SupportMat",supportMat);


  return;

}

///\cond TEMPLATE



///\endcond TEMPLATE

}  // NAMESPACE setVariable
