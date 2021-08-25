/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/FlangeMount.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "FlangeMount.h"

namespace xraySystem
{

FlangeMount::FlangeMount(const std::string& Key) :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedGroup("Flange","Body","Blade"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  inBeam(1),bladeActive(1),bladeCentreActive(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(6,"bladeCentre");
}

FlangeMount::FlangeMount(const FlangeMount& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),attachSystem::FrontBackCut(A),
  plateThick(A.plateThick),plateRadius(A.plateRadius),
  threadRadius(A.threadRadius),threadLength(A.threadLength),
  inBeam(A.inBeam),bladeXYAngle(A.bladeXYAngle),
  bladeLift(A.bladeLift),bladeThick(A.bladeThick),
  bladeWidth(A.bladeWidth),bladeHeight(A.bladeHeight),
  threadMat(A.threadMat),bladeMat(A.bladeMat),plateMat(A.plateMat),
  bladeActive(A.bladeCentreActive),
  bladeCentreActive(A.bladeActive),bladeCentre(A.bladeCentre),
  holeActive(A.holeActive),
  holeWidth(A.holeWidth),
  holeHeight(A.holeHeight)
  /*!
    Copy constructor
    \param A :: FlangeMount to copy
  */
{}

FlangeMount&
FlangeMount::operator=(const FlangeMount& A)
  /*!
    Assignment operator
    \param A :: FlangeMount to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      plateThick=A.plateThick;
      plateRadius=A.plateRadius;
      threadRadius=A.threadRadius;
      threadLength=A.threadLength;
      inBeam=A.inBeam;
      bladeXYAngle=A.bladeXYAngle;
      bladeLift=A.bladeLift;
      bladeThick=A.bladeThick;
      bladeWidth=A.bladeWidth;
      bladeHeight=A.bladeHeight;
      threadMat=A.threadMat;
      bladeMat=A.bladeMat;
      plateMat=A.plateMat;
      bladeCentreActive=A.bladeCentreActive;
      bladeActive=A.bladeActive;
      bladeCentre=A.bladeCentre;
      holeActive=A.holeActive;
      holeWidth=A.holeWidth;
      holeHeight=A.holeHeight;
    }
  return *this;
}

FlangeMount::~FlangeMount()
  /*!
    Destructor
  */
{}

void
FlangeMount::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlangeMount","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateRadius=Control.EvalVar<double>(keyName+"PlateRadius");

  threadRadius=Control.EvalVar<double>(keyName+"ThreadRadius");
  threadLength=Control.EvalVar<double>(keyName+"ThreadLength");

  inBeam=Control.EvalDefVar<int>(keyName+"InBeam",inBeam);

  bladeXYAngle=Control.EvalVar<double>(keyName+"BladeXYAngle");
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeHeight=Control.EvalVar<double>(keyName+"BladeHeight");
  bladeWidth=Control.EvalVar<double>(keyName+"BladeWidth");

  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");


  bladeCentreActive=Control.EvalDefVar<int>
    (keyName+"BladeCentreActive",bladeCentreActive);
  bladeActive=Control.EvalDefVar<int>(keyName+"BladeActive",1);

  holeActive=Control.EvalDefVar<int>(keyName+"HoleActive",0);
  holeWidth=Control.EvalDefVar<double>(keyName+"HoleWidth", 1.0);
  holeHeight=Control.EvalDefVar<double>(keyName+"HoleHeight", 1.0);

  return;
}


void
FlangeMount::calcThreadLength()
  /*!
    Internal function to calc threadLength based
    on the bladeCentre point [if set]
  */
{
  if (!bladeCentreActive)
    {
      const Geometry::Vec3D DVec=bladeCentre-Origin;
      threadLength=std::abs(DVec.dotProd(Y));
    }
  return;
}

void
FlangeMount::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("FlangeMount","createSurfaces");

  // front planes
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  if (plateThick>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*plateThick,Y);
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,plateRadius);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,threadRadius);

  const double lift((inBeam) ? 0.0 : bladeLift);
  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Z);
  const Geometry::Vec3D PZ(-Y);

  const Geometry::Vec3D BCent(Origin+PZ*(threadLength-lift));
  const Geometry::Quaternion QR
    (Geometry::Quaternion::calcQRotDeg(bladeXYAngle,Y));
  QR.rotate(PX);
  QR.rotate(PY);
  ModelSupport::buildPlane(SMap,buildIndex+101,BCent-PY*(bladeThick/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+102,BCent+PY*(bladeThick/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+103,BCent-PX*(bladeWidth/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+104,BCent+PX*(bladeWidth/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+105,BCent-PZ*(bladeHeight/2.0),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+106,BCent+PZ*(bladeHeight/2.0),PZ);

  if (holeActive)
    {
      ModelSupport::buildPlane(SMap,buildIndex+203,BCent-PX*(holeWidth/2.0),PX);
      ModelSupport::buildPlane(SMap,buildIndex+204,BCent+PX*(holeWidth/2.0),PX);
      ModelSupport::buildPlane(SMap,buildIndex+205,BCent-PZ*(holeHeight/2.0),PZ);
      ModelSupport::buildPlane(SMap,buildIndex+206,BCent+PZ*(holeHeight/2.0),PZ);
    }
  return;
}

void
FlangeMount::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FlangeMount","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getFrontRule();   // mount point on plate
  const HeadRule frontCompHR=frontHR.complement();
  // Flange
  if (plateThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," -2 -7 ");
      makeCell("Plate",System,cellIndex++,plateMat,0.0,HR*frontHR);
      addOuterSurf("Flange",HR*frontHR);
    }

  // Thread
  HeadRule threadHR=ModelSupport::getHeadRule(SMap,buildIndex," -17 -105 ");

  if (threadRadius*2>bladeThick)
    threadHR *= ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 ");
  makeCell("Thread",System,cellIndex++,threadMat,0.0,threadHR*frontCompHR);

  // blade
  if (bladeActive)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
      if (holeActive)
	{
	  const HeadRule hole=ModelSupport::getHeadRule
	    (SMap,buildIndex," 101 -102 203 -204 205 -206 ");
	  makeCell("Hole",System,cellIndex++,0,0.0,hole);

	  HR *= ModelSupport::getHeadRule
	    (SMap,buildIndex," (-203:204:-205:206) ");
	}
      makeCell("Blade",System,cellIndex++,bladeMat,0.0,HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"101 -102 103 -104 105 -106");
      addOuterSurf("Blade",HR);      
    }

  HR=threadHR;
  HR*=frontCompHR;

  addOuterSurf("Body",HR);

  return;
}

void
FlangeMount::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlangeMount","createLinks");

  Geometry::Vec3D PY(Z);
  const Geometry::Vec3D PZ(-Y);
  const Geometry::Quaternion QR
      (Geometry::Quaternion::calcQRotDeg(bladeXYAngle,Y));
  QR.rotate(PY);

  const double lift((inBeam) ? 0.0 : bladeLift);

  const Geometry::Vec3D BCent(Origin+PZ*(threadLength-lift));

  // Mid point of blade centre
  FixedComp::setConnect(6,BCent,-PY);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+105));

  return;
}

void
FlangeMount::setBladeCentre(const Geometry::Vec3D& Pt)
  /*!
    Set the blade centre
    \param Pt :: Centre point
  */
{
  bladeCentreActive=0;
  bladeCentre=Pt;
  return;
}

void
FlangeMount::setBladeCentre(const attachSystem::FixedComp& FC,
			    const long int BIndex)
  /*!
    Set the blade centre
    \param FC :: FixedComp to use
    \param BIndex :: Link point index
  */
{
  bladeCentreActive=0;
  bladeCentre=FC.getLinkPt(BIndex);
  return;
}

void
FlangeMount::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlangeMount","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  calcThreadLength();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
