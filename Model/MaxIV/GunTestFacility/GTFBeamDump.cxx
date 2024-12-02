/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/GTFBeamDump.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "GTFBeamDump.h"

namespace MAXIV::GunTestFacility
{

GTFBeamDump::GTFBeamDump(const std::string& Key)  :
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

GTFBeamDump::GTFBeamDump(const GTFBeamDump& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  brickLength(A.brickLength),brickWidth(A.brickWidth),brickThick(A.brickThick),
  gapThick(A.gapThick),
  brickMat(A.brickMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: GTFBeamDump to copy
  */
{}

GTFBeamDump&
GTFBeamDump::operator=(const GTFBeamDump& A)
  /*!
    Assignment operator
    \param A :: GTFBeamDump to copy
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
      brickLength=A.brickLength;
      brickWidth=A.brickWidth;
      brickThick=A.brickThick;
      gapThick=A.gapThick;
      brickMat=A.brickMat;
      voidMat=A.voidMat;
    }
  return *this;
}

GTFBeamDump*
GTFBeamDump::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new GTFBeamDump(*this);
}

GTFBeamDump::~GTFBeamDump()
  /*!
    Destructor
  */
{}

void
GTFBeamDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("GTFBeamDump","populate");

  FixedRotate::populate(Control);

  brickLength=Control.EvalVar<double>(keyName+"BrickLength");
  brickWidth=Control.EvalVar<double>(keyName+"BrickWidth");
  brickThick=Control.EvalVar<double>(keyName+"BrickThick");
  gapThick=Control.EvalVar<double>(keyName+"GapThick");

  brickMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
GTFBeamDump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GTFBeamDump","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(gapThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      gapThick);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(brickLength+gapThick),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(brickLength),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -gapThick);
    }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(brickWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(brickWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(brickThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(brickThick/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(brickWidth/2.0+gapThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(brickWidth/2.0+gapThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(brickThick/2.0+gapThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(brickThick/2.0+gapThick),Z);

  return;
}

void
GTFBeamDump::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GTFBeamDump","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("MainCell",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  makeCell("Wall",System,cellIndex++,brickMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 13 -14 15 -16");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
GTFBeamDump::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("GTFBeamDump","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(brickWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(brickWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(brickThick/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(brickThick/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
GTFBeamDump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("GTFBeamDump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // MAXIV::GunTestFacility
