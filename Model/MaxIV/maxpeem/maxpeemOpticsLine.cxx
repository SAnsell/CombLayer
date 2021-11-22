/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/maxpeemOpticsLine.cxx
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
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
#include "generalConstruct.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

#include "BlockStand.h"
#include "TriggerTube.h"
#include "CrossPipe.h"
#include "CrossWayTube.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCube.h"
#include "BeamMount.h"
#include "GrateMonoBox.h"
#include "GratingMono.h"
#include "TwinPipe.h"
#include "Mirror.h"
#include "maxpeemOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
maxpeemOpticsLine::maxpeemOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  buildZone(newName+"BuildZone"),
  IZLeft(newName+"IZLeft"),
  IZRight(newName+"IZRight"),
  innerMat(0),
  
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerPipe")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  screenExtra(new xraySystem::PipeShield(newName+"ScreenExtra")),
  pumpTubeA(new constructSystem::PipeTube(newName+"PumpTubeA")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  M1Stand(new xraySystem::BlockStand(newName+"M1Stand")),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusZ"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusZ")}),  
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  monoB(new xraySystem::GrateMonoBox(newName+"MonoBox")),
  monoXtal(new xraySystem::GratingMono(newName+"MonoXtal")),
  pipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  slitsB(new constructSystem::JawValveCube(newName+"SlitsB")),
  pumpTubeB(new constructSystem::PipeTube(newName+"PumpTubeB")),
  offPipeC(new constructSystem::OffsetFlangePipe(newName+"OffPipeC")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  M3Mirror(new xraySystem::Mirror(newName+"M3Mirror")),
  offPipeD(new constructSystem::OffsetFlangePipe(newName+"OffPipeD")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),
  screenB(new xraySystem::PipeShield(newName+"ScreenB")),
  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  gateAA(new constructSystem::GateValveCube(newName+"GateAA")),
  pumpTubeAA(new constructSystem::PortTube(newName+"PumpTubeAA")),
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  gateBA(new constructSystem::GateValveCube(newName+"GateBA")),
  pumpTubeBA(new constructSystem::PortTube(newName+"PumpTubeBA")),
  outPipeA(new constructSystem::VacuumPipe(newName+"OutPipeA")),
  outPipeB(new constructSystem::VacuumPipe(newName+"OutPipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(const auto& JM : jaws)
    OR.addObject(JM);

  OR.addObject(bellowA);
  OR.addObject(triggerPipe);
  OR.addObject(gateTubeA);
  OR.addObject(bellowB);
  OR.addObject(pipeA);
  OR.addObject(florTubeA);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(screenExtra);
  OR.addObject(pumpTubeA);
  OR.addObject(offPipeA);
  OR.addObject(M1Tube);
  OR.addObject(M1Mirror);
  OR.addObject(M1Stand);
  OR.addObject(offPipeB);
  OR.addObject(gateA);
  OR.addObject(pipeC);
  OR.addObject(screenA);
  OR.addObject(pipeD);
  OR.addObject(slitTube);
  OR.addObject(pipeE);
  OR.addObject(gateB);
  OR.addObject(bellowD);
  OR.addObject(pipeF);
  OR.addObject(monoB);
  OR.addObject(monoXtal);
  OR.addObject(pipeG);
  OR.addObject(gateC);
  OR.addObject(bellowE);
  OR.addObject(viewTube);
  OR.addObject(slitsB);
  OR.addObject(pumpTubeB);
  OR.addObject(offPipeC);
  OR.addObject(M3Tube);
  OR.addObject(M3Mirror);
  OR.addObject(offPipeD);
  OR.addObject(splitter);
  OR.addObject(screenB);
  OR.addObject(bellowAA);
  OR.addObject(gateAA);
  OR.addObject(pumpTubeAA);
  OR.addObject(bellowBA);
  OR.addObject(gateBA);  
  OR.addObject(pumpTubeBA);
  OR.addObject(outPipeA);
  OR.addObject(outPipeB);
}
  
maxpeemOpticsLine::~maxpeemOpticsLine()
  /*!
    Destructor
   */
{}

void
maxpeemOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","populate");
  FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  const int voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  buildZone.setInnerMat(voidMat);

  return;
}


void
maxpeemOpticsLine::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","createSurfaces");

  if (outerLeft>Geometry::zeroTol &&
      isActive("floor") &&
      isActive("roof"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      const HeadRule HR=
	ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");

      buildZone.setSurround(HR*getRule("floor")*getRule("roof"));
      buildZone.setFront(getRule("front"));
      buildZone.setMaxExtent(getRule("back"));
      buildZone.setInnerMat(innerMat);
    }
  return;
}

void
maxpeemOpticsLine::buildSplitter(Simulation& System,
				 const attachSystem::FixedComp& initFC,
				 const std::string& sideName)
  /*!
    Sub build of the spliter package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */

{
  ELog::RegMethod RegA("maxpeemOpticsBeamLine","buildSplitter");

  int outerCell;
  
  splitter->createAll(System,initFC,sideName);
  bellowAA->createAll(System,*splitter,2);
  bellowBA->createAll(System,*splitter,3);
  
  IZLeft=buildZone;
  IZRight=buildZone;

  const Geometry::Vec3D DPoint(initFC.getLinkPt(sideName));
  Geometry::Vec3D crossX,crossY,crossZ;

  initFC.selectAltAxis(2,crossX,crossY,crossZ);
  SurfMap::makePlane("midDivider",SMap,buildIndex+5003,DPoint,crossX);

  HeadRule HSurroundA=buildZone.getSurround();
  HeadRule HSurroundB=buildZone.getSurround();
  HSurroundA.removeMatchedPlanes(-X,0.9);   // remove left/right
  HSurroundB.removeMatchedPlanes(X,0.9);

  HSurroundA.addIntersection(-SMap.realSurf(buildIndex+5003));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5003));


  IZLeft.setSurround(HSurroundA);
  IZRight.setSurround(HSurroundB);
  IZLeft.setFront(initFC.getFullRule(sideName));
  IZRight.setFront(initFC.getFullRule(sideName));
  IZLeft.clearInsertCells();
  IZRight.clearInsertCells();

  outerCell=IZLeft.createUnit(System,*bellowAA,"back");
  splitter->insertAllInCell(System,outerCell);
  bellowAA->insertInCell(System,outerCell);
  outerCell=IZRight.createUnit(System,*bellowBA,"back");

  splitter->insertAllInCell(System,outerCell);
  bellowBA->insertInCell(System,outerCell);

  // LEFT SIDE:

  constructSystem::constructUnit
    (System,IZLeft,*bellowAA,"back",*gateAA);

  constructSystem::constructUnit
    (System,IZLeft,*gateAA,"back",*pumpTubeAA);

  outerCell=IZLeft.createUnit(System);
  
  outPipeA->createAll(System,*pumpTubeAA,"back");
  outPipeA->insertAllInCell(System,outerCell);

  // RIGHT SIDE
  constructSystem::constructUnit
    (System,IZRight,*bellowBA,"back",*gateBA);

  constructSystem::constructUnit
    (System,IZRight,*gateBA,"back",*pumpTubeBA);


  outerCell=IZRight.createUnit(System);
  
  outPipeB->createAll(System,*pumpTubeBA,"back");
  outPipeB->insertAllInCell(System,outerCell);


  // FINAL:
    // Get last two cells
  setCell("LeftVoid",IZLeft.getLastCell("Unit"));
  setCell("RightVoid",IZRight.getLastCell("Unit"));

  return;
}


void
maxpeemOpticsLine::buildM3Mirror(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the m3-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildM3Mirror");


  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*viewTube);

  constructSystem::constructUnit
    (System,buildZone,*viewTube,"back",*slitsB);

  pumpTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeB->createAll(System,*slitsB,"back");

  const constructSystem::portItem& CPI=pumpTubeB->getPort(1);
  outerCell=buildZone.createUnit(System,CPI,"OuterPlate");
  pumpTubeB->insertAllInCell(System,outerCell);
  pumpTubeB->splitObjectAbsolute(System,1501,outerCell,
				 pumpTubeB->getLinkPt(0),
				 Geometry::Vec3D(0,0,1));

  constructSystem::constructUnit
    (System,buildZone,CPI,"OuterPlate",*offPipeC);

  constructSystem::constructUnit
    (System,buildZone,*offPipeC,"FlangeBCentre",*M3Tube);

  M3Mirror->addInsertCell(M3Tube->getCell("Void"));
  M3Mirror->createAll(System,*M3Tube,0);

  return;
}

void
maxpeemOpticsLine::buildMono(Simulation& System,
			     const attachSystem::FixedComp& initFC, 
			     const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildMono");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoB);
  

  monoXtal->addInsertCell(monoB->getCell("Void"));
  monoXtal->createAll(System,*monoB,0);

  constructSystem::constructUnit
    (System,buildZone,*monoB,"back",*pipeG);

  constructSystem::constructUnit
    (System,buildZone,*pipeG,"back",*gateC);

  constructSystem::constructUnit
    (System,buildZone,*gateC,"back",*bellowE);

  return;
}


void
maxpeemOpticsLine::buildSlitPackage(Simulation& System,
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
  ELog::RegMethod RegA("maxpeemOpticsLine","buildSlitPackage");

  int outerCell;
  
  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*pipeD);

  slitTube->createAll(System,*pipeD,"back");
  outerCell=
    buildZone.createUnit(System,*slitTube,"back");
  slitTube->setCell("OuterVoid",outerCell);
  //  slitTube->insertMainInCell(System,outerCell);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
			   slitTube->getCell("Void"),
			   Geometry::Vec3D(0,1,0));

  slitTube->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));

  slitTube->insertMainInCell(System,slitTube->getCell("OuterVoid",0));
  slitTube->insertMainInCell(System,slitTube->getCell("OuterVoid",1));

  slitTube->insertPortInCell(System,0,slitTube->getCell("OuterVoid",0));
  slitTube->insertPortInCell(System,0,slitTube->getCell("OuterVoid",1));
  slitTube->insertPortInCell(System,1,slitTube->getCell("OuterVoid",0));
  slitTube->insertPortInCell(System,1,slitTube->getCell("OuterVoid",1));
  slitTube->insertPortInCell(System,2,slitTube->getCell("OuterVoid",0));
  slitTube->insertPortInCell(System,3,slitTube->getCell("OuterVoid",1));

  for(size_t i=0;i<jaws.size();i++)
    {
      const constructSystem::portItem& PI=slitTube->getPort(i);
      jaws[i]->addInsertCell("Support",PI.getCell("Void"));
      jaws[i]->addInsertCell("Support",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("Block",slitTube->getCell("SplitVoid",i));
      jaws[i]->createAll(System,*slitTube,"Origin",PI,"InnerPlate");
    }

  constructSystem::constructUnit
    (System,buildZone,*slitTube,"back",*pipeE);

  constructSystem::constructUnit
    (System,buildZone,*pipeE,"back",*gateB);

  constructSystem::constructUnit
    (System,buildZone,*gateB,"back",*bellowD);

  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*pipeF);

  return;
}
 
void
maxpeemOpticsLine::buildM1Mirror(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildM1Mirror");

  int outerCell;

  outerCell=constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*M1Tube);
  
  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);

  M1Stand->setCutSurf("floor",this->getRule("floor"));
  M1Stand->setCutSurf("front",*M1Tube,-1);
  M1Stand->setCutSurf("back",*M1Tube,-2);
  M1Stand->addInsertCell(outerCell);
  M1Stand->createAll(System,*M1Tube,0);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*M1Tube,"back",*offPipeB);
  offPipeB->setCell("OuterVoid",outerCell);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*offPipeB,"back",*gateA);
  gateA->setCell("OuterVoid",outerCell);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*pipeC);

  screenA->addAllInsertCell(outerCell);
  screenA->setCutSurf("inner",*pipeC,"pipeOuterTop");
  screenA->createAll(System,*pipeC,0);
  screenA->insertInCell("Wings",System,gateA->getCell("OuterVoid"));
  screenA->insertInCell("Wings",System,offPipeB->getCell("OuterVoid"));

  return;
}


void
maxpeemOpticsLine::buildExtras(Simulation& System,
			       const attachSystem::CellMap& hut)
  /*!
    Essential bulder to put pipes and shields into maxpeem
    \param System :: Simulation
    \param hut :: optics hut
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildExtras");
  
  screenB->addAllInsertCell(getCell("RightVoid"));
  screenB->addAllInsertCell(getCell("LeftVoid"));
  
  screenB->setCutSurf("inner",outPipeA->getSurfRule("OuterRadius"));
  screenB->setCutSurf("innerTwo",outPipeB->getSurfRule("OuterRadius"));

  screenB->createAll(System,*outPipeA,0);

  outPipeA->insertInCell("Main",System,hut.getCell("ExitHole",0));
  outPipeB->insertInCell("Main",System,hut.getCell("ExitHole",1));

  outPipeA->insertInCell("Main",System,hut.getCell("BackVoid"));
  outPipeB->insertInCell("Main",System,hut.getCell("BackVoid"));
  outPipeA->insertInCell("FlangeB",System,hut.getCell("BackVoid"));
  outPipeB->insertInCell("FlangeB",System,hut.getCell("BackVoid"));

  ELog::EM<<"SCREN == "<<screenB->getKeyName()<<ELog::endDiag;

  return;
}

void
maxpeemOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildObjects");

  int outerCell;
  buildZone.addInsertCells(this->getInsertCells());

  bellowA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*bellowA,"back");
  bellowA->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*pipeA);

  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*pipeA,"back");  

  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createUnit(System,FPI,"OuterPlate");
  florTubeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,FPI,"OuterPlate",*bellowC);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*pipeB);

  screenExtra->addAllInsertCell(outerCell);
  screenExtra->setCutSurf("inner",*pipeB,"pipeOuterTop");
  screenExtra->createAll(System,*pipeB,0);
  
  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*pipeB,"back");

  const constructSystem::portItem& CPI=pumpTubeA->getPort(1);
  outerCell=buildZone.createUnit(System,CPI,"OuterPlate");
  pumpTubeA->insertAllInCell(System,outerCell);
  pumpTubeA->intersectPorts(System,1,2);

  constructSystem::constructUnit
    (System,buildZone,CPI,"OuterPlate",*offPipeA);


  buildM1Mirror(System,*offPipeA,"FlangeBCentre");

  buildSlitPackage(System,*pipeC,"back");

  buildMono(System,*pipeF,"back");
    
  buildM3Mirror(System,*bellowE,"back");

  constructSystem::constructUnit
    (System,buildZone,*M3Tube,"back",*offPipeD);

  buildZone.createUnit(System);         // build to end (removed later)
  buildZone.rebuildInsertCells(System); // rebuild the whole track
  
  buildSplitter(System,*offPipeD,"back");
  System.removeCell(buildZone.getLastCell("Unit"));  // remove cell built above

  lastComp=offPipeA;

  return;
}

void
maxpeemOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  


void 
maxpeemOpticsLine::createAll(Simulation& System,
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
  ELog::RegMethod RControl("maxpeemOpticsLine","build");

  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

