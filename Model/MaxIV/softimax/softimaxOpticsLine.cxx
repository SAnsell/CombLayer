/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: softimax/softimaxOpticsLine.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "generateSurf.h"
#include "generalConstruct.h"
#include "insertObject.h"
#include "insertPlate.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "FlangeMount.h"
#include "BremOpticsColl.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "BlockStand.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawFlange.h"
#include "JawValveBase.h"
#include "JawValveTube.h"
#include "TankMonoVessel.h"
#include "GratingUnit.h"
#include "Mirror.h"
#include "BeamPair.h"
#include "TwinPipe.h"
#include "BiPortTube.h"
#include "PipeShield.h"
#include "TriggerTube.h"
#include "CylGateValve.h"
#include "softimaxOpticsLine.h"

#include "Surface.h"

namespace xraySystem
{

// Note currently uncopied:

softimaxOpticsLine::softimaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),

  buildZone(Key+"BuildZone"),
  
  IZLeft(Key+"IZLeft"),
  IZRight(Key+"IZRight"),

  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  pumpM1(new constructSystem::PipeTube(newName+"PumpM1")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  M1TubeFront(new constructSystem::OffsetFlangePipe(newName+"M1TubeFront")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1TubeBack(new constructSystem::OffsetFlangePipe(newName+"M1TubeBack")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  M1Stand(new xraySystem::BlockStand(newName+"M1Stand")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pumpTubeA(new constructSystem::PipeTube(newName+"PumpTubeA")),
  bremCollA(new xraySystem::BremOpticsColl(newName+"BremCollA")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  slitTube(new constructSystem::PortTube(newName+"SlitTube")),
  jaws({
	std::make_shared<xraySystem::BeamPair>(newName+"JawX"),
	std::make_shared<xraySystem::BeamPair>(newName+"JawZ")
    }),
  monoVessel(new xraySystem::TankMonoVessel(newName+"MonoVessel")),
  grating(new xraySystem::GratingUnit(newName+"Grating")),
  zeroOrderBlock(new xraySystem::FlangeMount(newName+"ZeroOrderBlock")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  pumpTubeB(new constructSystem::PipeTube(newName+"PumpTubeB")),
  gateD(new constructSystem::GateValveCube(newName+"GateD")),
  joinPipeA(new constructSystem::VacuumPipe(newName+"JoinPipeA")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  slitsA(new constructSystem::JawValveTube(newName+"SlitsA")),
  pumpTubeM3(new constructSystem::PipeTube(newName+"PumpTubeM3")),
  pumpTubeM3Baffle(new xraySystem::FlangeMount(newName+"PumpTubeM3Baffle")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  M3Front(new constructSystem::VacuumPipe(newName+"M3Front")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  M3Mirror(new xraySystem::Mirror(newName+"M3Mirror")),
  M3Stand(new xraySystem::BlockStand(newName+"M3Stand")),
  M3Back(new constructSystem::VacuumPipe(newName+"M3Back")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  gateE(new constructSystem::GateValveCube(newName+"GateE")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  pumpTubeC(new constructSystem::PipeTube(newName+"PumpTubeC")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  joinPipeC(new constructSystem::VacuumPipe(newName+"JoinPipeC")),
  gateF(new constructSystem::GateValveCube(newName+"GateF")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  M3STXMFront(new constructSystem::VacuumPipe(newName+"M3STXMFront")),
  M3STXMTube(new constructSystem::PipeTube(newName+"M3STXMTube")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),
  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  M3Pump(new xraySystem::BiPortTube(newName+"M3Pump")),
  bellowAB(new constructSystem::Bellows(newName+"BellowAB")),
  joinPipeAA(new constructSystem::VacuumPipe(newName+"JoinPipeAA")),
  bremCollAA(new xraySystem::BremOpticsColl(newName+"BremCollAA")),
  joinPipeAB(new constructSystem::VacuumPipe(newName+"JoinPipeAB")),
  bellowBB(new constructSystem::Bellows(newName+"BellowBB")),
  joinPipeBA(new constructSystem::VacuumPipe(newName+"JoinPipeBA")),
  bremCollBA(new xraySystem::BremOpticsColl(newName+"BremCollBA")),
  joinPipeBB(new constructSystem::VacuumPipe(newName+"JoinPipeBB")),

  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  innerScreen(new insertSystem::insertPlate(newName+"InnerScreen"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeInit);
  OR.addObject(triggerPipe);
  OR.addObject(gateTubeA);
  OR.addObject(bellowA);
  OR.addObject(pipeA);
  OR.addObject(pumpM1);
  OR.addObject(gateA);
  OR.addObject(bellowB);
  OR.addObject(M1TubeFront);
  OR.addObject(M1Tube);
  OR.addObject(M1TubeBack);
  OR.addObject(M1Mirror);
  OR.addObject(M1Stand);
  OR.addObject(bellowC);
  OR.addObject(pumpTubeA);
  OR.addObject(bremCollA);
  OR.addObject(gateB);
  OR.addObject(bellowD);
  OR.addObject(slitTube);
  OR.addObject(jaws[0]);
  OR.addObject(jaws[1]);
  OR.addObject(monoVessel);
  OR.addObject(grating);
  OR.addObject(zeroOrderBlock);
  OR.addObject(gateC);
  OR.addObject(bellowE);
  OR.addObject(pumpTubeB);
  OR.addObject(gateD);
  OR.addObject(joinPipeA);
  OR.addObject(bellowF);
  OR.addObject(slitsA);
  OR.addObject(pumpTubeM3);
  OR.addObject(pumpTubeM3Baffle);
  OR.addObject(bellowG);
  OR.addObject(M3Front);
  OR.addObject(M3Tube);
  OR.addObject(M3Mirror);
  OR.addObject(M3Stand);
  OR.addObject(M3Back);
  OR.addObject(bellowH);
  OR.addObject(gateE);
  OR.addObject(joinPipeB);
  OR.addObject(pumpTubeC);
  OR.addObject(joinPipeC);
  OR.addObject(gateF);
  OR.addObject(bellowJ);
  OR.addObject(M3STXMFront);
  OR.addObject(M3STXMTube);
  OR.addObject(splitter);
  OR.addObject(bellowAA);
  OR.addObject(bellowBA);
  OR.addObject(M3Pump);
  OR.addObject(bellowAB);
  OR.addObject(joinPipeAA);
  OR.addObject(bremCollAA);
  OR.addObject(joinPipeAB);
  OR.addObject(bellowBB);
  OR.addObject(joinPipeBA);
  OR.addObject(bremCollBA);
  OR.addObject(joinPipeBB);
  OR.addObject(screenA);
  OR.addObject(innerScreen);
}

softimaxOpticsLine::~softimaxOpticsLine()
  /*!
    Destructor
   */
{}

void
softimaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  return;
}


void
softimaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("softimaxOpticsLine","createSurface");

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
softimaxOpticsLine::buildM1Mirror(Simulation& System,
				  const attachSystem::FixedComp& initFC,
				  const std::string& sideName)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM1Mirror");

  int outerCell;
  
  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*M1TubeFront);
  
  
  M1Tube->setFront(*M1TubeFront,11);
  M1Tube->createAll(System,*M1TubeFront,11);
  outerCell=buildZone.createUnit(System,*M1Tube,2);
  M1Tube->insertAllInCell(System,outerCell);

  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);

  M1Stand->setCutSurf("floor",this->getRule("floor"));
  M1Stand->setCutSurf("front",*M1Tube,-1);
  M1Stand->setCutSurf("back",*M1Tube,-2);
  M1Stand->addInsertCell(outerCell);
  M1Stand->createAll(System,*M1Tube,0);

  constructSystem::constructUnit
    (System,buildZone,*M1Tube,"back",*M1TubeBack);

  return;
}

void
softimaxOpticsLine::buildM3Mirror(Simulation& System,
				  const attachSystem::FixedComp& initFC,
				  const std::string& sideName)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM3Mirror");

  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*M3Front);

  M3Tube->setFront(*M3Front,2);
  M3Tube->createAll(System,*M3Front,2);
  outerCell=buildZone.createUnit(System,*M3Tube,2);
  M3Tube->insertAllInCell(System,outerCell);

  M3Mirror->addInsertCell(M3Tube->getCell("Void"));
  M3Mirror->createAll(System,*M3Tube,0);

  M3Stand->setCutSurf("floor",this->getRule("floor"));
  M3Stand->setCutSurf("front",*M3Tube,-1);
  M3Stand->setCutSurf("back",*M3Tube,-2);
  M3Stand->addInsertCell(outerCell);
  M3Stand->createAll(System,*M3Tube,0);

  constructSystem::constructUnit
    (System,buildZone,*M3Tube,"back",*M3Back);

  return;
}

void
softimaxOpticsLine::buildM3STXMMirror(Simulation& System,
				      const attachSystem::FixedComp& initFC,
				      const std::string& sideName)
  /*!
    Sub build of the m3-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM3STXMMirror");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*M3STXMFront);

  constructSystem::constructUnit
    (System,buildZone,*M3STXMFront,"back",*M3STXMTube);


  return;
}

void
softimaxOpticsLine::constructSlitTube(Simulation& System,
				      const attachSystem::FixedComp& initFC,
				      const std::string& sideName)
  /*!
    Build the Baffle before Monochromator
    \param System :: Simuation to use
   */
{
  ELog::RegMethod RegA("softimaxOpticsLine","buildSlitTube");

  int outerCell;

  // FAKE insertcell: required
  //  slitTube->addAllInsertCell(masterCell->getName());
  slitTube->createAll(System,initFC,sideName);
  slitTube->intersectPorts(System,0,2);
  slitTube->intersectPorts(System,0,1);
  outerCell=buildZone.createUnit(System,*slitTube,2);
  // split front/back ends off
  const int newCellB=
    slitTube->splitObject(System,"PortBCut",outerCell).back();
  this->addCell("OuterVoid",newCellB);
  const int newCellA=
    slitTube->splitObject(System,"-PortACut",outerCell).back();
  this->addCell("OuterVoid",newCellA);
  cellIndex+=2;  // remember creates an extra cell in  primary

  slitTube->insertInCell("FlangeA",System,newCellA);
  slitTube->insertInCell("FlangeB",System,newCellB);
  slitTube->insertInCell("Main",System,outerCell);
  slitTube->insertPortInCell(System,outerCell);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
			   slitTube->getCell("Void"),
			   Geometry::Vec3D(0,1,0));

  std::vector<int> cellItems=
    slitTube->splitObject(System,1501,outerCell,
			  Geometry::Vec3D(0,0,0),
			  Geometry::Vec3D(0,0,1));
  this->addCell("OuterVoid",cellItems[1]);

  // Note need cell number later
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
softimaxOpticsLine::buildMono(Simulation& System,
			      const attachSystem::FixedComp& initFC,
			      const long int sideIndex)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildMono");

  int outerCell;

  // offPipeA->createAll(System,initFC,sideIndex);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeA,2);
  // offPipeA->insertInCell(System,outerCell);

  monoVessel->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createUnit(System,*monoVessel,2);
  monoVessel->insertInCell(System,outerCell);

  grating->addInsertCell(monoVessel->getCell("Void"));
  grating->copyCutSurf("innerCylinder",*monoVessel,"innerRadius");
  grating->createAll(System,*monoVessel,0);

  zeroOrderBlock->addInsertCell("Body", monoVessel->getCell("Void"));
  zeroOrderBlock->addInsertCell("Blade", monoVessel->getCell("Void"));
  zeroOrderBlock->setFront(*monoVessel,3);
  zeroOrderBlock->createAll(System,*monoVessel,3);

  return;
}

void
softimaxOpticsLine::createSplitZone()
  /*!
    Split the innerZone into two parts.
   */
{
  ELog::RegMethod RegA("Segment26","createSplitInnerZone");


  

  
//  const HeadRule HBack=buildZone->getBack();
//  ELog::EM<<"BACK == "<<HBack<<ELog::endDiag;
  /*
  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*pipeAA,2,*pipeBA,2);
  FB.createPairVector(*pipeBA,2,*pipeCA,2);

  ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
  ModelSupport::buildPlane(SMap,buildIndex+5015,FB.getCentre(),FB.getZ());
  SurfMap::addSurf("TopDivider",SMap.realSurf(buildIndex+5005));
  SurfMap::addSurf("LowDivider",SMap.realSurf(buildIndex+5015));

  const Geometry::Vec3D ZEffective(FA.getZ());

  HSurroundA.removeMatchedPlanes(ZEffective,0.9);   // remove base
  HSurroundB.removeMatchedPlanes(ZEffective,0.9);   // remove both
  HSurroundB.removeMatchedPlanes(-ZEffective,0.9);

  HSurroundA.addIntersection(SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5015));


  IZTop.setSurround(HSurroundA);
  IZLow.setSurround(HSurroundB);
  */
  return;
}


void
softimaxOpticsLine::buildSplitter(Simulation& System,
				  const attachSystem::FixedComp& initFC,
				  const long int sideIndex)
  /*!
    Sub build of the spliter package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamLine","buildSplitter");

  int outerCell;


  splitter->createAll(System,initFC,sideIndex);
  bellowAA->createAll(System,*splitter,2);
  bellowBA->createAll(System,*splitter,3);
  
  IZLeft=buildZone;
  IZRight=buildZone;
  IZLeft.setFront(buildZone.getBack());
  IZRight.setFront(buildZone.getBack());

  // build mid plane

  const Geometry::Vec3D DPoint(initFC.getLinkPt(sideIndex));
  Geometry::Vec3D crossX,crossY,crossZ;

  initFC.selectAltAxis(sideIndex,crossX,crossY,crossZ);
  SurfMap::makePlane("midDivider",SMap,buildIndex+5003,DPoint,crossX);

  HeadRule HSurroundA=buildZone.getSurround();
  HeadRule HSurroundB=buildZone.getSurround();

  HSurroundA.removeMatchedPlanes(-X,0.9);   // remove left/right
  HSurroundB.removeMatchedPlanes(X,0.9);

  HSurroundA.addIntersection(-SMap.realSurf(buildIndex+5003));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5003));

  IZLeft.setSurround(HSurroundA);
  IZRight.setSurround(HSurroundB);

  outerCell=IZLeft.createUnit(System,*bellowAA,"back");
  splitter->insertAllInCell(System,outerCell);
  bellowAA->insertInCell(System,outerCell);
  outerCell=IZRight.createUnit(System,*bellowBA,"back");
  splitter->insertAllInCell(System,outerCell);
  bellowBA->insertInCell(System,outerCell);


  M3Pump->setLeftPort(*bellowAA,"back");
  M3Pump->setRightPort(*bellowBA,"back");
  M3Pump->createAll(System,initFC,sideIndex);

  
  return;

  /*
  const constructSystem::portItem& CPI2=M3Pump->getPort(2);
  cellA=leftZone.createOuterVoidUnit
    (System,masterCellA,CPI2,CPI2.getSideIndex("OuterPlate"));
  const constructSystem::portItem& CPI3=M3Pump->getPort(3);
  cellB=rightZone.createOuterVoidUnit
    (System,masterCellB,CPI3,CPI3.getSideIndex("OuterPlate"));
  M3Pump->insertAllInCell(System,cellA);
  M3Pump->insertAllInCell(System,cellB);


    
  // now build left/ right
  // LEFT
  constructSystem::constructUnit
    (System,leftZone,masterCellA,CPI2,"OuterPlate",*bellowAB);
  constructSystem::constructUnit
    (System,leftZone,masterCellA,*bellowAB,"back",*joinPipeAA);

  constructSystem::constructUnit
    (System,leftZone,masterCellA,*joinPipeAA,"back",*bremCollAA);

  // constructSystem::constructUnit
  //   (System,leftZone,masterCellA,*bremCollAA,"back",*joinPipeAB);

  // RIGHT
  constructSystem::constructUnit
    (System,rightZone,masterCellB,CPI3,"OuterPlate",*bellowBB);

  constructSystem::constructUnit
    (System,rightZone,masterCellB,*bellowBB,"back",*joinPipeBA);

  constructSystem::constructUnit
    (System,rightZone,masterCellB,*joinPipeBA,"back",*bremCollBA);

  // constructSystem::constructUnit
  //   (System,rightZone,masterCellB,*bremCollBA,"back",*joinPipeBB);


  // gateAA->createAll(System,*bellowAB,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*gateAA,2);
  // gateAA->insertInCell(System,cellA);

  // // make build necessary
  // pumpTubeAA->addAllInsertCell(masterCellA->getName());
  // pumpTubeAA->createAll(System,*gateAA,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*pumpTubeAA,2);
  // pumpTubeAA->insertAllInCell(System,cellA);


  // // RIGHT
  // bellowBB->createAll(System,*splitter,3);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBB,2);
  // bellowBB->insertInCell(System,cellB);

  // gateBA->createAll(System,*bellowBB,2);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*gateBA,2);
  // gateBA->insertInCell(System,cellB);

  // pumpTubeBA->addAllInsertCell(masterCellB->getName());
  // pumpTubeBA->createAll(System,*gateBA,2);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*pumpTubeBA,2);
  // pumpTubeBA->insertAllInCell(System,cellB);

  // Get last two cells
  setCell("LeftVoid",masterCellA->getName());
  setCell("RightVoid",masterCellB->getName());
*/
  return;
}



void
softimaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("softimaxOpticsLine","buildObjects");

  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  // dump cell for joinPipe
  outerCell=buildZone.createUnit(System,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*pipeA);

  pumpM1->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpM1->setOuterVoid();
  pumpM1->createAll(System,*pipeA,"back");
  
  ///////////// split for FLUKA
  //  const constructSystem::portItem& VP0=pumpM1->getPort(0);
  const constructSystem::portItem& VP1=pumpM1->getPort(1);
  const constructSystem::portItem& VP2=pumpM1->getPort(2);
  //  const constructSystem::portItem& VP3=pumpM1->getPort(3);
  const constructSystem::portItem& VP4=pumpM1->getPort(4);
  //  const constructSystem::portItem& VP5=pumpM1->getPort(5);
  const constructSystem::portItem& VP6=pumpM1->getPort(6);

  ELog::EM<<"Point == "<<VP1.getLinkPt("OuterPlate")<<ELog::endDiag;
  outerCell=buildZone.createUnit(System,VP1,"OuterPlate");
  ELog::EM<<"Oyter Cell == "<<outerCell<<ELog::endDiag;
  pumpM1->insertAllInCell(System,outerCell);

  /*
  const Geometry::Vec3D  Axis12=pumpM1->getY()*(VP1.getY()+VP2.getY())/2.0;
  const Geometry::Vec3D  Axis26=pumpM1->getY()*(VP2.getY()+VP6.getY())/2.0;

  
  this->splitObjectAbsolute(System,1501,outerCell,
			    (VP1.getCentre()+VP4.getCentre())/2.0,
  			    Z);
  cellIndex++;


  /*  this->splitObjectAbsolute(System,1502,outerCell+1,
    			    pumpM1->getCentre(),
			    VP4.getY());
  this->splitObjectAbsolute(System,1503,outerCell,
			    pumpM1->getCentre(),Axis12);
  this->splitObjectAbsolute(System,1504,outerCell+3,
   			    pumpM1->getCentre(),Axis26);
  const std::vector<int> cellUnit=this->getCells("OuterVoid");
  pumpM1->insertMainInCell(System,cellUnit);

  pumpM1->insertPortInCell
    (System,{{outerCell+4,outerCell},{outerCell,outerCell+1,outerCell+2},{outerCell+3},{outerCell},
	     {outerCell+2},{outerCell+1},
	     {outerCell+4}});
  
  cellIndex+=5;
  */
  /////////////////////////////////////////
  

  constructSystem::constructUnit
    (System,buildZone,VP1,"OuterPlate",*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*bellowB);

  buildM1Mirror(System,*bellowB,"back");

  
  constructSystem::constructUnit
    (System,buildZone,*M1TubeBack,"back",*bellowC);

  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*bellowC,2);

  const constructSystem::portItem& CPI1=pumpTubeA->getPort(1);
  outerCell=buildZone.createUnit
    (System,CPI1,CPI1.getSideIndex("OuterPlate"));
  pumpTubeA->insertAllInCell(System,outerCell);
  //  pumpTubeA->intersectPorts(System,1,2);

  constructSystem::constructUnit
    (System,buildZone,CPI1,"OuterPlate",*bremCollA);

  constructSystem::constructUnit
    (System,buildZone,*bremCollA,"back",*gateB);

  //  bremCollA->createExtension(System,gateB->getCell("Void")); // !!! UGLY - it does not actually intersect gateB

  constructSystem::constructUnit
    (System,buildZone,*gateB,"back",*bellowD);
  
  constructSlitTube(System,*bellowD,"back");

  buildMono(System,*slitTube,2);

  
  constructSystem::constructUnit
    (System,buildZone,*monoVessel,"back",*gateC);

  constructSystem::constructUnit
    (System,buildZone,*gateC,"back",*bellowE);

  pumpTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeB->createAll(System,*bellowE,2);

  const constructSystem::portItem& pumpTubeBCPI=pumpTubeB->getPort(1);
  outerCell=buildZone.createUnit
    (System,pumpTubeBCPI,pumpTubeBCPI.getSideIndex("OuterPlate"));
  pumpTubeB->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,pumpTubeBCPI,"OuterPlate",*gateD);

  constructSystem::constructUnit
    (System,buildZone,*gateD,"back",*joinPipeA);

  constructSystem::constructUnit
    (System,buildZone,*joinPipeA,"back",*bellowF);

  constructSystem::constructUnit
    (System,buildZone,*bellowF,"back",*slitsA);

  pumpTubeM3->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeM3->createAll(System,*slitsA,"back");

  const constructSystem::portItem& GPI=pumpTubeM3->getPort(1);
  outerCell=buildZone.createUnit(System,GPI,"OuterPlate");
  pumpTubeM3->insertAllInCell(System,outerCell);

  pumpTubeM3Baffle->addInsertCell("Body",pumpTubeM3->getCell("Void"));
  pumpTubeM3Baffle->setBladeCentre(*pumpTubeM3,0);
  pumpTubeM3Baffle->createAll(System,*pumpTubeM3,std::string("InnerBack"));

  constructSystem::constructUnit
    (System,buildZone,GPI,"OuterPlate",*bellowG);

  buildM3Mirror(System,*bellowG,"back");

  constructSystem::constructUnit
    (System,buildZone,*M3Back,"back",*bellowH);

  constructSystem::constructUnit
    (System,buildZone,*bellowH,"back",*gateE);

  constructSystem::constructUnit
    (System,buildZone,*gateE,"back",*joinPipeB);

  //// pumpTubeC
  pumpTubeC->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeC->createAll(System,*joinPipeB,2);

  const constructSystem::portItem& pumpTubeCCPI=pumpTubeC->getPort(1);
  outerCell=buildZone.createUnit(System,pumpTubeCCPI,"OuterPlate");
  pumpTubeC->insertAllInCell(System,outerCell);
  ///////////////////////////////////////////////

  constructSystem::constructUnit
    (System,buildZone,pumpTubeCCPI,"back",*bellowI);

  
  constructSystem::constructUnit
    (System,buildZone,*bellowI,"back",*joinPipeC);

  constructSystem::constructUnit
    (System,buildZone,*joinPipeC,"back",*gateF);

  constructSystem::constructUnit
    (System,buildZone,*gateF,"back",*bellowJ);

  buildM3STXMMirror(System,*bellowJ,"back");
   
  //  MonteCarlo::Object* masterCellB(0);
  buildSplitter(System,*M3STXMTube,2);


  //  setCell("LastVoid",masterCell->getName());
  lastComp=bellowA; //gateJ;
  return;
}

void
softimaxOpticsLine::buildOutGoingPipes(Simulation& System,
				       const int leftCell,
				       const int rightCell,
				       const std::vector<int>& hutCells)
  /*!
    Construct outgoing tracks
    \param System :: Simulation
    \param leftCell :: additional left cell for insertion
    \param rightCell :: additional right cell for insertion
    \param hutCell :: Cells for construction in hut [common to both pipes]
  */
{
  ELog::RegMethod RegA("softimaxOpticsLine","buildOutgoingPipes");
  /*
  joinPipeAB->addAllInsertCell(hutCells);
  joinPipeAB->addAllInsertCell(leftCell);
  joinPipeAB->createAll(System,*bremCollAA,2);

  joinPipeBB->addAllInsertCell(hutCells);
  joinPipeBB->addAllInsertCell(rightCell);
  joinPipeBB->createAll(System,*bremCollBA,2);

  screenA->addAllInsertCell(rightCell);
  screenA->addAllInsertCell(leftCell);
  screenA->setCutSurf("inner",*joinPipeAB,"pipeOuterTop");
  screenA->setCutSurf("innerTwo",*joinPipeBA,"pipeOuterTop");
  screenA->createAll(System,*bremCollAA,2);

  // lineScreen->setNoInsert();
  // lineScreen->addInsertCell(ContainedComp::getInsertCells());
  // lineScreen->createAll(System,*gateA,"back");

  // innerScreen->setNoInsert();
  // for(size_t index=12;index<=20;index++)
  //   innerScreen->addInsertCell(this->getCell("OuterVoid",index));

  innerScreen->addInsertCell(this->getCell("OuterVoid",23));
  innerScreen->createAll(System,*gateA,"back");
  */
  return;
}


void
softimaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("softimaxOpticsLine","createLinks");

  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}


void
softimaxOpticsLine::createAll(Simulation& System,
			      const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("softimaxOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem
