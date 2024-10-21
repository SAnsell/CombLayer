/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/Duct.cxx
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
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "Duct.h"

namespace xraySystem
{

Duct::Duct(const std::string& Key)  :
  attachSystem::ContainedGroup("Main", "Shield"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Duct::Duct(const Duct& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  radius(A.radius),
  shieldType(A.shieldType),
  shieldPenetrationZOffset(A.shieldPenetrationZOffset),
  shieldPenetrationXOffset(A.shieldPenetrationXOffset),
  shieldPenetrationRadius(A.shieldPenetrationRadius),
  shieldThick(A.shieldThick),
  shieldWidthRight(A.shieldWidthRight),
  shieldWidthLeft(A.shieldWidthLeft),
  shieldDepth(A.shieldDepth),
  shieldHeight(A.shieldHeight),
  voidMat(A.voidMat),
  shieldMat(A.shieldMat)
  /*!
    Copy constructor
    \param A :: Duct to copy
  */
{}

Duct&
Duct::operator=(const Duct& A)
  /*!
    Assignment operator
    \param A :: Duct to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      radius=A.radius;
      shieldType=A.shieldType;
      shieldPenetrationZOffset=A.shieldPenetrationZOffset;
      shieldPenetrationXOffset=A.shieldPenetrationXOffset;
      shieldPenetrationRadius=A.shieldPenetrationRadius;
      shieldThick=A.shieldThick;
      shieldWidthRight=A.shieldWidthRight;
      shieldWidthLeft=A.shieldWidthLeft;
      shieldDepth=A.shieldDepth;
      shieldHeight=A.shieldHeight;
      voidMat=A.voidMat;
      shieldMat=A.shieldMat;
    }
  return *this;
}

Duct*
Duct::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Duct(*this);
}

Duct::~Duct()
  /*!
    Destructor
  */
{}

void
Duct::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Duct","populate");

  FixedRotate::populate(Control);

  length=Control.EvalDefVar<double>(keyName+"Length",0.0);
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  radius=Control.EvalDefVar<double>(keyName+"Radius",0.0);
  shieldType=Control.EvalVar<std::string>(keyName+"ShieldType");
  shieldPenetrationZOffset=Control.EvalVar<double>(keyName+"ShieldPenetrationZOffset");
  shieldPenetrationXOffset=Control.EvalVar<double>(keyName+"ShieldPenetrationXOffset");
  shieldPenetrationRadius=Control.EvalVar<double>(keyName+"ShieldPenetrationRadius");
  shieldThick=Control.EvalVar<double>(keyName+"ShieldThick");
  shieldWidthRight=Control.EvalVar<double>(keyName+"ShieldWidthRight");
  shieldWidthLeft=Control.EvalVar<double>(keyName+"ShieldWidthLeft");
  shieldDepth=Control.EvalVar<double>(keyName+"ShieldDepth");
  shieldHeight=Control.EvalVar<double>(keyName+"ShieldHeight");

  if (radius>Geometry::zeroTol)
    if (width>0.0 || height>0.0)
      throw ColErr::ExitAbort("Width and Height must be zero if Radius>0.0");

  if (radius<Geometry::zeroTol)
    if (width*height<Geometry::zeroTol)
      throw ColErr::ExitAbort("Both Width and Height must be defined if Radius is zero");

  if (radius<Geometry::zeroTol && length<Geometry::zeroTol &&
      width<Geometry::zeroTol  && height<Geometry::zeroTol)
    throw  ColErr::ExitAbort("Either Radius or Length/Width/Height must be non-zero");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");

  return;
}

void
Duct::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Duct","createSurfaces");

  if (length<Geometry::zeroTol)
    if (!frontActive() || !backActive())
      throw  ColErr::ExitAbort("Length must be defined of no front/back rules are set");

  if (!frontActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
  }

  if (radius>Geometry::zeroTol) {
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  } else {
    ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
    ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
    ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  }

  if (shieldType != "None") {
    ModelSupport::buildCylinder(SMap,buildIndex+17,
				Origin+X*shieldPenetrationXOffset+Z*shieldPenetrationZOffset,Y,
				shieldPenetrationRadius);
  }


  if (shieldType == "RectangularCover") {
    ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+11,Y,-shieldThick);

    ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(shieldWidthLeft),X);
    ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(shieldWidthRight),X);
    ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(shieldDepth),Z);
    ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(shieldHeight),Z);
  }

  return;
}

void
Duct::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Duct","createObjects");

  HeadRule Out;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  Out=ModelSupport::getSetHeadRule(SMap,buildIndex," 3 -4 5 -6 -7 ")*frontStr*backStr;
  makeCell("MainCell",System,cellIndex++,voidMat,0.0,Out);

  addOuterSurf("Main", Out);

  if (shieldType == "RectangularCover") {
    Out=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 13 -14 15 -16 17")*backStr.complement();
    makeCell("ShieldPlate",System,cellIndex++,shieldMat,0.0,Out);

    Out=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 -17")*backStr.complement();
    makeCell("ShieldPenetration",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 13 -14 15 -16")*backStr.complement();
    addOuterSurf("Shield", Out);
  }

  return;
}


void
Duct::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Duct","createLinks");

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
Duct::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Duct","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
