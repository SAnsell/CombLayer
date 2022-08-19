/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ProtonVoid.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ProtonVoid.h"

namespace ts1System
{

ProtonVoid::ProtonVoid(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,3),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ProtonVoid::ProtonVoid(const ProtonVoid& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
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
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
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
ProtonVoid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ProtonVoid","createSurface");

  // Void hole
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,viewRadius);  
  return;
}

void
ProtonVoid::createObjects(Simulation& System)
  /*!
    Creates the proton objects within the reflector front edge(s)
    and the target front face.
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ProtonVoid","createObjects");


  const HeadRule& TSurfHR=ExternalCut::getRule("TargetSurf");
  const HeadRule& RSurfHR=ExternalCut::getRule("RefBoundary");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  ExternalCut::setCutSurf("Boundary",HR);
  HR*=RSurfHR*TSurfHR;
  ELog::EM<<"Proton surf == "<<TSurfHR<<ELog::endDiag;
  CellMap::makeCell("VoidCell",System,cellIndex++,0,0.0,HR);
  addOuterSurf(HR);
  //  addBoundarySurf(-SMap.realSurf(buildIndex+7));    

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
		      const long int tIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target value
    \param tIndex :: Target plate surface
  */
{
  ELog::RegMethod RegA("ProtonVoid","createAll");
  populate(System.getDataBase());

  createUnitVector(TargetFC,tIndex);
  createSurfaces();

  if (!ExternalCut::isActive("TargetSurf"))
    ExternalCut::setCutSurf("TargetSurf",TargetFC,tIndex);  
  createObjects(System);
  createLinks();
  insertObjects(System);       
  //  buildChannels(System);
  //  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
