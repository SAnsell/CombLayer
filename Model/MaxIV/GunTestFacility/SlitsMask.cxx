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
  leftPortLength(A.leftPortLength),
  rightPortLength(A.rightPortLength),
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
      slitLength=A.slitLength;
      slitWidth=A.slitWidth;
      slitHeight=A.slitHeight;
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
      leftPortLength=A.leftPortLength;
      rightPortLength=A.rightPortLength;
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

  slitLength=Control.EvalVar<double>(keyName+"SlitLength");
  slitWidth=Control.EvalVar<double>(keyName+"SlitWidth");
  slitHeight=Control.EvalVar<double>(keyName+"SlitHeight");
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
  leftPortLength=Control.EvalVar<double>(keyName+"LeftPortLength");
  rightPortLength=Control.EvalVar<double>(keyName+"RightPortLength");
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
    ModelSupport::buildPlane(SMap,buildIndex+1,
			     Origin-Y*(frontLength+chamberLengthFront+chamberWallThick),Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,
			     Origin+Y*(chamberLengthBack+chamberWallThick),Y);
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
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(backLength+chamberLengthBack+chamberWallThick),Y);

  // left-right
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,X,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,X,portRadius+portThick);
  ModelSupport::buildPlane(SMap,buildIndex+303,
			   Origin-X*(chamberDepth+chamberWallThick+leftPortLength),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,
			   Origin+X*(chamberHeight+chamberWallThick+rightPortLength),X);

  // top-bottom
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,portRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,portRadius+portThick);
  ModelSupport::buildPlane(SMap,buildIndex+405,
			   Origin-Z*(chamberDepth+chamberWallThick+bottomPortLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+406,
			   Origin+Z*(chamberHeight+chamberWallThick+topPortLength),Z);



  // Slits
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(slitLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(slitLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(slitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(slitWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(slitHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(slitHeight/2.0),Z);

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
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 -217 1 -202 317 417");
  makeCell("PortFrontBackWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -207 1 -202 ");
  makeCell("PortFrontBackInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 217 317 417 1 -202 303 -304 405 -406");
  makeCell("Outer",System,cellIndex++,voidMat,0.0,HR);


  // top-bottom
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 407 -417 207 317 405 -406");
  makeCell("PortTopBottomWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 207 307 -407 405 -406");
  makeCell("PortTopBottomInner",System,cellIndex++,voidMat,0.0,HR);

  // // // left-right
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 307 -317 207 407 303 -304");
  makeCell("PortLeftWall",System,cellIndex++,chamberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -307 303 -304 207");
  makeCell("PortLeftInner",System,cellIndex++,voidMat,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," -307 16 -304");
  // makeCell("PortLeftRightInner",System,cellIndex++,voidMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -202 303 -304 405 -406 ");

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
