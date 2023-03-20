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
  curveRadius(CF350::innerRadius),
  innerRadius(CF350::innerRadius*1.2),
  curveStep(3.0),joinStep(4.0),flatLen(5.0),
  plateThick(CF350::wallThick),
  flangeRadius(CF350::flangeRadius),
  flangeLength(CF350::flangeLength),
  voidMat("Void"),mainMat("Stainless304L")
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
DomeConnectorGenerator::setFlangeCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;
  plateThick=CF::wallThick;
  return;
}

void
DomeConnectorGenerator::generateDome
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
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
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"Mat",mainMat);

  return;
}

///\cond TEMPLATE

template void DomeConnectorGenerator::setFlangeCF<CF150>();

///\endcond TEMPLATE


}  // NAMESPACE setVariable
