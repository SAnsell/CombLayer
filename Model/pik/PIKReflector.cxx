/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pik/PIKReflector.cxx
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
#include "Exception.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"

#include "PIKReflector.h"

namespace pikSystem
{

PIKReflector::PIKReflector(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  bottom(""),top(""),side("")
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PIKReflector::PIKReflector(const PIKReflector& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  roofPitch(A.roofPitch),floorPitch(A.floorPitch),height(A.height),
  radius(A.radius),
  wallThick(A.wallThick),
  mat(A.mat),shieldMat(A.shieldMat),wallMat(A.wallMat),
  bottom(A.bottom),top(A.top),side(A.side)
  /*!
    Copy constructor
    \param A :: PIKReflector to copy
  */
{}

PIKReflector&
PIKReflector::operator=(const PIKReflector& A)
  /*!
    Assignment operator
    \param A :: PIKReflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      roofPitch=A.roofPitch;
      floorPitch=A.floorPitch;
      height=A.height;
      radius=A.radius;
      wallThick=A.wallThick;
      mat=A.mat;
      shieldMat=A.shieldMat;
      wallMat=A.wallMat;
      bottom=A.bottom;
      top=A.top;
      side=A.side;
    }
  return *this;
}

PIKReflector*
PIKReflector::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PIKReflector(*this);
}

PIKReflector::~PIKReflector()
  /*!
    Destructor
  */
{}

void
PIKReflector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PIKReflector","populate");

  FixedRotate::populate(Control);

  roofPitch=Control.EvalVar<double>(keyName+"RoofPitch");
  floorPitch=Control.EvalVar<double>(keyName+"FloorPitch");
  height=Control.EvalVar<double>(keyName+"Height");
  radius=Control.EvalVar<double>(keyName+"OuterRadius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
PIKReflector::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKReflector","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
PIKReflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKReflector","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius-wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius);

  double ConeCut = 0.1; // guaranteed cone and auxiliary plane intersection

  double h = radius*tan(floorPitch*M_PI/180.0);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*((height/2.0+h)-ConeCut),Z); // aux plane
  ModelSupport::buildCone(SMap,buildIndex+18,Origin-Z*(height/2.0+h),Z,90-floorPitch);
  h -= wallThick/cos(floorPitch*M_PI/180.0);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*((height/2.0+h)-ConeCut),Z);
  ModelSupport::buildCone(SMap,buildIndex+8,Origin-Z*(height/2.0+h),Z,90-floorPitch);

  h = radius*tan(roofPitch*M_PI/180.0);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*((height/2.0+h)-ConeCut),Z); // aux plane 2
  ModelSupport::buildCone(SMap,buildIndex+19,Origin+Z*(height/2.0+h),Z,90-roofPitch);
  h -= wallThick/cos(roofPitch*M_PI/180.0);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*((height/2.0+h)-ConeCut),Z);
  ModelSupport::buildCone(SMap,buildIndex+9,Origin+Z*(height/2.0+h),Z,90-roofPitch);

  // aux planes

  return;
}

void
PIKReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PIKReflector","createObjects");

  if (side.empty())
    throw ColErr::ExitAbort("Reflector side surface not set");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 15 18 -17 ");
  makeCell("Bottom",System,cellIndex++,shieldMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -15 -17 ") + bottom;
  makeCell("Bottom",System,cellIndex++,shieldMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 19 -17 -16 ");
  makeCell("Top",System,cellIndex++,shieldMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 16 -17 ") + top;
  makeCell("Top",System,cellIndex++,shieldMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -8 -9 -7 -6 ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -18 8 -7 ");
  makeCell("WallBottomUpper",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -5 15 -18 ");
  makeCell("WallBottomLower",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -19 9 -7 -6 ");
  makeCell("WallTopLower",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -19 6 -16 ");
  makeCell("WallTopUpper",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 -19 -18 15 -16 ");
  makeCell("WallSide",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 ") + top + bottom;
  addOuterSurf(Out);

  return;
}


void
PIKReflector::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PIKReflector","createLinks");

  // FixedComp::setConnect(0,Origin-Y*(roofPitch/2.0),-Y);
  // FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  // FixedComp::setConnect(1,Origin+Y*(roofPitch/2.0),Y);
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  // FixedComp::setConnect(2,Origin-X*(floorPitch/2.0),-X);
  // FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  // FixedComp::setConnect(3,Origin+X*(floorPitch/2.0),X);
  // FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  // FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  // FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  // FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  // FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
PIKReflector::setBottom(const attachSystem::FixedComp& FC,
			const std::string& sideName)
/*!
  Set reflector bottom surface
 */
{
  ELog::RegMethod RegA("PIKReflector","setBottom");

  bottom = FC.getLinkString(FC.getSideIndex(sideName));

  return;
}

void
PIKReflector::setTop(const attachSystem::FixedComp& FC,
		     const std::string& sideName)
/*!
  Set reflector top surface
 */
{
  ELog::RegMethod RegA("PIKReflector","setTop");

  top = FC.getLinkString(FC.getSideIndex(sideName));

  return;
}

void
PIKReflector::setSide(const attachSystem::FixedComp& FC,
		      const std::string& sideName)
/*!
  Set reflector side surface
 */
{
  ELog::RegMethod RegA("PIKReflector","setSide");

  side = FC.getLinkString(FC.getSideIndex(sideName));

  return;
}

void
PIKReflector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKReflector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // pikSystem
