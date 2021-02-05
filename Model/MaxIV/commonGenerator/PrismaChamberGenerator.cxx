/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/PrismaChamberGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "PrismaChamberGenerator.h"

namespace setVariable
{


PrismaChamberGenerator::PrismaChamberGenerator() :
  radius(15.0),beamRadius(1.75),sideRadius(CF100::innerRadius),
  wallThick(0.338),height(16.6),depth(16.6),
  beamXStep(-7.0),frontLength(20.0),backLength(20.0),
  sideLength(18.9),flangeRadius(17.8),
  flangeXRadius(CF100::flangeRadius),
  flangeBeamRadius(CF40::flangeRadius),
  flangeLength(2.7),flangeXLength(CF100::flangeLength),
  flangeBeamLength(CF40::flangeLength),
  sideThick(CF100::flangeLength),plateThick(CF100::flangeLength),
  voidMat("Void"),wallMat("Stainless304L"),
  plateMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

PrismaChamberGenerator::~PrismaChamberGenerator() 
 /*!
   Destructor
 */
{}

void
PrismaChamberGenerator::generateChamber(FuncDataBase& Control,
					const std::string& keyName) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("PrismaChamberGenerator","generateChamber");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"BeamRadius",beamRadius);
  Control.addVariable(keyName+"SideRadius",sideRadius);

  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);

  Control.addVariable(keyName+"BeamXStep",beamXStep);
  Control.addVariable(keyName+"FrontLength",frontLength);
  Control.addVariable(keyName+"BackLength",backLength);
  Control.addVariable(keyName+"SideLength",sideLength);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeXRadius",flangeXRadius);
  Control.addVariable(keyName+"FlangeBeamRadius",flangeBeamRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"FlangeXLength",flangeXLength);
  Control.addVariable(keyName+"FlangeBeamLength",flangeBeamLength);

  Control.addVariable(keyName+"SideThick",sideThick);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"PlateMat",plateMat);
  

  return;

}

///\cond TEMPLATE



///\endcond TEPLATE
  
}  // NAMESPACE setVariable
