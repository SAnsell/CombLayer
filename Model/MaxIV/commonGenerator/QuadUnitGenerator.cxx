/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/QuadUnitGenerator.cxx
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
#include "QuadrupoleGenerator.h"
#include "QuadUnitGenerator.h"

namespace setVariable
{

QuadUnitGenerator::QuadUnitGenerator() :
  length(73.5),inWidth(0.915),
  ringWidth(1.89),outPointWidth(2.2),
  height(1.9),endGap(0.5),endLength(1.75),
  wallThick(0.3),
  flangeRadius(CF63::innerRadius),
  flangeLength(CF63::flangeLength),
  voidMat("Void"),wallMat("Copper"),
  flangeMat("Stainless304"),
  QGen(new QuadrupoleGenerator)
  /*!
    Constructor and defaults
  */
{}
  
QuadUnitGenerator::~QuadUnitGenerator() 
 /*!
   Destructor
 */
{}

void
QuadUnitGenerator::generatePipe(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("QuadUnitGenerator","generatePipe");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"InWidth",inWidth);
  Control.addVariable(keyName+"RingWidth",ringWidth);
  Control.addVariable(keyName+"OutPointWidth",outPointWidth);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"EndGap",endGap);
  Control.addVariable(keyName+"EndLength",endLength);

  
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  
  Control.addVariable(keyName+"WallThick",wallThick);
    
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  

  QGen->generateQuad(Control,keyName+"QuadX",-12.0,18.5);
  QGen->generateQuad(Control,keyName+"QuadZ",12.0,18.5);
  return;
}

  
}  // NAMESPACE setVariable
