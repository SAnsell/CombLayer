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

#include "FileReport.h"
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

#include "Exception.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"

#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"

#include "Plane.h"
#include "SurInter.h"

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
  ductType(A.ductType),
  shieldType(A.shieldType),
  shieldPenetrationType(A.shieldPenetrationType),
  shieldPenetrationZOffset(A.shieldPenetrationZOffset),
  shieldPenetrationXOffset(A.shieldPenetrationXOffset),
  shieldPenetrationRadius(A.shieldPenetrationRadius),
  shieldPenetrationWidth(A.shieldPenetrationWidth),
  shieldPenetrationHeight(A.shieldPenetrationHeight),
  shieldPenetrationTiltXmin(A.shieldPenetrationTiltXmin),
  shieldLedgeThick(A.shieldLedgeThick),
  shieldLedgeLength(A.shieldLedgeLength),
  shieldLedgeZOffset(A.shieldLedgeZOffset),
  shieldLedgeActive(A.shieldLedgeActive),
  shieldThick(A.shieldThick),
  shieldWidthRight(A.shieldWidthRight),
  shieldWidthLeft(A.shieldWidthLeft),
  shieldDepth(A.shieldDepth),
  shieldHeight(A.shieldHeight),
  shieldWallOffset(A.shieldWallOffset),
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
      ductType=A.ductType;
      shieldType=A.shieldType;
      shieldPenetrationType=A.shieldPenetrationType;
      shieldPenetrationZOffset=A.shieldPenetrationZOffset;
      shieldPenetrationXOffset=A.shieldPenetrationXOffset;
      shieldPenetrationRadius=A.shieldPenetrationRadius;
      shieldPenetrationWidth=A.shieldPenetrationWidth;
      shieldPenetrationHeight=A.shieldPenetrationHeight;
      shieldPenetrationTiltXmin=A.shieldPenetrationTiltXmin;
      shieldLedgeThick=A.shieldLedgeThick;
      shieldLedgeLength=A.shieldLedgeLength;
      shieldLedgeZOffset=A.shieldLedgeZOffset;
      shieldLedgeActive=A.shieldLedgeActive;
      shieldThick=A.shieldThick;
      shieldWidthRight=A.shieldWidthRight;
      shieldWidthLeft=A.shieldWidthLeft;
      shieldDepth=A.shieldDepth;
      shieldHeight=A.shieldHeight;
      shieldWallOffset=A.shieldWallOffset;
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
  radius=Control.EvalVar<double>(keyName+"Radius");
  ductType=Control.EvalVar<std::string>(keyName+"DuctType");
  shieldType=Control.EvalVar<std::string>(keyName+"ShieldType");
  shieldPenetrationType=Control.EvalVar<std::string>(keyName+"ShieldPenetrationType");
  shieldPenetrationZOffset=Control.EvalVar<double>(keyName+"ShieldPenetrationZOffset");
  shieldPenetrationXOffset=Control.EvalVar<double>(keyName+"ShieldPenetrationXOffset");
  shieldPenetrationRadius=Control.EvalVar<double>(keyName+"ShieldPenetrationRadius");
  shieldPenetrationWidth=Control.EvalVar<double>(keyName+"ShieldPenetrationWidth");
  shieldPenetrationHeight=Control.EvalVar<double>(keyName+"ShieldPenetrationHeight");
  shieldPenetrationTiltXmin=Control.EvalVar<double>(keyName+"ShieldPenetrationTiltXmin");
  shieldLedgeActive=Control.EvalVar<int>(keyName+"ShieldLedgeActive");
  if (shieldLedgeActive) {
    shieldLedgeThick=Control.EvalVar<double>(keyName+"ShieldLedgeThick");
    shieldLedgeLength=Control.EvalVar<double>(keyName+"ShieldLedgeLength");
    shieldLedgeZOffset=Control.EvalVar<double>(keyName+"ShieldLedgeZOffset");
  }
  shieldThick=Control.EvalVar<double>(keyName+"ShieldThick");
  shieldWidthRight=Control.EvalVar<double>(keyName+"ShieldWidthRight");
  shieldWidthLeft=Control.EvalVar<double>(keyName+"ShieldWidthLeft");
  shieldDepth=Control.EvalVar<double>(keyName+"ShieldDepth");
  shieldHeight=Control.EvalVar<double>(keyName+"ShieldHeight");
  shieldWallOffset=Control.EvalVar<double>(keyName+"ShieldWallOffset");

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

  if (ductType == "Cylinder") {
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  } else if (ductType == "Rectangle") {
    ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
    ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

    ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
    ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  } else
    throw  ColErr::ExitAbort(keyName+": Unknown duct type: " + ductType);

  if (shieldType == "None") {;}
  else if (shieldType == "RectangularCover") {
    ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+12,Y,-shieldWallOffset-shieldThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+12,Y,shieldThick);

    ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(shieldWidthLeft),X);
    ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(shieldWidthRight),X);
    ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(shieldDepth),Z);
    ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(shieldHeight),Z);
  } else
    throw  ColErr::ExitAbort(keyName+": Unknown shield type: " + shieldType);

  if (shieldType != "None") {
    if (shieldLedgeActive) {
      ModelSupport::buildPlane(SMap,buildIndex+105,Origin+Z*(shieldLedgeZOffset),Z);
      ModelSupport::buildShiftedPlane(SMap,buildIndex+106,buildIndex+105,Z,shieldLedgeThick);
      ModelSupport::buildShiftedPlane(SMap,buildIndex+111,buildIndex+12,Y,-shieldLedgeLength);
    }

    if (shieldPenetrationType=="Cylinder")
      ModelSupport::buildCylinder(SMap,buildIndex+27,
				  Origin+X*shieldPenetrationXOffset+Z*shieldPenetrationZOffset,Y,
				  shieldPenetrationRadius);
    else if (shieldPenetrationType=="Rectangle") {

      const Geometry::Vec3D pos = Origin-X*(shieldPenetrationWidth/2.0-shieldPenetrationXOffset);
      Geometry::Vec3D myX(X);
      Geometry::Quaternion::calcQRotDeg(shieldPenetrationTiltXmin,Z).rotate(myX);

      ModelSupport::buildPlane(SMap,buildIndex+23,pos,myX);

      ModelSupport::buildPlane(SMap,buildIndex+24,
			       Origin+X*(shieldPenetrationWidth/2.0+shieldPenetrationXOffset),X);
      ModelSupport::buildPlane(SMap,buildIndex+25,
				 Origin-Z*(shieldPenetrationHeight/2.0-shieldPenetrationZOffset),Z);
      ModelSupport::buildPlane(SMap,buildIndex+26,
			       Origin+Z*(shieldPenetrationHeight/2.0+shieldPenetrationZOffset),Z);

    } else if (shieldPenetrationType=="None") {
    } else
      throw  ColErr::ExitAbort(keyName+": wrong shield penetration type: " + shieldPenetrationType);
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

  if (ductType == "Cylinder")
    Out=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  else if (ductType == "Rectangle")
    Out=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  else
    throw  ColErr::ExitAbort(keyName+": Unknown duct type: " + ductType);

  makeCell("MainCell",System,cellIndex++,voidMat,0.0,Out*frontStr*backStr);

  addOuterSurf("Main", Out);

  HeadRule penetration=nullptr;
  if (shieldPenetrationType=="None")
    ;
  else if (shieldPenetrationType=="Cylinder")
    penetration=ModelSupport::getHeadRule(SMap,buildIndex,"-27");
  else if (shieldPenetrationType=="Rectangle")
    penetration=ModelSupport::getSetHeadRule(SMap,buildIndex,"23 -14 -24 25 -26"); // -14 allows the penetration to cut the shield
  else
    throw  ColErr::ExitAbort(keyName+": Unknown shield penetration type: " + shieldPenetrationType);

  if (shieldType != "None") {
    if (shieldType == "RectangularCover") {
      if (shieldWallOffset>0.0) {
	Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 13 -14 15 -16")*backStr.complement();
	makeCell("ShieldWallOffset",System,cellIndex++,voidMat,0.0,Out);
      }

      Out=ModelSupport::getHeadRule(SMap,buildIndex,"12 -11 13 -14 15 -16");
      if (shieldPenetrationType!="None")
	Out *= penetration.complement();
      makeCell("ShieldPlate",System,cellIndex++,shieldMat,0.0,Out);

      if (shieldPenetrationType!="None") {
	Out=ModelSupport::getHeadRule(SMap,buildIndex," 12 -11 ")*penetration;
	makeCell("ShieldPenetration",System,cellIndex++,voidMat,0.0,Out);
      }

      if (shieldLedgeActive > Geometry::zeroTol) {
	Out=ModelSupport::getHeadRule(SMap,buildIndex," -12 111 13 -14 105 -106 ");
	makeCell("Ledge",System,cellIndex++,shieldMat,0.0,Out);

	Out=ModelSupport::getHeadRule(SMap,buildIndex," -12 111 13 -14 15 -105 ");
	makeCell("LedgeBelow",System,cellIndex++,voidMat,0.0,Out);

	Out=ModelSupport::getHeadRule(SMap,buildIndex," -12 111 13 -14 106 -16");
	makeCell("LedgeAbove",System,cellIndex++,voidMat,0.0,Out);

	Out=ModelSupport::getHeadRule(SMap,buildIndex,"111 13 -14 15 -16")*backStr.complement();
	addOuterSurf("Shield", Out);
      } else {
	Out=ModelSupport::getHeadRule(SMap,buildIndex,"12 13 -14 15 -16")*backStr.complement();
	addOuterSurf("Shield", Out);
      }
    }
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
