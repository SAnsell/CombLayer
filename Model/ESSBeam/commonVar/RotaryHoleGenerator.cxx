/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/RotaryHoleGenerator.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "RotaryHoleGenerator.h"

namespace setVariable
{

RotaryHoleGenerator::RotaryHoleGenerator() :
  defMat("Tungsten")
  /*!
    Constructor and defaults
  */
{}
  
RotaryHoleGenerator::~RotaryHoleGenerator() 
 /*!
   Destructor
 */
{}

void
RotaryHoleGenerator::addHole(const std::string& type,
			     const double Rad,const double xRad,
			     const double aCent,const double aOff,
			     const rStep)
/*!
  Add a simple hole
  \param type :: type of hole []
  \param Rad :: radius
  \param xRad :: xRadius [if used]
  \param aCent :: Angular centre ??
  \param aCent :: Angular Offset 
  \param radStep :: distance from centre
*/
{
  ELog::RegMethod RegA("RotaryHoleGenerator","addHole");

  if (type=="
  return;
}
  
  
void
RotaryHoleGenerator::generatePinHole(FuncDataBase& Control,
                                 const std::string& keyName,
                                 const double yStep) const


  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param innerRadius :: Inner radius
    \param outerRadius :: Outer radius
  */
{
  ELog::RegMethod RegA("RotaryHoleGenerator","generatorPinHole");

    Control.addVariable("odinPinCollAInnerWall",1.0);
  Control.addVariable("odinPinCollAInnerWallMat","Void");

  Control.addVariable("odinPinCollANLayers",0);
  Control.addVariable("odinPinCollAHoleIndex",0);
  Control.addVariable("odinPinCollAHoleAngOff",0.0);

  Control.addVariable("odinPinCollADefMat","Inconnel");
  // collimator holes:
  Control.addVariable("odinPinCollANHole",3);
  Control.addVariable("odinPinCollAHole0Shape",1);
  Control.addVariable("odinPinCollAHole0Radius",3.0);

  Control.addVariable("odinPinCollAHole0AngleCentre",0.0);
  Control.addVariable("odinPinCollAHole0AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole0RadialStep",20.0);

  Control.addVariable("odinPinCollAHole1Shape",3);
  Control.addVariable("odinPinCollAHole1Radius",5.0);
  Control.addVariable("odinPinCollAHole1AngleCentre",120.0);
  Control.addVariable("odinPinCollAHole1AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole1RadialStep",15.0);
  
  Control.addVariable("odinPinCollAHole2Shape",1);
  Control.addVariable("odinPinCollAHole2Radius",4.0);
  Control.addVariable("odinPinCollAHole2AngleCentre",240.0);
  Control.addVariable("odinPinCollAHole2AngleOffset",0.0);
  Control.addVariable("odinPinCollAHole2RadialStep",20.0);

  return;

}

}  // NAMESPACE setVariable
