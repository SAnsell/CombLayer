/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/InjectionHall.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "InjectionHall.h"

namespace tdcSystem
{

InjectionHall::InjectionHall(const std::string& Key) :
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  nPillars(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

InjectionHall::~InjectionHall()
  /*!
    Destructor
  */
{}

void
InjectionHall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("InjectionHall","populate");

  FixedOffset::populate(Control);

  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  linearRCutLength=Control.EvalVar<double>(keyName+"LinearRCutLength");
  linearLTurnLength=Control.EvalVar<double>(keyName+"LinearLTurnLength");
  spfAngleLength=Control.EvalVar<double>(keyName+"SPFAngleLength");
  spfMidLength=spfAngleLength/2.0;
  spfAngle=Control.EvalVar<double>(keyName+"SPFAngle");
  spfLongLength=Control.EvalVar<double>(keyName+"SPFLongLength");
  rightWallStep=Control.EvalVar<double>(keyName+"RightWallStep");

  linearWidth=Control.EvalVar<double>(keyName+"LinearWidth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");

  midTXStep=Control.EvalVar<double>(keyName+"MidTXStep");
  midTYStep=Control.EvalVar<double>(keyName+"MidTYStep");
  midTThick=Control.EvalVar<double>(keyName+"MidTThick");
  midTAngle=Control.EvalVar<double>(keyName+"MidTAngle");
  midTLeft=Control.EvalVar<double>(keyName+"MidTLeft");
  midTRight=Control.EvalVar<double>(keyName+"MidTRight");
  midTFrontAngleStep=Control.EvalVar<double>(keyName+"MidTFrontAngleStep");
  midTBackAngleStep=Control.EvalVar<double>(keyName+"MidTBackAngleStep");

  klysDivThick=Control.EvalVar<double>(keyName+"KlysDivThick");

  midGateOut=Control.EvalVar<double>(keyName+"MidGateOut");
  midGateWidth=Control.EvalVar<double>(keyName+"MidGateWidth");
  midGateWall=Control.EvalVar<double>(keyName+"MidGateWall");
  backWallYStep=Control.EvalVar<double>(keyName+"BackWallYStep");
  backWallThick=Control.EvalVar<double>(keyName+"BackWallThick");
  backWallMat=ModelSupport::EvalMat<int>(Control,keyName+"BackWallMat");

  klystronXStep=Control.EvalVar<double>(keyName+"KlystronXStep");
  klystronLen=Control.EvalVar<double>(keyName+"KlystronLen");
  klystronFrontWall=Control.EvalVar<double>(keyName+"KlystronFrontWall");
  klystronSideWall=Control.EvalVar<double>(keyName+"KlystronSideWall");

  boundaryWidth=Control.EvalVar<double>(keyName+"BoundaryWidth");
  boundaryHeight=Control.EvalVar<double>(keyName+"BoundaryHeight");

  nPillars=Control.EvalDefVar<size_t>(keyName+"NPillars", 0);

  for (size_t i=0; i<nPillars; ++i)
    {
      const std::string n(std::to_string(i+1));

      const double R=Control.EvalPair<double>(keyName+"Pillar"+n+"Radius",
					keyName+"PillarRadius");
      pRadii.push_back(R);

      const int m=ModelSupport::EvalMat<int>(Control,
				       keyName+"Pillar"+n+"Mat",
				       keyName+"PillarMat");
      pMat.push_back(m);

      const double x=Control.EvalVar<double>(keyName+"Pillar"+n+"X");
      const double y=Control.EvalVar<double>(keyName+"Pillar"+n+"Y");
      pXY.emplace_back(x,y,0.0);
    }

  // THz penetration
  thzWidth=Control.EvalVar<double>(keyName+"THzWidth");
  thzHeight=Control.EvalVar<double>(keyName+"THzHeight");
  thzXStep=Control.EvalVar<double>(keyName+"THzXStep");
  thzZStep=Control.EvalVar<double>(keyName+"THzZStep");
  thzZAngle=Control.EvalVar<double>(keyName+"THzZAngle");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");

  return;
}

void
InjectionHall::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("InjectionHall","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*mainLength,Y);


  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(linearWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(linearWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(linearWidth/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(linearWidth/2.0+wallThick),X);

  // right hand step
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin+Y*linearRCutLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+111,Origin+Y*(linearRCutLength+wallThick),Y);


  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin+X*(linearWidth/2.0+rightWallStep),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+114,Origin+X*(linearWidth/2.0+wallThick+rightWallStep),X);


  // left hand angle
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(spfAngle,Z);
  const Geometry::Vec3D PX(QR.makeRotate(X));
  Geometry::Vec3D LWPoint(Origin-X*(linearWidth/2.0)+Y*linearLTurnLength);

  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin+Y*linearLTurnLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+211,Origin+Y*(linearLTurnLength+spfAngleLength),Y);
  // Mid long : not used but useful for InnerZone division
  ModelSupport::buildPlane
    (SMap,buildIndex+221,
     Origin+Y*(spfLongLength+linearLTurnLength+spfAngleLength),Y);
  // Mid Short : not used but useful for InnerZone division
  ModelSupport::buildPlane
    (SMap,buildIndex+251,
     Origin+Y*(linearLTurnLength+spfMidLength),Y);


  ModelSupport::buildPlane
    (SMap,buildIndex+203,LWPoint,PX);
  ModelSupport::buildPlane
    (SMap,buildIndex+213,LWPoint-X*wallThick,PX);

  // out step
  const double spfAngleStep(spfAngleLength*tan(M_PI*spfAngle/180.0));
  ModelSupport::buildPlane
    (SMap,buildIndex+223,Origin-X*(linearWidth/2.0+spfAngleStep),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+233,Origin-X*(linearWidth/2.0+spfAngleStep+wallThick),X);

  // roof / floor
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*roofHeight,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(roofHeight+roofThick),Z);

  // MID T [1000]:
  const Geometry::Vec3D MidPt(Origin+X*midTXStep+Y*midTYStep);
  ModelSupport::buildPlane(SMap,buildIndex+1001,MidPt-Y*midTThick,Y);

  ModelSupport::buildPlane(SMap,buildIndex+1011,MidPt,Y);
  ModelSupport::buildPlane(SMap,buildIndex+1003,MidPt-X*(midTThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,MidPt+X*(midTThick/2.0),X);

  const Geometry::Quaternion QRMid=
    Geometry::Quaternion::calcQRotDeg(midTAngle,Z);
  const Geometry::Vec3D MX(QRMid.makeRotate(X));
  const Geometry::Vec3D MY(QRMid.makeRotate(Y));

  const Geometry::Vec3D FMidPt(MidPt-X*(midTThick/2.0)-Y*midTThick);
  const Geometry::Vec3D BMidPt(MidPt-X*(midTThick/2.0));
  // end points of slope
  const Geometry::Vec3D FMidPtA(FMidPt-MX*midTFrontAngleStep);
  const Geometry::Vec3D BMidPtA(BMidPt-MX*midTBackAngleStep);


  ModelSupport::buildPlane(SMap,buildIndex+1111,
			   FMidPt,
			   FMidPtA,
			   FMidPtA+Z,
			   Y);
  ModelSupport::buildPlane(SMap,buildIndex+1112,
			   BMidPt,
			   BMidPtA,
			   BMidPtA+Z,
			   Y);
  // End divider
  ModelSupport::buildPlane(SMap,buildIndex+1153,
			   FMidPtA,
			   BMidPtA,
			   BMidPtA+Z,
			   X);

  SurfMap::setSurf("TMidFront",SMap.realSurf(buildIndex+1111));
  SurfMap::setSurf("TMidBack",SMap.realSurf(buildIndex+1112));


  ModelSupport::buildPlane(SMap,buildIndex+1201,FMidPtA,Y);
  ModelSupport::buildPlane(SMap,buildIndex+1202,BMidPtA,Y);

  // caps

  ModelSupport::buildPlane(SMap,buildIndex+1103,MidPt-X*midTLeft,X);
  ModelSupport::buildPlane(SMap,buildIndex+1104,MidPt+X*midTRight,X);

  // GATE:
  const Geometry::Vec3D gatePt((FMidPtA+BMidPtA)/2.0);
  ModelSupport::buildPlane
    (SMap,buildIndex+1511,gatePt-Y*(midGateWall+midGateWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1512,gatePt-Y*(midGateWidth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1521,gatePt+Y*(midGateWidth/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1522,gatePt+Y*(midGateWall+midGateWidth/2.0),Y);
   // step out
  ModelSupport::buildPlane
    (SMap,buildIndex+1503,Origin-X*(linearWidth/2.0-midGateOut),X);

  // Future KLYSTRON DIVDER
  ModelSupport::buildPlane
    (SMap,buildIndex+2111,Origin+Y*
     (linearRCutLength+wallThick-klysDivThick),Y);

  // Current Klystron box
  ModelSupport::buildPlane(SMap,buildIndex+3001,Origin+Y*klystronLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3002,
			   Origin+Y*(+klystronFrontWall+klystronLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+3004,Origin+X*klystronXStep,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+3014,Origin+X*(klystronXStep-klystronSideWall),X);

  // now build externals:

  ModelSupport::buildPlane
    (SMap,buildIndex+53,
       Origin-X*(linearWidth/2.0+spfAngleStep+boundaryWidth+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+54,
       Origin+X*(linearWidth/2.0+rightWallStep+boundaryWidth+wallThick),X);

  // Auxiliary cyliner to cure geometric problems in corners
  ModelSupport::buildCylinder(SMap,buildIndex+2007,FMidPt,Z,0.5);

  // Pillars
  int SI(buildIndex+3000);
  for (size_t i=0; i<nPillars; ++i)
    {
      ModelSupport::buildCylinder(SMap,SI+7,pXY[i],Z,pRadii[i]);
      SI += 10;
    }

  // THz penetration
  const Geometry::Quaternion QTHz =
        Geometry::Quaternion::calcQRotDeg(thzZAngle,Z);
  const Geometry::Vec3D THzX(QTHz.makeRotate(X));

  ModelSupport::buildPlane(SMap,buildIndex+5003,FMidPt-X*(thzWidth/2.0-thzXStep),THzX);
  ModelSupport::buildPlane(SMap,buildIndex+5004,FMidPt+X*(thzWidth/2.0+thzXStep),THzX);
  ModelSupport::buildPlane(SMap,buildIndex+5005,FMidPt-Z*(thzHeight/2.0-thzZStep),Z);
  ModelSupport::buildPlane(SMap,buildIndex+5006,FMidPt+Z*(thzHeight/2.0+thzZStep),Z);

  // Back wall
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*backWallYStep,Y);
  SurfMap::setSurf("BackWallFront",SMap.realSurf(buildIndex+22));

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin+Y*(backWallYStep+backWallThick),Y);
  SurfMap::setSurf("BackWallBack",SMap.realSurf(buildIndex+23));

  // transfer for later
  SurfMap::setSurf("Front",SMap.realSurf(buildIndex+1));
  SurfMap::setSurf("Back",SMap.realSurf(buildIndex+2));
  SurfMap::setSurf("Floor",SMap.realSurf(buildIndex+5));
  SurfMap::setSurf("SubFloor",SMap.realSurf(buildIndex+15));
  SurfMap::setSurf("MidWall",SMap.realSurf(buildIndex+1001));
  SurfMap::setSurf("MidAngleWall",-SMap.realSurf(buildIndex+1111));
  SurfMap::addSurf("MidAngleWall",-SMap.realSurf(buildIndex+1001));
  SurfMap::addSurf("MidAngleWall",SMap.realSurf(buildIndex+2007));

  SurfMap::setSurf("TAngleWall",SMap.realSurf(buildIndex+1112));
  SurfMap::setSurf("DoorEndWall",SMap.realSurf(buildIndex+1522));
  SurfMap::setSurf("KlystronWall",SMap.realSurf(buildIndex+3002));
  SurfMap::setSurf("KlystronCorner",SMap.realSurf(buildIndex+3002));
  SurfMap::addSurf("KlystronCorner",-SMap.realSurf(buildIndex+4));
  SurfMap::setSurf("TDCStart",SMap.realSurf(buildIndex+201));
  SurfMap::setSurf("TDCCorner",SMap.realSurf(buildIndex+201));
  SurfMap::addSurf("TDCCorner",SMap.realSurf(buildIndex+203));
  SurfMap::setSurf("TDCMid",SMap.realSurf(buildIndex+211));
  SurfMap::setSurf("TDCLong",SMap.realSurf(buildIndex+221));
  SurfMap::setSurf("TDCAngleMid",SMap.realSurf(buildIndex+251));

  return;
}


void
InjectionHall::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("InjectionHall","createObjects");

  std::string Out;
  int SI(buildIndex+3000);

  // INNER VOIDS:
  // up to bend anngle
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -3002  3004 -4 5 -6 ");
  makeCell("LinearVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,SI," 3002 -1001 -1511 3 -4 5 -6 7M 17M 27M 37M ");
  makeCell("LWideVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1511 -1001 -1111 1503 -4 5 -6 2007 ");
  makeCell("LTVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1112 -1003 5 -6 -1522  1503 ");
  makeCell("TVoidA",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -201  3 -1003 5 -6 1522 ");
  makeCell("TVoidB",System,cellIndex++,voidMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex,SI,
				 " 201 -211 203 -1003 5 -6 47M 57M 67M 77M 87M 97M 107M ");
  makeCell("SPFVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -2 1004 -4 5 -6");
  makeCell("KlystronVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1001 -2111 1004 (1011:1104) -4 5 -6");
  makeCell("KlystronExit",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,SI,
				 "211 -22 223 -1003 5 -6 97M 117M 127M 137M 147M 157M 167M ");
  makeCell("LongVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,SI,
				 "22 -23 223 -1003 5 -6 ");
  makeCell("BackWall",System,cellIndex++,backWallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,SI,
				 "23 -2 223 -1003 5 -6 ");
  makeCell("LongVoidAfter",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"111 -2 4 -104 5 -6");
  makeCell("CutVoid",System,cellIndex++,voidMat,0.0,Out);

  //OUTER WALLS:
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -201 -3 13 5 -6");
  makeCell("LeftWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -211 -203 213 5 -6");
  makeCell("SPFWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 -2 -223 233 5 -6");
  makeCell("LongWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -111 4 -14 5 -6");
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -111 14 -114 5 -6");
  makeCell("CutWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -2 104 -114 5 -6");
  makeCell("OuterWall",System,cellIndex++,wallMat,0.0,Out);

  // Klystrong divivde
  Out=ModelSupport::getComposite(SMap,buildIndex,"2111 -111 1004 -4 5 -6");
  makeCell("KlystronWall",System,cellIndex++,wallMat,0.0,Out);

  // OUTER VOIDS:

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -211 53 -13 -213 5 -16");
  makeCell("LeftOuter",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 211 -2 53 -233 5 -16");
  makeCell("LeftOuterLong",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -101 -54 14  5 -16 ");
  makeCell("RightLinear",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -2 -54 114  5 -16");
  makeCell("RightCut",System,cellIndex++,voidMat,0.0,Out);

  // ROOF/FLOOR
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 53 -54 -5 15 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -201 13 -14 6 -16 ");
  makeCell("Roof",System,cellIndex++,roofMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"201 -211 213 -14 6 -16 ");
  makeCell("Roof",System,cellIndex++,roofMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"211 -2 233 -14 6 -16 ");
  makeCell("Roof",System,cellIndex++,roofMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -2 14 -114 6 -16 ");
  makeCell("Roof",System,cellIndex++,roofMat,0.0,Out);

  // MID T
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1001 1003 -1004 -2 5 -6 2007 (-5003:5004:-5005:5006)");
  makeCell("MidT",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1001 -1011 1004 -1104 5 -6 ");
  makeCell("MidT",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1111 -1112 -1003 1153 5 -6 2007 ");
  makeCell("MidTAngle",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1201 -1202 1103 -1153 5 -6 ");
  makeCell("MidT",System,cellIndex++,wallMat,0.0,Out);

  // Auxiliary cyliner to cure geometric problems in corners
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 5 -6 -2007 ");
  makeCell("MidTAuxCyl",System,cellIndex++,wallMat,0.0,Out);


  // GATE:
  Out=ModelSupport::getComposite(SMap,buildIndex,"1511 -1512 3 -1503 5 -6  ");
  makeCell("GateA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1521 -1522 3 -1503 5 -6  ");
  makeCell("GateB",System,cellIndex++,wallMat,0.0,Out);
  // void in middle
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1512 -1521 3 -1503 5 -6 "
          "(-1201 : 1202 : 1153 : -1103)  "
          "( -1153 : -1111 : 1112)");
  makeCell("GateVoid",System,cellIndex++,0,0.0,Out);

  // KLYSTRONG WALLS
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -3001 -3014 3 5 -6");
  makeCell("KystronVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1 3 -3002 -3004 (3001:3014) 5 -6");
  makeCell("KystronWall",System,cellIndex++,wallMat,0.0,Out);

  // Pillars
  for (size_t i=0; i<nPillars; ++i)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,SI," 5 -6 -7M ");
      makeCell("Pillar"+std::to_string(i),
	       System,cellIndex++,pMat[i],0.0,Out);
      SI += 10;
    }

  // THz penetration
  Out=ModelSupport::getComposite(SMap,buildIndex," 1001 1003 5003 -5004 5005 -5006 ");
  makeCell("THz",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 53 -54 15 -16 ");
  addOuterSurf(Out);



  return;
}

void
InjectionHall::createLinks()
  /*!
    Determines the link points for the beam direction first:
    This is special : each beamport has two coordinates (and axis)
    First is the mid triangle point [ start of straight section]
    The second is the mid point on the wall it points to
  */
{
  ELog::RegMethod RegA("InjectionHall","createLinks");

  return;
}


void
InjectionHall::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("InjectionHall","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
