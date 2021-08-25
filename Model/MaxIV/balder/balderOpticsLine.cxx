/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/balderOpticsLine.cxx
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
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
#include "CopiedComp.h"
#include "BlockZone.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "generalConstruct.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "CylGateValve.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

#include "TriggerTube.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCube.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "MonoShutter.h"
#include "balderOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
balderOpticsLine::balderOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(newName+"BuildZone"),
  
  pipeInit(new constructSystem::VacuumPipe(newName+"InitPipe")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  filterBox(new constructSystem::PortTube(newName+"FilterBox")),
  filters({
	std::make_shared<xraySystem::FlangeMount>(newName+"Filter0"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter1"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter2"),
        std::make_shared<xraySystem::FlangeMount>(newName+"Filter3")}),

  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  mirrorBox(new constructSystem::VacuumBox(newName+"MirrorBox")),
  mirror(new xraySystem::Mirror(newName+"Mirror")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  driftA(new constructSystem::VacuumPipe(newName+"DriftA")),  
  driftB(new constructSystem::VacuumPipe(newName+"DriftB")),
  monoV(new xraySystem::MonoVessel(newName+"MonoVac")),
  monoXtal(new xraySystem::MonoCrystals(newName+"MonoXtal")),
  monoBellowA(new constructSystem::Bellows(newName+"MonoBellowA")),
  monoBellowB(new constructSystem::Bellows(newName+"MonoBellowB")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  driftC(new constructSystem::VacuumPipe(newName+"DriftC")),
  beamStop(new insertSystem::insertPlate(newName+"BeamStop")),
  slitsA(new constructSystem::JawValveCube(newName+"SlitsA")),
  shieldPipe(new constructSystem::PortTube(newName+"ShieldPipe")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  gateD(new constructSystem::GateValveCube(newName+"GateD")),
  mirrorBoxB(new constructSystem::VacuumBox(newName+"MirrorBoxB")),
  mirrorB(new xraySystem::Mirror(newName+"MirrorB")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  slitsB(new constructSystem::JawValveCube(newName+"SlitsB")),
  viewPipe(new constructSystem::PortTube(newName+"ViewTube")),
  viewMount({
      std::make_shared<xraySystem::FlangeMount>(newName+"ViewMount0")
	}),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  monoShutter(new xraySystem::MonoShutter(newName+"MonoShutter")),
  
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  gateE(new constructSystem::GateValveCube(newName+"GateE")),
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
  OR.addObject(triggerPipe);
  OR.addObject(gateTubeA);
  OR.addObject(bellowA);
  OR.addObject(filterBox);
  OR.addObject(bellowB);
  OR.addObject(gateA);
  OR.addObject(mirrorBox);
  OR.addObject(mirror);
  OR.addObject(gateB);
  OR.addObject(bellowC);
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
  OR.addObject(bellowD);
  OR.addObject(gateD);
  OR.addObject(mirrorBoxB);
  OR.addObject(mirrorB);
  OR.addObject(bellowE);
  OR.addObject(slitsB);
  OR.addObject(viewPipe);
  OR.addObject(bellowF);
  OR.addObject(monoShutter);

  OR.addObject(bellowG);
  OR.addObject(gateE);
}
  
balderOpticsLine::~balderOpticsLine()
  /*!
    Destructor
   */
{}

void
balderOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("balderOpticsLine","populate");
  FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}


void
balderOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
   */
{
  ELog::RegMethod RegA("balderOpticsLine","createSurface");

  if (outerLeft>Geometry::zeroTol && isActive("floor"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      const HeadRule HR=
	ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 -6");

     buildZone.setSurround(HR*getRule("floor"));
     buildZone.setFront(getRule("front"));
     buildZone.setMaxExtent(getRule("back"));
     buildZone.setInnerMat(innerMat);
    }
  return;
}

void
balderOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("balderOpticsLine","buildObjects");

  
  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*pipeInit,2);
  pipeInit->insertAllInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowA);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*filterBox);

  // split on both inner void

  filterBox->splitVoidPorts(System,"SplitVoid",1001,
			    filterBox->getCell("Void"),
			    Geometry::Vec3D(0,1,0));

  filterBox->splitVoidPorts(System,"SplitOuter",2001,
			    outerCell,
   			    Geometry::Vec3D(0,1,0));

  filterBox->splitObject(System,-11,filterBox->getCell("SplitOuter",0));
  filterBox->splitObject(System,12,filterBox->getCell("SplitOuter",3));
  cellIndex+=5;

  for(size_t i=0;i<filters.size();i++)
    {
      const constructSystem::portItem& PI=filterBox->getPort(i);
      filters[i]->addInsertCell("Body",PI.getCell("Plate"));
      filters[i]->addInsertCell("Body",PI.getCell("Void"));
      filters[i]->addInsertCell("Blade",PI.getCell("Void"));
      filters[i]->addInsertCell("Body",filterBox->getCell("SplitVoid",i));
      filters[i]->addInsertCell("Blade",filterBox->getCell("SplitVoid",i));
      filters[i]->setBladeCentre(PI,0);
      filters[i]->createAll(System,PI,2);
    }
  
  constructSystem::constructUnit
    (System,buildZone,*filterBox,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*gateA);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*mirrorBox);
  mirrorBox->setCell("OuterVoid",outerCell);
  
  mirrorBox->splitObject(System,-11,outerCell);
  mirrorBox->splitObject(System,12,outerCell);
  cellIndex+=2;

  mirror->addInsertCell(mirrorBox->getCell("Void"));
  mirror->createAll(System,*mirrorBox,0);

  constructSystem::constructUnit
    (System,buildZone,*mirrorBox,"back",*gateB);

  constructSystem::constructUnit
    (System,buildZone,*gateB,"back",*bellowC);

  constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*driftA);


  driftB->createAll(System,*driftA,2);
  monoV->createAll(System,*driftA,2);

  monoBellowA->setJoinFront(*driftA,2);
  monoBellowA->setJoinBack(*monoV,1);
  monoBellowA->createAll(System,*driftA,2);

  // Note : join flag so can rotate on front/back
  monoBellowB->setJoinFront(*monoV,2); 
  monoBellowB->setJoinBack(*driftB,1);
  monoBellowB->createAll(System,*driftB,-1);

  monoXtal->addInsertCell(monoV->getCell("Void"));
  monoXtal->createAll(System,*monoV,0);

  outerCell=buildZone.createUnit(System,*monoBellowA,2);
  monoBellowA->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*monoV,2);
  monoV->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*monoBellowB,2);
  monoBellowB->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*driftB,2);
  driftB->insertAllInCell(System,outerCell);
  driftB->setCell("OuterVoid",outerCell);

  monoV->constructPorts(System);


  constructSystem::constructUnit
    (System,buildZone,*driftB,"back",*gateC);

  constructSystem::constructUnit
    (System,buildZone,*gateC,"back",*driftC);

  beamStop->addInsertCell(driftC->getCell("Void"));
  beamStop->createAll(System,*driftC,0);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*driftC,"back",*slitsA);
  
  //  shieldPipe->addAllInsertCell(masterCell->getName());

  outerCell=constructSystem::constructUnit
    (System,buildZone,*slitsA,"back",*shieldPipe);
  
  shieldPipe->splitObject(System,1001,outerCell,
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));
  cellIndex++;

  constructSystem::constructUnit
    (System,buildZone,*shieldPipe,"back",*bellowD);

  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*gateD);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*gateD,"back",*mirrorBoxB);
  mirrorBoxB->setCell("OuterVoid",outerCell);
  
  mirrorB->addInsertCell(mirrorBoxB->getCell("Void"));
  mirrorB->createAll(System,*mirrorBoxB,0);

  mirrorBoxB->splitObject(System,-11,outerCell);
  mirrorBoxB->splitObject(System,12,outerCell);
  cellIndex+=2;

  constructSystem::constructUnit
    (System,buildZone,*mirrorBoxB,"back",*bellowE);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowE,"back",*slitsB);
  slitsB->setCell("OuterVoid",outerCell);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*slitsB,"back",*viewPipe);
  viewPipe->addCell("OuterVoid",outerCell);
  viewPipe->splitObject(System,-11,outerCell);
  viewPipe->splitObject(System,12,outerCell);
  viewPipe->splitObject(System,1001,outerCell,
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));
  const constructSystem::portItem& CPI=viewPipe->getPort(3);
  CPI.insertInCell(System,slitsB->getCell("OuterVoid"));

  // inner viewing item
  for(size_t i=0;i<viewMount.size();i++)
    {
      const constructSystem::portItem& PI=viewPipe->getPort(i);

      viewMount[i]->addInsertCell("Body",PI.getCell("Plate"));
      viewMount[i]->addInsertCell("Body",PI.getCell("Void"));
      viewMount[i]->addInsertCell("Blade",PI.getCell("Void"));
      viewMount[i]->addInsertCell("Blade",viewPipe->getCell("Void"));
      viewMount[i]->addInsertCell("Body",viewPipe->getCell("Void"));

      viewMount[i]->setBladeCentre(PI,0);
      viewMount[i]->createAll(System,PI,2);
    }


  constructSystem::constructUnit
    (System,buildZone,*viewPipe,"back",*bellowF);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowF,"back",*monoShutter);
  monoShutter->addCell("OuterVoid",outerCell);

  monoShutter->splitObject(System,"PortACut",outerCell);
  const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  monoShutter->splitObjectAbsolute(System,2001,
				   monoShutter->getCell("OuterVoid",1),
				   midPoint,midAxis);
  monoShutter->splitObject(System,"PortBCut",
			   monoShutter->getCell("OuterVoid",1));
  
  constructSystem::constructUnit
    (System,buildZone,*monoShutter,"back",*bellowG);

  constructSystem::constructUnit
    (System,buildZone,*bellowG,"back",*gateE);  

  neutShield[0]->addAllInsertCell(mirrorBox->getCell("FFlangeVoid"));
  neutShield[0]->addAllInsertCell(mirrorBox->getCell("OuterVoid",1));
  neutShield[0]->setCutSurf("inner",*mirrorBox,"frontPortWall");
  neutShield[0]->createAll(System,*mirrorBox,"#front");

  neutShield[1]->addAllInsertCell(driftB->getCell("OuterVoid"));
  neutShield[1]->setCutSurf("inner",*driftB,"pipeOuterTop");
  neutShield[1]->createAll(System,*driftB,-1);
  
  neutShield[2]->addAllInsertCell(mirrorBoxB->getCell("BFlangeVoid"));
  neutShield[2]->addAllInsertCell(mirrorBoxB->getCell("OuterVoid",2));
  neutShield[2]->setCutSurf("inner",*mirrorBoxB,"backPortWall");
  neutShield[2]->createAll(System,*mirrorBoxB,"#back");
  

  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=gateE;

  return;
}

void
balderOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  
  
void 
balderOpticsLine::createAll(Simulation& System,
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
  ELog::RegMethod RControl("balderOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  pipeInit->setFront(FC,sideIndex);
  buildObjects(System);

  createLinks();
  return;
}


}   // NAMESPACE xraySystem

