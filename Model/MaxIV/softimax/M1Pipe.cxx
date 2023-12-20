/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Pipe.cxx
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "Importance.h"
#include "Object.h"

#include "M1Pipe.h"

namespace xraySystem
{

M1Pipe::M1Pipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Join","Main"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1Pipe::~M1Pipe()
  /*!
    Destructor
   */
{}

void
M1Pipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Pipe","populate");

  FixedRotate::populate(Control);

  connectRadius=Control.EvalVar<double>(keyName+"ConnectRadius");
  connectLength=Control.EvalVar<double>(keyName+"ConnectLength");

  cubeWidth=Control.EvalVar<double>(keyName+"CubeWidth");
  cubeHeight=Control.EvalVar<double>(keyName+"CubeHeight");
  cubeDepth=Control.EvalVar<double>(keyName+"CubeDepth");

  outRadius=Control.EvalVar<double>(keyName+"OutRadius");
  outLength=Control.EvalVar<double>(keyName+"OutLength");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
M1Pipe::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Pipe","createSurfaces");

  // Origin is pipe connection point:

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,connectRadius);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*connectLength,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(cubeDepth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(cubeDepth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(cubeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(cubeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(connectLength+cubeHeight),Z);
  
  
  return;
}

  
void
M1Pipe::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Pipe","createObjects");

  const HeadRule cylHR=getRule("TubeRadius");
  const HeadRule baseHR=getRule("MirrorBase");
  ELog::EM<<"Base == "<<baseHR<<ELog::endDiag;

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -7");
  makeCell("Connect",System,cellIndex++,pipeMat,0.0,HR*baseHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 7 5");
  makeCell("Connect",System,cellIndex++,voidMat,0.0,HR*baseHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 -5 105");
  makeCell("Cube",System,cellIndex++,pipeMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 105");
  addOuterSurf("Join",HR*baseHR);
  
  return;
}

void
M1Pipe::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Pipe","createLinks");


  return;
}

void
M1Pipe::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Pipe","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  ELog::EM<<"LC Point == "<<FC.getLinkPt(sideIndex)<<ELog::endDiag;
  ELog::EM<<"Origin == "<<Origin<<ELog::endDiag;
  ELog::EM<<"Y == "<<Y<<ELog::endDiag;
  ELog::EM<<"Z == "<<Z<<ELog::endDiag;
      
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
