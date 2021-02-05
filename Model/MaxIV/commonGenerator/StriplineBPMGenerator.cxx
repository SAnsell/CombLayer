/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/StriplineBPMGenerator.cxx
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

#include "StriplineBPMGenerator.h"

namespace setVariable
{

StriplineBPMGenerator::StriplineBPMGenerator() :
  radius(CF27_TDC::innerRadius),length(22.0),
  outerThick(CF27_TDC::wallThick),innerRadius(1.035),innerThick(0.18),
  innerAngle(30.0),innerAngleOffset(45.0),
  flangeARadius(CF27_TDC::flangeRadius),flangeALength(CF27_TDC::flangeLength),
  flangeBRadius(CF27_TDC::flangeRadius),flangeBLength(CF27_TDC::flangeLength),
  striplineRadius(3.2),striplineThick(2.4),
  striplineYStep(3.1),striplineEnd(4.0),
  voidMat("Void"),striplineMat("Stainless304L"),
  flangeMat("Stainless304L"),outerMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

StriplineBPMGenerator::~StriplineBPMGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
StriplineBPMGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius = CF::innerRadius;
  outerThick = CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  return;
}

template<typename CF>
void
StriplineBPMGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  return;
}

template<typename CF>
void
StriplineBPMGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}


void
StriplineBPMGenerator::generateBPM(FuncDataBase& Control,
				   const std::string& keyName,
				   const double yStep)  const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("StriplineBPMGenerator","generateBPM");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"OuterThick",outerThick);

  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"InnerThick",innerThick);
  Control.addVariable(keyName+"InnerAngle",innerAngle);
  Control.addVariable(keyName+"InnerAngleOffset",innerAngleOffset);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"StriplineRadius",striplineRadius);
  Control.addVariable(keyName+"StriplineThick",striplineThick);
  Control.addVariable(keyName+"StriplineYStep",striplineYStep);
  Control.addVariable(keyName+"StriplineEnd",striplineEnd);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"StriplineMat",striplineMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"OuterMat",outerMat);


  return;

}

///\cond TEMPLATE

template void StriplineBPMGenerator::setCF<CF27_TDC>();
template void StriplineBPMGenerator::setCF<CF40_22>();
template void StriplineBPMGenerator::setCF<CF40>();

template void StriplineBPMGenerator::setAFlangeCF<CF40_22>();
template void StriplineBPMGenerator::setAFlangeCF<CF40>();

template void StriplineBPMGenerator::setBFlangeCF<CF40_22>();
template void StriplineBPMGenerator::setBFlangeCF<CF40>();

///\endcond TEMPLATE

}  // NAMESPACE setVariable
