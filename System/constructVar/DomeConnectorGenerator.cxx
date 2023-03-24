/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/DomeConnectorGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "DomeConnectorGenerator.h"

namespace setVariable
{

DomeConnectorGenerator::DomeConnectorGenerator() :
  curveRadius(CF150::innerRadius),
  innerRadius(CF200::innerRadius),
  curveStep(3.0),joinStep(4.0),
  flatLen(8.0),plateThick(CF200::wallThick),
  flangeRadius(CF200::flangeRadius),
  flangeLen(CF200::flangeLength),
  wallMat("Stainless304L"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

DomeConnectorGenerator::~DomeConnectorGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
DomeConnectorGenerator::setFlangeCF(const double scaleFraction)
  /*!
    Set pipe/flange to CF-X format
    \param scaleFraction :: size of the curveRadius relative
    to the inner radius
  */
{

  innerRadius=CF::innerRadius;
  curveRadius=CF::innerRadius*scaleFraction;
  flangeRadius=CF::flangeRadius;
  flangeLen=CF::flangeLength;
  plateThick=CF::wallThick;
  return;
}

void
DomeConnectorGenerator::generateDome
(FuncDataBase& Control,const std::string& keyName,
 const size_t nPorts) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param nPorts :: number of ports defined (external)
  */
{
  ELog::RegMethod RegA("DomeConnectorGenerator","generatorDomeConnector");

  Control.addVariable(keyName+"CurveRadius",curveRadius);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"CurveStep",curveStep);
  Control.addVariable(keyName+"JoinStep",joinStep);
  Control.addVariable(keyName+"FlatLen",flatLen);
  Control.addVariable(keyName+"PlateThick",plateThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLen",flangeLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"NPorts",nPorts);
  
  return;
}

///\cond TEMPLATE

template void DomeConnectorGenerator::setFlangeCF<CF150>(const double);

///\endcond TEMPLATE


}  // NAMESPACE setVariable
