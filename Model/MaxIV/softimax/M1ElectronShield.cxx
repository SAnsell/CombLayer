/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1ElectronShield.cxx
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

#include "M1ElectronShield.h"

namespace xraySystem
{

M1ElectronShield::M1ElectronShield(const std::string& Key) :
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

M1ElectronShield::~M1ElectronShield()
  /*!
    Destructor
   */
{}

void
M1ElectronShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1ElectronShield","populate");

  FixedRotate::populate(Control);

  elecXOut=Control.EvalVar<double>(keyName+"ElecXOut");
  elecLength=Control.EvalVar<double>(keyName+"ElecLength");
  elecHeight=Control.EvalVar<double>(keyName+"ElecHeight");
  elecThick=Control.EvalVar<double>(keyName+"ElecThick");
  elecEdge=Control.EvalVar<double>(keyName+"ElecEdge");
  elecHoleRadius=Control.EvalVar<double>(keyName+"ElecHoleRadius");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  electronMat=ModelSupport::EvalMat<int>(Control,keyName+"ElectronMat");

  return;
}

void
M1ElectronShield::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1ElectronShield","createSurfaces");


  // ELECTRON shield
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+3,Origin+X*(elecXOut-elecThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*elecXOut,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(elecHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(elecHeight/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,X,elecHoleRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin+X*(elecXOut-elecEdge),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(elecHeight/2.0-elecThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(elecHeight/2.0-elecThick),Z);
  
  return;
}

  
void
M1ElectronShield::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1ElectronShield","createObjects");


  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -7");
  makeCell("EPlateHole",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 13 -4 5 -6 (-15:16:3) 7");
  makeCell("EPlate",System,cellIndex++,electronMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 13 -3 15 -16");
  makeCell("EPlate",System,cellIndex++,voidMat,0.0,HR);  

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -4 5 -6");
  addOuterSurf(HR);

  return;
}

void
M1ElectronShield::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1ElectronShield","createLinks");


  // link points are defined in the end of createSurfaces

  return;
}

void
M1ElectronShield::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1ElectronShield","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
