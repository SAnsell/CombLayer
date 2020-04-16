/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CorrectorMagGenerator.cxx
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

#include "CorrectorMagGenerator.h"

namespace setVariable
{

CorrectorMagGenerator::CorrectorMagGenerator() :
  length(10.30),magOffset(6.185),
  magHeight(2.4),magWidth(5.62),magLength(14.04),
  magCorner(1.72),magInnerWidth(2.13),
  magInnerLength(11.5),frameHeight(2.0),
  voidMat("Void"),coilMat("Copper"),frameMat("Iron")
  /*!
    Constructor and defaults
  */
{}
  
CorrectorMagGenerator::~CorrectorMagGenerator() 
 /*!
   Destructor
 */
{}

void
CorrectorMagGenerator::generateMag(FuncDataBase& Control,
				   const std::string& keyName,
				   const double yStep,
				   const bool verticalFlag) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param verticalFlag :: is the object rotated vertically
  */
{
  ELog::RegMethod RegA("CorrectorMagGenerator","generateQuad");

  const double yAngle((verticalFlag) ? 90.0 : 0.0);
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"YAngle",yAngle);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"MagOffset",magOffset);
  Control.addVariable(keyName+"MagWidth",magWidth);
  Control.addVariable(keyName+"MagHeight",magHeight);
  Control.addVariable(keyName+"MagLength",magLength);
  Control.addVariable(keyName+"MagCorner",magCorner);
  Control.addVariable(keyName+"MagInnerWidth",magInnerWidth);
  Control.addVariable(keyName+"MagInnerLength",magInnerLength);
  Control.addVariable(keyName+"FrameHeight",frameHeight);

  Control.addVariable(keyName+"VoidMat",coilMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"FrameMat",frameMat);
  
  return;

}

  
}  // NAMESPACE setVariable
