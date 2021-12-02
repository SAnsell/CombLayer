/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/AreaDetector.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "ExternalCut.h"

#include "AreaDetector.h"

namespace xraySystem
{

AreaDetector::AreaDetector(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

AreaDetector::~AreaDetector()
  /*!
    Destructor
  */
{}

void
AreaDetector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("AreaDetector","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  screenThick=Control.EvalVar<double>(keyName+"ScreenThick");
  screenDepth=Control.EvalVar<double>(keyName+"ScreenDepth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  detMat=ModelSupport::EvalMat<int>(Control,keyName+"DetMat");
  screenMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
AreaDetector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("AreaDetector","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*screenDepth,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length-screenDepth),Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*screenThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+length-screenDepth),Y);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);

  return;
}

void
AreaDetector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("AreaDetector","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -2 3 -4 5 -6");
  makeCell("Detector",System,cellIndex++,detMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 3 -4 5 -6");
  makeCell("Screen",System,cellIndex++,screenMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 3 -4 5 -6");
  makeCell("Front",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "1 13 -14 15 -16 -12 (2:-3:4:-5:6)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -12 13 -14 15 -16");
  addOuterSurf(HR);

  return;
}


void
AreaDetector::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("AreaDetector","createLinks");

  FixedComp::setConnect(0,Origin-Y*screenDepth,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(wallThick+length-screenDepth),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(2,Origin-X*(wallThick+width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,Origin+X*(wallThick+width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,Origin-Z*(wallThick+height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(wallThick+height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void
AreaDetector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("AreaDetector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
