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
  slitLength(A.slitLength),slitWidth(A.slitWidth),slitHeight(A.slitHeight),
  wallThick(A.wallThick),
  portRadius(A.portRadius),
  frontPortLength(A.frontPortLength),
  backPortLength(A.backPortLength),
  leftPortLength(A.leftPortLength),
  rightPortLength(A.rightPortLength),
  bottomPortLength(A.bottomPortLength),
  topPortLength(A.topPortLength),
  outerFlangeRadius(A.outerFlangeRadius),
  outerFlangeThick(A.outerFlangeThick),
  outerFlangeCapThick(A.outerFlangeCapThick),
  outerFlangeWindowMat(A.outerFlangeWindowMat),
  slitsMat(A.slitsMat),
  chamberMat(A.chamberMat),
  voidMat(A.voidMat)
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
      slitLength=A.slitLength;
      slitWidth=A.slitWidth;
      slitHeight=A.slitHeight;
      wallThick=A.wallThick;
      portRadius=A.portRadius;
      frontPortLength=A.frontPortLength;
      backPortLength=A.backPortLength;
      leftPortLength=A.leftPortLength;
      rightPortLength=A.rightPortLength;
      bottomPortLength=A.bottomPortLength;
      topPortLength=A.topPortLength;
      outerFlangeRadius=A.outerFlangeRadius;
      outerFlangeThick=A.outerFlangeThick;
      outerFlangeCapThick=A.outerFlangeCapThick;
      outerFlangeWindowMat=A.outerFlangeWindowMat;
      slitsMat=A.slitsMat;
      chamberMat=A.chamberMat;
      voidMat=A.voidMat;
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

  slitLength=Control.EvalVar<double>(keyName+"SlitLength");
  slitWidth=Control.EvalVar<double>(keyName+"SlitWidth");
  slitHeight=Control.EvalVar<double>(keyName+"SlitHeight");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  frontPortLength=Control.EvalVar<double>(keyName+"FrontPortLength");
  backPortLength=Control.EvalVar<double>(keyName+"BackPortLength");
  leftPortLength=Control.EvalVar<double>(keyName+"LeftPortLength");
  rightPortLength=Control.EvalVar<double>(keyName+"RightPortLength");
  bottomPortLength=Control.EvalVar<double>(keyName+"BottomPortLength");
  topPortLength=Control.EvalVar<double>(keyName+"TopPortLength");
  outerFlangeRadius=Control.EvalVar<double>(keyName+"OuterFlangeRadius");
  outerFlangeThick=Control.EvalVar<double>(keyName+"OuterFlangeThickness");
  outerFlangeCapThick=Control.EvalVar<double>(keyName+"OuterFlangeCapThickness");
  outerFlangeWindowMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterFlangeWindowMat");

  slitsMat=ModelSupport::EvalMat<int>(Control,keyName+"SlitsMat");
  chamberMat=ModelSupport::EvalMat<int>(Control,keyName+"ChamberMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

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
	    frontPortLength+backPortLength+2*outerFlangeCapThick);

  } else {
    ModelSupport::buildPlane(SMap,buildIndex+1,
			     Origin-Y*(frontPortLength+outerFlangeCapThick),Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,
			     Origin+Y*(backPortLength+outerFlangeCapThick),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
  }

  SurfMap::makePlane("left", SMap,buildIndex+3, Origin-X*(leftPortLength+outerFlangeCapThick), X);
  SurfMap::makePlane("right",SMap,buildIndex+4, Origin+X*(rightPortLength+outerFlangeCapThick),X);

  SurfMap::makePlane("bottom",SMap,buildIndex+5, Origin-Z*(bottomPortLength+outerFlangeCapThick),Z);
  SurfMap::makePlane("top",   SMap,buildIndex+6, Origin+Z*(topPortLength+outerFlangeCapThick),   Z);

  // Ports
  // back-front
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Y,portRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,Origin,Y,outerFlangeRadius);

  // left-right
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,X,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,X,portRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,X,outerFlangeRadius);

  // top-bottom
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,portRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+427,Origin,Z,outerFlangeRadius);

  // Slits
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(slitLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(slitLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(slitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(slitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(slitHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(slitHeight/2.0),Z);

  // Outer flanges
  // ModelSupport::buildShiftedPlane(SMap, buildIndex+11,
  // 				  SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
  // 				  outerFlangeCapThick);
  // ModelSupport::buildShiftedPlane(SMap, buildIndex+21,buildIndex+11,outerFlangeThick);

  // bottom and top flanges
  ModelSupport::buildShiftedPlane(SMap, buildIndex+15,buildIndex+5,  Z, outerFlangeCapThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+25,buildIndex+15, Z, outerFlangeThick);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+16,buildIndex+6,  Z, -outerFlangeCapThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+26,buildIndex+16, Z, -outerFlangeThick);

  // left and right flanges
  ModelSupport::buildShiftedPlane(SMap, buildIndex+13,buildIndex+3,  X, outerFlangeCapThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+23,buildIndex+13, X, outerFlangeThick);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+14,buildIndex+4,  X, -outerFlangeCapThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+24,buildIndex+14, X, -outerFlangeThick);
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

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  // makeCell("Slits",System,cellIndex++,slitsMat,0.0,HR);

  // back-front
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 -217 1 -2 317 417");
  makeCell("PortFrontBackWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -207 1 -2 ");
  makeCell("PortFrontBackInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 317 417 1 -2 23 -24 25 -26");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);


  // top-bottom
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 407 -417 207 317 15 -16");
  makeCell("PortTopBottomWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 307 -407 15 -16");
  makeCell("PortTopBottomInner",System,cellIndex++,voidMat,0.0,HR);

  // left-right
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 307 -317 207 407 13 -14");
  makeCell("PortLeftWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -307 13 -14 207");
  makeCell("PortLeftInner",System,cellIndex++,voidMat,0.0,HR);

  // Flanges
  if (outerFlangeCapThick>Geometry::zeroTol) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex," 5 -15 -427");
    makeCell("BottomFlangeCap",System,cellIndex++,chamberMat,0.0,HR);

    HR=ModelSupport::getHeadRule(SMap,buildIndex,"16 -6 -427");
    makeCell("TopFlangeCap",System,cellIndex++,chamberMat,0.0,HR);

    HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -13 -327");
    makeCell("LeftFlangeCap",System,cellIndex++,chamberMat,0.0,HR);

    HR=ModelSupport::getHeadRule(SMap,buildIndex," 14 -4 -327");
    makeCell("RightFlangeCap",System,cellIndex++,chamberMat,0.0,HR);
  }
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 417 -427 15 -25");
  makeCell("BottomFlange",System,cellIndex++,chamberMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -25 427");
  makeCell("BottomFlangeOuter",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"417 -427 26 -16");
  makeCell("TopFlange",System,cellIndex++,chamberMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 26 -6 427");
  makeCell("TopFlangeOuter",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 317 -327 13 -23");
  makeCell("LeftFlange",System,cellIndex++,chamberMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -23 25 -26 327");
  makeCell("LeftFlangeOuter",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 317 -327 24 -14");
  makeCell("RightFlange",System,cellIndex++,chamberMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 24 -4 25 -26 327");
  makeCell("RightFlangeOuter",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");

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

  FixedComp::setConnect(2,Origin-X*(leftPortLength+outerFlangeCapThick),-X);
  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(rightPortLength+outerFlangeCapThick),X);
  FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(bottomPortLength+outerFlangeCapThick),-Z);
  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(topPortLength+outerFlangeCapThick),Z);
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
