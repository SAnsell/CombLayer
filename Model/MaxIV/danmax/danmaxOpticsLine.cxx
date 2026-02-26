/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: danmax/danmaxOpticsLine.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and Konstantin Batkov
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
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

#include "GeneralPipe.h"
#include "VacuumPipe.h"
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
#include "MonoShutterR3.h"
#include "BeamPair.h"
#include "DCMTank.h"
#include "MonoBlockXstals.h"
#include "MLMono.h"
#include "generalConstruct.h"
#include "SquareFMask.h"
#include "TwinPipe.h"
#include "FlangePlate.h"

#include "danmaxOpticsLine.h"


namespace xraySystem
{

// Note currently uncopied:

danmaxOpticsLine::danmaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),

  buildZone(Key+"BuildZone"),
  buildZoneSinCrys(newName+"BuildZoneSinCrys"),
  buildZoneDanMAX(newName+"BuildZoneDanMAX"),
  innerMat(0),

  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  valve4(new xraySystem::CylGateValve(newName+"Valve4")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  bremColl1Tube(new constructSystem::PipeTube(newName+"BremColl1Tube")),
  bremColl1(new xraySystem::BremBlock(newName+"BremColl1")),
  highPassFilter(new constructSystem::VacuumPipe(newName+"HighPassFilter")),
  valve5(new xraySystem::CylGateValve(newName+"Valve5")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  cm1(new constructSystem::PipeTube(newName+"CM1")),
  valveS1(new xraySystem::CylGateValve(newName+"ValveS1")),
  beamViewerS1(new constructSystem::PipeTube(newName+"BeamViewerS1")),
  beamViewerS1Screen(new xraySystem::FlangeMount(newName+"BeamViewerS1Screen")),
  cardanBellowUpstream(new constructSystem::Bellows(newName+"CardanBellowUpstream")),
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  pipeSinCrys(new constructSystem::VacuumPipe(newName+"PipeSinCrys")),
  linearlyGuidedBellowUpstream(new constructSystem::Bellows(newName+"LinearlyGuidedBellowUpstream")),
  cardanBellowDownstream(new constructSystem::Bellows(newName+"CardanBellowDownstream")),
  cm2(new constructSystem::PipeTube(newName+"CM2")),

  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  lauePipe(new constructSystem::VacuumPipe(newName+"LauePipe")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  slitTube(new constructSystem::PortTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamPair>(newName+"JawX"),
      std::make_shared<xraySystem::BeamPair>(newName+"JawZ")
	}),
  valve6(new xraySystem::CylGateValve(newName+"Valve6")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  monoVessel(new xraySystem::DCMTank(newName+"MonoVessel")),
  mbXstals(new xraySystem::MonoBlockXstals(newName+"MBXstals")),
  bellowAfterMono(new constructSystem::Bellows(newName+"BellowAfterMono")),
  valve7(new xraySystem::CylGateValve(newName+"Valve7")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  viewTubeScreen(new xraySystem::FlangeMount(newName+"ViewTubeScreen")),
  valve8(new xraySystem::CylGateValve(newName+"Valve8")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  MLMVessel(new constructSystem::VacuumBox(newName+"MLMVessel")),
  MLM(new xraySystem::MLMono(newName+"MLM")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  valve9(new xraySystem::CylGateValve(newName+"Valve9")),
  beamStopInPipe(new constructSystem::VacuumPipe(newName+"BeamStopInPipe")),
  beamStopSection(new constructSystem::PipeTube(newName+"BeamStopSection")),
  beamStopTube(new constructSystem::PipeTube(newName+"BeamStopTube")),
  beamStop(new xraySystem::BremBlock(newName+"BeamStop")),
  beamStopOutPipe(new constructSystem::VacuumPipe(newName+"BeamStopOutPipe")),
  monoSlitsTube(new constructSystem::PipeTube(newName+"MonoSlitsTube")),
  monoSlits({
      std::make_shared<xraySystem::BeamPair>(newName+"MonoSlitsX"),
      std::make_shared<xraySystem::BeamPair>(newName+"MonoSlitsZ")
	}),
  slitsAOut(new constructSystem::FlangePlate(newName+"SlitsAOut")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  viewTubeB(new constructSystem::PortTube(newName+"ViewTubeB")),
  viewTubeBScreen(new xraySystem::FlangeMount(newName+"ViewTubeBScreen")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  CRLGateIn(new constructSystem::GateValveCylinder(newName+"CRLGateIn")),
  lensBox(new xraySystem::MonoBox(newName+"LensBox")),
  CRLGateOut(new constructSystem::GateValveCylinder(newName+"CRLGateOut")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  revMonoSlitsIn(new constructSystem::FlangePlate(newName+"RevMonoSlitsIn")),
  revMonoSlitsTube(new constructSystem::PipeTube(newName+"RevMonoSlitsTube")),
  revMonoSlits({
      std::make_shared<xraySystem::BeamPair>(newName+"RevMonoSlitsX"),
      std::make_shared<xraySystem::BeamPair>(newName+"RevMonoSlitsZ")
	}),
  revBeamStopTube(new constructSystem::PipeTube(newName+"RevBeamStopTube")),
  revBeamStop(new xraySystem::BremBlock(newName+"RevBeamStop")),
  bellowK(new constructSystem::Bellows(newName+"BellowK")),
  monoShutter(new xraySystem::MonoShutterR3(newName+"MonoShutter")),
  bellowL(new constructSystem::Bellows(newName+"BellowL"))
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
  OR.addObject(valve4);

  OR.addObject(bellowA);
  OR.addObject(bremColl1Tube);
  OR.addObject(bremColl1);
  OR.addObject(highPassFilter);
  OR.addObject(valve5);
  OR.addObject(pipeA);
  OR.addObject(cm1);
  OR.addObject(valveS1);
  OR.addObject(beamViewerS1);
  OR.addObject(beamViewerS1Screen);
  OR.addObject(cardanBellowUpstream);
  OR.addObject(bellowBA);
  OR.addObject(pipeSinCrys);
  OR.addObject(linearlyGuidedBellowUpstream);
  OR.addObject(cardanBellowDownstream);
  OR.addObject(cm2);
  OR.addObject(bellowC);
  OR.addObject(lauePipe);
  OR.addObject(bellowD);
  OR.addObject(slitTube);
  OR.addObject(jaws[0]);
  OR.addObject(jaws[1]);

  OR.addObject(valve6);
  OR.addObject(bellowE);
  OR.addObject(monoVessel);
  OR.addObject(mbXstals);
  OR.addObject(bellowAfterMono),
  OR.addObject(valve7);
  OR.addObject(viewTube);
  OR.addObject(viewTubeScreen);
  OR.addObject(valve8);
  OR.addObject(bellowF);
  OR.addObject(MLMVessel);
  OR.addObject(MLM);
  OR.addObject(bellowG);
  OR.addObject(valve9);
  OR.addObject(beamStopInPipe);
  OR.addObject(beamStopSection);
  OR.addObject(beamStopTube);
  OR.addObject(beamStop);
  OR.addObject(beamStopOutPipe);
  OR.addObject(monoSlitsTube);
  OR.addObject(monoSlits[0]);
  OR.addObject(monoSlits[1]);
  OR.addObject(slitsAOut);
  OR.addObject(bellowH);
  OR.addObject(viewTubeB);
  OR.addObject(viewTubeBScreen);
  OR.addObject(bellowI);
  OR.addObject(CRLGateIn);
  OR.addObject(lensBox);
  OR.addObject(CRLGateOut);
  OR.addObject(bellowJ);
  OR.addObject(revMonoSlitsIn);
  OR.addObject(revMonoSlitsTube);
  OR.addObject(revMonoSlits[0]);
  OR.addObject(revMonoSlits[1]);
  OR.addObject(revBeamStopTube);
  OR.addObject(revBeamStop);
  OR.addObject(bellowK);
  OR.addObject(monoShutter);
  OR.addObject(bellowL);
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
    \param Control :: Data base for variables
   */
{
  FixedRotate::populate(Control);

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
      HeadRule HR;
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 -6");
      HR*=getRule("floor");
      buildZone.setSurround(HR);

      buildZone.setFront(getRule("front"));
      buildZone.setMaxExtent(getRule("back"));
    }
  return;
}

void
danmaxOpticsLine::constructBremColl1Tube
   (Simulation& System,
    const attachSystem::FixedComp& initFC,
    const std::string& sideName)
 /*!
    Sub build of the Bremsstrahlung Collimator 1 tube.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructBremColl1Tube");

  bremColl1Tube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  bremColl1Tube->createAll(System,*frontEnd,0);

  const constructSystem::portItem& VPA=bremColl1Tube->getPort(0);
  const constructSystem::portItem& VPB=bremColl1Tube->getPort(1);

  bellowA->setBack(VPA.getLinkSurf(2));
  bellowA->createAll(System,initFC,sideName);
  bellowA->insertAllInCell(System,
    buildZone.createUnit(System,VPA,VPA.getSideIndex("#OuterPlate")));

  int outerCell=buildZone.createUnit(System,VPB,VPB.getSideIndex("OuterPlate"));
  bremColl1Tube->insertAllInCell(System,outerCell);

  bremColl1->addInsertCell(bremColl1Tube->getCell("Void"));
  bremColl1->createAll(System,*bremColl1Tube,"Origin");

  // When using absolute positioning, YStep variables needs to be adjusted.
  const bool useHighPassFilterAbsY = false;
  if(!useHighPassFilterAbsY){
  constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*highPassFilter);
  } else {
    highPassFilter->createAll(System,*frontEnd,0);
    outerCell=buildZone.createUnit(System,*highPassFilter,"back");
    highPassFilter->insertAllInCell(System,outerCell);
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
  viewTube->createAll(System,*frontEnd,0);
  viewTube->intersectPorts(System,0,2);

  const constructSystem::portItem& VPA=viewTube->getPort(0);
  const constructSystem::portItem& VPB=viewTube->getPort(1);
  const constructSystem::portItem& VPC=viewTube->getPort(2); // screen)

  valve7->setRotation(0.0,180.0,0.0);
  valve7->createAll(System,VPA,VPA.getSideIndex("OuterPlate"));

  bellowAfterMono->setBack(initFC,sideName);
  bellowAfterMono->createAll(System,*valve7,"back");

  bellowAfterMono->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowAfterMono,"front"));

  int outerCell=buildZoneDanMAX.createUnit(System,*valve7,"front");
  valve7->insertInCell(System,outerCell);
  VPC.insertInCell(System,outerCell);

  outerCell=buildZoneDanMAX.createUnit
    (System,VPB,VPB.getSideIndex("OuterPlate"));
  this->addCell("OuterVoid",outerCell);

  std::vector<int> cellUnit;
  cellUnit.push_back(outerCell);

  const Geometry::Vec3D Axis=viewTube->getY()*(VPA.getY()+VPC.getY())/2.0;

  buildZoneDanMAX.splitObjectAbsolute(System,1501,"Unit",
			      viewTube->getCentre(),-VPB.getY());
  cellUnit.push_back(buildZoneDanMAX.getLastCell("Unit"));
  buildZoneDanMAX.splitObjectAbsolute(System,1502,"Unit",
			      viewTube->getCentre(),Axis);
  cellUnit.push_back(buildZoneDanMAX.getLastCell("Unit"));

  viewTube->insertMainInCell(System,cellUnit);

  VPB.insertInCell(System,buildZoneDanMAX.getLastCell("Unit"));
  viewTube->insertPortInCell(System,0,cellUnit[2]);
  viewTube->insertPortInCell(System,1,cellUnit[0]);
  viewTube->insertPortInCell(System,2,cellUnit[1]);

  cellIndex+=3;

  viewTubeScreen->addInsertCell("Body",viewTube->getCell("Void"));
  viewTubeScreen->addInsertCell("Blade",viewTube->getCell("Void"));
  viewTubeScreen->setBladeCentre(Geometry::Vec3D(0.0,0.0,0.0));
  // TODO: If built on "InnerBack" surface, the thread is not attached to the cap.
  viewTubeScreen->createAll(System,*viewTube,"InnerBack");

  outerCell=constructSystem::constructUnit
    (System,buildZoneDanMAX,VPB,"OuterPlate",*valve8);

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

  viewTubeB->createAll(System,*frontEnd,0);

  bellowH->setBack(initFC,sideName);
  bellowH->createAll(System,*viewTubeB,"front");

  bellowH->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowH,"front"));

  viewTubeB->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*viewTubeB,"back"));

  const constructSystem::portItem& VPI=viewTubeB->getPort(1);
  viewTubeBScreen->addInsertCell("Body",VPI.getCell("Void"));
  viewTubeBScreen->addInsertCell("Body",viewTubeB->getCell("Void"));
  viewTubeBScreen->addInsertCell("Blade",viewTubeB->getCell("Void"));
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

    The construction is largely analogous to Diagnostic Module 4.
    See that one for more information.

    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructRevBeamStopTube");

  revMonoSlitsTube->createAll(System,*frontEnd,0);

  revMonoSlitsIn->createAll(System,*revMonoSlitsTube,"front");

  bellowJ->setBack(initFC,sideName);
  bellowJ->createAll(System,*revMonoSlitsIn,"back");

  bellowJ->insertAllInCell(
    System,buildZoneDanMAX.createUnit(System,*bellowJ,"front")
  );

  revMonoSlitsIn->insertInCell(
    System,buildZoneDanMAX.createUnit(System,*revMonoSlitsIn,"front")
  );

  revMonoSlitsTube->insertAllInCell(
    System,buildZoneDanMAX.createUnit(System,*revMonoSlitsTube,"back"));

 for(auto i: std::vector<size_t>{0,1}){
    const constructSystem::portItem& port0=revMonoSlitsTube->getPort(2*i);
    const constructSystem::portItem& port1=revMonoSlitsTube->getPort(2*i+1);
    revMonoSlits[i]->addInsertCell("SupportA",port1.getCell("Void"));
    revMonoSlits[i]->addInsertCell("SupportA",revMonoSlitsTube->getCell("Void"));
    revMonoSlits[i]->addInsertCell("BlockA",revMonoSlitsTube->getCell("Void"));
    revMonoSlits[i]->addInsertCell("SupportB",port0.getCell("Void"));
    revMonoSlits[i]->addInsertCell("SupportB",revMonoSlitsTube->getCell("Void"));
    revMonoSlits[i]->addInsertCell("BlockB",revMonoSlitsTube->getCell("Void"));
    revMonoSlits[i]->createAll(System,*revMonoSlitsTube,0,port0,
      port0.getSideIndex("InnerPlate"));
  }

  revBeamStopTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  revBeamStopTube->createAll(System,*revMonoSlitsTube,"back");

  revBeamStop->addInsertCell(revBeamStopTube->getCell("Void"));
  revBeamStop->createAll(System,*revBeamStopTube,"Origin");

  const constructSystem::portItem& VPB=revBeamStopTube->getPort(1);
  revBeamStopTube->insertAllInCell(System,
    buildZoneDanMAX.createUnit(System,VPB,VPB.getSideIndex("OuterPlate")));

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

  monoVessel->createAll(System,*frontEnd,0);

  bellowE->setBack(initFC,sideName);
  bellowE->createAll(System,*monoVessel,"front");
  bellowE->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowE,"front"));

  monoVessel->insertInCell(System,buildZoneDanMAX.createUnit(System,*monoVessel,"back"));

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

  MLMVessel->createAll(System,*frontEnd,0);

  bellowF->setBack(initFC,sideName);
  bellowF->createAll(System,*MLMVessel,"front");
  bellowF->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowF,"front"));

  MLMVessel->insertInCell(System,buildZoneDanMAX.createUnit(System,*MLMVessel,"back"));

  MLM->addInsertCell(MLMVessel->getCell("Void"));
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

  slitTube->createAll(System,*frontEnd,0);

  bellowD->createAll(System,*slitTube,"front");

  lauePipe->setBack(initFC,sideName);
  lauePipe->createAll(System,*bellowD,"back");

  lauePipe->insertAllInCell(System,
    buildZoneDanMAX.createUnit(System,*lauePipe,"front"));


  bellowD->insertAllInCell(System,
    buildZoneDanMAX.createUnit(System,*bellowD,"front"));

  int outerCell=buildZoneDanMAX.createUnit(System,*slitTube,"back");
  slitTube->insertAllInCell(System,outerCell);

  slitTube->intersectPorts(System,0,1);
  slitTube->intersectPorts(System,1,2);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
   			   slitTube->getCell("Void"),
   			   Geometry::Vec3D(0,1,0));

  std::vector<int> splitCells = slitTube->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  cellIndex++;  // remember creates an extra cell in primary

  // Since the jaws have large xy offsets, they partially end up in the void cells of
  // the other ports.
  const constructSystem::portItem& port0=slitTube->getPort(0);
  jaws[0]->addInsertCell("SupportA",port0.getCell("Void"));
  jaws[0]->addInsertCell("SupportA",slitTube->getCell("SplitVoid",0));
  jaws[0]->addInsertCell("BlockA",slitTube->getCell("SplitVoid",0));
  jaws[0]->addInsertCell("SupportB",port0.getCell("Void"));
  jaws[0]->addInsertCell("SupportB",slitTube->getCell("SplitVoid",1));
  jaws[0]->addInsertCell("BlockB",slitTube->getCell("SplitVoid",1));
  jaws[0]->createAll(System,*slitTube,0,port0,port0.getSideIndex("InnerPlate"));

  const constructSystem::portItem& port1=slitTube->getPort(1);
  jaws[1]->addInsertCell("SupportA",port1.getCell("Void"));
  jaws[1]->addInsertCell("SupportA",slitTube->getCell("SplitVoid",0));
  jaws[1]->addInsertCell("BlockA",slitTube->getCell("SplitVoid",0));
  jaws[1]->addInsertCell("SupportB",port1.getCell("Void"));
  jaws[1]->addInsertCell("SupportB",slitTube->getCell("SplitVoid",1));
  jaws[1]->addInsertCell("BlockB",slitTube->getCell("SplitVoid",1));
  jaws[1]->createAll(System,*slitTube,0,port1,port1.getSideIndex("InnerPlate"));

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

  beamStopInPipe->createAll(System,*frontEnd,0);

  valve9->createAll(System,*beamStopInPipe,"front");
  bellowG->setBack(initFC,sideName);
  bellowG->createAll(System,*valve9,"back");

  bellowG->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowG,"front"));

  valve9->insertInCell(System,buildZoneDanMAX.createUnit(System,*valve9,"front"));

  int outerCell=buildZoneDanMAX.createUnit(System,*beamStopInPipe,"back");
  beamStopInPipe->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZoneDanMAX,*beamStopInPipe,"back",*beamStopSection);

  beamStopTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  beamStopTube->createAll(System,*beamStopSection,sideName);
  beamStopTube->insertAllInCell(System,buildZoneDanMAX.getLastCell("Unit"));

  const constructSystem::portItem& VPB=beamStopTube->getPort(1);

  beamStopTube->insertAllInCell(System,buildZoneDanMAX.createUnit(System,VPB,VPB.getSideIndex("OuterPlate")));
  beamStopSection->insertAllInCell(System,buildZoneDanMAX.getLastCell("Unit"));

  beamStop->addInsertCell(beamStopTube->getCell("Void"));
  beamStop->createAll(System,*beamStopTube,"Origin");

  constructSystem::constructUnit
    (System,buildZoneDanMAX,VPB,"OuterPlate",*beamStopOutPipe);

  constructSystem::constructUnit
    (System,buildZoneDanMAX,*beamStopOutPipe,"back",*monoSlitsTube);

  for(auto i: std::vector<size_t>{0,1}){
    const constructSystem::portItem& port0=monoSlitsTube->getPort(2*i);
    const constructSystem::portItem& port1=monoSlitsTube->getPort(2*i+1);
    monoSlits[i]->addInsertCell("SupportA",port1.getCell("Void"));
    monoSlits[i]->addInsertCell("SupportA",monoSlitsTube->getCell("Void"));
    monoSlits[i]->addInsertCell("BlockA",monoSlitsTube->getCell("Void"));
    monoSlits[i]->addInsertCell("SupportB",port0.getCell("Void"));
    monoSlits[i]->addInsertCell("SupportB",monoSlitsTube->getCell("Void"));
    monoSlits[i]->addInsertCell("BlockB",monoSlitsTube->getCell("Void"));
    // Both BeamPair elements are actually supposed to be constructed within a single
    // port. By setting the appropriate insert cells here and x- and y-step values
    // elsewhere, it will appear as if they were built in separate ports.
    monoSlits[i]->createAll(System,*monoSlitsTube,0,port0,
      port0.getSideIndex("InnerPlate"));
  }

  monoSlitsTube->insertAllInCell(System,buildZoneDanMAX.getLastCell("Unit")-1);

  constructSystem::constructUnit
    (System,buildZoneDanMAX,*monoSlitsTube,"back",*slitsAOut);

  monoSlitsTube->insertAllInCell(System,buildZoneDanMAX.getLastCell("Unit"));

  return;
}

void
danmaxOpticsLine::constructMonoShutter(Simulation& System,
				       const attachSystem::FixedComp& initFC,
				       const std::string& sideName)
  /*!
    Construct a monoshutter system
    \param System :: Simulation for building
    \param initFC :: FixedComp for start point
    \param sideName :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("danmaxOpticsLine","constructMonoShutter");

  monoShutter->createAll(System,*frontEnd,0);

  bellowK->setBack(initFC,initFC.getSideIndex(sideName));
  bellowK->createAll(System,*monoShutter,"front");

  bellowK->insertAllInCell(System,
    buildZoneDanMAX.createUnit(System,*bellowK,"front"));

  monoShutter->insertInCell(System,
    buildZoneDanMAX.createUnit(System,*monoShutter,"back"));

  constructSystem::constructUnit
    (System,buildZoneDanMAX,*monoShutter,"back",*bellowL);

  return;
}

void
danmaxOpticsLine::buildSplitter(Simulation& System,
				 const attachSystem::FixedComp& initFC,
				 const std::string& sideName)
  /*!
    Sub build of the spliter package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */

{
  ELog::RegMethod RegA("danmaxOpticsBeamLine","buildSplitter");

  int outerCell;

  cm1->setPortRotation(3,Geometry::Vec3D(1,0,0));
  cm1->createAll(System,*frontEnd,0);

  bellowC->setBack(initFC,sideName);
  bellowC->createAll(System,cm1->getPort(0),"OuterPlate");

  cm1->intersectPorts(System,1,2);

  const constructSystem::portItem& cm1PortDanMAX=cm1->getPort(1);
  const constructSystem::portItem& cm1PortSinCrys=cm1->getPort(2);

  bellowBA->createAll(System,cm1PortDanMAX,"OuterPlate"); // DanMAX

  buildZoneSinCrys=buildZone;
  buildZoneDanMAX=buildZone;

  const Geometry::Vec3D DPoint(cm1->getLinkPt(0));
  Geometry::Vec3D crossX,crossY,crossZ;

  cm1->calcLinkAxis(2,crossX,crossY,crossZ);
  const double midDividerAngle = 10.0;
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+5003,DPoint,crossX,-Z,-midDividerAngle);

  HeadRule HSurroundA=buildZone.getSurround();
  HeadRule HSurroundB=buildZone.getSurround();
  HSurroundA.removeMatchedPlanes(-X,0.9);   // remove left/right
  HSurroundB.removeMatchedPlanes(X,0.9);

  HSurroundA.addIntersection(-SMap.realSurf(buildIndex+5003));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5003));

  buildZoneSinCrys.setSurround(HSurroundA);
  buildZoneDanMAX.setSurround(HSurroundB);
  buildZoneSinCrys.setFront(initFC.getFullRule(sideName));
  buildZoneDanMAX.setFront(initFC.getFullRule(sideName));
  buildZoneSinCrys.clearInsertCells();
  buildZoneDanMAX.clearInsertCells();

  bellowC->insertAllInCell(System,
    buildZoneSinCrys.createUnit(System,*bellowC,"front"));
  bellowC->insertAllInCell(System,
    buildZoneDanMAX.createUnit(System,*bellowC,"front"));

  outerCell=buildZoneSinCrys.createUnit(System,cm1PortSinCrys,"OuterPlate");
  cm1->insertAllInCell(System,outerCell);
  constructSystem::constructUnit(
    System,buildZoneSinCrys,cm1PortSinCrys,"OuterPlate",*valveS1
  );

  constructSystem::constructUnit(
    System,buildZoneSinCrys,*valveS1,"back",*beamViewerS1
  );
  beamViewerS1->intersectPorts(System,0,2);
  beamViewerS1->intersectPorts(System,1,2);
  beamViewerS1Screen->addInsertCell("Body",beamViewerS1->getCell("Void"));
  beamViewerS1Screen->addInsertCell("Blade",beamViewerS1->getCell("Void"));
  beamViewerS1Screen->addInsertCell("Body",beamViewerS1->getPort(2).getCell("Void"));
  beamViewerS1Screen->setBladeCentre(beamViewerS1->getLinkPt(0));
  beamViewerS1Screen->createAll(System,beamViewerS1->getPort(2),"#InnerPlate");

  outerCell=buildZoneDanMAX.createUnit(System,*bellowBA,"back");
  cm1->insertAllInCell(System,outerCell);
  bellowBA->insertAllInCell(System,outerCell);
  valveS1->insertInCell(System,outerCell);
  bellowBA->insertAllInCell(System,valveS1->getCell("LowSpace"));

  constructSystem::constructUnit(
    System,buildZoneSinCrys,*beamViewerS1,"back",*cardanBellowUpstream
  );
  constructSystem::constructUnit(
    System,buildZoneSinCrys,*cardanBellowUpstream,"back",*pipeSinCrys
  );
  // constructSystem::constructUnit(
  //   System,buildZoneSinCrys,*pipeSinCrys,"back",*linearlyGuidedBellowUpstream
  // );
  // constructSystem::constructUnit(
  //   System,buildZoneSinCrys,
  //   *linearlyGuidedBellowUpstream,"back",*cardanBellowDownstream
  // );

  cm2->setPortRotation(4,Geometry::Vec3D(1,0,0));
  cm2->createAll(System,*frontEnd,0);

  cardanBellowDownstream->createAll(System,cm2->getPort(0),"OuterPlate");
  linearlyGuidedBellowUpstream->setBack(*pipeSinCrys,"back");
  linearlyGuidedBellowUpstream->createAll(System,*cardanBellowDownstream,"back");

  linearlyGuidedBellowUpstream->insertAllInCell(
    System,buildZoneSinCrys.createUnit(System,*linearlyGuidedBellowUpstream,"front")
  );
  cardanBellowDownstream->insertAllInCell(
    System,buildZoneSinCrys.createUnit(System,*cardanBellowDownstream,"front")
  );

  cm2->insertAllInCell(
    System,buildZoneSinCrys.createUnit(System,cm2->getPort(1),"OuterPlate"));

  outerCell=buildZoneSinCrys.createUnit(System);
  for (int i=0; i<1; ++i) {
    MonteCarlo::Object* OPtr = System.findObject(outerCell-i);
    OPtr->addIntersection(getRule("BackPlateFloorShine"));
  }

  constructSlitTube(System,*bellowBA,"back");


  constructSystem::constructUnit(System,buildZoneDanMAX,*slitTube,"back",*valve6);

  constructMono(System,*valve6,"back");
  constructViewScreen(System,*monoVessel,"back");
  constructMirrorMono(System,*valve8,"back");
  constructBeamStopTube(System,*MLMVessel,"back");

  constructViewScreenB(System,*slitsAOut,"back");

  lensBox->createAll(System,*frontEnd,0);

  CRLGateIn->createAll(System,*lensBox,"front");

  bellowI->setBack(*viewTubeB,"back");
  bellowI->createAll(System,*CRLGateIn,"back");

  bellowI->insertAllInCell(System,buildZoneDanMAX.createUnit(System,*bellowI,"front"));

  CRLGateIn->insertInCell(System,buildZoneDanMAX.createUnit(System,*CRLGateIn,"front"));

  lensBox->insertInCell(System,buildZoneDanMAX.createUnit(System,*lensBox,"back"));

  constructSystem::constructUnit(System,buildZoneDanMAX,*lensBox,"back",*CRLGateOut);

  constructRevBeamStopTube(System,*CRLGateOut,"back");
  
  constructMonoShutter(System,revBeamStopTube->getPort(1),"OuterPlate");

  outerCell = buildZoneDanMAX.createUnit(System);
  for (int i=0; i<4; ++i) {
    MonteCarlo::Object* OPtr = System.findObject(outerCell-i);
    OPtr->addIntersection(getRule("BackPlateFloorShine"));
  }

  buildZoneDanMAX.rebuildInsertCells(System);
  setCell("LastVoid",buildZoneDanMAX.getCells("Unit").back());
  lastComp=bellowL;

  // Intersect the 2nd buildZoneSinCrys cell (tilted plane, contains CM1) with cells 
  // from the SINCRYS and common build zone.
  for(int n = 0; n < 3; ++n){
    MonteCarlo::Object* SUnit=System.findObject(buildZoneSinCrys.getCell("Unit",10-n));
    SUnit->addIntersection(
      HeadRule(cm1PortSinCrys.getLinkSurf("OuterPlate")).complement());
  }

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

  buildZone.addInsertCells(this->getInsertCells());

  valve4->createAll(System,*frontEnd,0);
  triggerPipe->setRotation(0.0, 180.0, 0.0);
  triggerPipe->createAll(System,*valve4,"#front");
  pipeInit->setBack(*triggerPipe,"back");
  pipeInit->createAll(System,*this,0);

  if(preInsert)
    preInsert->insertAllInCell(System,buildZone.createUnit(System,*pipeInit,"#front"));

  pipeInit->insertAllInCell(System,buildZone.createUnit(System,*pipeInit,"back"));

  triggerPipe->insertInCell(System,buildZone.createUnit(System,*triggerPipe,"front"));

  valve4->insertInCell(System,buildZone.createUnit(System,*valve4,"back"));

  constructBremColl1Tube(System, *valve4, "back");

  constructSystem::constructUnit
    (System,buildZone,*highPassFilter,"back",*valve5);

  constructSystem::constructUnit(System,buildZone,*valve5,"back",*pipeA);

  buildZone.createUnit(System);         // build to end (removed later)
  buildZone.rebuildInsertCells(System); // rebuild the whole track

  buildSplitter(System,*pipeA,"back");
  System.removeCell(buildZone.getLastCell("Unit"));  // remove cell built above

  lastComp=bellowL;

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
