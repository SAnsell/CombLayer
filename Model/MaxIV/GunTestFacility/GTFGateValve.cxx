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
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  closed(0),clampActive(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GTFGateValve::GTFGateValve(const GTFGateValve& A) :
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),portARadius(A.portARadius),
  portAThick(A.portAThick),portALen(A.portALen),
  portBRadius(A.portBRadius),portBThick(A.portBThick),
  portBLen(A.portBLen),closed(A.closed),bladeLift(A.bladeLift),
  bladeThick(A.bladeThick),bladeRadius(A.bladeRadius),
  clampActive(A.clampActive),
  clampWidth(A.clampWidth),
  clampDepth(A.clampDepth),
  clampHeight(A.clampHeight),
  clampBulkHeight(A.clampBulkHeight),
  clampBaseThick(A.clampBaseThick),
  clampBaseWidth(A.clampBaseWidth),
  clampTopWidth(A.clampTopWidth),
  voidMat(A.voidMat),bladeMat(A.bladeMat),wallMat(A.wallMat),
  clampMat(A.clampMat)
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
      attachSystem::ContainedComp::operator=(A);
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
      clampActive=A.clampActive;
      clampWidth=A.clampWidth;
      clampDepth=A.clampDepth;
      clampHeight=A.clampHeight;
      clampBulkHeight=A.clampBulkHeight;
      clampBaseThick=A.clampBaseThick;
      clampBaseWidth=A.clampBaseWidth;
      clampTopWidth=A.clampTopWidth;
      voidMat=A.voidMat;
      bladeMat=A.bladeMat;
      wallMat=A.wallMat;
      clampMat=A.clampMat;
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
  clampActive=Control.EvalDefVar<int>(keyName+"ClampActive",clampActive);
  clampWidth=Control.EvalVar<int>(keyName+"ClampWidth");
  clampDepth=Control.EvalVar<double>(keyName+"ClampDepth");
  clampHeight=Control.EvalVar<double>(keyName+"ClampHeight");
  clampBulkHeight=Control.EvalVar<double>(keyName+"ClampBulkHeight");
  clampBaseThick=Control.EvalVar<double>(keyName+"ClampBaseThick");
  clampBaseWidth=Control.EvalVar<double>(keyName+"ClampBaseWidth");
  clampTopWidth=Control.EvalVar<double>(keyName+"ClampTopWidth");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  clampMat=ModelSupport::EvalMat<int>(Control,keyName+"ClampMat");

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

  if (closed)
    ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,bladeRadius);
  else
    ModelSupport::buildCylinder(SMap,buildIndex+307,Origin+Z*bladeLift,
				Y,bladeRadius);

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

  const bool portAExtends(wallThick<=portALen);  // port extends
  const bool portBExtends(wallThick<=portBLen);  // port extends

  const HeadRule frontHR=getFrontRule();  // 101
  const HeadRule backHR=getBackRule();    // -102
  const HeadRule frontComp=getFrontComplement();  // -101
  const HeadRule backComp=getBackComplement();    // 102
  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"1 -2 3 -4 5 -6 (307:-301:302)");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  // Main body
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"1 -2 13 -14 15 -16 (-3:4:-5:6)");
  makeCell("Body",System,cellIndex++,wallMat,0.0,HR);

  // blade
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-307 301 -302");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,HR);

  // front plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 11 13 -14 15 -16 107");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,HR);
  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 107 -117");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -107");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  if (!portAExtends)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -117");
      makeCell("FrontVoidExtra",System,cellIndex++,voidMat,0.0,HR*frontComp);
    }

  // back plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 13 -14 15 -16 207");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,HR);
  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 207 -217");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -207");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  if (!portBExtends)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -217");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,HR*backComp);
    }

  //  if (!clampActive) {
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 15 -16");
    addOuterSurf(HR);
    // }

  if (portAExtends || portBExtends)
    {
      if (clampActive) { // assume both port extend, otherwise the clamp does not make sence
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

	HR=ModelSupport::getHeadRule(SMap,buildIndex,"413 -414 415 -406");
	addOuterUnionSurf(HR*frontHR*backHR);
      } else {
	if (!portAExtends)
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -217");
	else if (!portBExtends)
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -117");
	else
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,"((-11 -117) : (12 -217))");

	addOuterUnionSurf(HR*frontHR*backHR);
      }
    }

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
