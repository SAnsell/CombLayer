/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   epbBuild/Building.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h" 
#include "ContainedComp.h"
#include "Building.h"

namespace epbSystem
{

Building::Building(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Building::Building(const Building& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  height(A.height),
  depth(A.depth),width(A.width),length(A.length),
  floorThick(A.floorThick),wallThick(A.wallThick),
  roofThick(A.roofThick),concMat(A.concMat)
  /*!
    Copy constructor
    \param A :: Building to copy
  */
{}

Building&
Building::operator=(const Building& A)
  /*!
    Assignment operator
    \param A :: Building to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      height=A.height;
      depth=A.depth;
      width=A.width;
      length=A.length;
      floorThick=A.floorThick;
      wallThick=A.wallThick;
      roofThick=A.roofThick;
      concMat=A.concMat;
    }
  return *this;
}

Building::~Building() 
  /*!
    Destructor
  */
{}

void
Building::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Building","populate");

  const FuncDataBase& Control=System.getDataBase();
  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");

  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}

void
Building::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: TwinComp to attach to
  */
{
  ELog::RegMethod RegA("Building","createUnitVector");
  FixedComp::createUnitVector(FC);
  applyOffset();

  return;
}

void
Building::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Building","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  // Outer Walls
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);
  return;
}

void
Building::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Building","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 13 -14 15 -16 ");
  addOuterSurf(Out);      
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 13 -14 15 -16 (-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  return;
}

void 
Building::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Building","createLinks");

  FixedComp::setConnect(0,Origin,-Y);     
  FixedComp::setConnect(1,Origin+Y*length,Y);     
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick),-X);     
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick),X);     
  FixedComp::setConnect(4,Origin+Z*(height+roofThick),-Z);     
  FixedComp::setConnect(5,Origin+Z*(height+floorThick),Z);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));
  
  return;
}


void
Building::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track ( epbline)
  */
{
  ELog::RegMethod RegA("Building","createAll");
  
  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
