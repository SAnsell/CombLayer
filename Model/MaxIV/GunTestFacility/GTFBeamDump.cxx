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

  if (frontActive()) {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+12,
				      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      brickLength);
  } else {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));

    ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(brickLength),Y);
  }

  if (backActive()) {
    ModelSupport::buildShiftedPlane(SMap, buildIndex+11,
				    SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				    -brickWidth*2.0);
  } else {
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(brickLength+brickThick),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));

    ModelSupport::buildShiftedPlane(SMap, buildIndex+11, buildIndex+2, Y, -brickWidth*2.0);
  }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(brickLength/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(brickLength/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(brickWidth/2.0+brickThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(brickWidth/2.0+brickThick),Z);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+13, buildIndex+3, X,  brickThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+14, buildIndex+4, X, -brickThick);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+15, buildIndex+5, Z,  brickThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+16, buildIndex+6, Z, -brickThick);

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

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -12 13 -14 15 -16 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -12 3 -13 15 -16 ");
  makeCell("Left",System,cellIndex++,brickMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -12 14 -4 15 -16 ");
  makeCell("Right",System,cellIndex++,brickMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 3 -4 15 -16 ");
  makeCell("Back",System,cellIndex++,brickMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 3 -4 5 -15 ");
  makeCell("Floor",System,cellIndex++,brickMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 3 -4 5 -15 ");
  makeCell("FloorVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 3 -4 16 -6 ");
  makeCell("Roof",System,cellIndex++,brickMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 3 -4 16 -6 ");
  makeCell("RoofVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -6 ");
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
