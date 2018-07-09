/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/RingDoorGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "RingDoorGenerator.h"

namespace setVariable
{

RingDoorGenerator::RingDoorGenerator() :
  innerHeight(180.0),innerWidth(180.0),innerThick(50.0),
  outerHeight(240.0),outerWidth(240.0),gapSpace(1.0),
  doorMat("Concrete")
  /*!
    Constructor and defaults
  */
{}


RingDoorGenerator::~RingDoorGenerator() 
 /*!
   Destructor
 */
{}

void
RingDoorGenerator::setInner(const double W,
			    const double H,
			    const double T)
  
  /*!
    Set the inner door
    \param W :: Width of door
    \param H :: Height of door
    \param T :: Thickness of dorr
   */
{
  innerWidth=W;
  innerHeight=H;
  innerThick=T;
  return;
}

void
RingDoorGenerator::setOuter(const double W,
			    const double H)
  
  /*!
    Set the outer door
    \param W :: Width of door
    \param H :: Height of door
    \param T :: Thickness of dorr
   */
{
  outerWidth=W;
  outerHeight=H;
  return;
}

				  
void
RingDoorGenerator::generateRingDoor(FuncDataBase& Control,
				    const std::string& keyName,
				    const double xStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param xStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("RingDoorGenerator","generateRingDoor");
  
  Control.addVariable(keyName+"XStep",xStep);

  Control.addVariable(keyName+"InnerHeight",innerHeight);
  Control.addVariable(keyName+"InnerWidth",innerWidth);

  Control.addVariable(keyName+"OuterHeight",outerHeight);
  Control.addVariable(keyName+"OuterWidth",outerWidth);

  Control.addVariable(keyName+"GapSpace",gapSpace);
  Control.addVariable(keyName+"InnerThick",innerThick);

  Control.addVariable(keyName+"DoorMat",doorMat);
       
  return;

}

  
}  // NAMESPACE setVariable
