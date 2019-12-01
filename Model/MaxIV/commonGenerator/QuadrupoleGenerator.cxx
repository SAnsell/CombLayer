/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/QuadrupoleGenerator.cxx
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

#include "QuadrupoleGenerator.h"

namespace setVariable
{

QuadrupoleGenerator::QuadrupoleGenerator() :
  vertGap(4.0),
  width(20.0),height(20.0),
  coilLength(2.0),coilCornerRad(2.0),
  coilWidth(8.0),frameThick(2.0),
  poleLength(6.0),poleRadius(1.2),
  poleZStep(0.5),poleYAngle(30.0),
  poleStep(1.5),poleWidth(1.5),
  poleMat("Iron"),coreMat("Iron"),
  coilMat("Copper"),frameMat("Aluminium")
  /*!
    Constructor and defaults
  */
{}
  
QuadrupoleGenerator::~QuadrupoleGenerator() 
 /*!
   Destructor
 */
{}

void
QuadrupoleGenerator::generateQuad(FuncDataBase& Control,
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
  ELog::RegMethod RegA("QuadrupoleGenerator","generateColl");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"VertGap",vertGap);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"CoilLength",coilLength+length);
  Control.addVariable(keyName+"CoilCornerRad",coilCornerRad);
  Control.addVariable(keyName+"CoilWidth",coilWidth);
  Control.addVariable(keyName+"FrameThick",frameThick);
  
  Control.addVariable(keyName+"PoleLength",poleLength);
  Control.addVariable(keyName+"PoleRadius",poleRadius);
  Control.addVariable(keyName+"PoleZStep",poleZStep);
  Control.addVariable(keyName+"PoleYAngle",poleYAngle);
  Control.addVariable(keyName+"PoleStep",poleStep);
  Control.addVariable(keyName+"PoleWidth",poleWidth);
  
  Control.addVariable(keyName+"PoleMat",poleMat);
  Control.addVariable(keyName+"CoreMat",coreMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"FrameMat",frameMat);
  
  return;

}

  
}  // NAMESPACE setVariable
