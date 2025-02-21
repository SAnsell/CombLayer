/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/HeatAbsorberToyama.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#include "Exception.h"
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

#include "HeatAbsorberToyama.h"

namespace xraySystem
{

HeatAbsorberToyama::HeatAbsorberToyama(const std::string& Key)  :
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

HeatAbsorberToyama::HeatAbsorberToyama(const HeatAbsorberToyama& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  dumpWidth(A.dumpWidth),
  dumpHeight(A.dumpHeight),
  dumpLength(A.dumpLength),
  dumpEndRadius(A.dumpEndRadius),
  dumpFrontRadius(A.dumpFrontRadius),
  dumpXOffset(A.dumpXOffset),
  gapWidth(A.gapWidth),
  gapHeight(A.gapHeight),
  closed(A.closed),
  mainMat(A.mainMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: HeatAbsorberToyama to copy
  */
{}

HeatAbsorberToyama&
HeatAbsorberToyama::operator=(const HeatAbsorberToyama& A)
  /*!
    Assignment operator
    \param A :: HeatAbsorberToyama to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      dumpWidth=A.dumpWidth;
      dumpHeight=A.dumpHeight;
      dumpLength=A.dumpLength;
      dumpEndRadius=A.dumpEndRadius;
      dumpFrontRadius=A.dumpFrontRadius;
      dumpXOffset=A.dumpXOffset;
      gapWidth=A.gapWidth;
      gapHeight=A.gapHeight;
      closed=A.closed;
      mainMat=A.mainMat;
      voidMat=A.voidMat;
    }
  return *this;
}

HeatAbsorberToyama*
HeatAbsorberToyama::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new HeatAbsorberToyama(*this);
}

HeatAbsorberToyama::~HeatAbsorberToyama()
  /*!
    Destructor
  */
{}

void
HeatAbsorberToyama::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HeatAbsorberToyama","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  dumpWidth=Control.EvalVar<double>(keyName+"DumpWidth");
  dumpHeight=Control.EvalVar<double>(keyName+"DumpHeight");
  dumpLength=Control.EvalVar<double>(keyName+"DumpLength");
  dumpEndRadius=Control.EvalVar<double>(keyName+"DumpEndRadius");
  dumpFrontRadius=Control.EvalVar<double>(keyName+"DumpFrontRadius");
  dumpXOffset=Control.EvalVar<double>(keyName+"DumpXOffset");
  gapWidth=Control.EvalVar<double>(keyName+"GapWidth");
  gapHeight=Control.EvalVar<double>(keyName+"GapHeight");
  closed=true;//Control.EvalVar<int>(keyName+"Closed");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
HeatAbsorberToyama::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("HeatAbsorberToyama","createSurfaces");

  Geometry::Vec3D APt;  // front
  Geometry::Vec3D BPt;  // back

  double zoffset = closed ? (gapHeight+dumpHeight)/2.0 : 0.0;

  if (!frontActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    APt = Origin;
  } else
    APt=ExternalCut::interPoint("front",Origin,Y);

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+dumpWidth),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    BPt = Origin+Y*(length);
  } else
    BPt=ExternalCut::interPoint("back",Origin,Y);

  //  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+11,Y,dumpEndRadius);
  const Geometry::Vec3D dumpFrontCentre =
    APt - X*(dumpWidth/2.0-dumpXOffset) + Y*(dumpFrontRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+7,dumpFrontCentre,Z,dumpFrontRadius);

  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+11,Y,dumpFrontRadius);
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+12,Y,dumpLength-dumpEndRadius);
  const Geometry::Vec3D dumpEndCentre =
    APt + X*(dumpWidth/2.0+dumpXOffset-dumpEndRadius) + Y*(dumpLength-dumpEndRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,dumpEndCentre,Z,dumpEndRadius);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0+zoffset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0-zoffset),Z);

  // wiggler mask (lower mask, wide)
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(gapWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(gapWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(gapHeight/2.0+zoffset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(gapHeight/2.0-zoffset),Z);

  // undulator mask (upper mask, tall)
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(dumpWidth/2.0-dumpXOffset),X);
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   dumpFrontCentre+X*dumpFrontRadius,
			   dumpFrontCentre+X*dumpFrontRadius+Z*1.0,
			   dumpEndCentre-X*dumpEndRadius, X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(dumpWidth/2.0+dumpXOffset),X);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(gapHeight/2.0+dumpHeight-zoffset),Z);

  return;
}

void
HeatAbsorberToyama::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HeatAbsorberToyama","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -13 5 -16 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 13 -14 5 -15 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 14 -4 5 -16 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -23 16 -26 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 24 -4 16 -26 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 26 -6 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 13 -14 15 -16 ");
  makeCell("Penetration",System,cellIndex++,voidMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -33 7 11 -12 16 -26");
  makeCell("Dump",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -33 7 -11 16 -26");
  makeCell("DumpVoidFront",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"33 -24 -12 16 -26");
  makeCell("DumpVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -7 16 -26 -33");
  makeCell("DumpFrontCylinder",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 16 -26 12 -17 ");
  makeCell("DumpEndCylinder",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 23 -24 16 -26 12 17 ");
  makeCell("DumpEnd",System,cellIndex++,mainMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -6 ");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
HeatAbsorberToyama::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("HeatAbsorberToyama","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
HeatAbsorberToyama::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("HeatAbsorberToyama","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
