/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/danmaxOpticsLine.cxx
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "BremBlock.h"
#include "GateValveCylinder.h"
#include "CylGateValve.h"
#include "TriggerTube.h"
#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveTube.h"
#include "FlangeMount.h"
#include "MonoBox.h"
#include "MonoShutter.h"
#include "BeamPair.h"
#include "DCMTank.h"
#include "MonoBlockXstals.h"
#include "MLMono.h"
#include "generalConstruct.h"
#include "SquareFMask.h"
#include "danmaxOpticsLine.h"



namespace xraySystem
{

// Note currently uncopied:
  
danmaxOpticsLine::danmaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),
  innerMat(0),
   
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  bremCollA(new xraySystem::SquareFMask(newName+"BremCollA")),
  filterPipe(new constructSystem::VacuumPipe(newName+"FilterPipe")),
  gateA(new constructSystem::GateValveCylinder(newName+"GateA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  lauePipe(new constructSystem::VacuumPipe(newName+"LauePipe")),    
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  slitTube(new constructSystem::PortTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamPair>(newName+"JawX"),
      std::make_shared<xraySystem::BeamPair>(newName+"JawZ")
	}),
  gateB(new constructSystem::GateValveCylinder(newName+"GateB")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  monoVessel(new xraySystem::DCMTank(newName+"MonoVessel")),
  mbXstals(new xraySystem::MonoBlockXstals(newName+"MBXstals")),
  gateC(new constructSystem::GateValveCylinder(newName+"GateC")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  viewTubeScreen(new xraySystem::FlangeMount(newName+"ViewTubeScreen")),
  gateD(new constructSystem::GateValveCylinder(newName+"GateD")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  MLMVessel(new constructSystem::VacuumBox(newName+"MLMVessel")),
  MLM(new xraySystem::MLMono(newName+"MLM")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  gateE(new constructSystem::GateValveCylinder(newName+"GateE")),
  beamStopTube(new constructSystem::PipeTube(newName+"BeamStopTube")),
  beamStop(new xraySystem::BremBlock(newName+"BeamStop")),
  slitsA(new constructSystem::JawValveTube(newName+"SlitsA")),
  slitsAOut(new constructSystem::VacuumPipe(newName+"SlitsAOut")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  viewTubeB(new constructSystem::PortTube(newName+"ViewTubeB")),
  viewTubeBScreen(new xraySystem::FlangeMount(newName+"ViewTubeBScreen")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  lensBox(new xraySystem::MonoBox(newName+"LensBox")),
  gateF(new constructSystem::GateValveCylinder(newName+"GateF")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  slitsBOut(new constructSystem::VacuumPipe(newName+"SlitsBOut")),
  revBeamStopTube(new constructSystem::PipeTube(newName+"RevBeamStopTube")),
  revBeamStop(new xraySystem::BremBlock(newName+"RevBeamStop")),
  slitsB(new constructSystem::JawValveTube(newName+"SlitsB")),
  bellowK(new constructSystem::Bellows(newName+"BellowK")),
  monoAdaptorA(new constructSystem::VacuumPipe(newName+"MonoAdaptorA")),
  monoShutter(new xraySystem::MonoShutter(newName+"MonoShutter")),
  monoAdaptorB(new constructSystem::VacuumPipe(newName+"MonoAdaptorB")),
  gateG(new constructSystem::GateValveCylinder(newName+"GateG"))
 /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","danmaxOpticsLine(constructor)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(pipeInit);
  OR.addObject(triggerPipe);
  OR.addObject(gateTubeA);

  OR.addObject(bellowA);
  OR.addObject(bremCollA);
  OR.addObject(filterPipe);
  OR.addObject(gateA);
  OR.addObject(bellowC);
  OR.addObject(lauePipe);
  OR.addObject(bellowD);
  OR.addObject(slitTube);
  OR.addObject(jaws[0]);
  OR.addObject(jaws[1]);
  
  OR.addObject(gateB);
  OR.addObject(bellowE);
  OR.addObject(monoVessel);
  OR.addObject(mbXstals);
  OR.addObject(gateC);
  OR.addObject(viewTube);
  OR.addObject(viewTubeScreen);
  OR.addObject(gateD);
  OR.addObject(bellowF);
  OR.addObject(MLMVessel);
  OR.addObject(MLM);
  OR.addObject(bellowG);
  OR.addObject(gateE);
  OR.addObject(beamStopTube);
  OR.addObject(beamStop);
  OR.addObject(slitsA);
  OR.addObject(slitsAOut);
  OR.addObject(bellowH);
  OR.addObject(viewTubeB);
  OR.addObject(viewTubeBScreen);
  OR.addObject(bellowI);
  OR.addObject(lensBox);
  OR.addObject(gateF);
  OR.addObject(bellowJ);
  OR.addObject(revBeamStopTube);
  OR.addObject(revBeamStop);
  OR.addObject(slitsB);
  OR.addObject(slitsBOut);
  OR.addObject(bellowK);
  OR.addObject(monoAdaptorA);
  OR.addObject(monoShutter);
  OR.addObject(monoAdaptorB);
  OR.addObject(gateG);
}
  
danmaxOpticsLine::~danmaxOpticsLine()
  /*!
    Destructor
   */
{}

void
danmaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  innerMat=ModelSupport::EvalDefMat(Control,keyName+"InnerMat",innerMat);
    
  return;
}


void
danmaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
    Mainly for the masterCell 
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","createSurface");

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

      buildZone.setFront(getRule("front"));
      buildZone.setMaxExtent(getRule("back"));
    }
  return;
}


void
danmaxOpticsLine::constructViewScreen(Simulation& System,
				      const attachSystem::FixedComp& initFC, 
				      const std::string& sideName)
  /*!
    Sub build of the first viewing package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructViewScreen");

  // FAKE insertcell: required


  viewTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  viewTube->setOuterVoid();
  viewTube->createAll(System,initFC,sideName);
  viewTube->intersectPorts(System,1,2);

  const constructSystem::portItem& VPA=viewTube->getPort(0);
  const constructSystem::portItem& VPB=viewTube->getPort(1);
  const constructSystem::portItem& VPC=viewTube->getPort(2); // screen)

  int outerCell=buildZone.createUnit
    (System,VPB,VPB.getSideIndex("OuterPlate"));
  this->addCell("OuterVoid",outerCell);

  std::vector<int> cellUnit;
  cellUnit.push_back(outerCell);

  const Geometry::Vec3D Axis=viewTube->getY()*(VPB.getY()+VPC.getY())/2.0;

  buildZone.splitObjectAbsolute(System,1501,"Unit",
			      viewTube->getCentre(),VPB.getY());
  cellUnit.push_back(buildZone.getLastCell("Unit"));
  buildZone.splitObjectAbsolute(System,1502,"Unit",
			      viewTube->getCentre(),Axis);
  cellUnit.push_back(buildZone.getLastCell("Unit"));
  
  viewTube->insertMainInCell(System,cellUnit);

  VPA.insertInCell(System,buildZone.getLastCell("Unit"));
  viewTube->insertPortInCell(System,0,cellUnit[0]);
  viewTube->insertPortInCell(System,1,cellUnit[1]);
  viewTube->insertPortInCell(System,2,cellUnit[2]);
  
  cellIndex+=3;


  viewTubeScreen->addInsertCell("Body",viewTube->getCell("Void"));
  viewTubeScreen->addInsertCell("Body",VPC.getCell("Void"));
  viewTubeScreen->addInsertCell("Blade",viewTube->getCell("Void"));
  viewTubeScreen->addInsertCell("Blade",VPC.getCell("Void"));
  viewTubeScreen->setBladeCentre(*viewTube,0);
  viewTubeScreen->createAll(System,VPC,"-InnerPlate");

  outerCell=constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*gateD);
  VPC.insertInCell(System,outerCell);
  return;
}

void
danmaxOpticsLine::constructViewScreenB(Simulation& System,
				       const attachSystem::FixedComp& initFC, 
				       const std::string& sideName)
  /*!
    Sub build of the second viewer package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructViewScreenB");

  int outerCell;
  
  outerCell=constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*viewTubeB);

  // Two port -- 3 splits
  viewTubeB->splitVoidPorts(System,"SplitVoid",1001,
			    viewTubeB->getCell("Void"),
			    Geometry::Vec3D(0,1,0));

  viewTubeB->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,1,0));
  cellIndex++;  // remember creates an extra cell in  primary

  const constructSystem::portItem& VPI=viewTubeB->getPort(1);
  viewTubeBScreen->addInsertCell("Body",VPI.getCell("Void"));
  viewTubeBScreen->addInsertCell("Body",viewTubeB->getCell("SplitVoid",2));
  viewTubeBScreen->addInsertCell("Blade",VPI.getCell("Void"));
  viewTubeBScreen->addInsertCell("Blade",viewTubeB->getCell("SplitVoid",2));
  viewTubeBScreen->setBladeCentre(*viewTubeB,0);
  viewTubeBScreen->createAll(System,VPI,"-InnerPlate");

  return;
}

void
danmaxOpticsLine::constructRevBeamStopTube
(Simulation& System,
 const attachSystem::FixedComp& initFC,
 const std::string& sideName)
/*!
    Sub build of the beamstop tube [reversed]
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructRevBeamStopTube");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*slitsBOut);

  constructSystem::constructUnit
    (System,buildZone,*slitsBOut,"back",*slitsB);

  // FAKE insertcell: required
  revBeamStopTube->setPortRotation(4,Geometry::Vec3D(1,0,0));
  revBeamStopTube->createAll(System,*slitsB,2);


  const constructSystem::portItem& VPB=revBeamStopTube->getPort(0);
  int outerCell=buildZone.createUnit
    (System,VPB,VPB.getSideIndex("OuterPlate"));
  revBeamStopTube->insertAllInCell(System,outerCell);

  revBeamStop->addInsertCell(revBeamStopTube->getCell("Void"));
  ELog::EM<<"Orig -- "<<revBeamStopTube->getLinkPt("OrgOrigin")<<ELog::endDiag;
  revBeamStop->createAll(System,*revBeamStopTube,"Origin");

  constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*bellowK);

  return;
}


void
danmaxOpticsLine::constructMono(Simulation& System,
				const attachSystem::FixedComp& initFC, 
				const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructMono");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoVessel);
  
  mbXstals->addInsertCell(monoVessel->getCell("Void"));
  mbXstals->createAll(System,*monoVessel,0);

  return;
}

void
danmaxOpticsLine::constructMirrorMono(Simulation& System,
				      const attachSystem::FixedComp& initFC, 
				      const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructMirrorMono");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*MLMVessel);
  
  MLM->addInsertCell(MLMVessel->getCell("Void"));
  //  MLM->copyCutSurf("innerCylinder",*MLMVessel,"innerRadius");
  MLM->createAll(System,*MLMVessel,0);


  return;
}

void
danmaxOpticsLine::constructSlitTube(Simulation& System,
				    const attachSystem::FixedComp& initFC, 
				    const std::string& sideName)
  /*!
    Build the DM2 split package
    \param System :: Simuation to use
    \param initFC :: start localtion (FixedComp)
    \param sideName :: link point
   */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constrcutSlitTube");

  int outerCell;
  
  outerCell=constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*slitTube);

  slitTube->intersectPorts(System,0,1);
  slitTube->intersectPorts(System,1,2);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
   			   slitTube->getCell("Void"),
   			   Geometry::Vec3D(0,1,0));

  slitTube->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  cellIndex++;  // remember creates an extra cell in  primary


  for(size_t i=0;i<jaws.size();i++)
    {
      const constructSystem::portItem& PI=slitTube->getPort(i);
      jaws[i]->addInsertCell("SupportA",PI.getCell("Void"));
      jaws[i]->addInsertCell("SupportA",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("SupportB",PI.getCell("Void"));
      jaws[i]->addInsertCell("SupportB",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("BlockA",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("BlockB",slitTube->getCell("SplitVoid",i));
      jaws[i]->createAll(System,*slitTube,0,
			 PI,PI.getSideIndex("InnerPlate"));
    }  

  return;
}

  
void
danmaxOpticsLine::constructBeamStopTube
   (Simulation& System,
    const attachSystem::FixedComp& initFC, 
    const std::string& sideName)
 /*!
    Sub build of the beamstoptube
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructBeamStopTube");

  int outerCell;
  
  beamStopTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  beamStopTube->createAll(System,initFC,sideName);
  //  beamStopTube->intersectPorts(System,1,2);

  const constructSystem::portItem& VPB=beamStopTube->getPort(1);
  
  outerCell=buildZone.createUnit(System,VPB,VPB.getSideIndex("OuterPlate"));
  beamStopTube->insertAllInCell(System,outerCell);
  
  beamStop->addInsertCell(beamStopTube->getCell("Void"));
  beamStop->createAll(System,*beamStopTube,"Origin");

  constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*slitsA);

  constructSystem::constructUnit
    (System,buildZone,*slitsA,"back",*slitsAOut);

  return;
}

void
danmaxOpticsLine::constructMonoShutter(Simulation& System,
				       const attachSystem::FixedComp& FC,
				       const std::string& linkName)
  /*!
    Construct a monoshutter system
    \param System :: Simulation for building
    \param FC :: FixedComp for start point
    \param linkName :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructMonoShutter");


  constructSystem::constructUnit
    (System,buildZone,FC,linkName,*monoAdaptorA);

  constructSystem::constructUnit
    (System,buildZone,*monoAdaptorA,"back",*monoShutter);

  /*
  monoShutter->splitObject(System,"-PortACut",outerCell);
  const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  monoShutter->splitObjectAbsolute(System,2001,outerCell,midPoint,midAxis);
  monoShutter->splitObject(System,"PortBCut",outerCell);
  cellIndex+=3;
  */
  constructSystem::constructUnit
    (System,buildZone,*monoShutter,"back",*monoAdaptorB);
  
  return;
}

void
danmaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","buildObjects");

  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*pipeInit,-1);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);
  outerCell=buildZone.createUnit(System,*pipeInit,2);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*bremCollA);

  constructSystem::constructUnit
    (System,buildZone,*bremCollA,"back",*filterPipe);

  constructSystem::constructUnit
    (System,buildZone,*filterPipe,"back",*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*bellowC);
  
  constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*lauePipe);
  
  constructSystem::constructUnit
    (System,buildZone,*lauePipe,"back",*bellowD);
 
  constructSlitTube(System,*bellowD,"back");

  constructSystem::constructUnit
    (System,buildZone,*slitTube,"back",*gateB);

  constructSystem::constructUnit
    (System,buildZone,*gateB,"back",*bellowE);
  
  constructMono(System,*bellowE,"back");

  
  constructSystem::constructUnit
    (System,buildZone,*monoVessel,"back",*gateC);

  constructViewScreen(System,*gateC,"back");

  constructSystem::constructUnit
    (System,buildZone,*gateD,"back",*bellowF);

  constructMirrorMono(System,*bellowF,"back");

  
  constructSystem::constructUnit
    (System,buildZone,*MLMVessel,"back",*bellowG);

  constructSystem::constructUnit
    (System,buildZone,*bellowG,"back",*gateE);

  constructBeamStopTube(System,*gateE,"back");

  constructSystem::constructUnit
    (System,buildZone,*slitsAOut,"back",*bellowH);

  constructViewScreenB(System,*bellowH,"back");

  constructSystem::constructUnit
    (System,buildZone,*viewTubeB,"back",*bellowI);

  constructSystem::constructUnit
    (System,buildZone,*bellowI,"back",*lensBox);

  // adds a portset to an object:
  /*
  constructSystem::portSet lensBoxPort(*lensBox);
  lensBoxPort.createPorts(System,"MainWall",lensBox->getInsertCells());
  lensBoxPort.splitVoidPorts(System,"OuterVoid",1501,CN);
  */
  constructSystem::constructUnit
    (System,buildZone,*lensBox,"back",*gateF);
  constructSystem::constructUnit
    (System,buildZone,*gateF,"back",*bellowJ);

  constructRevBeamStopTube(System,*bellowJ,"back");

  constructMonoShutter(System,*bellowK,"back");

  constructSystem::constructUnit
    (System,buildZone,*monoAdaptorB,"back",*gateG);

  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=gateG;

  return;

}

void
danmaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("danmaxOpticsLine","createLinks");
  
  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  
  
void 
danmaxOpticsLine::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("danmaxOpticsLine","createAll");


  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

