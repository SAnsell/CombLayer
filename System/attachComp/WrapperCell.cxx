/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   attachComp/WrapperCell.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "surfRegister.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"

#include "WrapperCell.h"

namespace attachSystem
{

WrapperCell::WrapperCell(const std::string& baseKey,
					 const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  baseName(baseKey)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

WrapperCell::~WrapperCell()
  /*!
    Destructor
  */
{}

void
WrapperCell::addUnit
(std::shared_ptr<attachSystem::FixedComp> UnitFC)
  /*!
    Adds a unit
    \param UnitFC :: Contained FixedComp unit
  */

{
  ELog::RegMethod RegA("WrapperCell","addUnit");

  Units.push_back(UnitFC);
  return;
}

void
WrapperCell::setSurfaces(mapTYPE&& surfMap)
  /*!
    Adds a unit
    \param surfMap :: Surfaces
  */

{
  ELog::RegMethod RegA("WrapperCell","setSurfaces");

  surfaces=surfMap;  
  return;
}

void
WrapperCell::setConnections(mapTYPE&& conMap)
  /*!
    Adds the connections map
    \param conMap :: connections
  */

{
  ELog::RegMethod RegA("WrapperCell","setConnections");

  connections=conMap;  
  return;
}
  
void
WrapperCell::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("WrapperCell","createObjects");

  // BUILD inner object first:
  
  mapTYPE::const_iterator mc;

  for(std::shared_ptr<attachSystem::FixedComp>& uPtr : Units)
    {      
      const std::string unitName=uPtr->getKeyName();
      mc=connections.find(unitName);

      if (mc!=connections.end())
	{
	  const std::string& linkUnit=mc->second.first;
	  const std::string& linkSide=mc->second.second;
	  if (linkUnit!="THIS" && linkUnit!="this")
	    {
	      const attachSystem::FixedComp* FCptr=
		System.getObjectThrow<attachSystem::FixedComp>
		(linkUnit,"FixedObject");
	      uPtr->createAll(System,*FCptr,linkSide);
	    }
	  else
	    uPtr->createAll(System,*this,0);
	}
      else
	throw ColErr::InContainerError<std::string>
	  (unitName,"Object for connection");
    }

  HeadRule HR;
  for(const auto& [name, sUnit] : surfaces)
    {
      const std::string& linkUnit=sUnit.first;
      const std::string& linkSide=sUnit.second;
      const attachSystem::SurfMap* SMptr=
	    System.getObjectThrow<attachSystem::SurfMap>
	    (baseName+linkUnit,"SurfMap item:");

      const HeadRule SurfHR = SMptr->getSurfRule(linkSide);
      HR *= SurfHR;
    }

  makeCell("Main",System,cellIndex++,0,0.0,HR);
  addOuterSurf(HR);

  const int mainCell=getCell("Main");
  for(std::shared_ptr<attachSystem::FixedComp>& uPtr : Units)
    {
      attachSystem::ContainedGroup* CGPtr=
	dynamic_cast<attachSystem::ContainedGroup*>(uPtr.get());
      if (CGPtr)
	CGPtr->insertAllInCell(System,mainCell);
      else
	{
	  attachSystem::ContainedComp* CPtr=
	    dynamic_cast<attachSystem::ContainedComp*>(uPtr.get());
	  if (CPtr)
	    CPtr->insertInCell(System,mainCell);

	}
    }
  
  
  return;
}


void
WrapperCell::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("WrapperCell","createLinks");

  // Use the Main cell:
  
  const HeadRule& HR=ContainedComp::outerSurf;

  const auto [SA,DA]=HR.trackSurfDistance(Origin,-Y);
  const auto [SB,DB]=HR.trackSurfDistance(Origin,Y);
  if (SA)
    {
      FixedComp::setConnect(0,Origin-Y*DA,-Y);
      FixedComp::setLinkSurf(0,SA);
    }
  if (SB)
    {
      FixedComp::setConnect(0,Origin-Y*DA,-Y);
      FixedComp::setLinkSurf(0,SA);
    }
  return;
}

void
WrapperCell::createAll(Simulation& System,
			      const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("WrapperCell","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
