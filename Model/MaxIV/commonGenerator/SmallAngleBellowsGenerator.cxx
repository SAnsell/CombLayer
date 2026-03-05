/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/SmallAngleBellowsGenerator.cxx
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

#include "SmallAngleBellowsGenerator.h"

namespace setVariable
{

SmallAngleBellowsGenerator::SmallAngleBellowsGenerator() :
  angle(0.0),bellowsStep(0.5),bellowsMaterialThick(0.05),
  bellowsThick(0.8*(CF40::flangeRadius-CF40::innerRadius)),
  bellowsVolumeFraction(0.0),flangeLength(CF40::flangeLength),
  flangeRadius(CF40::flangeRadius),length(10.0),
  pipeInnerRadius(CF40::innerRadius),pipeWallThick(CF40::wallThick),nFolds(10),
  nSectors(2),bellowsBaseMat("SteelUnknownGrade"),pipeMat("SteelUnknownGrade")
{}

template<typename CF> void
SmallAngleBellowsGenerator::setCF(){
  flangeLength=CF40::flangeLength;
  flangeRadius=CF40::flangeRadius;
  pipeInnerRadius=CF40::innerRadius;
  pipeWallThick=CF40::wallThick;
}

void SmallAngleBellowsGenerator::generateBellows(
  FuncDataBase& Control,const std::string& keyName) const
{
  ELog::RegMethod RegA("BellowsGenerator","generatorBellow");

  Control.addVariable(keyName+"Angle",angle);
  Control.addVariable(keyName+"BellowsMaterialThick",bellowsMaterialThick);
  Control.addVariable(keyName+"BellowsStep",bellowsStep);
  Control.addVariable(keyName+"BellowsThick",bellowsThick);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"PipeInnerRadius",pipeInnerRadius);
  Control.addVariable(keyName+"PipeWallThick",pipeWallThick);
  Control.addVariable(keyName+"NFolds",nFolds);
  Control.addVariable(keyName+"NSectors",nSectors);
  Control.addVariable(keyName+"BellowsBaseMat",bellowsBaseMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
}

///\cond TEMPLATE

  template void SmallAngleBellowsGenerator::setCF<CF40>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
