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
  radius(0.9),length(5.0),outerRadius(2.5),
  innerRadius(0.95),
  flangeInnerRadius(2.0),
  flangeARadius(3.5),flangeBLength(1.3),
  flangeBRadius(3.5),flangeALength(1.3),
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

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"FlangeInnerRadius",flangeInnerRadius);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
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
