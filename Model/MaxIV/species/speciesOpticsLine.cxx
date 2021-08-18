/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: species/speciesOpticsLine.cxx
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

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PipeShield.h"

#include "CrossPipe.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCube.h"
#include "BeamMount.h"
#include "TankMonoVessel.h"
#include "GratingUnit.h"
#include "TwinPipe.h"
#include "Mirror.h"
#include "TriggerTube.h"
#include "speciesOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
speciesOpticsLine::speciesOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  buildZone(newName+"BlockZone"),
  IZLeft(newName+"IZLeft"),
  IZRight(newName+"IZRight"),
  innerMat(0),

  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerPipe")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  leadBrick(new insertSystem::insertPlate(newName+"LeadBrick")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusZ"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusZ")}),  
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  screenB(new xraySystem::PipeShield(newName+"ScreenB")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  monoVessel(new xraySystem::TankMonoVessel(newName+"MonoVessel")),
  grating(new xraySystem::GratingUnit(newName+"Grating")),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  mirrorJaws(new constructSystem::JawValveCube(newName+"MirrorJaws")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),

  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  pumpTubeAA(new constructSystem::PipeTube(newName+"PumpTubeAA")),
  gateAA(new constructSystem::GateValveCube(newName+"GateAA")),
  bellowAB(new constructSystem::Bellows(newName+"BellowAB")),
  gateAB(new constructSystem::GateValveCube(newName+"GateAB")),
  bellowAC(new constructSystem::Bellows(newName+"BellowAC")),
  
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  pumpTubeBA(new constructSystem::PipeTube(newName+"PumpTubeBA")),
  gateBA(new constructSystem::GateValveCube(newName+"GateBA")),
  bellowBB(new constructSystem::Bellows(newName+"BellowBB")),
  gateBB(new constructSystem::GateValveCube(newName+"GateBB")),
  bellowBC(new constructSystem::Bellows(newName+"BellowBC")),

  screenC(new xraySystem::PipeShield(newName+"ScreenC")),
  
  outPipeA(new constructSystem::VacuumPipe(newName+"OutPipeA")),
  outPipeB(new constructSystem::VacuumPipe(newName+"OutPipeB"))

  
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(triggerPipe);
  OR.addObject(gateTubeA);
  OR.addObject(bellowB);
  OR.addObject(pipeA);
  OR.addObject(M1Tube);
  OR.addObject(M1Mirror);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(screenA);
  OR.addObject(leadBrick);
  OR.addObject(gateA);
  OR.addObject(pipeC);
  OR.addObject(slitTube);
  OR.addObject(pipeD);
  OR.addObject(screenB);
  OR.addObject(offPipeA);
  OR.addObject(monoVessel);
  OR.addObject(grating);
  OR.addObject(offPipeB);
  OR.addObject(bellowD);
  OR.addObject(pipeE);
  OR.addObject(bellowE);
  OR.addObject(pipeF);
  OR.addObject(mirrorJaws);
  OR.addObject(M3Tube);
  OR.addObject(splitter);
  OR.addObject(bellowAA);
  OR.addObject(pumpTubeAA);
  OR.addObject(gateAA);
  OR.addObject(bellowAB);
  OR.addObject(gateAB);
  OR.addObject(bellowAC);
  OR.addObject(bellowBA);
  OR.addObject(pumpTubeBA);
  OR.addObject(gateBA);
  OR.addObject(bellowBB);
  OR.addObject(gateBB);
  OR.addObject(bellowBC);
  OR.addObject(screenC);
  OR.addObject(outPipeA);
  OR.addObject(outPipeB);
}
  
speciesOpticsLine::~speciesOpticsLine()
  /*!
    Destructor
   */
{}

void
speciesOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","populate");
  FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);

  const int voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  buildZone.setInnerMat(voidMat);
  return;
}


void
speciesOpticsLine::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","createSurfaces");

  if (outerLeft>Geometry::zeroTol &&
      isActive("floor") && isActive("roof"))
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
speciesOpticsLine::buildFrontTable(Simulation& System,
				   const attachSystem::FixedComp& initFC,
				   const std::string& sideName)
  /*!
    Sub build of the first part of the beamline
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","buildFrontTable");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowB);

  return;
}

void
speciesOpticsLine::buildM1Mirror(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","buildM1Mirror");

  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*pipeA);

  M1Tube->setPortRotation(3,Geometry::Vec3D(0,1,0),
			  Geometry::Vec3D(0,0,1));
  M1Tube->createAll(System,*pipeA,"back");
  const constructSystem::portItem& API=M1Tube->getPort(1);
  outerCell=buildZone.createUnit(System,API,"OuterPlate");
  M1Tube->insertAllInCell(System,outerCell);
  
  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);

  constructSystem::constructUnit
    (System,buildZone,API,"OuterPlate",*bellowC);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*pipeB);

  screenA->addAllInsertCell(outerCell);
  screenA->setCutSurf("inner",*pipeB,"pipeOuterTop");
  screenA->createAll(System,*pipeB,0);
  leadBrick->createAll(System,*screenA,2);

  return;
}


void
speciesOpticsLine::buildSlitPackage(Simulation& System,
				    const attachSystem::FixedComp& initFC, 
				    const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","buildSlitPackage");
  
  int outerCell;


  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*pipeC);



  slitTube->createAll(System,*pipeC,"back");
  outerCell=
    buildZone.createUnit(System,*slitTube,"back");
  slitTube->setCell("OuterVoid",outerCell);

  
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

  outerCell=constructSystem::constructUnit
    (System,buildZone,*slitTube,"back",*pipeD);


    // const constructSystem::portItem& SPI=slitTube->getPort(3);
    // // this needs the plate as well if constructed
    // SPI.insertCellMapInCell(System,"Flange",0,outerCell);
  
  screenB->addAllInsertCell(outerCell);
  screenB->setCutSurf("inner",*pipeD,"pipeOuterTop");
  screenB->createAll(System,*pipeD,0);
  
  return;
}

void
speciesOpticsLine::buildMono(Simulation& System,
			     const attachSystem::FixedComp& initFC, 
			     const std::string& sideName)
/*!
  Sub build of the slit package unit
  \param System :: Simulation to use
  \param initFC :: Start point
  \param sideName :: start link point
*/
{
  ELog::RegMethod RegA("speciesOpticsLine","buildMono");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*offPipeA);

  
  constructSystem::constructUnit
    (System,buildZone,*offPipeA,"back",*monoVessel);
   
  grating->addInsertCell(monoVessel->getCell("Void"));
  grating->copyCutSurf("innerCylinder",*monoVessel,"innerRadius");
  grating->createAll(System,*monoVessel,0);

  constructSystem::constructUnit
    (System,buildZone,*monoVessel,"back",*offPipeB);

  return;
}

void
speciesOpticsLine::buildM3Mirror(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
/*!
  Sub build of the m3-mirror unit
  \param System :: Simulation to use
  \param initFC :: Start point
  \param sideName :: start link point
*/
{
  ELog::RegMethod RegA("speciesOpticsLine","buildM3Mirror");
  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*bellowD);

  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*pipeE);

  constructSystem::constructUnit
    (System,buildZone,*pipeE,"back",*bellowE);

  constructSystem::constructUnit
    (System,buildZone,*bellowE,"back",*pipeF);

  constructSystem::constructUnit
    (System,buildZone,*pipeF,"back",*mirrorJaws);

  M3Tube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  M3Tube->createAll(System,*mirrorJaws,2);


  const constructSystem::portItem& API=M3Tube->getPort(1);
  outerCell=buildZone.createUnit(System,API,"OuterPlate");
  M3Tube->insertAllInCell(System,outerCell);


  return;
}

void
speciesOpticsLine::buildSplitter(Simulation& System,
				 const attachSystem::FixedComp& initFC,
				 const std::string& sideName)
 /*!
   Sub build of the spliter package
   \param System :: Simulation to use
   \param initFC :: Start point
   \param sideName :: start link point
 */
{
  ELog::RegMethod RegA("speciesOpticsBeamLine","buildSplitter");

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

  // LEFT SIDE
  
  constructSystem::constructUnit
    (System,IZLeft,*bellowAA,"back",*pumpTubeAA);

  constructSystem::constructUnit
    (System,IZLeft,*pumpTubeAA,"back",*gateAA);

  constructSystem::constructUnit
    (System,IZLeft,*gateAA,"back",*bellowAB);

  constructSystem::constructUnit
    (System,IZLeft,*bellowAB,"back",*gateAB);

  constructSystem::constructUnit
    (System,IZLeft,*gateAB,"back",*bellowAC);

  outerCell=IZLeft.createUnit(System);
  
  outPipeA->createAll(System,*bellowAC,"back");
  outPipeA->insertAllInCell(System,outerCell);

  // RIGHT

  constructSystem::constructUnit
    (System,IZRight,*bellowBA,"back",*pumpTubeBA);

  constructSystem::constructUnit
    (System,IZRight,*pumpTubeBA,"back",*gateBA);

  constructSystem::constructUnit
    (System,IZRight,*gateBA,"back",*bellowBB);

  constructSystem::constructUnit
    (System,IZRight,*bellowBB,"back",*gateBB);

  constructSystem::constructUnit
    (System,IZRight,*gateBB,"back",*bellowBC);

  outerCell=IZRight.createUnit(System);
  
  outPipeB->createAll(System,*bellowBC,"back");
  outPipeB->insertAllInCell(System,outerCell);

  // Get last two cells
  setCell("LeftVoid",IZLeft.getLastCell("Unit"));
  setCell("RightVoid",IZRight.getLastCell("Unit"));

  return;
}

void
speciesOpticsLine::buildExtras(Simulation& System,
			       const attachSystem::CellMap& hut)
  /*!
    Essential bulder to put pipes and shields into maxpeem
    \param System :: Simulation
    \param hut :: optics hut
  */
{
  ELog::RegMethod RegA("maxpeemOpticsLine","buildExtras");

  outPipeA->insertInCell("Main",System,hut.getCell("ExitHole",0));
  outPipeB->insertInCell("Main",System,hut.getCell("ExitHole",1));

  outPipeA->insertInCell("Main",System,hut.getCell("BackVoid"));
  outPipeB->insertInCell("Main",System,hut.getCell("BackVoid"));
  outPipeA->insertInCell("FlangeB",System,hut.getCell("BackVoid"));
  outPipeB->insertInCell("FlangeB",System,hut.getCell("BackVoid"));

  screenC->addAllInsertCell(getCell("RightVoid"));
  screenC->addAllInsertCell(getCell("LeftVoid"));
  screenC->setCutSurf("inner",outPipeA->getSurfRule("OuterRadius"));
  screenC->setCutSurf("innerTwo",outPipeB->getSurfRule("OuterRadius"));

  screenC->createAll(System,*outPipeA,0);

  /*
  
    screenB->setCutSurf("inner",outPipeA->getSurfRule("OuterRadius"));
    screenB->setCutSurf("innerTwo",outPipeB->getSurfRule("OuterRadius"));

    screenB->createAll(System,*outPipeA,0);

  
  */
  return;
}

void
speciesOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("speciesOpticsLine","buildObjects");

  int outerCell;
  buildZone.addInsertCells(this->getInsertCells());

  bellowA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*bellowA,"back");
  bellowA->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  buildFrontTable(System,*bellowA,"back");
  buildM1Mirror(System,*bellowB,"back");
  buildSlitPackage(System,*pipeB,"back");
  
  buildMono(System,*pipeD,"back");
  buildM3Mirror(System,*offPipeB,"back");

  buildZone.createUnit(System);         // build to end (removed later)
  buildZone.rebuildInsertCells(System); // rebuild the whole track

  const constructSystem::portItem& API=M3Tube->getPort(1);
  buildSplitter(System,API,"OuterPlate");


  System.removeCell(buildZone.getLastCell("Unit"));  // remove cell built above
  lastComp=bellowB;

  return;
}

void
speciesOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  

void 
speciesOpticsLine::createAll(Simulation& System,
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
  ELog::RegMethod RControl("speciesOpticsLine","build");

  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  bellowA->setFront(FC,sideIndex);

  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE xraySystem

