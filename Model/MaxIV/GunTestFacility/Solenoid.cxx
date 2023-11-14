/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/Solenoid.cxx
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

#include "Solenoid.h"

namespace xraySystem
{

Solenoid::Solenoid(const std::string& Key)  :
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

Solenoid::Solenoid(const Solenoid& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),frameWidth(A.frameWidth),spacerThick(A.spacerThick),
  frameThick(A.frameThick),
  frameMat(A.frameMat),coilMat(A.coilMat),
  nCoils(A.nCoils),
  nFrameFacets(A.nFrameFacets)
  /*!
    Copy constructor
    \param A :: Solenoid to copy
  */
{}

Solenoid&
Solenoid::operator=(const Solenoid& A)
  /*!
    Assignment operator
    \param A :: Solenoid to copy
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
      frameWidth=A.frameWidth;
      spacerThick=A.spacerThick;
      frameThick=A.frameThick;
      frameMat=A.frameMat;
      coilMat=A.coilMat;
      nCoils=A.nCoils;
      nFrameFacets=A.nFrameFacets;
    }
  return *this;
}

Solenoid*
Solenoid::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Solenoid(*this);
}

Solenoid::~Solenoid()
  /*!
    Destructor
  */
{}

void
Solenoid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Solenoid","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  frameWidth=Control.EvalVar<double>(keyName+"FrameWidth");
  spacerThick=Control.EvalVar<double>(keyName+"SpacerThick");
  frameThick=Control.EvalVar<double>(keyName+"FrameThick");

  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  nCoils=Control.EvalVar<size_t>(keyName+"NCoils");
  nFrameFacets=Control.EvalVar<size_t>(keyName+"NFrameFacets");

  return;
}

void
Solenoid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Solenoid","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(frameThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      frameThick);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+frameThick),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -frameThick);
    }

  int CN(buildIndex+1000);
  double angle(0.0);
  const double dangle = 2.0*M_PI/static_cast<double>(nFrameFacets);
  for (size_t i=0; i<nFrameFacets; ++i) {
    const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
    ModelSupport::buildPlane(SMap,CN+51,Origin+QR*(frameWidth),QR);
    angle+=dangle;
    CN++;
  }

  return;
}

void
Solenoid::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Solenoid","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  std::string unitStr="11M -12M"; // front-back planes
  unitStr+=ModelSupport::getSeqIntersection(-51, -(50+static_cast<int>(nFrameFacets)),1);
  HR=ModelSupport::getHeadRule(SMap,buildIndex+1000,buildIndex,unitStr);

  makeCell("MainCell",System,cellIndex++,frameMat,0.0,HR);

  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
Solenoid::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("Solenoid","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(frameWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(frameWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(spacerThick/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(spacerThick/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
Solenoid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Solenoid","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
