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
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PortTube.h"

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "FlangeMount.h"
#include "makeBalder.h"

namespace xraySystem
{

makeBalder::makeBalder() :
  opticsHut(new OpticsHutch("Optics")),
  ionPA(new constructSystem::CrossPipe("IonPA")),
  triggerPipe(new constructSystem::CrossPipe("TriggerPipe")),
  pipeA(new constructSystem::Bellows("BellowA")),
  filterBox(new constructSystem::PortTube("FilterBox")),
  pipeB(new constructSystem::Bellows("BellowB")),
  gateA(new constructSystem::GateValve("GateA")),
  mirrorBox(new constructSystem::VacuumBox("MirrorBox")),
  gateB(new constructSystem::GateValve("GateB")),
  pipeC(new constructSystem::Bellows("BellowC")),
  driftA(new constructSystem::VacuumPipe("DriftA")),
  
  driftB(new constructSystem::VacuumPipe("DriftB")),
  monoV(new xraySystem::MonoVessel("MonoVac")),
  monoXtal(new xraySystem::MonoCrystals("MonoXtal")),
  monoBellowA(new constructSystem::Bellows("MonoBellowA")),
  monoBellowB(new constructSystem::Bellows("MonoBellowB")),
  gateC(new constructSystem::GateValve("GateC")),
  driftC(new constructSystem::VacuumPipe("DriftC")),
  slitsA(new constructSystem::JawValve("SlitsA")),
  shieldPipe(new constructSystem::PortTube("ShieldPipe")),

  // JUNK
  pipeD(new constructSystem::VacuumPipe("BellowD")),
  pipeE(new constructSystem::VacuumPipe("BellowE")),
  ionPumpC(new constructSystem::CrossPipe("IonPumpC")),
  focusBox(new constructSystem::VacuumBox("FocusBox")),
  ionPumpD(new constructSystem::CrossPipe("IonPumpD")),
  pipeF(new constructSystem::VacuumPipe("BellowF")),
  driftD(new constructSystem::VacuumPipe("DriftD"))

  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // can this be in an initializer??
  for(size_t i=0;i<4;i++)
    {
      filters[i]=std::make_shared<xraySystem::FlangeMount>
	("Filter"+std::to_string(i));
      OR.addObject(filters[i]);
    }
  
  OR.addObject(opticsHut);
  OR.addObject(ionPA);
  OR.addObject(triggerPipe);
  OR.addObject(pipeA);
  OR.addObject(filterBox);
  OR.addObject(pipeB);
  OR.addObject(gateA);
  OR.addObject(mirrorBox);
  OR.addObject(gateB);
  OR.addObject(pipeC);
  OR.addObject(driftA);
  OR.addObject(driftB);
  OR.addObject(monoV);
  OR.addObject(monoXtal);
  OR.addObject(monoBellowA);
  OR.addObject(monoBellowB);
  OR.addObject(gateC);
  OR.addObject(pipeD);
  OR.addObject(driftC);
  OR.addObject(pipeE);
  OR.addObject(ionPumpC);
  OR.addObject(focusBox);
  OR.addObject(ionPumpD);
  OR.addObject(pipeF);
  OR.addObject(driftD);
  OR.addObject(slitsA);
  OR.addObject(shieldPipe);
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

  ionPA->addInsertCell(opticsHut->getCell("Void"));
  ionPA->createAll(System,*opticsHut,0);

  triggerPipe->addInsertCell(opticsHut->getCell("Void"));
  triggerPipe->setFront(*ionPA,2);
  triggerPipe->createAll(System,*ionPA,2);

  pipeA->addInsertCell(opticsHut->getCell("Void"));
  pipeA->setFront(*triggerPipe,2);
  pipeA->createAll(System,*triggerPipe,2);

  filterBox->addInsertCell(opticsHut->getCell("Void"));
  filterBox->setFront(*pipeA,2);
  filterBox->createAll(System,*pipeA,2);

  for(size_t i=0;i<4;i++)
    {
      const constructSystem::portItem& PI=filterBox->getPort(i);
      filters[i]->addInsertCell("Flange",opticsHut->getCell("Void"));
      filters[i]->addInsertCell("Body",PI.getCell("Void"));
      filters[i]->addInsertCell("Body",filterBox->getCell("Void"));
      filters[i]->setBladeCentre(PI,0);
      filters[i]->createAll(System,PI,2);
    }

  pipeB->addInsertCell(opticsHut->getCell("Void"));
  pipeB->setFront(*filterBox,2);
  pipeB->createAll(System,*filterBox,2);

  gateA->addInsertCell(opticsHut->getCell("Void"));
  gateA->setFront(*pipeB,2);
  gateA->createAll(System,*pipeB,2);

  mirrorBox->addInsertCell(opticsHut->getCell("Void"));
  mirrorBox->setFront(*gateA,2);
  mirrorBox->createAll(System,*gateA,2);

  gateB->addInsertCell(opticsHut->getCell("Void"));
  gateB->setFront(*mirrorBox,2);
  gateB->createAll(System,*mirrorBox,2);

  pipeC->addInsertCell(opticsHut->getCell("Void"));
  pipeC->setFront(*gateB,2);
  pipeC->createAll(System,*gateB,2);

  driftA->addInsertCell(opticsHut->getCell("Void"));
  driftA->setFront(*pipeC,2);
  driftA->createAll(System,*pipeC,2);

  driftB->addInsertCell(opticsHut->getCell("Void"));
  driftB->createAll(System,*driftA,2);
  
  monoV->addInsertCell(opticsHut->getCell("Void"));
  monoV->createAll(System,*driftA,2);

  monoXtal->addInsertCell(monoV->getCell("Void"));
  monoXtal->createAll(System,*monoV,0);

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

  gateC->addInsertCell(opticsHut->getCell("Void"));
  gateC->setFront(*driftB,2);
  gateC->createAll(System,*driftB,2);

  driftC->addInsertCell(opticsHut->getCell("Void"));
  driftC->setFront(*gateC,2);
  driftC->createAll(System,*gateC,2);

  slitsA->addInsertCell(opticsHut->getCell("Void"));
  slitsA->setFront(*driftC,2);
  slitsA->createAll(System,*driftC,2);

  
  shieldPipe->addInsertCell(opticsHut->getCell("Void"));
  shieldPipe->setFront(*slitsA,2);
  shieldPipe->createAll(System,*slitsA,2);

  return;
  
  pipeD->addInsertCell(opticsHut->getCell("Void"));
  pipeD->setFront(*gateC,2);
  pipeD->createAll(System,*gateC,2);





  pipeE->addInsertCell(opticsHut->getCell("Void"));
  pipeE->setFront(*driftC,2);
  pipeE->createAll(System,*driftC,2);

  ionPumpC->addInsertCell(opticsHut->getCell("Void"));
  ionPumpC->setFront(*pipeE,2);
  ionPumpC->createAll(System,*pipeE,2);
  
  focusBox->addInsertCell(opticsHut->getCell("Void"));
  focusBox->setFront(*ionPumpC,2);
  focusBox->createAll(System,*ionPumpC,2);

  ionPumpD->addInsertCell(opticsHut->getCell("Void"));
  ionPumpD->setFront(*focusBox,2);
  ionPumpD->createAll(System,*focusBox,2);

  pipeF->addInsertCell(opticsHut->getCell("Void"));
  pipeF->setFront(*ionPumpD,2);
  pipeF->createAll(System,*ionPumpD,2);

  driftD->addInsertCell(opticsHut->getCell("Void"));
  driftD->setFront(*pipeF,2);
  driftD->createAll(System,*pipeF,2);


  return;
}


}   // NAMESPACE xraySystem

