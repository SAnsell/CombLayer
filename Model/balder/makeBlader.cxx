/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/makeBalder.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"

#include "VacuumPipe.h"
#include "VacuumBox.h"

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "MonoVessel.h"
#include "makeBalder.h"

namespace xraySystem
{

makeBalder::makeBalder() :
  opticsHut(new OpticsHutch("Optics")),
  triggerPipe(new constructSystem::CrossPipe("TriggerPipe")),
  pipeA(new constructSystem::VacuumPipe("BellowA")),
  filterBox(new constructSystem::VacuumBox("FilterBox")),
  pipeB(new constructSystem::VacuumPipe("BellowB")),
  ionPumpA(new constructSystem::CrossPipe("IonPumpA")),
  mirrorBox(new constructSystem::VacuumBox("MirrorBox")),
  ionPumpB(new constructSystem::CrossPipe("IonPumpB")),
  pipeC(new constructSystem::VacuumPipe("BellowC")),
  driftA(new constructSystem::VacuumPipe("DriftA")),
  driftB(new constructSystem::VacuumPipe("DriftB")),
  monoV(new xraySystem::MonoVessel("MonoVac")),
  monoBellowA(new constructSystem::VacuumPipe("MonoBellowA")),
  monoBellowB(new constructSystem::VacuumPipe("MonoBellowB"))

  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(opticsHut);
  OR.addObject(triggerPipe);
  OR.addObject(pipeA);
  OR.addObject(filterBox);
  OR.addObject(pipeB);
  OR.addObject(ionPumpA);
  OR.addObject(mirrorBox);
  OR.addObject(ionPumpB);
  OR.addObject(pipeC);
  OR.addObject(driftA);
  OR.addObject(driftB);
  OR.addObject(monoV);
  OR.addObject(monoBellowA);
  OR.addObject(monoBellowB);

}

makeBalder::~makeBalder()
  /*!
    Destructor
   */
{}

void 
makeBalder::build(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeBalder","build");

  int voidCell(74123);
 
  opticsHut->addInsertCell(voidCell);
  opticsHut->createAll(System,World::masterOrigin(),0);

  triggerPipe->addInsertCell(opticsHut->getCell("Void"));
  triggerPipe->createAll(System,*opticsHut,0);

  pipeA->addInsertCell(opticsHut->getCell("Void"));
  pipeA->setFront(*triggerPipe,2);
  pipeA->createAll(System,*triggerPipe,2);

  filterBox->addInsertCell(opticsHut->getCell("Void"));
  filterBox->setFront(*pipeA,2);
  filterBox->createAll(System,*pipeA,2);

  pipeB->addInsertCell(opticsHut->getCell("Void"));
  pipeB->setFront(*filterBox,2);
  pipeB->createAll(System,*filterBox,2);

  ionPumpA->addInsertCell(opticsHut->getCell("Void"));
  ionPumpA->setFront(*pipeB,2);
  ionPumpA->createAll(System,*pipeB,2);

  mirrorBox->addInsertCell(opticsHut->getCell("Void"));
  mirrorBox->setFront(*ionPumpA,2);
  mirrorBox->createAll(System,*ionPumpA,2);

  ionPumpB->addInsertCell(opticsHut->getCell("Void"));
  ionPumpB->setFront(*mirrorBox,2);
  ionPumpB->createAll(System,*mirrorBox,2);

  pipeC->addInsertCell(opticsHut->getCell("Void"));
  pipeC->setFront(*ionPumpB,2);
  pipeC->createAll(System,*ionPumpB,2);

  driftA->addInsertCell(opticsHut->getCell("Void"));
  driftA->setFront(*pipeC,2);
  driftA->createAll(System,*pipeC,2);

  driftB->addInsertCell(opticsHut->getCell("Void"));
  driftB->createAll(System,*driftA,2);
  
  monoV->addInsertCell(opticsHut->getCell("Void"));
  monoV->createAll(System,*driftA,2);

  // Note : join flag so can rotate on front/back
  monoBellowA->addInsertCell(opticsHut->getCell("Void"));
  monoBellowA->setFront(*driftA,2,1);
  monoBellowA->setBack(*monoV,1,1);
  monoBellowA->createAll(System,*driftA,2);

  // Note : join flag so can rotate on front/back
  monoBellowB->addInsertCell(opticsHut->getCell("Void"));
  monoBellowB->setFront(*monoV,2,1); 
  monoBellowB->setBack(*driftB,1,1); 
  monoBellowB->createAll(System,*driftB,-1);
  
  return;
}


}   // NAMESPACE xraySystem

