/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1FrontShield.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"

#include "M1FrontShield.h"


namespace xraySystem
{

M1FrontShield::M1FrontShield(const std::string& Key) :
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


M1FrontShield::~M1FrontShield()
  /*!
    Destructor
   */
{}

void
M1FrontShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1FrontShield","populate");

  FixedRotate::populate(Control);

  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseHeight=Control.EvalVar<double>(keyName+"BaseHeight");

  clubPlateThick=Control.EvalVar<double>(keyName+"ClubPlateThick");
  cubeThick=Control.EvalVar<double>(keyName+"CubeThick");
  cubeWidth=Control.EvalVar<double>(keyName+"CubeWidth");
  cubeHeight=Control.EvalVar<double>(keyName+"CubeHeight");
  cubeSideWall=Control.EvalVar<double>(keyName+"CubeSideWall");
  cubeBaseWall=Control.EvalVar<double>(keyName+"CubeBaseWall");

  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}


void
M1FrontShield::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1FrontShield","createSurfaces");

  makeShiftedSurf(SMap,"Front",buildIndex+12,Y,baseThick);
  makeShiftedSurf(SMap,"Base",buildIndex+4,X,baseWidth);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(baseWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(baseWidth/2.0),Z);

  return;
}

void
M1FrontShield::createObjects(Simulation& System)
  /*!
    Create the front shield of the M1 mirror 
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1FrontShield","createObjects");

  const HeadRule frontHR=getRule("Front");
  const HeadRule baseHR=getRule("Base");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -4 5 -6");
  makeCell("Plate",System,cellIndex++,mat,0.0,HR*frontHR*baseHR);

  addOuterSurf(HR*frontHR*baseHR);


  return;
}

void
M1FrontShield::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1FrontShield","createLinks");

  // link points are defined in the end of createSurfaces

  
  return;
}

void
M1FrontShield::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1FrontShield","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
