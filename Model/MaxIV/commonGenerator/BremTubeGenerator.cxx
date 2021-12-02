/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/BremTubeGenerator.cxx
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

#include "BremTubeGenerator.h"

namespace setVariable
{


BremTubeGenerator::BremTubeGenerator() :
  frontRadius(CF40::innerRadius),
  frontLength(4.0),
  frontFlangeRadius(CF40::flangeRadius),
  frontFlangeLength(CF40::flangeLength), 
  midRadius(CF63::innerRadius),
  midLength(8.0+CF150::outerRadius),
  tubeRadius(CF150::innerRadius),
  tubeHeight(15.0),tubeDepth(35.0),
  tubeFlangeRadius(CF150::flangeRadius),
  tubeFlangeLength(CF150::flangeLength),
  backRadius(CF150::innerRadius),
  backLength(12.0),
  backFlangeRadius(CF150::flangeRadius),
  backFlangeLength(CF150::flangeLength),
  plateThick(CF150::flangeLength),
  wallThick(CF150::wallThick),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{} 

BremTubeGenerator::~BremTubeGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
BremTubeGenerator::setCF()
  /*!
    Setter for flange A
   */
{
  tubeRadius=CF::innerRadius;
  tubeFlangeRadius=CF::flangeRadius;
  tubeFlangeLength=CF::flangeLength;
  wallThick=CF::wallThick;

  return;
}

void
BremTubeGenerator::generateTube(FuncDataBase& Control,
				  const std::string& keyName) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("BremTubeGenerator","generateTube");

  Control.addVariable(keyName+"FrontRadius",frontRadius);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"FrontFlangeRadius",frontFlangeRadius);
  Control.addVariable(keyName+"FrontFlangeLength",frontFlangeLength);

  Control.addVariable(keyName+"MidRadius",midRadius);
  Control.addVariable(keyName+"MidLength",midLength);

  Control.addVariable(keyName+"TubeRadius",tubeRadius);
  Control.addVariable(keyName+"TubeHeight",tubeHeight);
  Control.addVariable(keyName+"TubeDepth",tubeDepth);
  Control.addVariable(keyName+"TubeFlangeRadius",tubeFlangeRadius);
  Control.addVariable(keyName+"TubeFlangeLength",tubeFlangeLength);

  Control.addVariable(keyName+"BackRadius",backRadius);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"BackFlangeRadius",backFlangeRadius);
  Control.addVariable(keyName+"BackFlangeLength",backFlangeLength);

  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);

  Control.addVariable(keyName+"FrontNPorts",0);
  Control.addVariable(keyName+"MainNPorts",0);
  return;

}

///\cond TEMPLATE

template void BremTubeGenerator::setCF<CF100>();


///\endcond TEPLATE

}  // NAMESPACE setVariable
