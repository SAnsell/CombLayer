/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/DipoleSndBendGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "DipoleSndBendGenerator.h"

namespace setVariable
{

DipoleSndBendGenerator::DipoleSndBendGenerator() :
  arcAngle(40.0),curveRadius(379.77),
  innerWidth(3.6),flatWidth(14.0),
  wideWidth(16.5),outerFlat(24.0),
  height(2.0),outerHeight(5.4),
  wallThick(0.2),
  voidMat("Void"),wallMat("Copper"),outerMat("Void")
  /*!
    Constructor and defaults
  */
{}
  
DipoleSndBendGenerator::~DipoleSndBendGenerator() 
 /*!
   Destructor
 */
{}

void
DipoleSndBendGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("DipoleSndBendGenerator","generatePipe");


  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"ArcAngle",arcAngle);
  Control.addVariable(keyName+"CurveRadius",curveRadius);
  Control.addVariable(keyName+"InnerWidth",innerWidth);
  Control.addVariable(keyName+"FlatWidth",flatWidth);
  Control.addVariable(keyName+"WideWidth",wideWidth);
  Control.addVariable(keyName+"OuterFlat",outerFlat);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"OuterHeight",outerHeight);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"OuterMat",outerMat);

  return;
}

  
}  // NAMESPACE setVariable
