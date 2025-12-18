/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3common/OpticsStepHutch.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "forkHoles.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"

#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "SurInter.h"


#include "OpticsHutch.h"
#include "OpticsStepHutch.h"

namespace xraySystem
{

OpticsStepHutch::OpticsStepHutch(const std::string& Key) :
  OpticsHutch(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

OpticsStepHutch::~OpticsStepHutch()
  /*!
    Destructor
  */
{}

void
OpticsStepHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("OpticsStepHutch","populate");

  OpticsHutch::populate(Control);

  ringStepLength=Control.EvalVar<double>(keyName+"RingStepLength");
  ringStepWidth=Control.EvalVar<double>(keyName+"RingStepWidth");

  return;
}

void
OpticsStepHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("OpticsStepHutch","createSurfaces");

  OpticsHutch::createSurfaces();

  Geometry::Vec3D BPoint(Origin+Y*(ringStepLength-outerThick-pbBackThick-innerThick));
  ModelSupport::buildPlane(SMap,buildIndex+202,BPoint,Y);
  BPoint+=Y*innerThick;
  ModelSupport::buildPlane(SMap,buildIndex+212,BPoint,Y);
  BPoint+=Y*pbBackThick;
  ModelSupport::buildPlane(SMap,buildIndex+222,BPoint,Y);
  BPoint+=Y*outerThick;
  ModelSupport::buildPlane(SMap,buildIndex+232,BPoint,Y);

  Geometry::Vec3D SPoint(Origin+X*(ringStepWidth-outerThick-pbWallThick-innerThick));
  ModelSupport::buildPlane(SMap,buildIndex+204,SPoint,X);
  SPoint+=X*innerThick;
  ModelSupport::buildPlane(SMap,buildIndex+214,SPoint,X);
  SPoint+=X*pbWallThick;
  ModelSupport::buildPlane(SMap,buildIndex+224,SPoint,X);
  SPoint+=X*outerThick;
  ModelSupport::buildPlane(SMap,buildIndex+234,SPoint,X);

  // floor shine
  ModelSupport::buildShiftedPlane(SMap, buildIndex+301, buildIndex+232, Y, -floorShineLength);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+304, buildIndex+234, Y, -floorShineLength);

  // wall shine along the ring side wall
  const Geometry::Vec3D corner = SurInter::getPoint(SMap.realPtr<Geometry::Surface>(buildIndex+232), SMap.realPtr<Geometry::Surface>(buildIndex+6), pSideWall);

  Geometry::Vec3D n = pSideWall->getNormal();
  n.rotate(Z, -M_PI_2);

  ModelSupport::buildPlane(SMap, buildIndex+341, corner-n*wallShineLength, n);

  return;
}

void
OpticsStepHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("OpticsStepHutch","createObjects");

  const HeadRule sideCut=ExternalCut::getValidRule("SideWallCut",Origin);
  const HeadRule floor=ExternalCut::getValidRule("Floor",Origin);
  const HeadRule flatOuterCut=ExternalCut::getValidRule("FlatOuterCut",Origin);

  HeadRule holeCut;
  int BI(buildIndex);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      holeCut*=ModelSupport::getHeadRule(SMap,BI,"107");
      BI+=100;
    }

  const HeadRule forkWallRing=forks.getRingCut();
  const HeadRule forkWallOuter=forks.getOuterCut();
  const HeadRule forkWallBack=forks.getBackCut();

  HeadRule HR;

  if (innerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-112 3 501 -1003 305 -605");
      makeCell("OuterWallVoid",System,cellIndex++,voidMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -303 501 -302 -305");
      makeCell("OuterWallFloorShine",System,cellIndex++,floorShineMat,0.0,HR*floor);

      if (floorShineLength-innerOutVoid<Geometry::zeroTol) {
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"303 -1003 -302 -305");
	makeCell("OuterWallFloorShineVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      }

      // Big void cell
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-112 1003 (-304:-301) -605");
      if (floorShineLength-backPlateThick>Geometry::zeroTol)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"-302:305");

      if (floorShineLength-innerOutVoid>Geometry::zeroTol)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"303:305");

      // Wall shine along the ring side wall
      HR*=ModelSupport::getHeadRule(SMap,buildIndex,"-403:-341");

      // Wall shine along the ratchet-end wall inside OH
      HR*=ModelSupport::getHeadRule(SMap,buildIndex,"501:503");

    }
  else
    {
      ELog::EM << "not yet implemented with floor / wall / roof shine" << ELog::endWarn;
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-112 3 (-204:-202) -6");
    }
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*frontWall*sideCut);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 -36 234 232");
  makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*floor*sideWall);

  // walls:

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -3 13 -6");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*frontWall*forkWallOuter);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -13 23 -16");
  makeCell("LeadWall",System,cellIndex++,pbMat,0.0,
	   HR*floor*frontWall*forkWallOuter);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 -23 33 -26");
  makeCell("OuterWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*frontWall*forkWallOuter);

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"2 -12 13 -204 -6 (-103:104:-105:106)");
  makeCell("BackIWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*holeCut*forkWallBack);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 -22 23 -16 -214");
  makeCell("BackPbWall",System,cellIndex++,pbMat,0.0,
	   HR*floor*holeCut*forkWallBack);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"22 -32 33 -26 -224");
  makeCell("BackOuterWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*holeCut*forkWallBack);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 6 -16 (-204:-202)");
  makeCell("RoofIWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 23 16 -26 (-214:-212)");
  makeCell("RoofPbWall",System,cellIndex++,pbMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33  26 -36 (-224:-222)");
  makeCell("RoofOuterWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);

  // ring cutout
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 204 -214 -12 -16");
  makeCell("ringIWall",System,cellIndex++,skinMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 -302 304 -204 -305");
  makeCell("RingWallFloorShine",System,cellIndex++,floorShineMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 -112 304 -204 305 -605");
  makeCell("RingWallFloorShineVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"212 214 -224 -22 -26");
  makeCell("ringPbWall",System,cellIndex++,pbMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"222 224 -234 -32 -36");
  makeCell("ringOWall",System,cellIndex++,skinMat,0.0,HR*floor);

  // flat cutout
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 -212 214 -16");
  makeCell("flatIWall",System,cellIndex++,skinMat,0.0,HR*floor*sideWall);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -202 304 -403 -305");
  makeCell("FlatWallFloorShine",System,cellIndex++,floorShineMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -202 304 -403 305 -605");
  makeCell("FlatWallFloorShineVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"212 -222 224 -26");
  makeCell("flatPbWall",System,cellIndex++,pbMat,0.0,HR*floor*sideWall);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"222 -232 234 234 -36");
  makeCell("flatOWall",System,cellIndex++,skinMat,0.0,HR*floor*sideWall);

  // Back plate:
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"102 -12 103 -104 105 -106");
  makeCell("BackPlate",System,cellIndex++,pbMat,0.0,HR*holeCut);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"112 -102 103 -104 105 -106");
  makeCell("BackPlateSkin",System,cellIndex++,skinMat,0.0,HR*holeCut);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex, "112 -2 3 -204 305 -605 (-103:104:-105:106)");
  makeCell("BackPlateVoid",System,cellIndex++,voidMat,0.0,HR);

  // floor shine
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex, "302 -2 3 -204 -305");
  makeCell("BackPlateFloorShine",System,cellIndex++,floorShineMat,0.0,HR*floor);

  if (floorShineLength-backPlateThick<Geometry::zeroTol) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex, "112 -302 -2 1003 -304 -305");
    makeCell("BackPlateFloorShineVoid",System,cellIndex++,0,0.0,HR*floor);
  }

  // Wall shine along the ring side wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "341 -202 403 -605");
  makeCell("RingSideWallWallShine",System,cellIndex++,floorShineMat,0.0,HR*sideWall*floor);

  // Wall shine along the ratchet-end wall inside OH
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "-501 3 -503 -605");
  makeCell("REWInWallShine",System,cellIndex++,floorShineMat,0.0,HR*frontWall*floor);

  // Roof shine along the ring side wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex, "-603 -202 605 -6");
  makeCell("RingSideWallRoofShineRingSideWall",System,cellIndex++,floorShineMat,0.0,HR*sideWall*flatOuterCut.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "202 -2 3 -204 605 -6");
  makeCell("RoofShineVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "601 -202 603 3 605 -6");
  makeCell("RingSideWallRoofShineVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "601 -603 -604 605 -6");
  makeCell("RingSideWallRoofShineFlatOuterCut",System,cellIndex++,floorShineMat,0.0,HR*flatOuterCut);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "601 -603 604 605 -6");
  makeCell("RingSideWallRoofShineFlatOuterCutVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex, "3 -601 605 -6");
  makeCell("RingSideWallRoofShineREW",System,cellIndex++,floorShineMat,0.0,HR*frontWall*flatOuterCut);


  // Outer void for pipe(s)
  BI=buildIndex;
  for(size_t i=0;i<holeRadius.size();i++)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,BI,"112 -32 -107");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,HR);
      BI+=100;
    }

  OpticsHutch::createForkCut(System);

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 1033 -33 -36");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);

      const HeadRule notFrontWall = frontWall.complement();

      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"502 504 -33 -36");
      makeCell("WallShineREW",System,cellIndex++,wallShineMat,0.0,HR*floor*notFrontWall);

      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"502 1033 -504 -36");
      makeCell("WallShineREWVoid",System,cellIndex++,voidMat,0.0,HR*floor*notFrontWall);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -32 1033 -33 -36");
      addOuterSurf("WallShineREW", HR*floor);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 1033 -36");
    }
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33 -36");

  addOuterSurf("Hutch", HR*frontWall*sideCut);

  return;
}

void
OpticsStepHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("OpticsStepHutch","createLinks");


  //  const double extraBack(innerThick+outerThick+pbBackThick);
  //  const double extraWall(innerThick+outerThick+pbWallThick);

  OpticsHutch::createLinks();

  FixedComp::setLinkSurf(14,-SMap.realSurf(buildIndex+305));
  FixedComp::addLinkSurf(14,SMap.realSurf(buildIndex+302));
  FixedComp::nameSideIndex(14,"BackPlateFloorShine");

  return;
}

}  // NAMESPACE xraySystem
