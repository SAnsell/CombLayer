/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/TriggerGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "TriggerGenerator.h"

namespace setVariable
{


TriggerGenerator::TriggerGenerator() :
  radius(CF100::innerRadius),
  xRadius(CF40::innerRadius),
  yRadius(CF40::innerRadius),
  wallThick(CF40::wallThick),
  height(25.0),depth(15.0),
  frontLength(10.0),backLength(10.0),
  flangeXRadius(CF40::flangeRadius),
  flangeYRadius(CF40::flangeRadius),
  flangeZRadius(CF100::flangeRadius),
  flangeXLength(CF40::flangeLength),
  flangeYLength(CF40::flangeLength),
  flangeZLength(CF100::flangeLength),
  sideZOffset(12.0),sideLength(12.0),
  plateThick(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

TriggerGenerator::~TriggerGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
TriggerGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius=CF::innerRadius;
  flangeZRadius=CF::flangeRadius;
  flangeZLength=CF::flangeLength;
  wallThick=CF::wallThick;

  return;
}

template<typename CF>
void
TriggerGenerator::setPortCF()
  /*!
    Setter for flange beam direction flanges
  */
{
  yRadius=CF::innerRadius;
  flangeYRadius=CF::flangeRadius;
  flangeYLength=CF::flangeLength;
  return;
}

template<typename CF>
void
TriggerGenerator::setSideCF(const double L)
  /*!
    Setter for flange beam direction flanges
  */
{
  yRadius=CF::innerRadius;
  sideLength=L;
  flangeYRadius=CF::flangeRadius;
  flangeYLength=CF::flangeLength;
  return;
}

void
TriggerGenerator::setMainLength(const double F,const double B)
  /*!
    Setter for front / back port length
    \param F :: Front length
    \param B :: Back length
  */
{
  frontLength=F;
  backLength=B;
  return;
}

void
TriggerGenerator::generateTube(FuncDataBase& Control,
				  const std::string& keyName) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("TriggerGenerator","generateTrigger");


  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"XRadius",xRadius);
  Control.addVariable(keyName+"YRadius",yRadius);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);

  Control.addVariable(keyName+"FlangeXRadius",flangeYRadius);
  Control.addVariable(keyName+"FlangeYRadius",flangeYRadius);
  Control.addVariable(keyName+"FlangeZRadius",flangeZRadius);

  Control.addVariable(keyName+"FlangeXLength",flangeYLength);
  Control.addVariable(keyName+"FlangeYLength",flangeYLength);
  Control.addVariable(keyName+"FlangeZLength",flangeZLength);

  Control.addVariable(keyName+"SideZOffset",sideZOffset);
  Control.addVariable(keyName+"SideLength",sideLength);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void TriggerGenerator::setCF<CF100>();

template void TriggerGenerator::setSideCF<CF40>(const double);

template void TriggerGenerator::setPortCF<CF40>();


///\endcond TEPLATE

}  // NAMESPACE setVariable
