/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pik/PIKFuelRod.cxx
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

#include "PIKFuelRod.h"

namespace pikSystem
{

PIKFuelRod::PIKFuelRod(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PIKFuelRod::PIKFuelRod(const PIKFuelRod& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  outerR(A.outerR),height(A.height),
  radius(A.radius),
  mainMat(A.mainMat)
  /*!
    Copy constructor
    \param A :: PIKFuelRod to copy
  */
{}

PIKFuelRod&
PIKFuelRod::operator=(const PIKFuelRod& A)
  /*!
    Assignment operator
    \param A :: PIKFuelRod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      outerR=A.outerR;
      radius=A.radius;
      height=A.height;
      mainMat=A.mainMat;
    }
  return *this;
}

PIKFuelRod*
PIKFuelRod::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PIKFuelRod(*this);
}

PIKFuelRod::~PIKFuelRod()
  /*!
    Destructor
  */
{}

void
PIKFuelRod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PIKFuelRod","populate");

  FixedRotate::populate(Control);

  outerR=Control.EvalVar<double>(keyName+"OuterRadius");
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}

void
PIKFuelRod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(radius),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(radius),Y);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(outerR-radius),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(outerR-radius),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(radius),X);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerR-radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(outerR-radius),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
PIKFuelRod::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,
	 " (11 -12 3 -4) : (1 -2 13 -14) 5 -6");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  addOuterSurf(Out);

  return;
}


void
PIKFuelRod::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createLinks");

  FixedComp::setConnect(0,Origin-Y*(outerR/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  //  FixedComp::nameSideIndex(0,"NegativeY");

  FixedComp::setConnect(1,Origin+Y*(outerR/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(radius),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
PIKFuelRod::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // pikSystem
