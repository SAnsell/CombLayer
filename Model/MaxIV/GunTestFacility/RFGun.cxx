/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/RFGun.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "RFGun.h"

namespace xraySystem
{

RFGun::RFGun(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RFGun::RFGun(const RFGun& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),cavityRadius(A.cavityRadius),cavityLength(A.cavityLength),
  cavitySideWallThick(A.cavitySideWallThick),
  cavityOffset(A.cavityOffset),
  wallThick(A.wallThick),
  frontFlangeThick(A.frontFlangeThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: RFGun to copy
  */
{}

RFGun&
RFGun::operator=(const RFGun& A)
  /*!
    Assignment operator
    \param A :: RFGun to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      cavityRadius=A.cavityRadius;
      cavityLength=A.cavityLength;
      cavitySideWallThick=A.cavitySideWallThick;
      cavityOffset=A.cavityOffset;
      wallThick=A.wallThick;
      frontFlangeThick=A.frontFlangeThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

RFGun*
RFGun::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new RFGun(*this);
}

RFGun::~RFGun()
  /*!
    Destructor
  */
{}

void
RFGun::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RFGun","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  cavityRadius=Control.EvalVar<double>(keyName+"CavityRadius");
  cavityLength=Control.EvalVar<double>(keyName+"CavityLength");
  cavitySideWallThick=Control.EvalVar<double>(keyName+"CavitySideWallThick");
  cavityOffset=Control.EvalVar<double>(keyName+"CavityOffset");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  frontFlangeThick=Control.EvalVar<double>(keyName+"FrontFlangeThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
RFGun::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RFGun","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(frontFlangeThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      wallThick);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+wallThick),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -wallThick);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,cavityRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,cavityRadius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*(cavityOffset+frontFlangeThick),Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+21,Y,cavityLength);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+21,Y,-cavitySideWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+22,Y,cavitySideWallThick);

  return;
}

void
RFGun::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RFGun","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -31 -7 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 32 -2 -7 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 7 -17 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 -17 ");
  makeCell("WallFront",System,cellIndex++,wallMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 2 -17 ");
  makeCell("WallBack",System,cellIndex++,wallMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 -7 ");
  makeCell("MainCavity",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 31 -21 -7 ");
  makeCell("MainCavitySideWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 -32 -7 ");
  makeCell("MainCavitySideWall",System,cellIndex++,wallMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
RFGun::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("RFGun","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(cavityRadius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(cavityRadius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(cavityLength/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(cavityLength/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
RFGun::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("RFGun","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
