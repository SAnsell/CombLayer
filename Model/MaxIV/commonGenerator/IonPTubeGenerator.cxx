/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/IonPTubeGenerator.cxx
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

#include "IonPTubeGenerator.h"

namespace setVariable
{


IonPTubeGenerator::IonPTubeGenerator() :
  radius(CF66_TDC::innerRadius),
  yRadius(CF35_TDC::innerRadius),
  wallThick(CF40::wallThick),
  height(3.8),depth(6.4),
  frontLength(8.5),backLength(7.5),
  flangeYRadius(CF35_TDC::flangeRadius),
  flangeZRadius(CF66_TDC::flangeRadius),
  flangeYLength(CF35_TDC::flangeLength),
  flangeZLength(CF66_TDC::flangeLength),
  plateThick(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

IonPTubeGenerator::~IonPTubeGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
IonPTubeGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  radius=CF::innerRadius;
  yRadius=CF::innerRadius;
  wallThick=CF::wallThick;

  setMainCF<CF>();
  return;
}

template<typename CF>
void
IonPTubeGenerator::setMainCF()
  /*!
    Setter for flange A
   */
{
  flangeZRadius=CF::flangeRadius;
  flangeZLength=CF::flangeLength;
  return;
}

template<typename CF>
void
IonPTubeGenerator::setPortCF()
  /*!
    Setter for flange beam direction flanges
  */
{
  yRadius=CF::innerRadius;
  flangeYLength=CF::flangeLength;
  flangeYRadius=CF::flangeRadius;
  return;
}

void
IonPTubeGenerator::setMainLength(const double F,const double B)
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
IonPTubeGenerator::generateTube(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("IonPTubeGenerator","generateIonPTube");


  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"YRadius",yRadius);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);

  Control.addVariable(keyName+"FlangeYRadius",flangeYRadius);
  Control.addVariable(keyName+"FlangeZRadius",flangeZRadius);

  Control.addVariable(keyName+"FlangeYLength",flangeYLength);
  Control.addVariable(keyName+"FlangeZLength",flangeZLength);

  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void IonPTubeGenerator::setCF<CF35_TDC>();
template void IonPTubeGenerator::setCF<CF63>();
template void IonPTubeGenerator::setCF<CF66_TDC>();

template void IonPTubeGenerator::setMainCF<CF35_TDC>();
template void IonPTubeGenerator::setMainCF<CF66_TDC>();

template void IonPTubeGenerator::setPortCF<CF35_TDC>();
template void IonPTubeGenerator::setPortCF<CF66_TDC>();


///\endcond TEPLATE

}  // NAMESPACE setVariable
