/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CollTubeGenerator.cxx
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

#include "CollTubeGenerator.h"

namespace setVariable
{


CollTubeGenerator::CollTubeGenerator() :
  mainRadius(CF120::innerRadius),
  xRadius(CF40::innerRadius),
  yRadius(CF40::innerRadius),
  zRadius(CF100::innerRadius),
  wallThick(CF40::wallThick),
  height(15.0),depth(35.0),
  frontLength(10.0),backLength(3.0),
  flangeXRadius(CF40::flangeRadius),
  flangeYRadius(CF40::flangeRadius),
  flangeZRadius(CF100::flangeRadius),
  flangeXLength(CF40::flangeLength),
  flangeYLength(CF40::flangeLength),
  flangeZLength(CF100::flangeLength),
  sideZOffset(0.0),sideLength(5.0),
  gaugeZOffset(0.0),gaugeRadius(CF40::innerRadius),
  gaugeLength(14.0),gaugeHeight(10.0),
  gaugeFlangeRadius(CF40::flangeRadius),
  gaugeFlangeLength(CF40::flangeLength),
  plateThick(CF40::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

CollTubeGenerator::~CollTubeGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
CollTubeGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  mainRadius=CF::innerRadius;
  wallThick=CF::wallThick;
  return;
}

template<typename CF>
void
CollTubeGenerator::setPortCF()
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
CollTubeGenerator::setSideCF(const double L)
  /*!
    Setter for flange beam direction flanges
  */
{
  yRadius=CF::innerRadius;
  sideLength=L;
  flangeXRadius=CF::flangeRadius;
  flangeXLength=CF::flangeLength;
  return;
}

void
CollTubeGenerator::setMainPort(const double F,const double B)
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
CollTubeGenerator::generateTube(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("CollTubeGenerator","generateCollTube");

  Control.addVariable(keyName+"MainRadius",mainRadius);
  Control.addVariable(keyName+"XRadius",xRadius);
  Control.addVariable(keyName+"YRadius",yRadius);
  Control.addVariable(keyName+"ZRadius",zRadius);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"FrontLength",frontLength+length/2.0+wallThick);
  Control.addVariable(keyName+"BackLength",backLength+length/2.0+wallThick);

  Control.addVariable(keyName+"FlangeXRadius",flangeXRadius);
  Control.addVariable(keyName+"FlangeYRadius",flangeYRadius);
  Control.addVariable(keyName+"FlangeZRadius",flangeZRadius);

  Control.addVariable(keyName+"FlangeXLength",flangeXLength);
  Control.addVariable(keyName+"FlangeYLength",flangeYLength);
  Control.addVariable(keyName+"FlangeZLength",flangeZLength);

  Control.addVariable(keyName+"SideZOffset",sideZOffset);
  Control.addVariable(keyName+"SideLength",sideLength+zRadius);

  Control.addVariable(keyName+"GaugeZOffset",gaugeZOffset);
  Control.addVariable(keyName+"GaugeRadius",gaugeRadius);
  Control.addVariable(keyName+"GaugeLength",gaugeLength);
  Control.addVariable(keyName+"GaugeHeight",gaugeHeight);
  Control.addVariable(keyName+"GaugeFlangeRadius",gaugeFlangeRadius);
  Control.addVariable(keyName+"GaugeFlangeLength",gaugeFlangeLength);

  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  return;

}

///\cond TEMPLATE

template void CollTubeGenerator::setCF<CF100>();
template void CollTubeGenerator::setCF<CF150>();

template void CollTubeGenerator::setSideCF<CF40>(const double);

template void CollTubeGenerator::setPortCF<CF40>();


///\endcond TEMPLATE

}  // NAMESPACE setVariable
