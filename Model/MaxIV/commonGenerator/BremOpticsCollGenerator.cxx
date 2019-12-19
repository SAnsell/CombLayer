/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/BremOpticsCollGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <sstream>
// #include <cmath>
// #include <complex>
// #include <list>
#include <vector>
// #include <stack>
// #include <set>
#include <map>
// #include <string>
// #include <algorithm>
// #include <numeric>
// #include <memory>

#include "NameStack.h"
#include "RegMethod.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "BremOpticsCollGenerator.h"

namespace setVariable
{

BremOpticsCollGenerator::BremOpticsCollGenerator() :
  extWidth(13.0),extHeight(10.0),wallThick(0.5),
  flangeAInnerRadius(setVariable::CF40::innerRadius),flangeBInnerRadius(setVariable::CF40::innerRadius),
  holeXStep(0.0),holeZStep(0.0),
  holeWidth(1.0),holeHeight(1.2),
  colLength(8.4),
  colYStep(0.0),extActive(1),
  extXStep(0.0),extZStep(0.0),
  extLength(5.0),extRadius(2.5),colRadius(2.0),
  voidMat("Void"),colMat("Tungsten"),
  wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{
  setCF<CF40>();
}

BremOpticsCollGenerator::~BremOpticsCollGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
BremOpticsCollGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  innerRadius=CF::innerRadius;
  colRadius=innerRadius;
  flangeARadius=CF::flangeRadius;
  flangeBRadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  flangeBLength=CF::flangeLength;
  return;
}

void
BremOpticsCollGenerator::setMaterial(const std::string& IMat,
			       const std::string& WMat)
  /*!
    Set the materials
    \param PMat :: Plate Material
    \param TMat :: thread Material
    \param BMat :: Blade Material
  */
{
  colMat=IMat;
  wallMat=WMat;
  return;
}

void
BremOpticsCollGenerator::setAperture(const double W,const double H)
  /*!
    Set the widths
    \param W :: width
    \param H :: height
  */
{
  holeWidth=W;
  holeHeight=H;
  return;
}

void
BremOpticsCollGenerator::setFlangeInnerRadius(const double ra,const double rb)
{
  flangeAInnerRadius=ra;
  flangeBInnerRadius=rb;
}


void
BremOpticsCollGenerator::generateColl(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Forward step
    \param length :: length of W block
  */
{
  ELog::RegMethod RegA("BremOpticsCollGenerator","generatorColl");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"ExtWidth",extWidth);
  Control.addVariable(keyName+"ExtHeight",extHeight);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeAInnerRadius",flangeAInnerRadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBInnerRadius",flangeBInnerRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"HoleXStep",holeXStep);
  Control.addVariable(keyName+"HoleZStep",holeZStep);
  Control.addVariable(keyName+"ColYStep",colYStep);
  Control.addVariable(keyName+"HoleWidth",holeWidth);
  Control.addVariable(keyName+"HoleHeight",holeHeight);
  Control.addVariable(keyName+"ColLength",colLength);
  Control.addVariable(keyName+"ColRadius",colRadius);

  Control.addVariable(keyName+"ExtActive",extActive);
  Control.addVariable(keyName+"ExtXStep",extXStep);
  Control.addVariable(keyName+"ExtZStep",extZStep);

  Control.addVariable(keyName+"ExtLength",extLength);
  Control.addVariable(keyName+"ExtRadius",extRadius);

  Control.addVariable(keyName+"ColMat",colMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}

///\cond TEMPLATE

template void BremOpticsCollGenerator::setCF<CF40>();
template void BremOpticsCollGenerator::setCF<CF50>();
template void BremOpticsCollGenerator::setCF<CF63>();
template void BremOpticsCollGenerator::setCF<CF100>();

///\endcond  TEMPLATE

}  // NAMESPACE setVariable
