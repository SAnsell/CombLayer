/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/MonoSlitsJJGenerator.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#include "MonoSlitsJJGenerator.h"

namespace setVariable
{
MonoSlitsJJGenerator
::MonoSlitsJJGenerator() :
  length(14.0),
  adapterThick(2.4),
  adapterInnerRadius(CF40::innerRadius),
  auxPortAngleStep(28.0), // [1]
  auxPortCenterAngle(3.0), // [1]
  auxPortFlangeLength(CF40::flangeLength), // [4]
  auxPortFlangeRadius(CF40::flangeRadius), // [4]
  auxPortInnerRadius(CF40::innerRadius), // [4]
  auxPortOpticalAxisOffset(0.9), // [1]
  auxPortWallThick(CF40::wallThick), // [4]
  mainInnerRadius(8.0),
  mainWallThick(2.125),
  bladeAngle(0.5),
  bladeLongEdge(4.0),
  bladeM1Pos(-1.5),
  bladeM2Pos(-1.5),
  bladeM3Pos(-1.5),
  bladeM4Pos(-1.5),
  bladePortCenterDist(2.5),
  bladePortDist(6.0),
  bladePortFlangeLength(CF40::flangeLength),
  bladePortFlangeRadius(CF40::flangeRadius),
  bladePortInnerRadius(CF25::innerRadius),
  bladePortLength(15.405-CF40::flangeLength),
  bladePortWallThick(CF25::wallThick),
  bladeShortEdge(3.0),
  bladeThick(0.2),
  bladeThreadLength(19.0),
  bladeThreadRadius(0.5),
  bladeToThreadDist(0.1),
  bladeMat("TungstenCarbide"),
  mainMat("SteelUnknownGrade")

  /*!
    Constructor and defaults
  */
{}

void MonoSlitsJJGenerator
::generate(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("MonoSlitsJJGenerator","generate");

  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"AdapterThick",adapterThick);
  Control.addVariable(keyName+"AdapterInnerRadius",adapterInnerRadius);

  Control.addVariable(keyName+"AuxPortAngleStep",auxPortAngleStep);
  Control.addVariable(keyName+"AuxPortCenterAngle",auxPortCenterAngle);
  Control.addVariable(keyName+"AuxPortFlangeLength",auxPortFlangeLength);
  Control.addVariable(keyName+"AuxPortFlangeRadius",auxPortFlangeRadius);
  Control.addVariable(keyName+"AuxPortInnerRadius",auxPortInnerRadius);
  Control.addVariable(keyName+"AuxPortOpticalAxisOffset",auxPortOpticalAxisOffset);
  Control.addVariable(keyName+"AuxPortWallThick",auxPortWallThick);

  Control.addVariable(keyName+"MainInnerRadius",mainInnerRadius);
  Control.addVariable(keyName+"MainWallThick",mainWallThick);

  Control.addVariable(keyName+"BladeAngle",bladeAngle);
  Control.addVariable(keyName+"BladeLongEdge",bladeLongEdge);
  Control.addVariable(keyName+"BladeM1Pos",bladeM1Pos);
  Control.addVariable(keyName+"BladeM2Pos",bladeM2Pos);
  Control.addVariable(keyName+"BladeM3Pos",bladeM3Pos);
  Control.addVariable(keyName+"BladeM4Pos",bladeM4Pos);
  Control.addVariable(keyName+"BladePortCenterDist",bladePortCenterDist);
  Control.addVariable(keyName+"BladePortDist",bladePortDist);
  Control.addVariable(keyName+"BladePortFlangeLength",bladePortFlangeLength);
  Control.addVariable(keyName+"BladePortFlangeRadius",bladePortFlangeRadius);
  Control.addVariable(keyName+"BladePortInnerRadius",bladePortInnerRadius);
  Control.addVariable(keyName+"BladePortLength",bladePortLength);
  Control.addVariable(keyName+"BladePortWallThick",bladePortWallThick);
  Control.addVariable(keyName+"BladeShortEdge",bladeShortEdge);
  Control.addVariable(keyName+"BladeThick",bladeThick);
  Control.addVariable(keyName+"BladeThreadLength",bladeThreadLength);
  Control.addVariable(keyName+"BladeThreadRadius",bladeThreadRadius);
  Control.addVariable(keyName+"BladeToThreadDist",bladeToThreadDist);

  Control.addVariable(keyName+"BladeMat",bladeMat);
  Control.addVariable(keyName+"MainMat",mainMat);
}

}  // NAMESPACE setVariable
