/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/DCMTankGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "VacBoxGenerator.h"
#include "DCMTankGenerator.h"

namespace setVariable
{

DCMTankGenerator::DCMTankGenerator() :
  VacBoxGenerator(),
  baseThick(2.0),baseWidth(65.0),baseLength(65.0),
  topRadius(80.0),topLift(9.0)
  /*!
    Constructor and defaults
  */
{
  setWallThick(1.0);
}


DCMTankGenerator::~DCMTankGenerator() 
 /*!
   Destructor
 */
{}
  
void
DCMTankGenerator::generateBox(FuncDataBase& Control,
			      const std::string& keyName,
			      const double radius,
			      const double height,
			      const double depth)
  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param radius :: radius of main cylinder
    \param depth :: depth of main cylinder
    \param height :: height of main cylinder
  */
{
  ELog::RegMethod RegA("DCMTankGenerator","generateBox");
  

  VacBoxGenerator::generateBox(Control,keyName,radius*2.0,
			       height,depth,radius*2.0);

  Control.addVariable(keyName+"VoidRadius",radius);
  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"BaseLength",baseLength);
  Control.addVariable(keyName+"BaseWidth",baseWidth);

  Control.addVariable(keyName+"TopRadius",topRadius);
  Control.addVariable(keyName+"TopLift",topLift);
  
  return;

}
  
}  // NAMESPACE setVariable
