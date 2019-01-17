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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ExternalCut.h"
#include "InnerZone.h"
#include "generateSurf.h"
#include "ModelSupport.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

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
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
  pipeInit(new constructSystem::VacuumPipe(newName+"InitPipe")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  triggerPipe(new constructSystem::CrossPipe(newName+"TriggerPipe")),
  pipeA(new constructSystem::Bellows(newName+"BellowA")),
  filterBox(new constructSystem::PortTube(newName+"FilterBox")),
  filters({
	std::make_shared<xraySystem::FlangeMount>(newName+"Filter0"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter1"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter2"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter3")}),

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
  beamStop(new insertSystem::insertPlate(newName+"BeamStop")),
  slitsA(new constructSystem::JawValve(newName+"SlitsA")),
  shieldPipe(new constructSystem::PortTube(newName+"ShieldPipe")),
  pipeD(new constructSystem::Bellows(newName+"BellowD")),
  gateD(new constructSystem::GateValve(newName+"GateD")),
  mirrorBoxB(new constructSystem::VacuumBox(newName+"MirrorBoxB")),
  mirrorB(new xraySystem::Mirror(newName+"MirrorB")),
  pipeE(new constructSystem::Bellows(newName+"BellowE")),
  slitsB(new constructSystem::JawValve(newName+"SlitsB")),
  viewPipe(new constructSystem::PortTube(newName+"ViewTube")),
  viewMount({
      std::make_shared<xraySystem::FlangeMount>(newName+"ViewMount0")
	}),
  pipeF(new constructSystem::Bellows(newName+"BellowF")),
  shutterPipe(new constructSystem::CrossPipe(newName+"ShutterPipe")),
  monoShutter(new xraySystem::FlangeMount(newName+"MonoShutter")),
  pipeG(new constructSystem::Bellows(newName+"BellowG")),
  gateE(new constructSystem::GateValve(newName+"GateE")),
  neutShield({
      std::make_shared<xraySystem::PipeShield>(newName+"NShield0"),
      std::make_shared<xraySystem::PipeShield>(newName+"NShield1"),
      std::make_shared<xraySystem::PipeShield>(newName+"NShield2"),
      std::make_shared<xraySystem::PipeShield>(newName+"NShield3")
	})
  
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(const auto& FM : filters)
    OR.addObject(FM);
  for(const auto& FM : viewMount)
    OR.addObject(FM);
  for(const auto& FM : neutShield)
    OR.addObject(FM);
  
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
  OR.addObject(beamStop);
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
  OR.addObject(shutterPipe);
  OR.addObject(monoShutter);
  OR.addObject(pipeG);
  OR.addObject(gateE);
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
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("OpticsBeamline","populate");
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
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
OpticsBeamline::createSurfaces()
  /*!
    Create surfaces for outer void
   */
{
  ELog::RegMethod RegA("OpticsBeamLine","createSurface");
  
  if (outerLeft>Geometry::zeroTol &&  isActive("floor"))
    {
      std::string Out;
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("floor"));
      buildZone.setSurround(HR);
    }
  return;
}

void
OpticsBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("OpticsBeamline","buildObjects");

  
  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));  
  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System,*this);

  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  // dump cell for joinPipe
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,-1);
  // real cell for initPipe
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);

    
  ionPA->setFront(*pipeInit,2);
  ionPA->createAll(System,*pipeInit,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPA,2);
  ionPA->insertInCell(System,outerCell);
  
  
  triggerPipe->setFront(*ionPA,2);
  triggerPipe->createAll(System,*ionPA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*triggerPipe,2);
  triggerPipe->insertInCell(System,outerCell);

  pipeA->setFront(*triggerPipe,2);
  pipeA->createAll(System,*triggerPipe,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  
  // fake inser for ports
  filterBox->addAllInsertCell(masterCell->getName());
  filterBox->setFront(*pipeA,2);
  filterBox->createAll(System,*pipeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*filterBox,2);
  filterBox->insertAllInCell(System,outerCell);



  // split on both inner void 
  filterBox->splitVoidPorts(System,"SplitVoid",1001,
			    filterBox->getCell("Void"),
			    Geometry::Vec3D(0,1,0));
  // split on outer  boid
  //  filterBox->getBuildCell(),
			      
  filterBox->splitVoidPorts(System,"SplitOuter",2001,
			    outerCell,
   			    Geometry::Vec3D(0,1,0));
  filterBox->splitObject(System,-11,filterBox->getCell("SplitOuter",0));
  filterBox->splitObject(System,12,filterBox->getCell("SplitOuter",3));
  // increase cellIndex because 4 ports + two outer -1
  cellIndex+=5;

  for(size_t i=0;i<filters.size();i++)
    {
      const constructSystem::portItem& PI=filterBox->getPort(i);
      filters[i]->addInsertCell("Flange",filterBox->getCell("SplitOuter",i));
      filters[i]->addInsertCell("Body",PI.getCell("Void"));
      filters[i]->addInsertCell("Body",filterBox->getCell("SplitVoid",i));
      filters[i]->setBladeCentre(PI,0);
      filters[i]->createAll(System,PI,2);
    }
  
  
  pipeB->setFront(*filterBox,2);
  pipeB->createAll(System,*filterBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertInCell(System,outerCell);  

  gateA->setFront(*pipeB,2);
  gateA->createAll(System,*pipeB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  mirrorBox->setFront(*gateA,2);
  mirrorBox->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorBox,2);
  mirrorBox->insertInCell(System,outerCell);
  mirrorBox->setCell("OuterVoid",outerCell);
  
  mirrorBox->splitObject(System,-11,outerCell);
  mirrorBox->splitObject(System,12,outerCell);
  cellIndex+=2;

  mirror->addInsertCell(mirrorBox->getCell("Void"));
  mirror->createAll(System,*mirrorBox,0);


  gateB->setFront(*mirrorBox,2);
  gateB->createAll(System,*mirrorBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateB,2);
  gateB->insertInCell(System,outerCell);
    
  pipeC->setFront(*gateB,2);
  pipeC->createAll(System,*gateB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  driftA->setFront(*pipeC,2);
  driftA->createAll(System,*pipeC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*driftA,2);
  driftA->insertInCell(System,outerCell);
  
  driftB->createAll(System,*driftA,2);
  monoV->createAll(System,*driftA,2);
  monoXtal->addInsertCell(monoV->getCell("Void"));
  monoXtal->createAll(System,*monoV,0);

  // Note : join flag so can rotate on front/back
  monoBellowA->setFront(*driftA,2,1);
  monoBellowA->setBack(*monoV,1,1);
  monoBellowA->createAll(System,*driftA,2);

  // Note : join flag so can rotate on front/back
  monoBellowB->setFront(*monoV,2,1); 
  monoBellowB->setBack(*driftB,1,1);
  monoBellowB->createAll(System,*driftB,-1);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoBellowA,2);
  monoBellowA->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoV,2);
  monoV->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoBellowB,2);
  monoBellowB->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*driftB,2);
  driftB->insertInCell(System,outerCell);
  driftB->setCell("OuterVoid",outerCell);

  monoV->constructPorts(System);


  gateC->setFront(*driftB,2);
  gateC->createAll(System,*driftB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateC,2);
  gateC->insertInCell(System,outerCell);

  driftC->setFront(*gateC,2);
  driftC->createAll(System,*gateC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*driftC,2);
  driftC->insertInCell(System,outerCell);

  beamStop->addInsertCell(driftC->getCell("Void"));
  beamStop->createAll(System,*driftC,0);
  
  slitsA->setFront(*driftC,2);
  slitsA->createAll(System,*driftC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*slitsA,2);
  slitsA->insertInCell(System,outerCell);
  
  // fake inser for ports
  shieldPipe->addAllInsertCell(masterCell->getName());
  shieldPipe->setFront(*slitsA,2);
  shieldPipe->createAll(System,*slitsA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*shieldPipe,2);
  shieldPipe->insertAllInCell(System,outerCell);

  shieldPipe->splitObject(System,1001,outerCell,
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));
  cellIndex++;


  pipeD->setFront(*shieldPipe,2);
  pipeD->createAll(System,*shieldPipe,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeD,2);
  pipeD->insertInCell(System,outerCell);


  gateD->setFront(*pipeD,2);
  gateD->createAll(System,*pipeD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateD,2);
  gateD->insertInCell(System,outerCell);


  mirrorBoxB->setFront(*gateD,2);
  mirrorBoxB->createAll(System,*gateD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				*mirrorBoxB,2);
  mirrorBoxB->insertInCell(System,outerCell);
  mirrorBoxB->setCell("OuterVoid",outerCell);


  mirrorB->addInsertCell(mirrorBoxB->getCell("Void"));
  mirrorB->createAll(System,*mirrorBoxB,0);

  mirrorBoxB->splitObject(System,-11,outerCell);
  mirrorBoxB->splitObject(System,12,outerCell);
  cellIndex+=2;
  


  pipeE->setFront(*mirrorBoxB,2);
  pipeE->createAll(System,*mirrorBoxB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				*pipeE,2);
  pipeE->insertInCell(System,outerCell);
  

  slitsB->setFront(*pipeE,2);
  slitsB->createAll(System,*pipeE,2);
  outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,*slitsB,2);
  slitsB->setCell("OuterVoid",outerCell);
  slitsB->insertInCell(System,outerCell);


  // fake insert for ports
  viewPipe->addAllInsertCell(masterCell->getName());
  viewPipe->setFront(*slitsB,2);
  viewPipe->createAll(System,*slitsB,2);
  outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,*viewPipe,2);
  viewPipe->insertAllInCell(System,outerCell);

  // split the object into four
  const int cNumOffset(outerCell);
  viewPipe->splitObject(System,1001,outerCell,
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));
  viewPipe->splitObject(System,2001,outerCell,
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));

  viewPipe->splitObject(System,2002,cNumOffset+1,
  			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));
  cellIndex+=3;

  for(size_t i=0;i<viewMount.size();i++)
    {
      const constructSystem::portItem& PI=viewPipe->getPort(i);
      viewMount[i]->addInsertCell("Flange",cNumOffset+1);
      viewMount[i]->addInsertCell("Flange",cNumOffset+3);
      viewMount[i]->addInsertCell("Body",PI.getCell("Void"));
      viewMount[i]->addInsertCell("Body",viewPipe->getCell("Void"));
      viewMount[i]->setBladeCentre(PI,0);
      viewMount[i]->createAll(System,PI,2);
    }

  
  pipeF->setFront(*viewPipe,2);
  pipeF->createAll(System,*viewPipe,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeF,2);
  pipeF->insertInCell(System,outerCell);


  shutterPipe->setFront(*pipeF,2);
  shutterPipe->createAll(System,*pipeF,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*shutterPipe,2);
  shutterPipe->insertInCell(System,outerCell);

  
  
  monoShutter->addInsertCell("Flange",shutterPipe->getCell("Void"));
  monoShutter->addInsertCell("Body",shutterPipe->getCell("Void"));
  monoShutter->setBladeCentre(*shutterPipe,0);

  monoShutter->createAll
    (System,*shutterPipe,shutterPipe->getSideIndex("topFlange"));

  pipeG->addInsertCell(ContainedComp::getInsertCells());
  pipeG->setFront(*shutterPipe,2);
  pipeG->createAll(System,*shutterPipe,2);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				*pipeG,2);
  pipeG->insertInCell(System,outerCell);


  gateE->addInsertCell(ContainedComp::getInsertCells());
  gateE->setFront(*pipeG,2);
  gateE->createAll(System,*pipeG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				*gateE,2);
  gateE->insertInCell(System,outerCell);

  
  neutShield[0]->addAllInsertCell(mirrorBox->getCell("FFlangeVoid"));
  neutShield[0]->addAllInsertCell(mirrorBox->getCell("OuterVoid",1));
  neutShield[0]->setCutSurf("inner",*mirrorBox,"frontPortWall");
  neutShield[0]->createAll(System,*mirrorBox,-1);

  neutShield[1]->addAllInsertCell(driftB->getCell("OuterVoid"));
  neutShield[1]->setCutSurf("inner",*driftB,"pipeOuterTop");
  neutShield[1]->createAll(System,*driftB,-1);

  
  neutShield[2]->addAllInsertCell(mirrorBoxB->getCell("BFlangeVoid"));
  neutShield[2]->addAllInsertCell(mirrorBoxB->getCell("OuterVoid",2));
  neutShield[2]->setCutSurf("inner",*mirrorBoxB,"backPortWall");
  neutShield[2]->createAll(System,*mirrorBoxB,-2);

  setCell("LastVoid",masterCell->getName());
  lastComp=gateE;  

  return;
}

void
OpticsBeamline::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeInit,1);
  setLinkSignedCopy(1,*lastComp,2);
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
  ELog::RegMethod RControl("OpticsBeamline","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  pipeInit->setFront(FC,sideIndex);
  buildObjects(System);

  createLinks();
  return;
}


}   // NAMESPACE xraySystem

