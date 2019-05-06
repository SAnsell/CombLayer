/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MagnetM1Generator.cxx
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

#include "MagnetM1Generator.h"

namespace setVariable
{

MagnetM1Generator::MagnetM1Generator() :
  blockYStep(1.5),length(229.0),
  outerVoid(8.0),ringVoid(8.0),baseVoid(8.0),
  topVoid(8.0),baseThick(5.0),wallThick(5.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
MagnetM1Generator::~MagnetM1Generator() 
 /*!
   Destructor
 */
{}

void
MagnetM1Generator::generateBlock(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("MagnetM1Generator","generateBlock");

  Control.addVariable(keyName+"BlockYStep",blockYStep);
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"OuterVoid",outerVoid);
  Control.addVariable(keyName+"RingVoid",ringVoid);
  Control.addVariable(keyName+"TopVoid",topVoid);
  Control.addVariable(keyName+"BaseVoid",baseVoid);

  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"WallThick",wallThick);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  
  return;
}

  
}  // NAMESPACE setVariable
