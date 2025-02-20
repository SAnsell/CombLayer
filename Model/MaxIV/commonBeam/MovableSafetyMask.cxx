/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/MovableSafetyMask.cxx
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

#include "MovableSafetyMask.h"

namespace xraySystem
{

MovableSafetyMask::MovableSafetyMask(const std::string& Key)  :
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

MovableSafetyMask::MovableSafetyMask(const MovableSafetyMask& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  uMaskWidth(A.uMaskWidth),
  uMaskHeight(A.uMaskHeight),
  wMaskWidth(A.wMaskWidth),
  wMaskHeight(A.wMaskHeight),
  idType(A.idType),
  mainMat(A.mainMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: MovableSafetyMask to copy
  */
{}

MovableSafetyMask&
MovableSafetyMask::operator=(const MovableSafetyMask& A)
  /*!
    Assignment operator
    \param A :: MovableSafetyMask to copy
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
      uMaskWidth=A.uMaskWidth;
      uMaskHeight=A.uMaskHeight;
      wMaskWidth=A.wMaskWidth;
      wMaskHeight=A.wMaskHeight;
      idType=A.idType;
      mainMat=A.mainMat;
      voidMat=A.voidMat;
    }
  return *this;
}

MovableSafetyMask*
MovableSafetyMask::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MovableSafetyMask(*this);
}

MovableSafetyMask::~MovableSafetyMask()
  /*!
    Destructor
  */
{}

void
MovableSafetyMask::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MovableSafetyMask","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  uMaskWidth=Control.EvalVar<double>(keyName+"UMaskWidth");
  uMaskHeight=Control.EvalVar<double>(keyName+"UMaskHeight");
  wMaskWidth=Control.EvalVar<double>(keyName+"WMaskWidth");
  wMaskHeight=Control.EvalVar<double>(keyName+"WMaskHeight");
  idType=Control.EvalVar<std::string>(keyName+"IDType");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
MovableSafetyMask::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MovableSafetyMask","createSurfaces");

  Geometry::Vec3D APt;  // front
  Geometry::Vec3D BPt;  // back

  double zoffset;
  if (idType == "wiggler")
    zoffset = 0.0;
  else if (idType == "undulator")
    zoffset = (wMaskHeight+uMaskHeight)/2.0;
  else throw ColErr::ExitAbort("Wrong IDType value: " + idType + ". Allowed are either 'wiggler' or 'undulator'.");

  if (!frontActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    APt = Origin;
  } else
    APt=ExternalCut::interPoint("front",Origin,Y);

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+uMaskWidth),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    BPt = Origin+Y*(length);
  } else
    BPt=ExternalCut::interPoint("back",Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0+zoffset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0-zoffset),Z);

  // wiggler mask (lower mask, wide)
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wMaskWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wMaskWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wMaskHeight/2.0+zoffset),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wMaskHeight/2.0-zoffset),Z);

  // undulator mask (upper mask, tall)
  const double AW2 = wMaskWidth/2.0;
  const double BW2 = uMaskWidth/2.0;
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   APt-X*AW2+Z*wMaskHeight/2.0,
			   APt-X*AW2+Z*(wMaskHeight/2.0+uMaskHeight),
			   BPt-X*BW2+Z*wMaskHeight/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   APt+X*AW2+Z*wMaskHeight/2.0,
			   APt+X*AW2+Z*(wMaskHeight/2.0+uMaskHeight),
			   BPt+X*BW2+Z*wMaskHeight/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(wMaskHeight/2.0+uMaskHeight-zoffset),Z);

  return;
}

void
MovableSafetyMask::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MovableSafetyMask","createObjects");

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
  makeCell("WigglerMask",System,cellIndex++,voidMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 23 -24 16 -26 ");
  makeCell("UndulatorMask",System,cellIndex++,voidMat,0.0,HR*frontStr*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -6 ");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
MovableSafetyMask::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("MovableSafetyMask","createLinks");

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
MovableSafetyMask::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MovableSafetyMask","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
