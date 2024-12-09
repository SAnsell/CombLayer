/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/SlitsMask.cxx
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

#include "SlitsMask.h"

namespace xraySystem
{

SlitsMask::SlitsMask(const std::string& Key)  :
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

SlitsMask::SlitsMask(const SlitsMask& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  chamberLengthBack(A.chamberLengthBack),
  chamberLengthFront(A.chamberLengthFront),
  chamberDepth(A.chamberDepth),
  chamberHeight(A.chamberHeight),
  chamberWidth(A.chamberWidth),
  chamberWallThick(A.chamberWallThick),
  slitsMat(A.slitsMat),
  chamberMat(A.chamberMat)
  /*!
    Copy constructor
    \param A :: SlitsMask to copy
  */
{}

SlitsMask&
SlitsMask::operator=(const SlitsMask& A)
  /*!
    Assignment operator
    \param A :: SlitsMask to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      chamberLengthBack=A.chamberLengthBack;
      chamberLengthFront=A.chamberLengthFront;
      chamberDepth=A.chamberDepth;
      chamberHeight=A.chamberHeight;
      chamberWidth=A.chamberWidth;
      chamberWallThick=A.chamberWallThick;
      slitsMat=A.slitsMat;
      chamberMat=A.chamberMat;
    }
  return *this;
}

SlitsMask*
SlitsMask::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new SlitsMask(*this);
}

SlitsMask::~SlitsMask()
  /*!
    Destructor
  */
{}

void
SlitsMask::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SlitsMask","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  chamberLengthBack=Control.EvalVar<double>(keyName+"ChamberLengthBack");
  chamberLengthFront=Control.EvalVar<double>(keyName+"ChamberLengthFront");
  chamberDepth=Control.EvalVar<double>(keyName+"ChamberDepth");
  chamberHeight=Control.EvalVar<double>(keyName+"ChamberHeight");
  chamberWidth=Control.EvalVar<double>(keyName+"ChamberWidth");
  chamberWallThick=Control.EvalVar<double>(keyName+"ChamberWallThick");

  slitsMat=ModelSupport::EvalMat<int>(Control,keyName+"SlitsMat");
  chamberMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberMat");

  return;
}

void
SlitsMask::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SlitsMask","createSurfaces");

  if (frontActive()) {
    ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
				    SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				    chamberLengthFront+chamberLengthBack+chamberWallThick*2.0);

  } else {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*chamberLengthBack,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }

  if (backActive()) {
    ModelSupport::buildShiftedPlane(SMap, buildIndex+12,
				    SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				    -chamberWallThick);
  } else {
    ModelSupport::buildPlane(SMap,buildIndex+2,
			     Origin+Y*(chamberLengthFront+chamberLengthBack+chamberWallThick*2.0),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));

    ModelSupport::buildShiftedPlane(SMap, buildIndex+12, buildIndex+2, Y, -chamberWallThick);
  }

  ModelSupport::buildShiftedPlane(SMap, buildIndex+11,
				  SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				  chamberWallThick);

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
SlitsMask::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SlitsMask","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("Slits",System,cellIndex++,slitsMat,0.0,HR);

  addOuterSurf(HR);

  return;
}


void
SlitsMask::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("SlitsMask","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

  return;
}

void
SlitsMask::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SlitsMask","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
