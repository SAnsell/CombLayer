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
  portRadius(A.portRadius),
  portThick(A.portThick),
  frontLength(A.frontLength),
  backLength(A.backLength),
  bottomPortLength(A.bottomPortLength),
  topPortLength(A.topPortLength),
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
      length=A.length;
      width=A.width;
      height=A.height;
      chamberLengthBack=A.chamberLengthBack;
      chamberLengthFront=A.chamberLengthFront;
      chamberDepth=A.chamberDepth;
      chamberHeight=A.chamberHeight;
      chamberWidth=A.chamberWidth;
      chamberWallThick=A.chamberWallThick;
      portRadius=A.portRadius;
      portThick=A.portThick;
      frontLength=A.frontLength;
      backLength=A.backLength;
      bottomPortLength=A.bottomPortLength;
      topPortLength=A.topPortLength;
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

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  chamberLengthBack=Control.EvalVar<double>(keyName+"ChamberLengthBack");
  chamberLengthFront=Control.EvalVar<double>(keyName+"ChamberLengthFront");
  chamberDepth=Control.EvalVar<double>(keyName+"ChamberDepth");
  chamberHeight=Control.EvalVar<double>(keyName+"ChamberHeight");
  chamberWidth=Control.EvalVar<double>(keyName+"ChamberWidth");
  chamberWallThick=Control.EvalVar<double>(keyName+"ChamberWallThick");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portThick=Control.EvalVar<double>(keyName+"PortThick");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  bottomPortLength=Control.EvalVar<double>(keyName+"BottomPortLength");
  topPortLength=Control.EvalVar<double>(keyName+"TopPortLength");

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
				    chamberLengthFront+chamberLengthBack+chamberWallThick*2.0);

  } else {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*chamberLengthBack,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,
			     Origin+Y*(chamberLengthBack+chamberWallThick*2.0),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
  }

  ModelSupport::buildShiftedPlane(SMap, buildIndex+11,
				  SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				  chamberWallThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+12,
				  SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				  -chamberWallThick);

  SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(chamberWidth/2.0),X);
  SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(chamberWidth/2.0),X);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+13, buildIndex+3, X, chamberWallThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+14, buildIndex+4, X, -chamberWallThick);

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(chamberDepth),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(chamberHeight),Z);

  ModelSupport::buildShiftedPlane(SMap, buildIndex+15, buildIndex+5, X, chamberWallThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+16, buildIndex+6, X, -chamberWallThick);

  // Ports
  // back-front
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Y,portRadius+portThick);
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*(frontLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(backLength),Y);

  // top-bottom
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Z,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,Z,portRadius+portThick);
  ModelSupport::buildPlane(SMap,buildIndex+305,
			   Origin-Z*(chamberDepth+chamberWallThick+bottomPortLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,
			   Origin+Z*(chamberHeight+chamberWallThick+topPortLength),Z);



  // Slits
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(height/2.0),Z);

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
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 (-11:12:-13:14:-15:16) 207 307");
  makeCell("ChamberWalls",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -12 13 -14 15 -16 (-101:102:-103:104:-105:106) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("Slits",System,cellIndex++,slitsMat,0.0,HR);

  // front
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 -217 201 -1");
  makeCell("PortFrontWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -207 201 -11");
  makeCell("PortFrontInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 201 -1 3 -4 5 -6");
  makeCell("PortFrontOuter",System,cellIndex++,voidMat,0.0,HR);

  // back
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 -217 2 -202");
  makeCell("PortBackWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -207 12 -202");
  makeCell("PortBackInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 -202 2 3 -4 5 -6");
  makeCell("PortBackOuter",System,cellIndex++,voidMat,0.0,HR);

  // top-bottom
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 307 -317 305 -5");
  makeCell("PortBottomWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -307 305 -15");
  makeCell("PortBottomInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -202 3 -4 305 -5 317");
  makeCell("PortBottomOuter",System,cellIndex++,voidMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 307 -317 6 -306");
  makeCell("PortTopWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -307 16 -306");
  makeCell("PortTopInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -202 3 -4 6 -306 317");
  makeCell("PortTopOuter",System,cellIndex++,voidMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -202 3 -4 305 -306 ");

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

  FixedComp::setConnect(2,Origin-X*(chamberWidth/2.0),-X);
  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(chamberWidth/2.0),X);
  FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(chamberDepth),-Z);
  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(chamberHeight),Z);
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
