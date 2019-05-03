/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/PreBendPipeGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "PreBendPipeGenerator.h"

namespace setVariable
{

PreBendPipeGenerator::PreBendPipeGenerator() :
  length(205.5),radius(1.10),
  straightLength(109.4),
  wallThick(0.1),
  electronRadius(1910.0),
  electronAngle(1.5),

  flangeARadius(CF50::flangeRadius),
  flangeALength(CF50::flangeLength),
  flangeBRadius(CF50::flangeRadius),
  flangeBLength(CF50::flangeLength),
  voidMat("Void"),wallMat("Copper"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
PreBendPipeGenerator::~PreBendPipeGenerator() 
 /*!
   Destructor
 */
{}

void
PreBendPipeGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("PreBendPipeGenerator","generateColl");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"StraightLength",straightLength);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"ElectronRadius",electronRadius);
  Control.addVariable(keyName+"ElectronAngle",electronAngle);

  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  return;
}

  
}  // NAMESPACE setVariable
