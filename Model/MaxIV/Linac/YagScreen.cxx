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
#include "BaseMap.h"
#include "CellMap.h"

#include "YagScreen.h"

namespace tdcSystem
{

YagScreen::YagScreen(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

YagScreen::YagScreen(const YagScreen& A) :
  attachSystem::ContainedComp(A),
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
      attachSystem::ContainedComp::operator=(A);
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
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

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
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("FFInner",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 7 -17 ");
  makeCell("FFWall",System,cellIndex++,ffWallMat,0.0,Out);

  // flange
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 17 -27 ");
  makeCell("FFFlange",System,cellIndex++,ffWallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 17 -27 ");
  makeCell("FFFlangeAir",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -27 ");
  addOuterSurf(Out);

  // electronics junction box
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("JBVoid",System,cellIndex++,jbMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
  				 " 111 -112 113 -114 115 -116 (-101:102:-103:104:-105:106) ");
  makeCell("JBWall",System,cellIndex++,jbWallMat,0.0,Out);

   Out=ModelSupport::getComposite(SMap,buildIndex," 111 -112 113 -114 115 -116 ");
   addOuterUnionSurf(Out);

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
