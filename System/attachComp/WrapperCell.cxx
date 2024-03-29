/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   attachComp/WrapperCell.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "interPoint.h"
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
#include "support.h"
#include "generateSurf.h"

#include "WrapperCell.h"

namespace attachSystem
{

WrapperCell::WrapperCell(std::string  baseKey,
			 const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  baseName(std::move(baseKey)),
  nextSurfIndex(cellIndex)
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

HeadRule
WrapperCell::processSurfLink(const Simulation& System,
			     const std::string& linkUnit,
			     const std::string& linkSide)
/*!
  process the surface linke to all "linkSide + value"
  
*/
{
  ELog::RegMethod RegA("WrapperCell","processSurfLink");

  const attachSystem::SurfMap* SMptr=
    System.getObjectThrow<attachSystem::SurfMap>
	(baseName+linkUnit,"SurfMap item:");

  const std::string::size_type pos=linkSide.find_first_of("+-");
  double shiftV;
  if (pos!=std::string::npos &&
      StrFunc::convert(linkSide.substr(pos+1),shiftV))
    {
      const std::string linkPart=linkSide.substr(0,pos);

      const int surfN=SMptr->getSignedSurf(linkPart);
      const int surfSignN=(surfN>0) ? 1 : -1;
      ModelSupport::buildShiftedSurf
	(SMap,nextSurfIndex,std::abs(surfN),Y,shiftV);
      nextSurfIndex++;
      return HeadRule(SMap,surfSignN*(nextSurfIndex-1));
    }

  return SMptr->getSurfRule(linkSide);
    
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

  // This is the construction of all the parts of the wrapper
  // cell.
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
      const HeadRule SurfHR=processSurfLink(System,sUnit.first,sUnit.second);
      ELog::EM<<"Surface["<<linkUnit<<"] == "
	      <<" "<<linkSide<<" :"<<SurfHR<<ELog::endDiag;
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

  const Geometry::interPoint interAA=
    HR.trackSurfIntersect(Origin,-Y);
  const Geometry::interPoint interBB=
    HR.trackSurfIntersect(Origin,Y);

  if (interAA.SNum)
    {
      FixedComp::setConnect(0,Origin-Y*interAA.D,-Y);
      FixedComp::setLinkSurf(0,interAA.SNum);
    }
  if (interBB.SNum)
    {
      FixedComp::setConnect(1,Origin-Y*interBB.D,Y);
      FixedComp::setLinkSurf(1,interBB.SNum);
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
