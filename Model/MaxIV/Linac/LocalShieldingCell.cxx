/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/LocalShieldingCell.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "Surface.h"
#include "ExternalCut.h"

#include "LocalShielding.h"
#include "LocalShieldingCell.h"

namespace tdcSystem
{

LocalShieldingCell::LocalShieldingCell(const std::string& Key,
				       const std::string& baseKey)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  baseName(baseKey),
  Units({
	 std::make_shared<tdcSystem::LocalShielding>(baseKey+"ShieldB"),
	 std::make_shared<tdcSystem::LocalShielding>(baseKey+"ShieldC"),
	 std::make_shared<tdcSystem::LocalShielding>(baseKey+"ShieldD"),
	 std::make_shared<tdcSystem::LocalShielding>(baseKey+"ShieldE"),
    }),
  connections({
	       {baseKey+"ShieldB",{"THIS",""}},
	       {baseKey+"ShieldC",{baseKey+"ShieldB","bottom"}},
	       {baseKey+"ShieldD",{baseKey+"ShieldB","front"}},
	       {baseKey+"ShieldE",{baseKey+"ShieldB","front"}}
    }),
  surfaces({
	    {"front",{"ShieldE","#back"}},     // -1050002
	    {"back",{"ShieldB","#back"}},     // -1020002
	    {"left",{"ShieldC","left"}},        // 1030003
	    {"right",{"ShieldB","#right"}},         // -102004
	    {"base",{"ShieldE","base"}},         // 1050005 
	    {"top",{"ShieldB","#top"}}       // -1020006 
    })
    
    
	       
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  for(const std::shared_ptr<LocalShielding>& uPtr : Units)
    OR.addObject(uPtr);

}

LocalShieldingCell::~LocalShieldingCell()
  /*!
    Destructor
  */
{}

void
LocalShieldingCell::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LocalShieldingCell","createObjects");

  // BUILD inner object first:
  
  mapTYPE::const_iterator mc;

  for(std::shared_ptr<LocalShielding>& uPtr : Units)
    {      
      const std::string unitName=uPtr->getKeyName();
      mc=connections.find(unitName);

      if (unitName=="THIS")
	{
	  ELog::EM<<"CREATE ING "<<ELog::endDiag;
	  uPtr->createAll(System,*this,0);
	}
      else if (mc!=connections.end())
	{
	  const std::string& linkUnit=mc->second.first;
	  const std::string& linkSide=mc->second.second;
	  const attachSystem::FixedComp* FCptr=
	    System.getObjectThrow<attachSystem::FixedComp>
	    (linkUnit,"FixedObject");

	  uPtr->createAll(System,*FCptr,linkSide);
	}
      else
	ELog::EM<<"HERE "<<unitName<<ELog::endDiag;
      
    }

  HeadRule HR;
  for(const auto& [name, sUnit] : surfaces)
    {
      const std::string& linkUnit=sUnit.first;
      const std::string& linkSide=sUnit.second;
      const attachSystem::SurfMap* SMptr=
	    System.getObjectThrow<attachSystem::SurfMap>
	    (baseName+linkUnit,"SurfMap");

      const HeadRule SurfHR = SMptr->getSurfRule(linkSide);
      HR *= SurfHR;
    }
  ELog::EM<<ELog::endDiag;

  makeCell("Main",System,cellIndex++,0,0.0,HR);
  addOuterSurf(HR);

  const int mainCell=getCell("Main");
  for(std::shared_ptr<LocalShielding>& uPtr : Units)
    uPtr->insertInCell(System,mainCell);
  
  return;
}


void
LocalShieldingCell::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("LocalShieldingCell","createLinks");

  // ExternalCut::createLink("front",*this,0,Origin,Y);
  // ExternalCut::createLink("back",*this,1,Origin,Y);

  return;
}

void
LocalShieldingCell::createAll(Simulation& System,
			      const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("LocalShieldingCell","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
