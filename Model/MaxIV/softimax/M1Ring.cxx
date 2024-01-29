/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Ring.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "Exception.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "Importance.h"
#include "Object.h"

#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "M1Ring.h"

namespace xraySystem
{

M1Ring::M1Ring(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1Ring::~M1Ring()
  /*!
    Destructor
   */
{}

void
M1Ring::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Ring","populate");

  FixedRotate::populate(Control);

  outerThick=Control.EvalVar<double>(keyName+"OuterThick");
  outerLength=Control.EvalVar<double>(keyName+"OuterLength");
  innerYStep=Control.EvalVar<double>(keyName+"InnerYStep");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");

  ringMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
M1Ring::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Ring","createSurfaces");

  const HeadRule cylHR=getRule("TubeRadius");
  makeExpandedSurf(SMap,"TubeRadius",buildIndex+7,Y,-outerThick);
  makeExpandedSurf(SMap,"TubeRadius",buildIndex+17,Y,-innerThick);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(outerLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(outerLength/2.0),Y);
  const Geometry::Vec3D IOrg(Origin+Y*innerYStep);
  ModelSupport::buildPlane(SMap,buildIndex+11,IOrg-Y*(innerLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,IOrg+Y*(innerLength/2.0),Y);
  
  return;
}

  
void
M1Ring::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Ring","createObjects");

  const HeadRule cylHR=getRule("TubeRadius");
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 7");
  makeCell("Outer",System,cellIndex++,ringMat,0.0,HR*cylHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 17 -7");
  makeCell("Inner",System,cellIndex++,ringMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 7");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*cylHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 7");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*cylHR);

  // Create inner units:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 17");
  addOuterSurf(HR*cylHR);
  
  return;
}

void
M1Ring::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Ring","createLinks");

  
  const Geometry::Vec3D IOrg(Origin+Y*innerYStep);

  FixedComp::setConnect(0,IOrg-Y*(innerLength/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  
  FixedComp::setConnect(1,IOrg+Y*(innerLength/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  // link points are defined in the end of createSurfaces

  FixedComp::setConnect(2,Origin,Y);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+17));
  // link points are defined in the end of createSurfaces

  
  nameSideIndex(2,"innerRing");

  const HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12");
  setCutSurf("RingGap",HR);

  return;
}

void
M1Ring::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Ring","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
