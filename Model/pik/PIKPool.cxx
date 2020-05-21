/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pik/PIKPool.cxx
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


#include "PIKPool.h"

namespace pikSystem
{

PIKPool::PIKPool(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PIKPool::PIKPool(const PIKPool& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  waterRadius(A.waterRadius),
  height(A.height),
  depth(A.depth),
  shieldRadius(A.shieldRadius),
  shieldWidth(A.shieldWidth),
  waterMat(A.waterMat),
  shieldMat(A.shieldMat)
  /*!
    Copy constructor
    \param A :: PIKPool to copy
  */
{}

PIKPool&
PIKPool::operator=(const PIKPool& A)
  /*!
    Assignment operator
    \param A :: PIKPool to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      waterRadius=A.waterRadius;
      height=A.height;
      depth=A.depth;
      shieldRadius=A.shieldRadius;
      shieldWidth=A.shieldWidth;
      waterMat=A.waterMat;
      shieldMat=A.shieldMat;
    }
  return *this;
}

PIKPool*
PIKPool::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PIKPool(*this);
}

PIKPool::~PIKPool()
  /*!
    Destructor
  */
{}

void
PIKPool::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PIKPool","populate");

  FixedRotate::populate(Control);

  waterRadius=Control.EvalVar<double>(keyName+"WaterRadius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  shieldRadius=Control.EvalVar<double>(keyName+"ShieldRadius");
  shieldWidth=Control.EvalVar<double>(keyName+"ShieldWidth");

  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");

  return;
}

void
PIKPool::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKPool","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
PIKPool::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKPool","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,waterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,shieldRadius);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(shieldWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(shieldWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height),Z);

  return;
}

void
PIKPool::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PIKPool","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -6 ");
  makeCell("Water",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 5 -6 3 -4 ");
  makeCell("Container",System,cellIndex++,shieldMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -6 -3 ");
  makeCell("VoidLeft",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -6 4 ");
  makeCell("VoidRight",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -6 ");
  addOuterSurf(Out);

  return;
}


void
PIKPool::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PIKPool","createLinks");

  FixedComp::setConnect(0,Origin-Y*(waterRadius/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(waterRadius/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(shieldRadius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(shieldRadius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
PIKPool::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKPool","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // pikSystem
