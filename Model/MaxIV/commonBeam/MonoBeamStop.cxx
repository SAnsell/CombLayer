/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/MonoBeamStop.cxx
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

#include "MonoBeamStop.h"

namespace xraySystem
{

MonoBeamStop::MonoBeamStop(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,3),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MonoBeamStop::~MonoBeamStop()
  /*!
    Destructor
  */
{}

void
MonoBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MonoBeamStop","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
MonoBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MonoBeamStop","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  return;
}

void
MonoBeamStop::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MonoBeamStop","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7 ");
  makeCell("Main",System,cellIndex++,mat,0.0,HR);

  addOuterSurf(HR);
  return;
}


void
MonoBeamStop::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("MonoBeamStop","createLinks");

  setConnect(0,Origin-Y*(length/2.0),-Y);
  setConnect(1,Origin+Y*(length/2.0),Y);
  
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  return;
}

void
MonoBeamStop::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MonoBeamStop","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
