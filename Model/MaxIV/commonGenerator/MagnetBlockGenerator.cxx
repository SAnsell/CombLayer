/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MagnetBlockGenerator.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MagnetBlockGenerator.h"

namespace setVariable
{

MagnetBlockGenerator::MagnetBlockGenerator() :
  blockYStep(10.0),aLength(54.0),bLength(179.6),
  midLength(120.0),sectorAngle(30.0),
  height(70.0),width(34.0),
  voidMat("Void"),outerMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}


MagnetBlockGenerator::~MagnetBlockGenerator() 
 /*!
   Destructor
 */
{}

				  
void
MagnetBlockGenerator::generateBlock(FuncDataBase& Control,
				    const std::string& keyName,
				    const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("MagnetBlockGenerator","generateBlock");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"BlockYStep",blockYStep);
  Control.addVariable(keyName+"ALength",aLength);
  Control.addVariable(keyName+"BLength",bLength);
  Control.addVariable(keyName+"MidLength",midLength);

  Control.addVariable(keyName+"SectorAngle",sectorAngle);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"FrontWidth",width);


  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"OuterMat",outerMat);

       
  return;

}

  
}  // NAMESPACE setVariable
