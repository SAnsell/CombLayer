/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/GaugeGenerator.cxx
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

#include "GaugeGenerator.h"

namespace setVariable
{


GaugeGenerator::GaugeGenerator() :
  yRadius(CF37_TDC::innerRadius),
  xRadius(CF37_TDC::innerRadius),
  wallThick(CF37_TDC::wallThick),
  length(12.6),sideLength(6.8),
  flangeXRadius(CF37_TDC::flangeRadius),
  flangeYRadius(CF37_TDC::flangeRadius),
  flangeXLength(CF37_TDC::flangeLength),
  flangeYLength(CF37_TDC::flangeLength),
  plateThick(CF37_TDC::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

GaugeGenerator::~GaugeGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
GaugeGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  yRadius=CF::innerRadius;
  wallThick=CF::wallThick;

  setFlangeCF<CF>();
  return;
}

template<typename CF>
void
GaugeGenerator::setFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeYLength=CF::flangeLength;
  flangeYRadius=CF::flangeRadius;
  return;
}

void
GaugeGenerator::setLength(const double L)
  /*!
    Setter total port length
    \param L :: total void length
  */
{
  length=L;
  return;
}

template<typename CF>
void
GaugeGenerator::setSidePortCF(const double L)
  /*!
    Setter for side port
   */
{
  flangeXRadius=CF::flangeRadius;
  flangeXLength=CF::flangeLength;
  sideLength=L;
  return;
}

void
GaugeGenerator::generateGauge(FuncDataBase& Control,
			      const std::string& keyName,
			      const double sideYStep,
			      const double sideAngle) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("GaugeGenerator","generateGauge");


  Control.addVariable(keyName+"YRadius",yRadius);
  Control.addVariable(keyName+"XRadius",xRadius);

  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"SideLength",sideLength);
  Control.addVariable(keyName+"SideYStep",sideYStep);
  Control.addVariable(keyName+"SideAngle",sideAngle);

  Control.addVariable(keyName+"FlangeXRadius",flangeXRadius);
  Control.addVariable(keyName+"FlangeYRadius",flangeYRadius);

  Control.addVariable(keyName+"FlangeXLength",flangeXLength);
  Control.addVariable(keyName+"FlangeYLength",flangeYLength);

  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void GaugeGenerator::setCF<CF34_TDC>();
template void GaugeGenerator::setCF<CF35_TDC>();
template void GaugeGenerator::setCF<CF37_TDC>();
template void GaugeGenerator::setCF<CF63>();
template void GaugeGenerator::setCF<CF100>();
template void GaugeGenerator::setCF<CF120>();
template void GaugeGenerator::setCF<CF150>();

template void GaugeGenerator::setFlangeCF<CF34_TDC>();
template void GaugeGenerator::setFlangeCF<CF35_TDC>();
template void GaugeGenerator::setFlangeCF<CF37_TDC>();
template void GaugeGenerator::setFlangeCF<CF40>();
template void GaugeGenerator::setFlangeCF<CF100>();
template void GaugeGenerator::setFlangeCF<CF120>();
template void GaugeGenerator::setFlangeCF<CF150>();

template void GaugeGenerator::setSidePortCF<CF34_TDC>(const double);
template void GaugeGenerator::setSidePortCF<CF35_TDC>(const double);
template void GaugeGenerator::setSidePortCF<CF37_TDC>(const double);
template void GaugeGenerator::setSidePortCF<CF40>(const double);

///\endcond TEPLATE

}  // NAMESPACE setVariable
