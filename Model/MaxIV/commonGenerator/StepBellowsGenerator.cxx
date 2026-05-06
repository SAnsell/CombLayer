/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/StepBellowsGenerator.cxx
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

#include "StepBellowsGenerator.h"

namespace setVariable
{

StepBellowsGenerator::StepBellowsGenerator() :
  bellowsStep(0.5),bellowsMaterialThick(0.05),
  bellowsThick(0.8*(CF40::flangeRadius-CF40::innerRadius)),
  flangeLength(CF40::flangeLength),
  flangeRadius(CF40::flangeRadius),length(20.0),
  pipeInnerRadius(CF40::innerRadius),pipeWallThick(CF40::wallThick),step(0.0),
  nFolds(10),nSectors(2),bellowsBaseMat("SteelUnknownGrade"),
  pipeMat("SteelUnknownGrade"),useFrontPipe(1),useBackPipe(1)
{}

StepBellowsGenerator::StepBellowsGenerator(const double s, const double l) :
  bellowsStep(0.5),bellowsMaterialThick(0.05),
  bellowsThick(0.8*(CF40::flangeRadius-CF40::innerRadius)),
  flangeLength(CF40::flangeLength),
  flangeRadius(CF40::flangeRadius),length(l),
  pipeInnerRadius(CF40::innerRadius),pipeWallThick(CF40::wallThick),step(s),
  nFolds(10),nSectors(2),bellowsBaseMat("SteelUnknownGrade"),
  pipeMat("SteelUnknownGrade"),useFrontPipe(1),useBackPipe(1)
{}

template<typename CF> void
StepBellowsGenerator::setCF(){
  flangeLength=CF40::flangeLength;
  flangeRadius=CF40::flangeRadius;
  pipeInnerRadius=CF40::innerRadius;
  pipeWallThick=CF40::wallThick;
}

void StepBellowsGenerator::generateBellows(
  FuncDataBase& Control,const std::string& keyName) const
{
  ELog::RegMethod RegA("BellowsGenerator","generatorBellow");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Step",step);

  double angle = 0.0;
  double partBellowsLength = length/2.0;
  if(fabs(step) > Geometry::zeroTol){
    angle = 2.0*atan(step/length);
    partBellowsLength = step/sin(angle);
  }
  const double angleDeg = angle*180.0/M_PI;

  std::string prefix;
  for(auto partBellowsName : std::vector<std::string>{"FrontBellows","BackBellows"}){
    prefix = keyName + partBellowsName;
    Control.addVariable(prefix+"BellowsMaterialThick",bellowsMaterialThick);
    Control.addVariable(prefix+"BellowsStep",bellowsStep);
    Control.addVariable(prefix+"BellowsThick",bellowsThick);
    Control.addVariable(prefix+"FlangeLength",flangeLength);
    Control.addVariable(prefix+"FlangeRadius",flangeRadius);
    Control.addVariable(prefix+"Length",partBellowsLength);
    Control.addVariable(prefix+"PipeInnerRadius",pipeInnerRadius);
    Control.addVariable(prefix+"PipeWallThick",pipeWallThick);
    Control.addVariable(prefix+"NFolds",nFolds/2);
    Control.addVariable(prefix+"BellowsBaseMat",bellowsBaseMat);
    Control.addVariable(prefix+"PipeMat",pipeMat);
    Control.addVariable(prefix+"NSectors",nSectors);
  }

  Control.addVariable(keyName+"FrontBellowsAngle",angleDeg);
  Control.addVariable(keyName+"FrontBellowsUseFrontPipe",useFrontPipe);
  Control.addVariable(keyName+"FrontBellowsUseBackPipe",0);

  Control.addVariable(keyName+"BackBellowsAngle",-angleDeg);
  Control.addVariable(keyName+"BackBellowsUseFrontPipe",0);
  Control.addVariable(keyName+"BackBellowsUseBackPipe",useBackPipe);

}

///\cond TEMPLATE

  template void StepBellowsGenerator::setCF<CF40>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
