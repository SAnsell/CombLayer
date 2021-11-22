/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuild/vacTube.cxx
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "GateValveCylinder.h"
#include "vacTube.h"

namespace exampleSystem
{

vacTube::vacTube(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(keyName+"BlockZone"),

  pipeA(new constructSystem::VacuumPipe("PipeA")),
  gateA(new constructSystem::GateValveCylinder("GateA")),
  pipeB(new constructSystem::VacuumPipe("PipeB"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gateA);
  OR.addObject(pipeA);
  OR.addObject(pipeB);
  //  OR.addObject(plate);
}

vacTube::vacTube(const vacTube& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  buildZone(A.buildZone),
  pipeA(A.pipeA),gateA(A.gateA),pipeB(A.pipeB),
  boxWidth(A.boxWidth)
  /*!
    Copy constructor
    \param A :: vacTube to copy
  */
{}

vacTube&
vacTube::operator=(const vacTube& A)
  /*!
    Assignment operator
    \param A :: vacTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      buildZone=A.buildZone;

      pipeA=A.pipeA;
      gateA=A.gateA;
      pipeB=A.pipeB;
      boxWidth=A.boxWidth;
    }
  return *this;
}

vacTube::~vacTube()
  /*!
    Destructor
  */
{}

void
vacTube::populate(const FuncDataBase& Control)
{
  FixedOffset::populate(Control);
  boxWidth=Control.EvalVar<double>(keyName+"BoxWidth");
  return;
}
  
void
vacTube::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("vacTube","createSurface");

  if (boxWidth>Geometry::zeroTol)
    {
      HeadRule HR;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(boxWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(boxWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(boxWidth/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(boxWidth/2.0),Z);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
      buildZone.setSurround(HR);

      buildZone.setFront(getRule("front"));
      buildZone.setMaxExtent(getRule("back"));

    }
  
  return;
}

void
vacTube::createObjects(Simulation& System)
  /*!
    Build objects
   */
{
  ELog::RegMethod RegA("vacTube","buildObjects");
  
  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());

  pipeA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*pipeA,"back");
  pipeA->insertAllInCell(System,outerCell);
  
  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*pipeB);

  /*
  gateA->addInsertCell(shieldRoom->getCell("Void"));
  gateA->createAll(System,*pipeA,2);

  pipeB->addInsertCell(shieldRoom->getCell("Void"));
  pipeB->createAll(System,*gateA,2);
  */
  return;
}
  
void 
vacTube::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC:: Fixed comp
    \param sideIndex :: line point
  */
{
  // For output stream
  ELog::RegMethod RControl("vacTube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
   
  return;
}


}   // NAMESPACE exampleSystem

