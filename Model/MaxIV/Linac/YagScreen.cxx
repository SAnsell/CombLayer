/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/YagScreen.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "Exception.h"
#include "Quaternion.h"

#include "YagScreen.h"

namespace tdcSystem
{

YagScreen::YagScreen(const std::string& Key)  :
  attachSystem::ContainedGroup("Mirror", "Holder", "Body"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  closed(false)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

YagScreen::YagScreen(const YagScreen& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  jbLength(A.jbLength),jbWidth(A.jbWidth),jbHeight(A.jbHeight),
  jbWallThick(A.jbWallThick),
  jbWallMat(A.jbWallMat),
  jbMat(A.jbMat),
  ffLength(A.ffLength),
  ffInnerRadius(A.ffInnerRadius),
  ffWallThick(A.ffWallThick),
  ffFlangeLen(A.ffFlangeLen),
  ffFlangeRadius(A.ffFlangeRadius),
  ffWallMat(A.ffWallMat),
  holderLift(A.holderLift),
  holderRad(A.holderRad),
  holderMat(A.holderMat),
  mirrorRadius(A.mirrorRadius),
  mirrorAngle(A.mirrorAngle),
  mirrorThick(A.mirrorThick),
  mirrorMat(A.mirrorMat),
  voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: YagScreen to copy
  */
{}

YagScreen&
YagScreen::operator=(const YagScreen& A)
  /*!
    Assignment operator
    \param A :: YagScreen to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      jbLength=A.jbLength;
      jbWidth=A.jbWidth;
      jbHeight=A.jbHeight;
      jbWallThick=A.jbWallThick;
      jbWallMat=A.jbWallMat;
      jbMat=A.jbMat;
      ffLength=A.ffLength;
      ffInnerRadius=A.ffInnerRadius;
      ffWallThick=A.ffWallThick;
      ffFlangeLen=A.ffFlangeLen;
      ffFlangeRadius=A.ffFlangeRadius;
      ffWallMat=A.ffWallMat;
      holderLift=A.holderLift;
      holderRad=A.holderRad;
      holderMat=A.holderMat;
      mirrorRadius=A.mirrorRadius;
      mirrorAngle=A.mirrorAngle;
      mirrorThick=A.mirrorThick;
      mirrorMat=A.mirrorMat;
      voidMat=A.voidMat;
    }
  return *this;
}

YagScreen*
YagScreen::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new YagScreen(*this);
}

YagScreen::~YagScreen()
  /*!
    Destructor
  */
{}

void
YagScreen::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("YagScreen","populate");

  FixedRotate::populate(Control);

  jbLength=Control.EvalVar<double>(keyName+"JBLength");
  jbWidth=Control.EvalVar<double>(keyName+"JBWidth");
  jbHeight=Control.EvalVar<double>(keyName+"JBHeight");
  jbWallThick=Control.EvalVar<double>(keyName+"JBWallThick");
  jbWallMat=ModelSupport::EvalMat<int>(Control,keyName+"JBWallMat");
  jbMat=ModelSupport::EvalMat<int>(Control,keyName+"JBMat");

  ffLength=Control.EvalVar<double>(keyName+"FFLength");
  ffInnerRadius=Control.EvalVar<double>(keyName+"FFInnerRadius");
  ffWallThick=Control.EvalVar<double>(keyName+"FFWallThick");
  ffFlangeLen=Control.EvalVar<double>(keyName+"FFFlangeLength");
  ffFlangeRadius=Control.EvalVar<double>(keyName+"FFFlangeRadius");
  ffWallMat=ModelSupport::EvalMat<int>(Control,keyName+"FFWallMat");
  holderLift=Control.EvalVar<double>(keyName+"HolderLift");
  holderRad=Control.EvalVar<double>(keyName+"HolderRadius");
  holderMat=ModelSupport::EvalMat<int>(Control,keyName+"HolderMat");
  mirrorRadius=Control.EvalVar<double>(keyName+"MirrorRadius");
  mirrorAngle=Control.EvalVar<double>(keyName+"MirrorAngle");
  mirrorThick=Control.EvalVar<double>(keyName+"MirrorThick");
  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");

  if (holderRad>=ffInnerRadius)
    throw ColErr::RangeError<double>(holderRad,0,ffInnerRadius,
				     "HolderInnerRad >= FFFlangeRadius:");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  closed=Control.EvalVar<int>(keyName+"Closed");

  return;
}

void
YagScreen::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("YagScreen","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
YagScreen::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("YagScreen","createSurfaces");

  const double holderZStep(closed ? holderLift : 1.0);

  // linear pneumatics feedthrough
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*ffLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,ffInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,ffInnerRadius+ffWallThick);
  // flange
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*ffFlangeLen,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,ffFlangeRadius);

  // electronics junction box
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(ffLength+jbWallThick),Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+102,
				  SMap.realPtr<Geometry::Plane>(buildIndex+101),jbLength);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(jbWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(jbWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(jbHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(jbHeight/2.0),Z);


  SMap.addMatch(buildIndex+111, SMap.realSurf(buildIndex+2));

  ModelSupport::buildShiftedPlane(SMap,buildIndex+112,
				  SMap.realPtr<Geometry::Plane>(buildIndex+2),
				  jbLength+jbWallThick*2);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+113,
				  SMap.realPtr<Geometry::Plane>(buildIndex+103),
				  -jbWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+114,
				  SMap.realPtr<Geometry::Plane>(buildIndex+104),
				  jbWallThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+115,
  				  SMap.realPtr<Geometry::Plane>(buildIndex+105),
  				  -jbWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+116,
  				  SMap.realPtr<Geometry::Plane>(buildIndex+106),
 				  jbWallThick);

  // screen holder
  ModelSupport::buildShiftedPlane(SMap,buildIndex+201,
				  SMap.realPtr<Geometry::Plane>(buildIndex+1),
				  -holderZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,holderRad);

  // mirror
  Geometry::Vec3D MVec(Z);
  Geometry::Quaternion QV = Geometry::Quaternion::calcQRotDeg(mirrorAngle,X);
  QV.rotate(MVec);

  double c = cos(mirrorAngle*M_PI/180.0);

  const Geometry::Vec3D or305(Origin-Z*(mirrorThick/2.0/c)-Y*(holderZStep));
  ModelSupport::buildPlane(SMap,buildIndex+305,or305,MVec);

  const Geometry::Vec3D or306(Origin+Z*(mirrorThick/2.0/c)-Y*(holderZStep));
  ModelSupport::buildPlane(SMap,buildIndex+306,or306,MVec);

  // tmp is intersect of mirror cylinder and holder
  const Geometry::Vec3D tmp = mirrorAngle > 0.0 ? or305 : or306;
  const Geometry::Vec3D or307(tmp-MVec*X*mirrorRadius+MVec*mirrorThick/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+307,
			      or307,
			      MVec,mirrorRadius);

  return;
}

void
YagScreen::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("YagScreen","createObjects");

  std::string Out;

  // linear pneumatics feedthrough
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 207 -7 ");
  makeCell("FFInner",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 7 -17 ");
  makeCell("FFWall",System,cellIndex++,ffWallMat,0.0,Out);

  // flange
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 17 -27 ");
  makeCell("FFFlange",System,cellIndex++,ffWallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 17 -27 ");
  makeCell("FFFlangeAir",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -27 ");
  addOuterSurf("Body",Out);

  // electronics junction box
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("JBVoid",System,cellIndex++,jbMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
  				 " 111 -112 113 -114 115 -116 (-101:102:-103:104:-105:106) ");
  makeCell("JBWall",System,cellIndex++,jbWallMat,0.0,Out);

  // mirror/screen holder
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -207 ");
  makeCell("Holder",System,cellIndex++,holderMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -1 -207 ");
  makeCell("Holder",System,cellIndex++,holderMat,0.0,Out);
  addOuterSurf("Holder",Out);

  // mirror
  Out=ModelSupport::getComposite(SMap,buildIndex," 305 -306 -307 ");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,Out);
  addOuterSurf("Mirror",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -112 113 -114 115 -116 ");
  addOuterUnionSurf("Body",Out);

  return;
}


void
YagScreen::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("YagScreen","createLinks");

  FixedComp::setConnect(0,Origin-Y*(jbLength/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(jbLength/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(jbWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(jbWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(jbHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(jbHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
YagScreen::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("YagScreen","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
