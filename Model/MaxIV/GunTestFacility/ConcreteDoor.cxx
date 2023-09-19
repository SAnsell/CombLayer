/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/ConcreteDoor.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell / Konstantin Batkov
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"
#include "SurInter.h"

#include "ConcreteDoor.h"


namespace MAXIV::GunTestFacility
{

ConcreteDoor::ConcreteDoor(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

void
ConcreteDoor::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("ConcreteDoor","populate");

  FixedRotate::populate(Control);

  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  innerTopGap=Control.EvalVar<double>(keyName+"InnerTopGap");
  outerTopGap=Control.EvalVar<double>(keyName+"OuterTopGap");
  gapSpace=Control.EvalVar<double>(keyName+"GapSpace");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");

  underStepHeight=Control.EvalVar<double>(keyName+"UnderStepHeight");
  underStepWidth=Control.EvalVar<double>(keyName+"UnderStepWidth");
  sideCutAngle=Control.EvalVar<double>(keyName+"SideCutAngle");
  cornerCut=Control.EvalVar<double>(keyName+"CornerCut");
  jambCornerCut=Control.EvalVar<double>(keyName+"JambCornerCut");

  underMat=ModelSupport::EvalDefMat(Control,keyName+"UnderMat",0);
  doorMat=ModelSupport::EvalMat<int>(Control,keyName+"DoorMat");

  return;
}

void
ConcreteDoor::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    Note that Y points OUT of the ring
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("ConcreteDoor","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  const HeadRule& HR=ExternalCut::getRule("innerWall");

  const int SN=HR.getPrimarySurface();
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(HR.getSurface(SN));

  if (PPtr)
    {
      Geometry::Vec3D PAxis=PPtr->getNormal();
      if (Y.dotProd(PAxis)*SN < -Geometry::zeroTol)
	PAxis*=-1;
      FixedComp::reOrientate(1,PAxis);
    }
  applyOffset();
  return;
}

Geometry::Vec3D
ConcreteDoor::getCorner(const int a, const int b, const int c) const
/*!
  Return coordinates of the vertical corner formed by 3 planes
  \param a :: first plane number
  \param a :: second plane number
  \param a :: third plane number
 */
{
  const Geometry::Plane* pA = dynamic_cast<const Geometry::Plane*>(SMap.realSurfPtr(a));
  const Geometry::Plane* pB = dynamic_cast<const Geometry::Plane*>(SMap.realSurfPtr(b));
  const Geometry::Plane* pC = dynamic_cast<const Geometry::Plane*>(SMap.realSurfPtr(c));

  return SurInter::getPoint(pA, pB, pC);
}

void
ConcreteDoor::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConcreteDoor","createSurface");

  // InnerWall and OuterWall MUST be set
  if (!ExternalCut::isActive("innerWall") ||
      !ExternalCut::isActive("outerWall") ||
      !ExternalCut::isActive("floor"))
    throw ColErr::InContainerError<std::string>
      ("InnerWall/OuterWall/floor","Door:"+keyName);

  // origin in on outer wall:


  const Geometry::Quaternion qSide = Geometry::Quaternion::calcQRotDeg(180-sideCutAngle, Z);
  const Geometry::Vec3D vSide(qSide.makeRotate(-Y));

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(innerWidth/2.0),vSide); //
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(innerWidth/2.0),X);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+6,Z,innerHeight);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(gapSpace+innerWidth/2.0),vSide); //
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(gapSpace+innerWidth/2.0),X);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+16,Z,
			       innerTopGap+innerHeight);


  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(outerWidth/2.0),vSide); //
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(outerWidth/2.0),X);

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+26,Z,outerHeight);

  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(gapSpace+outerWidth/2.0),vSide); //
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(gapSpace+outerWidth/2.0),X);
  // ModelSupport::buildPlane(SMap,buildIndex+36,
  // 			   Origin+Z*(outerTopGap+outerHeight/2.0),Z);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+36,Z,
			       outerTopGap+outerHeight);

  // Y Points out of ring:
  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+200,Y,innerThick-gapSpace);
  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+201,Y,innerThick);

  // lift step
  ExternalCut::makeShiftedSurf
    (SMap,"floor",buildIndex+1005,Z,underStepHeight);

  ModelSupport::buildPlane
    (SMap,buildIndex+1003,Origin-X*(underStepWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1004,Origin+X*(underStepWidth/2.0),X);

  // cutting the angles
  constexpr double cornerCutAngle = 45 * M_PI/180.0;
  const double c = cos(cornerCutAngle);
  const double legDoor = cornerCut*c;
  const double legDoorJamb = jambCornerCut*c;

  const Geometry::Quaternion qVCorner = Geometry::Quaternion::calcQRot(cornerCutAngle, Z);
  const Geometry::Quaternion qHCorner = Geometry::Quaternion::calcQRot(cornerCutAngle, X);
  const Geometry::Vec3D v1(qVCorner.makeRotate(Y));
  const Geometry::Vec3D v2(qVCorner.makeRotate(X));

  const HeadRule innerHR=ExternalCut::getRule("innerWall");
  const HeadRule outerHR=ExternalCut::getRule("outerWall");

  const Geometry::Vec3D c9 = getCorner(buildIndex+4, innerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+9, c9+Y*(legDoor), v1);

  const Geometry::Vec3D c19 = getCorner(buildIndex+201, buildIndex+24, buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+19, c19+Y*(legDoor), v1);

  const Geometry::Vec3D c29 = getCorner(buildIndex+24, outerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+29, c29-Y*(legDoor), v2);

  const Geometry::Vec3D c39 = getCorner(buildIndex+23, outerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+39, c39+X*(legDoor), v1);

  return;
}

void
ConcreteDoor::createObjects(Simulation& System)
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("ConcreteDoor","createObjects");

  HeadRule HR;
  const HeadRule innerHR=ExternalCut::getRule("innerWall");
  const HeadRule outerHR=ExternalCut::getRule("outerWall");
  const HeadRule floorHR=ExternalCut::getRule("floor");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-201 3 -4 -6 (-1003:1004:1005) 9");
  makeCell("InnerDoor",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -4 -9 -6 ");
  makeCell("Corner9",System,cellIndex++,0,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 (-3:4:6) 13 -14 -16");
  makeCell("InnerGap",System,cellIndex++,0,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 (-13:14:16) 33 -34 -36");
  makeCell("InnerExtra",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -201 23 -24 -26 (-3:4:6)");
  makeCell("MidGap",System,cellIndex++,0,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 23 -24 19 -29 -39 1005 -26 ");
  makeCell("OuterDoor",System,cellIndex++,doorMat,0.0,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -19 -24 -26");
  makeCell("Corner19",System,cellIndex++,0,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"29 -24 -26");
  makeCell("Corner29",System,cellIndex++,0,0.0,HR*floorHR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"39 23 -26");
  makeCell("Corner39",System,cellIndex++,0,0.0,HR*floorHR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 23 -39 -1003 -1005 ");
  makeCell("OuterDoorBottom",System,cellIndex++,doorMat,0.0,HR*outerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 1004 -24 19 -29 -1005 ");
  makeCell("OuterDoorBottom",System,cellIndex++,doorMat,0.0,HR*outerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 (-23:24:26) 33 -34 -36");
  makeCell("OuterGap",System,cellIndex++,0,0.0,HR*outerHR*floorHR);


  // Lift points
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1003 -1004 -1005");
  makeCell("LiftA",System,cellIndex++,underMat,
	   0.0,HR*outerHR*innerHR*floorHR);

  // main door
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"33 -34 -36");
  addOuterSurf(HR);

  return;
}

void
ConcreteDoor::createLinks()
  /*!
    Construct the links for the system
    Note that Y points OUT of the ring
  */
{
  ELog::RegMethod RegA("ConcreteDoor","createLinks");

  ExternalCut::createLink("innerWall",*this,0,Origin,-Y);
  ExternalCut::createLink("outerWall",*this,1,Origin,Y);

  return;
}

void
ConcreteDoor::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("ConcreteDoor","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}


}  // NAMESPACE xraySystem
