/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/DipoleGenerator.cxx
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

#include "DipoleGenerator.h"

namespace setVariable
{

DipoleGenerator::DipoleGenerator() :
  poleAngle(1.5),poleRadius(191.4),
  poleGap(1.5),poleWidth(3.0),poleHeight(4.0),
  coilGap(4.0),coilLength(8.0),coilWidth(6.0),
  poleMat("Stainless304"),coilMat("Copper")
  /*!
    Constructor and defaults
  */
{}
  
DipoleGenerator::~DipoleGenerator() 
 /*!
   Destructor
 */
{}

void
DipoleGenerator::generateDipole(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double length) const
 /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param length :: length
  */
{
  ELog::RegMethod RegA("DipoleGenerator","generateColl");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"PoleAngle",poleAngle);
  Control.addVariable(keyName+"PoleRadius",poleRadius);
  Control.addVariable(keyName+"PoleGap",poleGap);
  Control.addVariable(keyName+"PoleWidth",poleWidth);
  Control.addVariable(keyName+"PoleHeight",poleHeight);

  Control.addVariable(keyName+"CoilGap",coilGap);
  Control.addVariable(keyName+"CoilLength",coilLength);
  Control.addVariable(keyName+"CoilWidth",coilWidth);
  Control.addVariable(keyName+"CoilHeight",coilHeight);

  Control.addVariable(keyName+"PoleMat",poleMat);
  Control.addVariable(keyName+"CoilMat",coilMat);

  return;

}

  
}  // NAMESPACE setVariable
