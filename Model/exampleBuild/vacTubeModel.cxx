/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuild/vacTubeModel.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include <boost/format.hpp>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"
#include "InnerZone.h"

#include "VacuumPipe.h"
#include "GateValveCylinder.h"
#include "ShieldRoom.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "insertPlate.h"

#include "vacTubeModel.h"

namespace exampleSystem
{

vacTubeModel::vacTubeModel(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(*this,cellIndex),
  
  shieldRoom(new ShieldRoom("LinacRoom")),
  pipeA(new constructSystem::VacuumPipe("PipeA")),
  gateA(new constructSystem::GateValveCylinder("GateA")),
  pipeB(new constructSystem::VacuumPipe("PipeB"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(shieldRoom);
  OR.addObject(gateA);
  OR.addObject(pipeA);
  OR.addObject(pipeB);
  //  OR.addObject(plate);
}

vacTubeModel::vacTubeModel(const vacTubeModel& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  buildZone(A.buildZone),shieldRoom(A.shieldRoom),
  pipeA(A.pipeA),gateA(A.gateA),pipeB(A.pipeB),
  boxWidth(A.boxWidth)
  /*!
    Copy constructor
    \param A :: vacTubeModel to copy
  */
{}

vacTubeModel&
vacTubeModel::operator=(const vacTubeModel& A)
  /*!
    Assignment operator
    \param A :: vacTubeModel to copy
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
      shieldRoom=A.shieldRoom;
      pipeA=A.pipeA;
      gateA=A.gateA;
      pipeB=A.pipeB;
      boxWidth=A.boxWidth;
    }
  return *this;
}

vacTubeModel::~vacTubeModel()
  /*!
    Destructor
   */
{}

void
vacTubeModel::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("vacTubeModel","createSurface");

  if (boxWidth>Geometry::zeroTol)
    {
      std::string Out;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(boxWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(boxWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(boxWidth/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(boxWidth/2.0),Z);

      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6");
      const HeadRule HR(Out);
      buildZone.setSurround(HR);
    }
  
  return;
}

void
vacTubeModel::createObjects(Simulation& System)
  /*!
    Build objects
   */
{
  ELog::RegMethod RegA("vacTubeModel","buildObjects");
  
  int outerCell;


  shieldRoom->addInsertCell(74123);
  shieldRoom->createAll(System,World::masterOrigin(),0);


  buildZone.setFront(shieldRoom->getSurfRule("Front"));
  buildZone.setBack(shieldRoom->getSurfRule("-Back"));

  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System,*this);

  pipeA->addInsertCell(shieldRoom->getCell("Void"));
  pipeA->createAll(System,*shieldRoom,0);

  gateA->addInsertCell(shieldRoom->getCell("Void"));
  gateA->createAll(System,*pipeA,2);

  pipeB->addInsertCell(shieldRoom->getCell("Void"));
  pipeB->createAll(System,*gateA,2);

  return;
}
  
void 
vacTubeModel::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC:: Fixed comp
  */
{
  // For output stream
  ELog::RegMethod RControl("vacTubeModel","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
   
  return;
}


}   // NAMESPACE exampleSystem

