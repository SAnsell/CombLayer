/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/CardanBellowGenerator.cxx
 *
 * Copyright (c) 2026 U. Friman-Gayer
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

#include "CardanBellowGenerator.h"

namespace setVariable
{

CardanBellowGenerator::CardanBellowGenerator() :
  angle(0.0),bellowStep(0.5),
  bellowThick(0.8*(CF40::flangeRadius-CF40::innerRadius)),
  bellowsVolumeFraction(0.0),flangeLength(CF40::flangeLength),
  flangeRadius(CF40::flangeRadius),length(10.0),
  pipeInnerRadius(CF40::innerRadius),pipeWallThick(CF40::wallThick),
  bellowBaseMat("SteelUnknownGrade"),pipeMat("SteelUnknownGrade")
{}

double CardanBellowGenerator::bellowLength() const {
  return (length-2.0*(flangeLength+bellowStep));
}

double CardanBellowGenerator::bellowSheetVolumeOverPi(
  const int nFolds,const double bellowSheetThick) const {
  return (
    (pipeInnerRadius+bellowSheetThick)*(pipeInnerRadius+bellowSheetThick)
    *(bellowLength()-2.0*nFolds*bellowSheetThick)
    +(
      (pipeInnerRadius+bellowThick)*(pipeInnerRadius+bellowThick)
      -pipeInnerRadius*pipeInnerRadius
    )*2.0*nFolds*bellowSheetThick
  );
}

void CardanBellowGenerator::setMat(
  const std::string baseMat, const int nFolds, const double bellowSheetThick){
    bellowBaseMat = baseMat;
    const double bellowsAvailableVolumeOverPi = (
      (pipeInnerRadius+bellowThick)*(pipeInnerRadius+bellowThick)
      -pipeInnerRadius*pipeInnerRadius
    );
    bellowsVolumeFraction = (
      bellowSheetVolumeOverPi(nFolds,bellowSheetThick)
      /bellowsAvailableVolumeOverPi);
}

template<typename CF> void
CardanBellowGenerator::setCF(){
  flangeLength=CF40::flangeLength;
  flangeRadius=CF40::flangeRadius;
  pipeInnerRadius=CF40::innerRadius;
  pipeWallThick=CF40::wallThick;
}

void CardanBellowGenerator::generateBellows(
  FuncDataBase& Control,const std::string& keyName) const
{
  ELog::RegMethod RegA("BellowGenerator","generatorBellow");

  Control.addVariable(keyName+"Angle",angle);
  Control.addVariable(keyName+"BellowStep",bellowStep);
  Control.addVariable(keyName+"BellowThick",bellowThick);
  Control.addVariable(keyName+"BellowsVolumeFraction",bellowsVolumeFraction);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"PipeInnerRadius",pipeInnerRadius);
  Control.addVariable(keyName+"PipeWallThick",pipeWallThick);
  Control.addVariable(keyName+"BellowBaseMat",bellowBaseMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
}

///\cond TEMPLATE

  template void CardanBellowGenerator::setCF<CF40>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
