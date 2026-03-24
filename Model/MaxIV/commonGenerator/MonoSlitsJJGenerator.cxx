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
  length(14.0), // [2]
  adapterThick(2.4), // [2]
  adapterInnerRadius(CF40::innerRadius), // [4]
  auxPortAngleStep(28.0), // [1]
  auxPortCenterAngle(3.0), // [1]
  auxPortFlangeLength(CF40::flangeLength), // [4]
  auxPortFlangeRadius(CF40::flangeRadius), // [4]
  auxPortInnerRadius(CF40::innerRadius), // [4]
  auxPortOpticalAxisOffset(0.9), // [1]
  auxPortWallThick(CF40::wallThick), // [4]
  mainInnerRadius(8.0), // [1]
  mainWallThick(2.125), // [1]
  baseDepth(10.925), // [2]
  baseThick(1.2), // [1]
  baseWidth(22.0), // [1]
  bladeAngle(0.5), // [4]
  bladeLongEdge(4.0), // [1]
  bladeM1InPos(-1.5), // [4]
  bladeM1OutPos(1.5), // [4]
  bladeM1Pos(-1.5), // [4]
  bladeM2InPos(-1.5), // [4]
  bladeM2OutPos(1.5), // [4]
  bladeM2Pos(-1.5), // [4]
  bladeM3InPos(-1.5), // [4]
  bladeM3OutPos(1.5), // [4]
  bladeM3Pos(-1.5), // [4]
  bladeM4InPos(-1.5), // [4]
  bladeM4OutPos(1.5), // [4]
  bladeM4Pos(-1.5), // [4]
  bladePortCenterDist(2.5), // [2]
  bladePortDist(6.0), // [1]
  bladePortFlangeLength(CF40::flangeLength), // [1]
  bladePortFlangeRadius(CF40::flangeRadius), // [1]
  bladePortInnerRadius(CF25::innerRadius), // [1]
  // Total length of port including motor: 201.05 mm [2]
  // Type of motor                       : PK245m-01b [3]
  // Length of motor                     : 67 mm [5]
  // -> Port length (including cap)      : 201.05 mm - 67 mm = 154.05 mm
  bladePortLength(15.405-CF40::flangeLength),
  bladePortWallThick(CF25::wallThick), // [1]
  bladeShortEdge(3.0), // [1]
  bladeThick(0.2), // [4]
  bladeThreadLength(19.0), // Estimated from [1]
  bladeThreadRadius(0.5), // Estimated from [1]
  bladeToThreadDist(0.1), // Estimated from [1]
  bladeMat("TungstenCarbide"), // [4]
  mainMat("SteelUnknownGrade")

  /*!
    Constructor and defaults
  */
{}

void MonoSlitsJJGenerator
::generate(FuncDataBase& Control,const std::string& keyName,
  const int m1PosFlag,const int m2PosFlag,const int m3PosFlag,const int m4PosFlag)
  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param m1PosFlag :: 0     : blade at BladeM1InPos
                        1     : blade at BladeM1OutPos
                        other : blade at BladeM1Pos
    \param m2PosFlag :: see m1PosFlag
    \param m3PosFlag :: see m1PosFlag
    \param m4PosFlag :: see m1PosFlag
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

  Control.addVariable(keyName+"BaseDepth",baseDepth);
  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"BaseWidth",baseWidth);

  Control.addVariable(keyName+"BladeAngle",bladeAngle);
  Control.addVariable(keyName+"BladeLongEdge",bladeLongEdge);
  Control.addVariable(keyName+"BladeM1InPos",bladeM1InPos);
  Control.addVariable(keyName+"BladeM1PosFlag",m1PosFlag);
  Control.addVariable(keyName+"BladeM1OutPos",bladeM1OutPos);
  Control.addVariable(keyName+"BladeM1Pos",bladeM1Pos);
  Control.addVariable(keyName+"BladeM2InPos",bladeM2InPos);
  Control.addVariable(keyName+"BladeM2PosFlag",m2PosFlag);
  Control.addVariable(keyName+"BladeM2OutPos",bladeM2OutPos);
  Control.addVariable(keyName+"BladeM2Pos",bladeM2Pos);
  Control.addVariable(keyName+"BladeM3InPos",bladeM3InPos);
  Control.addVariable(keyName+"BladeM3PosFlag",m3PosFlag);
  Control.addVariable(keyName+"BladeM3OutPos",bladeM3OutPos);
  Control.addVariable(keyName+"BladeM3Pos",bladeM3Pos);
  Control.addVariable(keyName+"BladeM4InPos",bladeM4InPos);
  Control.addVariable(keyName+"BladeM4PosFlag",m4PosFlag);
  Control.addVariable(keyName+"BladeM4OutPos",bladeM4OutPos);
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
