/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/EPCombineGenerator.cxx
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
#include "EPCombineGenerator.h"

namespace setVariable
{

EPCombineGenerator::EPCombineGenerator() :
  length(30.0),photonXStep(-0.81),
  //  electronXStep(1.7270-0.81),
  electronXStep(0.0),
  electronXYAngle(-1.5),
  electronRadius(1.1),photonRadius(0.3),
  skinThick(0.1),wallXStep(1.8),
  wallStartLen(15.0),wallWidth(6.25),
  wallHeight(2.65),flangeAXStep(0.0),  
  flangeARadius(CF50::flangeRadius),
  flangeALength(CF50::flangeLength),
  flangeBXStep(0.0),
  flangeBRadius(CF63::flangeRadius),
  flangeBLength(CF63::flangeLength),

  voidMat("Void"),wallMat("Copper"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
EPCombineGenerator::~EPCombineGenerator() 
 /*!
   Destructor
 */
{}

void
EPCombineGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("EPCombineGenerator","generateColl");

  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"PhotonXStep",photonXStep);
  Control.addVariable(keyName+"ElectronXStep",electronXStep);
  Control.addVariable(keyName+"PhotonXYAngle",photonXYAngle);
  Control.addVariable(keyName+"ElectronXYAngle",electronXYAngle);

  Control.addVariable(keyName+"ElectronRadius",electronRadius);
  Control.addVariable(keyName+"PhotonRadius",photonRadius);

  Control.addVariable(keyName+"SkinThick",skinThick);
  
  Control.addVariable(keyName+"WallXStep",wallXStep);
  Control.addVariable(keyName+"WallStartLen",wallStartLen);
  Control.addVariable(keyName+"WallWidth",wallWidth);
  Control.addVariable(keyName+"WallHeight",wallHeight);
  
  Control.addVariable(keyName+"FlangeAXStep",flangeAXStep);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);

  Control.addVariable(keyName+"FlangeBXStep",flangeBXStep);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  
  return;
}

  
}  // NAMESPACE setVariable
