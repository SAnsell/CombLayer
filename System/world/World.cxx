/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   world/World.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "objectRegister.h"

namespace World
{

const attachSystem::FixedComp&
masterOrigin()
  /*!
    Provides a master origin point for the model
    - X/Y/Z are normal unit vectors 
    \return Fixed Unit
  */
{
  static attachSystem::FixedUnit MO("World",0);
  return MO;
}

const attachSystem::FixedComp&
masterZMinusOrigin()
  /*!
    Provides a master origin point for the model
    - X/Y/Z are normal unit vectors 
    \return Fixed Unit
  */
{
  static attachSystem::FixedUnit MO("World",0);
  
  return MO;
}

const attachSystem::FixedComp&
masterTS2Origin()
  /*!
    Provides a master origin point for the model
    - X/Y/Z are normal unit vectors 
    \return Fixed Unit
  */
{
  static attachSystem::FixedUnit MO("WorldTS2",0,
				    Geometry::Vec3D(0,0,0),
				    Geometry::Vec3D(0,1,0),
				    Geometry::Vec3D(0,0,-1),
				    Geometry::Vec3D(-1,0,0));
  return MO;
}


void
buildWorld(objectGroups& OGrp)
  /*!
    Build and register a world object in a object group
    \param OGrp :: Simluation to add a world to.
  */
{
  ELog::RegMethod RegA("World[F]","buildWorld");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.setObjectGroup(OGrp);

  std::shared_ptr<attachSystem::FixedComp> worldPtr=
    std::make_shared<attachSystem::FixedUnit>(World::masterOrigin());
  if (!OGrp.hasRegion("World"))
    ModelSupport::objectRegister::Instance().cell("World",10000);
  OGrp.addObject(worldPtr);
  return;
}
  
void 
createOuterObjects(Simulation& System,
		   const double worldRadius)
  /*!
    Create all the world outer objects
    \param System :: Simulation to modify
    \param worldRadius :: Size of world
  */
{ 
  ELog::RegMethod RegA("World","createOuterObjects");
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();

  // Create object 1
  SurI.createSurface(1,"so "+std::to_string(worldRadius));
  MonteCarlo::Object tmpCell(1,0,0.0," 1 ");
  tmpCell.setImp(0);
  System.addCell(tmpCell);

  // MASTER DIVIDE:

  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(40000);   
  PX->setPlane(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  SurI.insertSurface(PX);                        // Force onto stack

  PX=SurI.createUniqSurf<Geometry::Plane>(50000);   
  PX->setPlane(Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));
  SurI.insertSurface(PX);                        // Force onto stack

  PX=SurI.createUniqSurf<Geometry::Plane>(60000);   
  PX->setPlane(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));
  SurI.insertSurface(PX);                        // Force onto stack

  //   Create object 74123
  System.addCell(MonteCarlo::Object(74123,0,0.0," -1 "));
  return;
}
    
} // NAMESPACE World
