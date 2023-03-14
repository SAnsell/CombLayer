/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Support.cxx
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
#include "surfRegister.h"
#include "BaseVisit.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "M1Support.h"

namespace xraySystem
{

M1Support::M1Support(const std::string& Key) :
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

M1Support::~M1Support()
  /*!
    Destructor
   */
{}

void
M1Support::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Support","populate");

  FixedRotate::populate(Control);
  
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  edge=Control.EvalVar<double>(keyName+"Edge");
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");

  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  

  return;
}

void
M1Support::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Support","createSurfaces");

  if (!isActive("Top"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin,Z);
      ExternalCut::setCutSurf("Top",SMap.realSurf(buildIndex+6));
    }

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*thick,Z);


  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(width/2.0-thick),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(width/2.0-thick),X);
  
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*edge,Z);
  
  
  return;
}

void
M1Support::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Support","createObjects");

  const HeadRule topHR=getRule("Top");
  HeadRule HR;
  // xstal
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5");
  makeCell("Plate",System,cellIndex++,supportMat,0.0,HR*topHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -103 -5 105");
  makeCell("Edge",System,cellIndex++,supportMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 104 -4 -5 105");
  makeCell("Edge",System,cellIndex++,supportMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 103 -1044 -5 105");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 105");
  addOuterSurf(HR*topHR);

  return;
}

void
M1Support::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Support","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
M1Support::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Support","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
