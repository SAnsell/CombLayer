/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   species/TankMonoVesselGenerator.cxx
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

#include "CFFlanges.h"
#include "VacBoxGenerator.h"
#include "TankMonoVesselGenerator.h"

namespace setVariable
{

TankMonoVesselGenerator::TankMonoVesselGenerator() :
  VacBoxGenerator(),
  baseGap(4.0),topGap(6.0),
  lidOffset(3.0),lidRadius(3.0),lidDepth(3.0)
  /*!
    Constructor and defaults
  */
{
  setWallThick(1.0);
}


TankMonoVesselGenerator::~TankMonoVesselGenerator() 
 /*!
   Destructor
 */
{}
  
void
TankMonoVesselGenerator::generateBox(FuncDataBase& Control,
				     const std::string& keyName,
				     const double yStep,const double radius,
				     const double height,const double depth)
  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param radius :: radius of main cylinder
    \param depth :: depth of main cylinder
    \param height :: height of main cylinder
  */
{
  ELog::RegMethod RegA("TankMonoVesselGenerator","generateBox");
  

  VacBoxGenerator::generateBox(Control,keyName,yStep,radius*2.0,
			       height,depth,radius*2.0);
  const double LR= (radius>lidRadius) ? radius+lidRadius : lidRadius;

  Control.addVariable(keyName+"VoidRadius",radius);
  Control.addVariable(keyName+"BaseGap",baseGap);
  Control.addVariable(keyName+"TopGap",topGap);
  Control.addVariable(keyName+"LidOffset",lidOffset);
  Control.addVariable(keyName+"LidRadius",LR);
  Control.addVariable(keyName+"LidDepth",lidDepth);
  
  return;

}
  
}  // NAMESPACE setVariable
