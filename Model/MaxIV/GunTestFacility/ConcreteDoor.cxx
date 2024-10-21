/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/ConcreteDoor.cxx
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
  innerSideGapLeft=Control.EvalVar<double>(keyName+"InnerSideGapLeft");
  innerSideGapRight=Control.EvalVar<double>(keyName+"InnerSideGapRight");
  outerSideGapLeft=Control.EvalVar<double>(keyName+"OuterSideGapLeft");
  outerSideGapRight=Control.EvalVar<double>(keyName+"OuterSideGapRight");
  innerThickGap=Control.EvalVar<double>(keyName+"InnerThickGap");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");

  underStepHeight=Control.EvalVar<double>(keyName+"UnderStepHeight");
  underStepWidth=Control.EvalVar<double>(keyName+"UnderStepWidth");
  underStepXStep=Control.EvalVar<double>(keyName+"UnderStepXStep");
  innerSideAngle=Control.EvalVar<double>(keyName+"InnerSideAngle");
  outerSideAngle=Control.EvalVar<double>(keyName+"OuterSideAngle");
  innerXStep=Control.EvalVar<double>(keyName+"InnerXStep");
  cornerCut=Control.EvalVar<double>(keyName+"CornerCut");
  jambCornerCut=Control.EvalVar<double>(keyName+"JambCornerCut");

  underMat=ModelSupport::EvalDefMat(Control,keyName+"UnderMat",0);
  doorMat=ModelSupport::EvalMat<int>(Control,keyName+"DoorMat");

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
ConcreteDoor::testDistances() const
  /*!
    Test selected distances and print warnings if they are not maintained
  */
{
  const HeadRule innerHR=ExternalCut::getRule("innerWall");
  const HeadRule outerHR=ExternalCut::getRule("outerWall");
  const HeadRule floorHR=ExternalCut::getRule("floor");

  // checking distances
  constexpr double epsilon = 0.1;
  // middle shorter width
  const Geometry::Vec3D a1 = getCorner(buildIndex+4, innerHR.getPrimarySurface(), buildIndex+6);
  const Geometry::Vec3D b1 = getCorner(buildIndex+3, innerHR.getPrimarySurface(), buildIndex+6);
  const double d1 = a1.Distance(b1);
  if (std::abs(d1-130.5) > epsilon)
    ELog::EM << "WARNING: The 130.5 distance is not maintained: " << d1 << ELog::endDiag;

  // middle longer width
  const Geometry::Vec3D a9 = getCorner(buildIndex+24, buildIndex+201, buildIndex+6);
  const Geometry::Vec3D b9 = getCorner(buildIndex+23, buildIndex+201, buildIndex+6);
  const double d9 = a9.Distance(b9);
  if (std::abs(d9-153.1) > epsilon)
    ELog::EM << "WARNING: The 153.1 distance is not maintained: " << d9 << ELog::endWarn;

  const Geometry::Vec3D a2 = getCorner(buildIndex+23, buildIndex+201, buildIndex+6);
  const Geometry::Vec3D b2 = getCorner(buildIndex+3, buildIndex+201, buildIndex+6);
  const double d2 = a2.Distance(b2);
  if (std::abs(d2-6.3) > epsilon)
    ELog::EM << "WARNING: The 6.3 distance is not maintained: " << d2 << ELog::endWarn;

  const Geometry::Vec3D a3 = getCorner(buildIndex+4, buildIndex+201, buildIndex+6);
  const Geometry::Vec3D b3 = getCorner(buildIndex+24, buildIndex+201, buildIndex+6);
  const double d3 = a3.Distance(b3);
  if (std::abs(d3-10.0) > epsilon)
    ELog::EM << "WARNING: The 10.0 distance is not maintained: " << d3 << ELog::endWarn;

  const Geometry::Vec3D a4 = getCorner(buildIndex+39, outerHR.getPrimarySurface(), buildIndex+6);
  const Geometry::Vec3D b4 = getCorner(buildIndex+29, outerHR.getPrimarySurface(), buildIndex+6);
  const double d4 = a4.Distance(b4);
  if (std::abs(d4-152.9) > epsilon)
    ELog::EM << "WARNING: The inner-inner outer width of 152.9 cm is not maintained: " << d4 << ELog::endWarn;

  // doorjamb
  // inner height
  const Geometry::Vec3D a5 = getCorner(buildIndex+16, innerHR.getPrimarySurface(), buildIndex+24);
  const Geometry::Vec3D b5 = getCorner(floorHR.getPrimarySurface(), innerHR.getPrimarySurface(), buildIndex+24);
  const double d5 = a5.Distance(b5);
  if (std::abs(d5-199.2) > epsilon)
    ELog::EM << "WARNING: The 199.2 cm inner door height is not maintained: " << d5 << ELog::endWarn;

  // outer height
  const Geometry::Vec3D a6 = getCorner(buildIndex+36, innerHR.getPrimarySurface(), buildIndex+24);
  const Geometry::Vec3D b6 = b5;
  const double d6 = a6.Distance(b6);
  if (std::abs(d6-209.5) > epsilon)
    ELog::EM << "WARNING: The 209.5 cm outer door height is not maintained: " << d6 << ELog::endWarn;

  // inner width
  const Geometry::Vec3D a7 = getCorner(buildIndex+13, buildIndex+16, buildIndex+200);
  const Geometry::Vec3D b7 = getCorner(buildIndex+14, buildIndex+16, buildIndex+200);
  const double d7 = a7.Distance(b7);
  if (std::abs(d7-141.0) > epsilon)
    ELog::EM << "WARNING: The 141.0 cm inner door width is not maintained: " << d7 << ELog::endWarn;

  // outer width
  const Geometry::Vec3D a8 = getCorner(buildIndex+33, buildIndex+36, buildIndex+200);
  const Geometry::Vec3D b8 = getCorner(buildIndex+34, buildIndex+36, buildIndex+200);
  const double d8 = a8.Distance(b8);
  if (std::abs(d8-161.0) > epsilon)
    ELog::EM << "WARNING: The doorjamb outer width of 161.0 cm is not maintained: " << d8 << ELog::endWarn;


  return;
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

  const double rotSign(FixedRotate::flipX ? -1.0 : 1.0);

  const Geometry::Quaternion qInnerSide = Geometry::Quaternion::calcQRotDeg(rotSign*(180-innerSideAngle), Z);
  const Geometry::Vec3D vInnerSide(qInnerSide.makeRotate(-Y));
  const Geometry::Quaternion qOuterSide = Geometry::Quaternion::calcQRotDeg(rotSign*(180-outerSideAngle), Z);
  const Geometry::Vec3D vOuterSide(qOuterSide.makeRotate(-Y));

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin+X*(outerWidth/2.0-innerXStep-innerWidth),vInnerSide);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(outerWidth/2.0-innerXStep),X);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+6,Z,innerHeight);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+4,X,-innerWidth-innerSideGapLeft);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,X,innerSideGapRight);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+16,Z,innerHeight+innerTopGap);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(outerWidth/2.0),vOuterSide);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(outerWidth/2.0),X);

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+26,Z,outerHeight);

  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*(outerWidth/2.0+outerSideGapLeft),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*(outerWidth/2.0+outerSideGapRight),X);

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+36,Z,outerHeight+outerTopGap);

  ExternalCut::makeShiftedSurf(SMap,"innerWall",buildIndex+200,Y,innerThick-innerThickGap);
  ExternalCut::makeShiftedSurf(SMap,"innerWall",buildIndex+201,Y,innerThick);

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+1005,Z,underStepHeight);

  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-X*(underStepWidth/2.0+underStepXStep),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*(underStepWidth/2.0-underStepXStep),X);

  // cutting the wall corners
  constexpr double cornerCutAngle = 45.0;
  constexpr double cornerCutAngleRad = cornerCutAngle * M_PI/180.0;
  const double c = cos(cornerCutAngleRad);
  const double legDoor = cornerCut*c;
  const double legDoorJamb = jambCornerCut*c;

  const Geometry::Quaternion qVCorner = Geometry::Quaternion::calcQRotDeg(rotSign*cornerCutAngle, Z);
  const Geometry::Quaternion qHCorner = Geometry::Quaternion::calcQRotDeg(rotSign*cornerCutAngle, X);
  const Geometry::Vec3D v1(qVCorner.makeRotate(Y));
  const Geometry::Vec3D v2(qVCorner.makeRotate(X));
  const Geometry::Vec3D h1(qHCorner.makeRotate(Y));
  const Geometry::Vec3D h2(qHCorner.makeRotate(Z));

  const Geometry::Quaternion qVCorner1 =
    Geometry::Quaternion::calcQRotDeg(rotSign*(outerSideAngle-cornerCutAngle/2.0), Z);
  const Geometry::Vec3D v3(qVCorner1.makeRotate(Y));

  const HeadRule innerHR=ExternalCut::getRule("innerWall");
  const HeadRule outerHR=ExternalCut::getRule("outerWall");

  const Geometry::Vec3D c8 = getCorner(buildIndex+26, outerHR.getPrimarySurface(), buildIndex+24);
  ModelSupport::buildPlane(SMap,buildIndex+8, c8-Z*(legDoor), h1);

  const Geometry::Vec3D c9 = getCorner(buildIndex+4, innerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+9, c9+Y*(legDoor), v1);

  const Geometry::Vec3D c18 = getCorner(buildIndex+26, buildIndex+201, buildIndex+24);
  ModelSupport::buildPlane(SMap,buildIndex+18, c18-Z*(legDoor), h2);

  const Geometry::Vec3D c19 = getCorner(buildIndex+201, buildIndex+24, buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+19, c19+Y*(legDoor), v1);

  const Geometry::Vec3D c28 = getCorner(buildIndex+6, innerHR.getPrimarySurface(),buildIndex+24);
  ModelSupport::buildPlane(SMap,buildIndex+28, c28-Z*(legDoor), h2);

  const Geometry::Vec3D c29 = getCorner(buildIndex+24, outerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+29, c29-Y*(legDoor), v2);

  const Geometry::Vec3D c38 = getCorner(buildIndex+16, innerHR.getPrimarySurface(),buildIndex+24);
  ModelSupport::buildPlane(SMap,buildIndex+38, c38+Y*(legDoorJamb), h1);

  const Geometry::Vec3D c39 = getCorner(buildIndex+23, outerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+39, c39+X*(legDoor), v3);

  const Geometry::Vec3D c49 = getCorner(buildIndex+14, innerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+49, c49+Y*(legDoorJamb), v2);

  const Geometry::Vec3D c59 = getCorner(buildIndex+13, innerHR.getPrimarySurface(), buildIndex+6);
  ModelSupport::buildPlane(SMap,buildIndex+59, c59+Y*(legDoorJamb), v1);

  testDistances();

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

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-201 3 -4 -6 (-1003:1004:1005) 9 -28");
  makeCell("InnerDoor",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 (-3:4:6) 13 -14 -16");
  makeCell("InnerGap",System,cellIndex++,0,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -34 -200 49 -16 ");
  makeCell("JambInnerRight",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"33 -34 -200 16 -36 38 ");
  makeCell("JambInnerTop",System,cellIndex++,doorMat,0.0,HR*innerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -59 -49 16 -38");
  makeCell("Corner38",System,cellIndex++,0,0.0,HR*innerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 33 59 16 -38");
  makeCell("Corner38Left",System,cellIndex++,doorMat,0.0,HR*innerHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 49 -34 16 -38");
  makeCell("Corner38Right",System,cellIndex++,doorMat,0.0,HR*innerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"33 -13 -200 -16 59 ");
  makeCell("JambInnerLeft",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"200 -201 23 -3 -6");
  makeCell("MidGapLeft",System,cellIndex++,0,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"200 -201 23 -24 6 -26");
  makeCell("MidGapTop",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"200 -201 4 -24 -6 ");
  makeCell("MidGapRight",System,cellIndex++,0,0.0,HR*floorHR);



  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 23 -24 -8 -18 19 -29 -39 1005 -26 ");
  makeCell("OuterDoor",System,cellIndex++,doorMat,0.0,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"8 -26 -29 -39 23 -24 ");
  makeCell("Corner8",System,cellIndex++,0,0.0,HR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -4 -9 -6 ");
  makeCell("Corner9",System,cellIndex++,0,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"18 19 -26 201 23 -24 ");
  makeCell("Corner18",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -19 -24 -26");
  makeCell("Corner19",System,cellIndex++,0,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"28 9 3 -4 -6 ");
  makeCell("Corner28",System,cellIndex++,0,0.0,HR*innerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"29 -24 -26");
  makeCell("Corner29",System,cellIndex++,0,0.0,HR*floorHR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"39 23 -26");
  makeCell("Corner39",System,cellIndex++,0,0.0,HR*floorHR*outerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -49 -16 ");
  makeCell("Corner49",System,cellIndex++,0,0.0,HR*floorHR*innerHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-13 -59 -16 ");
  makeCell("Corner59",System,cellIndex++,0,0.0,HR*floorHR*innerHR);

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
