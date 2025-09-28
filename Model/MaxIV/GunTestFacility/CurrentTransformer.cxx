/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/CurrentTransformer.cxx
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

#include "CurrentTransformer.h"

namespace xraySystem
{

CurrentTransformer::CurrentTransformer(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,7),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CurrentTransformer::CurrentTransformer(const CurrentTransformer& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),frontRadius(A.frontRadius),midRadius(A.midRadius),
  backRadius(A.backRadius),
  outerRadius(A.outerRadius),
  wallThick(A.wallThick),
  voidMat(A.voidMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: CurrentTransformer to copy
  */
{}

CurrentTransformer&
CurrentTransformer::operator=(const CurrentTransformer& A)
  /*!
    Assignment operator
    \param A :: CurrentTransformer to copy
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
      frontRadius=A.frontRadius;
      midRadius=A.midRadius;
      backRadius=A.backRadius;
      outerRadius=A.outerRadius;
      wallThick=A.wallThick;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}

CurrentTransformer*
CurrentTransformer::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new CurrentTransformer(*this);
}

CurrentTransformer::~CurrentTransformer()
  /*!
    Destructor
  */
{}

void
CurrentTransformer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CurrentTransformer","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  frontRadius=Control.EvalVar<double>(keyName+"FrontRadius");
  midRadius=Control.EvalVar<double>(keyName+"MidRadius");
  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
CurrentTransformer::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CurrentTransformer","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(wallThick),Y);
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

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,frontRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,midRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,backRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,outerRadius);

  return;
}

void
CurrentTransformer::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CurrentTransformer","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -37 -1");
  makeCell("Front",System,cellIndex++,wallMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -1");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"17 -37 1 -2");
  makeCell("Mid",System,cellIndex++,wallMat,0.0,HR );

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 1 -2");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR );

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"27 -37 2");
  makeCell("Back",System,cellIndex++,wallMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 2");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-37");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
CurrentTransformer::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("CurrentTransformer","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(outerRadius/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+37));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(40000));

  FixedComp::setConnect(3,Origin+X*(outerRadius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+37));
  FixedComp::setBridgeSurf(3,SMap.realSurf(40000));

  FixedComp::setConnect(4,Origin-Z*(outerRadius/2.0),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+37));
  FixedComp::setBridgeSurf(4,-SMap.realSurf(60000));

  FixedComp::setConnect(5,Origin+Z*(outerRadius/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+37));
  FixedComp::setBridgeSurf(5,SMap.realSurf(60000));

  FixedComp::setConnect(6,Origin+Y*(length),Y);
  FixedComp::setNamedLinkSurf(6,"InnerBack",SMap.realSurf(buildIndex+2));

  return;
}

void
CurrentTransformer::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CurrentTransformer","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
