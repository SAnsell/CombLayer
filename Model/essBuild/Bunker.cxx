/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/Bunker.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
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
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "Bunker.h"

namespace essSystem
{

Bunker::Bunker(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bunker::~Bunker() 
  /*!
    Destructor
  */
{}

void
Bunker::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("Bunker","populate");
  
  return;
}
  
void
  Bunker::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("Bunker","createUnitVector");

  FixedComp::createUnitVector(FC);
    

  return;
}
  
void
Bunker::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bunker","createSurface");

  // rotation of axis:

  return;
}

void
Bunker::createObjects(Simulation& System,
		      const attachSystem::FixedComp&,
		      const long int)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Bunker","createObjects");
  return;
}

void
Bunker::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Bunker","createLinks");
  return;
}


void
Bunker::createAll(Simulation& System,
		  const attachSystem::FixedComp& MainCentre,
		  const attachSystem::FixedComp& FC,
		  const long int linkIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("Bunker","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,linkIndex);
  createSurfaces();
  createLinks();
  createObjects(System,FC,linkIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
