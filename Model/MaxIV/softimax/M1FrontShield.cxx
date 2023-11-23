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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "M1FrontShield.h"


namespace xraySystem
{

M1FrontShield::M1FrontShield(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Main","Extra"),
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

  cubePlateThick=Control.EvalVar<double>(keyName+"CubePlateThick");
  cubeThick=Control.EvalVar<double>(keyName+"CubeThick");
  cubeWidth=Control.EvalVar<double>(keyName+"CubeWidth");
  cubeHeight=Control.EvalVar<double>(keyName+"CubeHeight");
  cubeSideWall=Control.EvalVar<double>(keyName+"CubeSideWall");
  cubeBaseWall=Control.EvalVar<double>(keyName+"CubeBaseWall");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  pipeYStep=Control.EvalVar<double>(keyName+"PipeYStep");
  pipeLength=Control.EvalVar<double>(keyName+"PipeLength");
  pipeRadius=Control.EvalVar<double>(keyName+"PipeRadius");
  pipeWall=Control.EvalVar<double>(keyName+"PipeWall");
  
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

  makeShiftedSurf(SMap,"Front",buildIndex+2,Y,cubeThick);
  makeShiftedSurf(SMap,"Base",buildIndex+4,X,-cubeWidth);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(cubeHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(cubeHeight/2.0),Z);

  //cube cuts
  makeShiftedSurf(SMap,"Front",buildIndex+101,Y,cubePlateThick);
  makeShiftedSurf(SMap,"Base",buildIndex+104,X,-cubeSideWall);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(cubeHeight/2.0-cubeBaseWall),Z);

  //cube insert
  makeShiftedSurf(SMap,"Front",buildIndex+201,Y,innerThick+cubePlateThick);
  makeShiftedSurf(SMap,"Base",buildIndex+204,X,
		  -(innerWidth+cubeSideWall));
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin-Z*(cubeHeight/2.0-cubeBaseWall-innerHeight),Z);

  // pipe

  ModelSupport::buildCylinder
    (SMap,buildIndex+307,Origin+Y*pipeYStep,X,pipeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin+Y*pipeYStep,X,pipeRadius+pipeWall);
  makeShiftedSurf(SMap,"Base",buildIndex+304,X,pipeLength);

  
  ModelSupport::buildCylinder
    (SMap,buildIndex+407,Origin-X*(cubeWidth/2.0)
     +Y*pipeYStep,Z,pipeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+417,Origin-X*(cubeWidth/2.0)
     +Y*pipeYStep,Z,pipeRadius+pipeWall);
  ModelSupport::buildPlane
    (SMap,buildIndex+405,Origin-Z*(cubeHeight/2.0+pipeLength),Z);

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
  const HeadRule baseCompHR=baseHR.complement();
  HeadRule HR;

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-2 4 5 -6 (104:-105:-101)");
  makeCell("Main",System,cellIndex++,mat,0.0,HR*frontHR*baseHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -2 105 -6 -104 4 (206:201:-204)");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  // inner space
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 105 -206 -104 204");
  makeCell("Inner",System,cellIndex++,mat,0.0,HR*frontHR*baseHR);

  // pipes
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-304 -307");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR*baseCompHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-304 -317 307");
  makeCell("Pipe",System,cellIndex++,mat,0.0,HR*baseCompHR);

  //  HR=ModelSupport::getHeadRule(SMap,buildIndex,"405 -6 -304  317 -2");
  //  makeCell("PipeVoid",System,cellIndex++,voidMat,0.0,HR*baseCompHR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"405 -5 -407");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"405 -5 -417 407");
  makeCell("Pipe",System,cellIndex++,mat,0.0,HR);
  
  //  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 -5 405 -2 417");
  //  makeCell("PipeVoid",System,cellIndex++,voidMat,0.0,HR*baseHR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 4 5 -6");
  addOuterSurf("Main",HR*frontHR*baseHR);
  addOuterSurf("Extra",HR*frontHR*baseHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-304 -317");
  addOuterUnionSurf("Extra",HR*baseCompHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"405 -5 -417");
  addOuterUnionSurf("Extra",HR);


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
