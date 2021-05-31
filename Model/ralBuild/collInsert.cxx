/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/collInsert.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "collInsert.h"

namespace shutterSystem
{

collInsert::collInsert(const std::string& Key,const int ID) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffsetGroup(Key+std::to_string(ID),"Main",2,"Beam",2),
  baseName(Key),blockID(ID)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value of block
    \param SN :: surface Index value
    \param Key :: Name for item in search
  */
{}


void
collInsert::populate(const FuncDataBase& Control)
{
  ELog::RegMethod RegA("collInsert","populate");

  ELog::EM<<"Un finished method"<<ELog::endErr;
  return;
}
  
void
collInsert::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BeamTube","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  beamFC.createUnitVector(FC,sideIndex);
  mainFC.createUnitVector(FC,sideIndex);
  
  FixedOffsetGroup::applyOffset();
   
  setDefault("Main");
  return;
}
  

void
collInsert::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{
  ELog::RegMethod RegA("collInsert","createLinks");
  
  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");
  
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*length,Y);
 
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));

  const Geometry::Vec3D beamOrigin(beamFC.getCentre());
  const Geometry::Vec3D bY(beamFC.getY());

  beamFC.setConnect(0,beamOrigin,-bY);
  beamFC.setConnect(1,beamOrigin+bY*length,bY);
 
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}


Geometry::Vec3D
collInsert::getWindowCentre() const
  /*!
    Calculate the effective window in the void
    \return Centre 
  */
{
  ELog::RegMethod RegA("collInsert","getWindowCentre");
  
  const attachSystem::FixedComp& beamFC=getKey("Beam");
  const Geometry::Vec3D beamOrigin(beamFC.getCentre());
  
  return beamOrigin;
}

void
collInsert::createSurfaces()
{
  ELog::RegMethod RegA("collInsert","createSurfaces");
  return;
}

void
collInsert::createObjects(Simulation&)
{
  ELog::RegMethod RegA("collInsert","createObject");
  return;
}
  
  
void
collInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp (shutter)
    \param sideIndex :: side poitn
  */
{
  ELog::RegMethod RegA("collInsert","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

} // NAMESPACE shutterSystem
