/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/SPFCameraShield.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "SurfMap.h"

#include "SPFCameraShield.h"

namespace tdcSystem
{

SPFCameraShield::SPFCameraShield(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SPFCameraShield::SPFCameraShield(const SPFCameraShield& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  length(A.length),width(A.width),height(A.height),
  wallThick(A.wallThick),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: SPFCameraShield to copy
  */
{}

SPFCameraShield&
SPFCameraShield::operator=(const SPFCameraShield& A)
  /*!
    Assignment operator
    \param A :: SPFCameraShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      wallThick=A.wallThick;
      mat=A.mat;
    }
  return *this;
}

SPFCameraShield*
SPFCameraShield::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new SPFCameraShield(*this);
}

SPFCameraShield::~SPFCameraShield()
  /*!
    Destructor
  */
{}

void
SPFCameraShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SPFCameraShield","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
SPFCameraShield::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SPFCameraShield","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wallThick+10.0),Z); // TODO
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(height),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,Y,wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+12,buildIndex+2,Y,wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+3,Y,-wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,Y,-wallThick);

  return;
}

void
SPFCameraShield::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SPFCameraShield","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("Floor",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 3 -4 6 -16 ");
  makeCell("SideWallShort",System,cellIndex++,mat,0.0,Out); // lower tier

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 13 -14 16 -26 ");
  makeCell("SideWallShort",System,cellIndex++,mat,0.0,Out); // upper tier

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 14 -4 6 -16 ");
  makeCell("SideWallLong",System,cellIndex++,mat,0.0,Out); // lower tier, beam line side

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 14 -4 16 -26 ");
  makeCell("SideWallLong",System,cellIndex++,mat,0.0,Out); // upper tier, beam line side

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 14 -4 16 -26 ");
  makeCell("OuterVoidLong",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -12 13 -3 5 -16 ");
  makeCell("OuterVoidLong",System,cellIndex++,0,0.0,Out); // lower tier, other side

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -3 16 -26 ");
  makeCell("OuterVoidLong",System,cellIndex++,0,0.0,Out); // upper tier, other side

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 3 -14 6 -26 ");
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 3 -4 5 -6 ");
  makeCell("OuterVoidFloor",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -26 ");
  addOuterSurf(Out);

  return;
}


void
SPFCameraShield::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("SPFCameraShield","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(2,Origin-X*(width/2.0-wallThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+26));

  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");

  FixedComp::nameSideIndex(2,"left");
  FixedComp::nameSideIndex(3,"right");

  FixedComp::nameSideIndex(4,"bottom");
  FixedComp::nameSideIndex(5,"top");

  return;
}

void
SPFCameraShield::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SPFCameraShield","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
