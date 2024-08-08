/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   GunTestFacility/GTFGateValve.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov and Stuart Ansell
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
#include "Quaternion.h"
#include "SurfMap.h"

#include "GTFGateValve.h"

namespace constructSystem
{

GTFGateValve::GTFGateValve(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Main", "Flange", "Shaft"),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GTFGateValve::GTFGateValve(const GTFGateValve& A) :
  attachSystem::FixedRotate(A),attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),portARadius(A.portARadius),
  portAThick(A.portAThick),portALen(A.portALen),
  portBRadius(A.portBRadius),portBThick(A.portBThick),
  portBLen(A.portBLen),closed(A.closed),bladeLift(A.bladeLift),
  bladeThick(A.bladeThick),bladeRadius(A.bladeRadius),
  bladeCutThick(A.bladeCutThick),
  bladeScrewHousingRadius(A.bladeScrewHousingRadius),
  bladeScrewRadius(A.bladeScrewRadius),
  bladeScrewLength(A.bladeScrewLength),
  bladeScrewTipLength(A.bladeScrewTipLength),
  bladeNotchRadius(A.bladeNotchRadius),
  clampWidth(A.clampWidth),
  clampDepth(A.clampDepth),
  clampHeight(A.clampHeight),
  clampBulkHeight(A.clampBulkHeight),
  clampBaseThick(A.clampBaseThick),
  clampBaseWidth(A.clampBaseWidth),
  clampTopWidth(A.clampTopWidth),
  lsFlangeWidth(A.lsFlangeWidth),
  lsFlangeDepth(A.lsFlangeDepth),
  lsFlangeHeight(A.lsFlangeHeight),
  lsFlangeHoleRadius(A.lsFlangeHoleRadius),
  lsShaftRadius(A.lsShaftRadius),
  lsShaftThick(A.lsShaftThick),
  lsShaftLength(A.lsShaftLength),
  lsShaftFlangeRadius(A.lsShaftFlangeRadius),
  lsShaftFlangeThick(A.lsShaftFlangeThick),
  voidMat(A.voidMat),bladeMat(A.bladeMat),wallMat(A.wallMat),
  clampMat(A.clampMat),
  lsFlangeMat(A.lsFlangeMat)
  /*!
    Copy constructor
    \param A :: GTFGateValve to copy
  */
{}

GTFGateValve&
GTFGateValve::operator=(const GTFGateValve& A)
  /*!
    Assignment operator
    \param A :: GTFGateValve to copy
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
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      portARadius=A.portARadius;
      portAThick=A.portAThick;
      portALen=A.portALen;
      portBRadius=A.portBRadius;
      portBThick=A.portBThick;
      portBLen=A.portBLen;
      closed=A.closed;
      bladeLift=A.bladeLift;
      bladeThick=A.bladeThick;
      bladeRadius=A.bladeRadius;
      bladeCutThick=A.bladeCutThick;
      bladeScrewHousingRadius=A.bladeScrewHousingRadius;
      bladeScrewRadius=A.bladeScrewRadius;
      bladeScrewLength=A.bladeScrewLength;
      bladeScrewTipLength=A.bladeScrewTipLength;
      bladeNotchRadius=A.bladeNotchRadius;
      clampWidth=A.clampWidth;
      clampDepth=A.clampDepth;
      clampHeight=A.clampHeight;
      clampBulkHeight=A.clampBulkHeight;
      clampBaseThick=A.clampBaseThick;
      clampBaseWidth=A.clampBaseWidth;
      clampTopWidth=A.clampTopWidth;
      lsFlangeWidth=A.lsFlangeWidth;
      lsFlangeDepth=A.lsFlangeDepth;
      lsFlangeHeight=A.lsFlangeHeight;
      lsFlangeHoleRadius=A.lsFlangeHoleRadius;
      lsShaftRadius=A.lsShaftRadius;
      lsShaftThick=A.lsShaftThick;
      lsShaftLength=A.lsShaftLength;
      lsShaftFlangeRadius=A.lsShaftFlangeRadius;
      lsShaftFlangeThick=A.lsShaftFlangeThick;
      voidMat=A.voidMat;
      bladeMat=A.bladeMat;
      wallMat=A.wallMat;
      clampMat=A.clampMat;
      lsFlangeMat=A.lsFlangeMat;
    }
  return *this;
}


GTFGateValve::~GTFGateValve()
  /*!
    Destructor
  */
{}

void
GTFGateValve::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GTFGateValve","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portARadius=Control.EvalPair<double>(keyName+"PortARadius",
				       keyName+"PortRadius");
  portAThick=Control.EvalPair<double>(keyName+"PortAThick",
				      keyName+"PortThick");
  portALen=Control.EvalPair<double>(keyName+"PortALen",
				    keyName+"PortLen");

  portBRadius=Control.EvalPair<double>(keyName+"PortBRadius",
				       keyName+"PortRadius");
  portBThick=Control.EvalPair<double>(keyName+"PortBThick",
				      keyName+"PortThick");
  portBLen=Control.EvalPair<double>(keyName+"PortBLen",
				    keyName+"PortLen");

  closed=Control.EvalDefVar<int>(keyName+"Closed",closed);
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeRadius=Control.EvalVar<double>(keyName+"BladeRadius");
  bladeCutThick=Control.EvalVar<double>(keyName+"BladeCutThick");
  bladeScrewHousingRadius=Control.EvalVar<double>(keyName+"BladeScrewHousingRadius");
  bladeScrewRadius=Control.EvalVar<double>(keyName+"BladeScrewRadius");
  bladeScrewLength=Control.EvalVar<double>(keyName+"BladeScrewLength");
  bladeScrewTipLength=Control.EvalVar<double>(keyName+"BladeScrewTipLength");
  bladeNotchRadius=Control.EvalVar<double>(keyName+"BladeNotchRadius");
  clampWidth=Control.EvalVar<int>(keyName+"ClampWidth");
  clampDepth=Control.EvalVar<double>(keyName+"ClampDepth");
  clampHeight=Control.EvalVar<double>(keyName+"ClampHeight");
  clampBulkHeight=Control.EvalVar<double>(keyName+"ClampBulkHeight");
  clampBaseThick=Control.EvalVar<double>(keyName+"ClampBaseThick");
  clampBaseWidth=Control.EvalVar<double>(keyName+"ClampBaseWidth");
  clampTopWidth=Control.EvalVar<double>(keyName+"ClampTopWidth");
  lsFlangeWidth=Control.EvalVar<double>(keyName+"LSFlangeWidth");
  lsFlangeDepth=Control.EvalVar<double>(keyName+"LSFlangeDepth");
  lsFlangeHeight=Control.EvalVar<double>(keyName+"LSFlangeHeight");
  lsFlangeHoleRadius=Control.EvalVar<double>(keyName+"LSFlangeMatHoleRadius");
  lsShaftRadius=Control.EvalVar<double>(keyName+"LSShaftRadius");
  lsShaftThick=Control.EvalVar<double>(keyName+"LSShaftThick");
  lsShaftLength=Control.EvalVar<double>(keyName+"LSShaftLength");
  lsShaftFlangeRadius=Control.EvalVar<double>(keyName+"LSShaftFlangeRadius");
  lsShaftFlangeThick=Control.EvalVar<double>(keyName+"LSShaftFlangeThick");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  clampMat=ModelSupport::EvalMat<int>(Control,keyName+"ClampMat");
  lsFlangeMat=ModelSupport::EvalMat<int>(Control,keyName+"LSFlangeMat");

  return;
}

void
GTFGateValve::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GTFGateValve","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // moved to centre
  Origin+=Y*(length/2.0+portALen);

  return;
}


void
GTFGateValve::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("GTFGateValve","createSurfaces");

  // front planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(portALen+length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+101));
    }

  // back planes
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
			       Origin+Y*(portALen+length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+102));
    }

  // sides
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+wallThick),Z);

  // flange

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,portARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,portARadius+portAThick);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portBRadius+portBThick);


  // Blade
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(bladeThick/2.0),Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+311,buildIndex+301,Y,bladeCutThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+312,buildIndex+302,Y,-bladeScrewLength);

  const double bladeOffset = closed ? 0.0 : bladeLift;

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin+Z*bladeOffset,Y,bladeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin+Z*bladeOffset,Y,bladeScrewHousingRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin+Z*bladeOffset,Y,bladeNotchRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+337,Origin+Z*bladeOffset,Y,bladeScrewRadius);
  // Blade tip
  ModelSupport::buildShiftedPlane(SMap,buildIndex+321,buildIndex+312,Y,-bladeScrewTipLength);
  const double tipAngle = atan(bladeScrewRadius/bladeScrewTipLength)*180.0/M_PI * 0.999; // 0.999 is just to avoid having the outer cylinder surface in the blade tip cell
  ModelSupport::buildCone(SMap,buildIndex+309,Origin+Y*(bladeThick/2.0-bladeScrewLength-bladeScrewTipLength),Y,tipAngle);


  // Clamp
  ModelSupport::buildPlane(SMap,buildIndex+403,Origin-X*(clampWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+404,Origin+X*(clampWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+405,Origin-Z*(clampDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+406,Origin+Z*(clampHeight),Z);

  ModelSupport::buildPlane(SMap,buildIndex+413,Origin-X*(clampBaseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+414,Origin+X*(clampBaseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+415,Origin-Z*(clampDepth+clampBaseThick),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+416,buildIndex+415,Z,clampBulkHeight);

  const Geometry::Quaternion qTop1 = Geometry::Quaternion::calcQRotDeg(-45, Y);
  const Geometry::Vec3D vTop1(qTop1.makeRotate(Z));
  ModelSupport::buildPlane(SMap,buildIndex+417,Origin+Z*clampHeight-X*(clampTopWidth/2.0),vTop1);

  const Geometry::Quaternion qTop2 = Geometry::Quaternion::calcQRotDeg(45, Y);
  const Geometry::Vec3D vTop2(qTop2.makeRotate(Z));
  ModelSupport::buildPlane(SMap,buildIndex+418,Origin+Z*clampHeight+X*(clampTopWidth/2.0),vTop2);

  // Lifting structure
  /// Square flange
  ModelSupport::buildPlane(SMap,buildIndex+503,Origin-X*(lsFlangeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+504,Origin+X*(lsFlangeWidth/2.0),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+505,buildIndex+16,Z,-lsFlangeDepth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+506,buildIndex+16,Z,lsFlangeHeight);
  ModelSupport::buildCylinder(SMap,buildIndex+507,Origin,Z,lsFlangeHoleRadius);
  /// Shaft
  ModelSupport::buildShiftedPlane(SMap,buildIndex+516,buildIndex+506,Z,lsShaftLength);
  ModelSupport::buildCylinder(SMap,buildIndex+517,Origin,Z,lsShaftRadius);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+525,buildIndex+506,Z,lsShaftFlangeThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+526,buildIndex+516,Z,-lsShaftFlangeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+527,Origin,Z,lsShaftRadius+lsShaftThick);
  ModelSupport::buildCylinder(SMap,buildIndex+537,Origin,Z,lsShaftFlangeRadius);

  return;
}

void
GTFGateValve::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GTFGateValve","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getFrontRule();  // 101
  const HeadRule backHR=getBackRule();    // -102
  const HeadRule frontComp=getFrontComplement();  // -101
  const HeadRule backComp=getBackComplement();    // 102
  // Inner void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -301 3 -4 5 -16");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -2 3 -4 5 -16");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 3 -4 5 -16 307");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  // Main body
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -3 15 -16");
  makeCell("Body",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 4 -14 15 -16");
  makeCell("Body",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4  15 -5");
  makeCell("Body",System,cellIndex++,wallMat,0.0,HR);

  // blade
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 337 -317");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -321 -337");
  makeCell("BladeBeforeTip",System,cellIndex++,bladeMat,0.0,HR);
  ELog::EM << "BladeBeforeTip: possible lost particles - touching surfaces" << ELog::endWarn;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"321 -312 -337 309");
  makeCell("BladeAroundTip",System,cellIndex++,bladeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"312 -302 -337");
  makeCell("BladeScrew",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"321 -312 -309");
  makeCell("BladeTip",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -311 317 -327");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -311 327 -307");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"311 -302 317 -307");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,HR);

  // front plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 11 13 -14 15 -16 107");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,HR);
  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 107 -117");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -107");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  // back plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 13 -14 15 -16 207");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,HR);
  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 207 -217");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -207");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 416 -406 -417 -418 117");
  makeCell("ClampFrontTruncated",System,cellIndex++,clampMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 403 -404 405 -416 117");
  makeCell("ClampFrontBulk",System,cellIndex++,clampMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 417 403 416 -406");
  makeCell("ClampFrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 418 -404 416 -406");
  makeCell("ClampFrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 416 -406 -417 -418 217");
  makeCell("ClampBackTruncated",System,cellIndex++,clampMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 403 -404 405 -416 217");
  makeCell("ClampBackBulk",System,cellIndex++,clampMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 417 403 416 -406");
  makeCell("ClampBackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 418 -404 416 -406");
  makeCell("ClampBackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -414 415 -405");
  makeCell("ClampBase",System,cellIndex++,clampMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -403 405 -406");
  makeCell("ClampVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 403 -13 405 -406");
  makeCell("ClampVoidInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 14 -404 405 -406");
  makeCell("ClampVoidInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"404 -414 405 -406");
  makeCell("ClampVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 405 -15");
  makeCell("ClampVoidBase",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -13 406 -505");
  makeCell("OuterVoidTop",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -414 406 -505");
  makeCell("OuterVoidTop",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 -11 406 -505");
  makeCell("OuterVoidTop",System,cellIndex++,voidMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 12 406 -505");
  makeCell("OuterVoidTop",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"503 -504 505 -16 (-11:12:-13:14)");
  makeCell("LSFlangeLow",System,cellIndex++,lsFlangeMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"503 -504 16 -506 507");
  makeCell("LSFlangeUp",System,cellIndex++,lsFlangeMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"16 -506 -507");
  makeCell("LSFlangeUpHole",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -503 505 -506");
  makeCell("LSFlangeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"504 -414 505 -506");
  makeCell("LSFlangeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // Lifting structure shaft
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-517 506 -516");
  makeCell("LSShaftVoidInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"517 -527 506 -516");
  makeCell("LSShaft",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"527 -537 525 -526");
  makeCell("LSShaftVoidOuter",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"506 -525 527 -537");
  makeCell("LSShaftFlangeLow",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"526 -516 527 -537");
  makeCell("LSShaftFlangeUp",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -414 415 -506");
  addOuterSurf("Main",HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-537 506 -516");
  addOuterSurf("Shaft",HR);

  return;
}

void
GTFGateValve::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("GTFGateValve","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}


void
GTFGateValve::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GTFGateValve","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
