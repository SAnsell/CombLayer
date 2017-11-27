/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ProtonVoid.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "generateSurf.h"
#include "SimProcess.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ProtonVoid.h"

namespace ts1System
{

ProtonVoid::ProtonVoid(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  pvIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pvIndex+1),protonVoidCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ProtonVoid::ProtonVoid(const ProtonVoid& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  pvIndex(A.pvIndex),cellIndex(A.cellIndex),protonVoidCell(A.protonVoidCell),
  viewRadius(A.viewRadius)
  /*!
    Copy constructor
    \param A :: ProtonVoid to copy
  */
{}

ProtonVoid&
ProtonVoid::operator=(const ProtonVoid& A)
  /*!
    Assignment operator
    \param A :: ProtonVoid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      protonVoidCell=A.protonVoidCell;
      viewRadius=A.viewRadius;
    }
  return *this;
}

ProtonVoid::~ProtonVoid() 
 /*!
   Destructor
 */
{}

void
ProtonVoid::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("ProtonVoid","populate");  

  // Master values
  viewRadius=Control.EvalVar<double>(keyName+"ViewRadius");

  return;
}
  
void
ProtonVoid::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
  */
{
  ELog::RegMethod RegA("ProtonVoid","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  return;
}

void
ProtonVoid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ProtonVoid","createSurface");

  // Void hole
  ModelSupport::buildCylinder(SMap,pvIndex+7,Origin,Y,viewRadius);  
  return;
}

void
ProtonVoid::createObjects(Simulation& System,
			  const std::string& TargetSurfBoundary,
			  const std::string& RefSurfBoundary)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param TargetSurfBoundary :: boundary layer [expect to be target edge]
    \param RefSurfBoundary :: boundary layer [expect to be reflector edge]
  */
{
  ELog::RegMethod RegA("ProtonVoid","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,pvIndex, " -7 ");
  Out+=RefSurfBoundary+" "+TargetSurfBoundary;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  protonVoidCell=cellIndex-1;
  addOuterSurf(Out);
  addBoundarySurf(-SMap.realSurf(pvIndex+7));    

  return;
}

void
ProtonVoid::createLinks()
  /*!
    Creates a full attachment set [Internal]
  */
{

  return;
}

void
ProtonVoid::createAll(Simulation& System,
		      const attachSystem::FixedComp& TargetFC,
		      const long int tIndex,
		      const attachSystem::FixedComp& RefFC,
		      const long int rIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target value
    \param tIndex :: Target plate surface
    \param RefFC :: FixedComp for reflector (bounding surf)
    \param rIndex :: Reflector outer surf
  */
{
  ELog::RegMethod RegA("ProtonVoid","createAll");
  populate(System.getDataBase());

  createUnitVector(TargetFC);
  createSurfaces();
  const std::string TSurf=TargetFC.getLinkString(tIndex);
  const std::string RSurf=RefFC.getLinkString(rIndex);

  createObjects(System,TSurf,RSurf);
  createLinks();
  insertObjects(System);       
  //  buildChannels(System);
  //  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
