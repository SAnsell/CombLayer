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
#include "VacuumPipe.h"
#include "GateValveCylinder.h"
#include "ShieldRoom.h"

#include "vacTubeModel.h"

namespace exampleSystem
{

vacTubeModel::vacTubeModel() :
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
}

vacTubeModel::~vacTubeModel()
  /*!
    Destructor
   */
{}

void 
vacTubeModel::build(Simulation& System)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("vacTubeModel","build");

  int voidCell(74123);

  shieldRoom->addInsertCell(voidCell);
  shieldRoom->createAll(System,World::masterOrigin(),0);
  
  //  pipeA->addInsertCell(shieldRoom->getCell("Void"));
  //  pipeA->createAll(System,*shieldRoom,0);

  gateA->addInsertCell(shieldRoom->getCell("Void"));
  gateA->createAll(System,*shieldRoom,0);

  //  pipeB->addInsertCell(shieldRoom->getCell("Void"));
  //  pipeB->createAll(System,*gateA,2);


  return;
}


}   // NAMESPACE exampleSystem

