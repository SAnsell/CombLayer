/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/YagUnitBigGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell / Konstantin Batkov
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

#include "YagUnitBigGenerator.h"

namespace setVariable
{

YagUnitBigGenerator::YagUnitBigGenerator() :
  radius(6.8),height(13.2),
  depth(6.8),wallThick(CF63::wallThick),
  flangeRadius(CF150::flangeRadius-0.25), // -0.25 otherwise 2*R>front+back length => cuts neighbours
  flangeLength(CF100::flangeLength),
  plateThick(CF63::flangeLength),
  viewAZStep(3.2),viewARadius(3.3),
  viewAThick(CF63::wallThick),viewALength(13.67),
  viewAFlangeRadius(5.6),
  viewAFlangeLength(CF63::flangeLength),
  viewAPlateThick(CF63::flangeLength),
  viewBYStep(3.2),viewBRadius(3.3),
  viewBThick(CF63::wallThick),viewBLength(11.27),
  viewBFlangeRadius(5.6),
  viewBFlangeLength(CF63::flangeLength),
  viewBPlateThick(CF63::flangeLength),
  portRadius(CF35_TDC::innerRadius),portThick(CF35_TDC::wallThick),
  portFlangeRadius(CF35_TDC::flangeRadius),
  portFlangeLength(CF35_TDC::flangeLength),
  frontLength(10.0),backLength(10.0),
  outerRadius(CF63::flangeRadius*1.2),
  voidMat("Void"),mainMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

YagUnitBigGenerator::~YagUnitBigGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
YagUnitBigGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  setFlangeCF<CF>();
  return;
}

template<typename CF>
void
YagUnitBigGenerator::setFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  return;
}

void
YagUnitBigGenerator::generateYagUnit(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("YagUnitBigGenerator","generateYagUnitBig");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"ViewAZStep",viewAZStep);
  Control.addVariable(keyName+"ViewARadius",viewARadius);
  Control.addVariable(keyName+"ViewAThick",viewAThick);
  Control.addVariable(keyName+"ViewALength",viewALength);
  Control.addVariable(keyName+"ViewAFlangeRadius",viewAFlangeRadius);
  Control.addVariable(keyName+"ViewAFlangeLength",viewAFlangeLength);
  Control.addVariable(keyName+"ViewAPlateThick",viewAPlateThick);

  Control.addVariable(keyName+"ViewBYStep",viewBYStep);
  Control.addVariable(keyName+"ViewBRadius",viewBRadius);
  Control.addVariable(keyName+"ViewBThick",viewBThick);
  Control.addVariable(keyName+"ViewBLength",viewBLength);
  Control.addVariable(keyName+"ViewBFlangeRadius",viewBFlangeRadius);
  Control.addVariable(keyName+"ViewBFlangeLength",viewBFlangeLength);
  Control.addVariable(keyName+"ViewBPlateThick",viewBPlateThick);

  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"PortThick",portThick);
  Control.addVariable(keyName+"PortFlangeRadius",portFlangeRadius);
  Control.addVariable(keyName+"PortFlangeLength",portFlangeLength);

  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"OuterRadius",outerRadius);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MainMat",mainMat);

  return;

}

///\cond TEMPLATE

template void YagUnitBigGenerator::setCF<CF35_TDC>();
template void YagUnitBigGenerator::setCF<CF40_22>();
template void YagUnitBigGenerator::setCF<CF40>();
template void YagUnitBigGenerator::setCF<CF63>();

template void YagUnitBigGenerator::setFlangeCF<CF35_TDC>();
template void YagUnitBigGenerator::setFlangeCF<CF40_22>();
template void YagUnitBigGenerator::setFlangeCF<CF40>();
template void YagUnitBigGenerator::setFlangeCF<CF63>();


///\endcond TEPLATE

}  // NAMESPACE setVariable
