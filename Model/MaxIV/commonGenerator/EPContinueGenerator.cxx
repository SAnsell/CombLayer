/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/EPContinueGenerator.cxx
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
#include "EPContinueGenerator.h"

namespace setVariable
{

EPContinueGenerator::EPContinueGenerator() :
  length(30.0),electronRadius(1.1),
  photonAGap(1.1),photonBGap(2.5),
  photonStep(0.5),
  photonWidth(1.0),height(2.65),

  voidMat("Void"),wallMat("Copper")
  /*!
    Constructor and defaults
  */
{}
  
EPContinueGenerator::~EPContinueGenerator() 
 /*!
   Destructor
 */
{}

void
EPContinueGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("EPContinueGenerator","generateColl");
  
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"ElectronRadius",electronRadius);

  Control.addVariable(keyName+"PhotonAGap",photonAGap);
  Control.addVariable(keyName+"PhotonBGap",photonBGap);
  Control.addVariable(keyName+"PhotonStep",photonStep);

  Control.addVariable(keyName+"PhotonWidth",photonWidth);
  Control.addVariable(keyName+"Height",height);

  Control.addVariable(keyName+"OuterRadius",outerRadius);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  
  return;
}

  
}  // NAMESPACE setVariable
