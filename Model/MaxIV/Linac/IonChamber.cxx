/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/IonChamber.cxx
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

#include "IonChamber.h"

namespace xraySystem
{

IonChamber::IonChamber(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

IonChamber::IonChamber(const IonChamber& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  radius(A.radius),
  height(A.height),
  airMat(A.airMat)
  /*!
    Copy constructor
    \param A :: IonChamber to copy
  */
{}

IonChamber&
IonChamber::operator=(const IonChamber& A)
  /*!
    Assignment operator
    \param A :: IonChamber to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      radius=A.radius;
      height=A.height;
      airMat=A.airMat;
    }
  return *this;
}

IonChamber*
IonChamber::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new IonChamber(*this);
}

IonChamber::~IonChamber()
  /*!
    Destructor
  */
{}

void
IonChamber::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("IonChamber","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");

  return;
}

void
IonChamber::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("IonChamber","createSurfaces");

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  SurfMap::makeCylinder("OuterCyl",SMap,buildIndex+7,Origin,Z,radius);

  return;
}

void
IonChamber::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("IonChamber","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 5 -6 ");
  makeCell("MainCell",System,cellIndex++,airMat,0.0,HR);
  addOuterSurf(HR);

  return;
}


void
IonChamber::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("IonChamber","createLinks");

  FixedComp::setConnect(0,Origin-Z*(height/2.0),-Z);
  FixedComp::setNamedLinkSurf(0,"bottom",SurfMap::getSignedSurf("#bottom"));

  FixedComp::setConnect(1,Origin+Z*(height/2.0),Z);
  FixedComp::setNamedLinkSurf(1,"top",SurfMap::getSignedSurf("top"));

  FixedComp::setConnect(2,Origin-X*(radius),-X);
  FixedComp::setNamedLinkSurf(2,"left",SurfMap::getSignedSurf("OuterCyl"));
  FixedComp::setBridgeSurf(2,-SMap.realSurf(40000));

  FixedComp::setConnect(3,Origin+X*(radius),X);
  FixedComp::setNamedLinkSurf(3,"right",SurfMap::getSignedSurf("OuterCyl"));
  FixedComp::setBridgeSurf(3,SMap.realSurf(40000));

  FixedComp::setConnect(4,Origin-Y*(radius),-Y);
  FixedComp::setNamedLinkSurf(4,"front",SurfMap::getSignedSurf("OuterCyl"));
  FixedComp::setBridgeSurf(4,-SMap.realSurf(50000));

  FixedComp::setConnect(5,Origin+Y*(radius),Y);
  FixedComp::setNamedLinkSurf(5,"back",SurfMap::getSignedSurf("OuterCyl"));
  FixedComp::setBridgeSurf(5,SMap.realSurf(50000));

  return;
}

void
IonChamber::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("IonChamber","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
