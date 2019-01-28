/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/DipoleChamberGenerator.cxx
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
#include "DipoleChamberGenerator.h"

namespace setVariable
{

DipoleChamberGenerator::DipoleChamberGenerator() :
  length(40.0),outWidth(3.6),
  ringWidth(3.6),curveRadius(379.77),
  curveAngle(15.0),height(1.9),
  exitWidth(4.6),exitHeight(1.47),
  exitLength(15.0),
  flangeRadius(CF120::innerRadius),
  flangeLength(CF120::flangeLength),
  wallThick(0.5),
  voidMat("Void"),wallMat("Copper"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
DipoleChamberGenerator::~DipoleChamberGenerator() 
 /*!
   Destructor
 */
{}

void
DipoleChamberGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("DipoleChamberGenerator","generatePipe");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"OutWidth",outWidth);
  Control.addVariable(keyName+"RingWidth",ringWidth);
  Control.addVariable(keyName+"CurveRadius",curveRadius);
  Control.addVariable(keyName+"CurveAngle",curveAngle);
  Control.addVariable(keyName+"Height",height);
  
  Control.addVariable(keyName+"ExitWidth",exitWidth);
  Control.addVariable(keyName+"ExitHeight",exitHeight);
  Control.addVariable(keyName+"ExitLength",exitLength);



  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  
  Control.addVariable(keyName+"WallThick",wallThick);
    
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  
  return;
}

  
}  // NAMESPACE setVariable
