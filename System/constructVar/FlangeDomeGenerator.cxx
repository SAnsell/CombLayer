/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/FlangeDomeGenerator.cxx
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
#include "FlangeDomeGenerator.h"

namespace setVariable
{

FlangeDomeGenerator::FlangeDomeGenerator() :
  curveRadius(CF350::innerRadius),
  curveStep(3.0),
  plateThick(CF350::flangeLength),
  flangeRadius(CF350::flangeRadius),
  voidMat("Void"),mainMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

FlangeDomeGenerator::~FlangeDomeGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
FlangeDomeGenerator::setFlangeCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  flangeRadius=CF::flangeRadius;
  return;
}

void
FlangeDomeGenerator::generateDome
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("FlangeDomeGenerator","generatorFlangeDome");

  Control.addVariable(keyName+"CurveRadius",curveRadius);
  Control.addVariable(keyName+"CurveStep",curveStep);
  Control.addVariable(keyName+"PlateThick",plateThick);
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"Mat",mainMat);

  return;
}

///\cond TEMPLATE

  template void FlangeDomeGenerator::setFlangeCF<CF150>();

///\endcond TEMPLATE


}  // NAMESPACE setVariable
