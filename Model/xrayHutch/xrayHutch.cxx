/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayHutch/xrayHutch.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "xrayHutch.h"

namespace xrayHutSystem
{

xrayHutch::xrayHutch(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

xrayHutch::~xrayHutch()
  /*!
    Destructor
  */
{}
  

void
xrayHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("xrayHutch","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
xrayHutch::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("xrayHutch","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);  

  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin-Y*(length/2.0+wallThick),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(length/2.0+wallThick),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  return; 
}

void
xrayHutch::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("xrayHutch","createObjects");

  HeadRule HR;
  // Inner 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 15 -16");
  addOuterSurf(HR);
  return; 
}

void
xrayHutch::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("xrayHutch","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(1,Origin+Y*(length/2.0+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  
  FixedComp::setConnect(2,Origin-X*(length/2.0+wallThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+14));
  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+16));

  return;
}
  
void
xrayHutch::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("xrayHutch","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xrayHutSystem
