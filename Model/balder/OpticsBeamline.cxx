/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/OpticsBeamline.cxx
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
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
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
#include "Mirror.h"
#include "OpticsBeamline.h"

namespace xraySystem
{

// Note currently uncopied:
  
OpticsBeamline::OpticsBeamline(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  
  pipeInit(new constructSystem::VacuumPipe(newName+"InitPipe")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  triggerPipe(new constructSystem::CrossPipe(newName+"TriggerPipe")),
  pipeA(new constructSystem::Bellows(newName+"BellowA")),
  filterBox(new constructSystem::PortTube(newName+"FilterBox")),
  pipeB(new constructSystem::Bellows(newName+"BellowB")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  mirrorBox(new constructSystem::VacuumBox(newName+"MirrorBox")),
  mirror(new xraySystem::Mirror(newName+"Mirror")),
  gateB(new constructSystem::GateValve(newName+"GateB")),
  pipeC(new constructSystem::Bellows(newName+"BellowC")),
  driftA(new constructSystem::VacuumPipe(newName+"DriftA")),
  
  driftB(new constructSystem::VacuumPipe(newName+"DriftB")),
  monoV(new xraySystem::MonoVessel(newName+"MonoVac")),
  monoXtal(new xraySystem::MonoCrystals(newName+"MonoXtal")),
  monoBellowA(new constructSystem::Bellows(newName+"MonoBellowA")),
  monoBellowB(new constructSystem::Bellows(newName+"MonoBellowB")),
  gateC(new constructSystem::GateValve(newName+"GateC")),
  driftC(new constructSystem::VacuumPipe(newName+"DriftC")),
  slitsA(new constructSystem::JawValve(newName+"SlitsA")),
  shieldPipe(new constructSystem::PortTube(newName+"ShieldPipe")),
  pipeD(new constructSystem::Bellows(newName+"BellowD")),
  gateD(new constructSystem::GateValve(newName+"GateD")),
  mirrorBoxB(new constructSystem::VacuumBox(newName+"MirrorBoxB")),
  mirrorB(new xraySystem::Mirror(newName+"MirrorB")),
  pipeE(new constructSystem::Bellows(newName+"BellowE")),
  slitsB(new constructSystem::JawValve(newName+"SlitsB")),
  viewPipe(new constructSystem::PortTube(newName+"ViewTube")),
  pipeF(new constructSystem::Bellows(newName+"BellowF")),
  shutterPipe(new constructSystem::CrossPipe(newName+"ShutterPipe")),
  pipeG(new constructSystem::Bellows(newName+"BellowG")),
  gateE(new constructSystem::GateValve(newName+"GateE"))
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // can this be in an initializer??
  for(size_t i=0;i<4;i++)
    {
      filters[i]=std::make_shared<xraySystem::FlangeMount>
	(newName+"Filter"+std::to_string(i));
      OR.addObject(filters[i]);
    }
  
  OR.addObject(pipeInit);
  OR.addObject(ionPA);
  OR.addObject(triggerPipe);
  OR.addObject(pipeA);
  OR.addObject(filterBox);
  OR.addObject(pipeB);
  OR.addObject(gateA);
  OR.addObject(mirrorBox);
  OR.addObject(mirror);
  OR.addObject(gateB);
  OR.addObject(pipeC);
  OR.addObject(driftA);
  OR.addObject(driftB);
  OR.addObject(monoV);
  OR.addObject(monoXtal);
  OR.addObject(monoBellowA);
  OR.addObject(monoBellowB);
  OR.addObject(gateC);
  OR.addObject(driftC);
  OR.addObject(slitsA);
  OR.addObject(shieldPipe);
  OR.addObject(pipeD);
  OR.addObject(gateD);
  OR.addObject(mirrorBoxB);
  OR.addObject(mirrorB);
  OR.addObject(pipeE);
  OR.addObject(slitsB);
  OR.addObject(viewPipe);
  OR.addObject(pipeF);
}
  
OpticsBeamline::~OpticsBeamline()
  /*!
    Destructor
   */
{}

void
OpticsBeamline::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);
  return;
}

void
OpticsBeamline::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
/*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("OpticsBeamline","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}


void
OpticsBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("OpticsBeamline","buildObjects");

  pipeInit->addInsertCell(ContainedComp::getInsertCells());
  pipeInit->createAll(System,*this,0);

  ionPA->addInsertCell(ContainedComp::getInsertCells());
  ionPA->setFront(*pipeInit,2);
  ionPA->createAll(System,*pipeInit,2);

  triggerPipe->addInsertCell(ContainedComp::getInsertCells());
  triggerPipe->setFront(*ionPA,2);
  triggerPipe->createAll(System,*ionPA,2);

  pipeA->addInsertCell(ContainedComp::getInsertCells());
  pipeA->setFront(*triggerPipe,2);
  pipeA->createAll(System,*triggerPipe,2);

  filterBox->addInsertCell(ContainedComp::getInsertCells());
  filterBox->setFront(*pipeA,2);
  filterBox->createAll(System,*pipeA,2);

  for(size_t i=0;i<4;i++)
    {
      const constructSystem::portItem& PI=filterBox->getPort(i);
      filters[i]->addInsertCell("Flange",ContainedComp::getInsertCells());
      filters[i]->addInsertCell("Body",PI.getCell("Void"));
      filters[i]->addInsertCell("Body",filterBox->getCell("Void"));
      filters[i]->setBladeCentre(PI,0);
      filters[i]->createAll(System,PI,2);
    }

  pipeB->addInsertCell(ContainedComp::getInsertCells());
  pipeB->setFront(*filterBox,2);
  pipeB->createAll(System,*filterBox,2);

  gateA->addInsertCell(ContainedComp::getInsertCells());
  gateA->setFront(*pipeB,2);
  gateA->createAll(System,*pipeB,2);

  mirrorBox->addInsertCell(ContainedComp::getInsertCells());
  mirrorBox->setFront(*gateA,2);
  mirrorBox->createAll(System,*gateA,2);

  mirror->addInsertCell(mirrorBox->getCell("Void"));
  mirror->createAll(System,*mirrorBox,0);

  gateB->addInsertCell(ContainedComp::getInsertCells());
  gateB->setFront(*mirrorBox,2);
  gateB->createAll(System,*mirrorBox,2);

  pipeC->addInsertCell(ContainedComp::getInsertCells());
  pipeC->setFront(*gateB,2);
  pipeC->createAll(System,*gateB,2);

  driftA->addInsertCell(ContainedComp::getInsertCells());
  driftA->setFront(*pipeC,2);
  driftA->createAll(System,*pipeC,2);

  driftB->addInsertCell(ContainedComp::getInsertCells());
  driftB->createAll(System,*driftA,2);
  
  monoV->addInsertCell(ContainedComp::getInsertCells());
  monoV->createAll(System,*driftA,2);

  monoXtal->addInsertCell(monoV->getCell("Void"));
  monoXtal->createAll(System,*monoV,0);

  // Note : join flag so can rotate on front/back
  monoBellowA->addInsertCell(ContainedComp::getInsertCells());
  monoBellowA->setFront(*driftA,2,1);
  monoBellowA->setBack(*monoV,1,1);
  monoBellowA->createAll(System,*driftA,2);

  // Note : join flag so can rotate on front/back
  monoBellowB->addInsertCell(ContainedComp::getInsertCells());
  monoBellowB->setFront(*monoV,2,1); 
  monoBellowB->setBack(*driftB,1,1); 
  monoBellowB->createAll(System,*driftB,-1);

  gateC->addInsertCell(ContainedComp::getInsertCells());
  gateC->setFront(*driftB,2);
  gateC->createAll(System,*driftB,2);

  driftC->addInsertCell(ContainedComp::getInsertCells());
  driftC->setFront(*gateC,2);
  driftC->createAll(System,*gateC,2);

  slitsA->addInsertCell(ContainedComp::getInsertCells());
  slitsA->setFront(*driftC,2);
  slitsA->createAll(System,*driftC,2);

  
  shieldPipe->addInsertCell(ContainedComp::getInsertCells());
  shieldPipe->setFront(*slitsA,2);
  shieldPipe->createAll(System,*slitsA,2);

  pipeD->addInsertCell(ContainedComp::getInsertCells());
  pipeD->setFront(*shieldPipe,2);
  pipeD->createAll(System,*shieldPipe,2);

  gateD->addInsertCell(ContainedComp::getInsertCells());
  gateD->setFront(*pipeD,2);
  gateD->createAll(System,*pipeD,2);

  mirrorBoxB->addInsertCell(ContainedComp::getInsertCells());
  mirrorBoxB->setFront(*gateD,2);
  mirrorBoxB->createAll(System,*gateD,2);

  mirrorB->addInsertCell(mirrorBoxB->getCell("Void"));
  mirrorB->createAll(System,*mirrorBoxB,0);

  pipeE->addInsertCell(ContainedComp::getInsertCells());
  pipeE->setFront(*mirrorBoxB,2);
  pipeE->createAll(System,*mirrorBoxB,2);

  slitsB->addInsertCell(ContainedComp::getInsertCells());
  slitsB->setFront(*pipeE,2);
  slitsB->createAll(System,*pipeE,2);

  viewPipe->addInsertCell(ContainedComp::getInsertCells());
  viewPipe->setFront(*slitsB,2);
  viewPipe->createAll(System,*slitsB,2);

  pipeF->addInsertCell(ContainedComp::getInsertCells());
  pipeF->setFront(*viewPipe,2);
  pipeF->createAll(System,*viewPipe,2);

  shutterPipe->addInsertCell(ContainedComp::getInsertCells());
  shutterPipe->setFront(*pipeF,2);
  shutterPipe->createAll(System,*pipeF,2);

  pipeG->addInsertCell(ContainedComp::getInsertCells());
  pipeG->setFront(*shutterPipe,2);
  pipeG->createAll(System,*shutterPipe,2);

  gateE->addInsertCell(ContainedComp::getInsertCells());
  gateE->setFront(*pipeG,2);
  gateE->createAll(System,*pipeG,2);

  return;
}

void
OpticsBeamline::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeInit,1);
  setLinkSignedCopy(1,*gateE,2);
  return;
}
  
  
void 
OpticsBeamline::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("OpticsBeamline","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

