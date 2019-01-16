/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/EPSeparatorGenerator.cxx
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
#include "EPSeparatorGenerator.h"

namespace setVariable
{

EPSeparatorGenerator::EPSeparatorGenerator() :
  length(73.5),photonOuterGap(0.25),
  photonRadius(0.35),
  photonAGap(0.5),photonBGap(0.7),
  electronRadius(1.1),initEPSeparation(2.8),   // guess
  electronAngle(1.54),wallPhoton(0.3),
  wallElectron(4.1),wallHeight(2.4),
  voidMat("Void"),wallMat("Copper")
  /*!
    Constructor and defaults
  */
{}
  
EPSeparatorGenerator::~EPSeparatorGenerator() 
 /*!
   Destructor
 */
{}

void
EPSeparatorGenerator::generatePipe(FuncDataBase& Control,
				   const std::string& keyName,
				   const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("EPSeparatorGenerator","generateColl");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"PhotonOuterGap",photonOuterGap);
  Control.addVariable(keyName+"PhotonRadius",photonRadius);
  Control.addVariable(keyName+"PhotonAGap",photonAGap);
  Control.addVariable(keyName+"PhotonBGap",photonBGap);
  Control.addVariable(keyName+"ElectronRadius",electronRadius);
  Control.addVariable(keyName+"InitEPSeparation",initEPSeparation);
  Control.addVariable(keyName+"ElectronAngle",electronAngle);

  Control.addVariable(keyName+"WallPhoton",wallPhoton);
  Control.addVariable(keyName+"WallElectron",wallElectron);
  Control.addVariable(keyName+"WallHeight",wallHeight);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  
  return;

}

  
}  // NAMESPACE setVariable
