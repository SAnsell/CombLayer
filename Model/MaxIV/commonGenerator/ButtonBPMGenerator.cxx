/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/ButtonBPMGenerator.cxx
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
#include "CFFlanges.h"

#include "ButtonBPMGenerator.h"

namespace setVariable
{

ButtonBPMGenerator::ButtonBPMGenerator() :
  length(5.05),innerRadius(0.95),
  outerRadius(2.5),
  nButtons(4),
  flangeInnerRadius(2.0),
  flangeALength(1.3),flangeARadius(3.5),
  flangeBLength(1.3),flangeBRadius(3.5),
  flangeGap(0.1),
  buttonYAngle(45),
  buttonFlangeRadius(0.65),
  buttonFlangeLength(0.3),
  buttonCaseLength(1.5), // approx. email from EM 2020-07-17
  buttonCaseRadius(0.4),
  buttonCaseMat("Stainless304"),
  buttonHandleRadius(0.27),
  buttonHandleLength(0.85),
  elThick(0.4),
  elGap(0.05),
  elCase(0.035),
  elRadius(0.47),
  elMat("Stainless316L"),
  ceramicThick(0.2),
  ceramicMat("Al2O3"),
  pinRadius(0.05),
  pinOuterRadius(0.09),
  pinMat("Stainless304"),
  voidMat("Void"),wallMat("Stainless304L"),
  flangeMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

ButtonBPMGenerator::~ButtonBPMGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
ButtonBPMGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  innerRadius = CF::innerRadius;
  return;
}

template<typename CF>
void
ButtonBPMGenerator::setAFlangeCF()
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
ButtonBPMGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;
  return;
}

void
ButtonBPMGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("ButtonBPMGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"NButtons",nButtons);
  Control.addVariable(keyName+"FlangeInnerRadius",flangeInnerRadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeGap",flangeGap);
  Control.addVariable(keyName+"ButtonYAngle",buttonYAngle);
  Control.addVariable(keyName+"ButtonFlangeRadius",buttonFlangeRadius);
  Control.addVariable(keyName+"ButtonFlangeLength",buttonFlangeLength);
  Control.addVariable(keyName+"ButtonCaseLength",buttonCaseLength);
  Control.addVariable(keyName+"ButtonCaseRadius",buttonCaseRadius);
  Control.addVariable(keyName+"ButtonCaseMat",buttonCaseMat);
  Control.addVariable(keyName+"ButtonHandleRadius",buttonHandleRadius);
  Control.addVariable(keyName+"ButtonHandleLength",buttonHandleLength);
  Control.addVariable(keyName+"ElectrodeThick",elThick);
  Control.addVariable(keyName+"ElectrodeGap",elGap);
  Control.addVariable(keyName+"ElectrodeCase",elCase);
  Control.addVariable(keyName+"ElectrodeRadius",elRadius);
  Control.addVariable(keyName+"ElectrodeMat",elMat);
  Control.addVariable(keyName+"CeramicThick",ceramicThick);
  Control.addVariable(keyName+"CeramicMat",ceramicMat);
  Control.addVariable(keyName+"PinRadius",pinRadius);
  Control.addVariable(keyName+"PinOuterRadius",pinOuterRadius);
  Control.addVariable(keyName+"PinMat",pinMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;

}


///\cond TEMPLATE

template void ButtonBPMGenerator::setCF<CF40_22>();
template void ButtonBPMGenerator::setCF<CF40>();

template void ButtonBPMGenerator::setAFlangeCF<CF40_22>();
template void ButtonBPMGenerator::setAFlangeCF<CF40>();

template void ButtonBPMGenerator::setBFlangeCF<CF40_22>();
template void ButtonBPMGenerator::setBFlangeCF<CF40>();

///\endcond TEMPLATE

}  // namespace setVariable
