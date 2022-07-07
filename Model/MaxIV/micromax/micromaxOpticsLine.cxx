/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: micromax/micromaxOpticsLine.cxx
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
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"
#include "Line.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "CrossPipe.h"
#include "BeamScrapper.h"
#include "BremColl.h"
#include "BremMonoColl.h"
#include "BremBlock.h"
#include "CRLTube.h"
#include "FourPortTube.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "RoundMonoShutter.h"
#include "TriggerTube.h"
#include "CylGateValve.h"
#include "SquareFMask.h"
#include "IonGauge.h"
#include "BeamPair.h"
#include "MonoBlockXstals.h"
#include "MLMono.h"
#include "DCMTank.h"
#include "BremTube.h"
#include "HPJaws.h"
#include "ViewScreenTube.h"
#include "CollTube.h"
#include "CollUnit.h"

#include "CooledScreen.h"
#include "YagScreen.h"
#include "Table.h"

#include "micromaxOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
micromaxOpticsLine::micromaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),
  
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  whiteCollA(new xraySystem::SquareFMask(newName+"WhiteCollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  bremHolderA(new xraySystem::IonGauge(newName+"BremHolderA")),
  bremCollA(new xraySystem::BremBlock(newName+"BremCollA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  viewTubeA(new xraySystem::ViewScreenTube(newName+"ViewTubeA")),
  cooledScreenA(new xraySystem::CooledScreen(newName+"CooledScreenA")),
  
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  attnTube(new xraySystem::CollTube(newName+"AttnTube")),
  attnUnit(new xraySystem::CollUnit(newName+"AttnUnit")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  tableA(new xraySystem::Table(newName+"TableA")),

  dmmVessel(new xraySystem::DCMTank(newName+"DMMVessel")),
  mlm(new xraySystem::MLMono(newName+"MLM")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  gateTubeB(new xraySystem::CylGateValve(newName+"GateTubeB")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  
  dcmVessel(new xraySystem::DCMTank(newName+"DCMVessel")),
  mbXstals(new xraySystem::MonoBlockXstals(newName+"MBXstals")),

  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  gateTubeC(new xraySystem::CylGateValve(newName+"GateTubeC")),
  
  monoBremTube(new xraySystem::BremTube(newName+"MonoBremTube")),
  bremScrapper(new xraySystem::BeamScrapper(newName+"BremScrapper")),
  bremCollB(new xraySystem::BremBlock(newName+"BremCollB")),

  hpJawsA(new xraySystem::HPJaws(newName+"HPJawsA")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  viewTubeB(new xraySystem::ViewScreenTube(newName+"ViewTubeB")),
  cooledScreenB(new xraySystem::CooledScreen(newName+"CooledScreenB")),
  gateTubeD(new xraySystem::CylGateValve(newName+"GateTubeD")),

  crlPipeA(new constructSystem::VacuumPipe(newName+"CRLPipeA")),
  crlTubeA(new xraySystem::CRLTube(newName+"CRLTubeA")),
  crlPipeB(new constructSystem::VacuumPipe(newName+"CRLPipeB")),
  crlPipeC(new constructSystem::VacuumPipe(newName+"CRLPipeC")),
  crlTubeB(new xraySystem::CRLTube(newName+"CRLTubeB")),
  crlPipeD(new constructSystem::VacuumPipe(newName+"CRLPipeD")),

  longPipeA(new constructSystem::VacuumPipe(newName+"LongPipeA")),
  longPipeB(new constructSystem::VacuumPipe(newName+"LongPipeB")),
  gateTubeE(new xraySystem::CylGateValve(newName+"GateTubeE")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),  
  viewTubeC(new xraySystem::ViewScreenTube(newName+"ViewTubeC")),
  cooledScreenC(new xraySystem::CooledScreen(newName+"CooledScreenC")),
  bellowK(new constructSystem::Bellows(newName+"BellowK")),
  hpJawsB(new xraySystem::HPJaws(newName+"HPJawsB")),
  crlBremTube(new xraySystem::FourPortTube(newName+"CRLBremTube")),  
  bremCollC(new xraySystem::BremBlock(newName+"BremCollC")),
  bellowL(new constructSystem::Bellows(newName+"BellowL")),
  monoShutter(new xraySystem::RoundMonoShutter(newName+"RMonoShutter"))
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
  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(whiteCollA);
  OR.addObject(bellowB);
  OR.addObject(bremHolderA);
  OR.addObject(bremCollA);
  OR.addObject(bellowC);
  OR.addObject(viewTubeA);
  OR.addObject(cooledScreenA);
  OR.addObject(bellowD);
  OR.addObject(attnTube);
  OR.addObject(attnUnit);
  OR.addObject(bellowE);
  OR.addObject(tableA);
  OR.addObject(dmmVessel);
  OR.addObject(bellowF);
  OR.addObject(gateTubeB);
  OR.addObject(bellowG);
  OR.addObject(dcmVessel);
  OR.addObject(mbXstals);
  OR.addObject(bellowH);
  OR.addObject(pipeB);
  OR.addObject(gateTubeC);
  OR.addObject(monoBremTube);
  OR.addObject(bremScrapper);
  OR.addObject(bremCollB);
  OR.addObject(hpJawsA);
  OR.addObject(bellowI);
  OR.addObject(viewTubeB);
  OR.addObject(cooledScreenB);
  OR.addObject(gateTubeD);
  OR.addObject(crlPipeA);
  OR.addObject(crlTubeA);
  OR.addObject(crlPipeB);
  OR.addObject(crlPipeC);
  OR.addObject(crlTubeB);
  OR.addObject(crlPipeD);
  OR.addObject(longPipeA);
  OR.addObject(longPipeB);
  OR.addObject(gateTubeE);
  OR.addObject(bellowJ);
  OR.addObject(viewTubeC);
  OR.addObject(cooledScreenC);
  OR.addObject(bellowK);
  OR.addObject(hpJawsB);
  OR.addObject(crlBremTube);
  OR.addObject(bremCollC);
  OR.addObject(bellowL);
  OR.addObject(monoShutter);
}
  
micromaxOpticsLine::~micromaxOpticsLine()
  /*!
    Destructor
   */
{}

void
micromaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database 
   */
{
  ELog::RegMethod RegA("micromaxOpticsLine","populate");
  
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}


void
micromaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","createSurface");

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
micromaxOpticsLine::constructHDMM(Simulation& System,
				  const attachSystem::FixedComp& initFC, 
				  const std::string& sideName)
/*!
    Sub build of the Diffraction Mirror Mono package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructHDMM");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*dmmVessel);

  mlm->addInsertCell(dmmVessel->getCell("Void"));
  //  MLM->copyCutSurf("innerCylinder",*MLMVessel,"innerRadius");
  mlm->createAll(System,*dmmVessel,0);

  return;
}

void
micromaxOpticsLine::constructHDCM(Simulation& System,
				  const attachSystem::FixedComp& initFC, 
				  const std::string& sideName)
/*!
    Sub build of the mono package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructHDCM");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*dcmVessel);
  
  mbXstals->addInsertCell(dcmVessel->getCell("Void"));
  mbXstals->createAll(System,*dcmVessel,0);

  return;
}

void
micromaxOpticsLine::constructDiag2(Simulation& System,
				   const attachSystem::FixedComp& initFC, 
				   const std::string& sideName)
/*!
    Sub build of the post first mono system.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructDiag2");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoBremTube);

  const constructSystem::portItem& PI=
    monoBremTube->getFrontPort(0);
  bremScrapper->setBeamAxis(*monoBremTube,1);
  bremScrapper->createAll(System,PI,"InnerPlate");
  bremScrapper->insertInCell("Connect",System,PI.getCell("Void"));
  bremScrapper->insertInCell
    ("Connect",System,monoBremTube->getCell("MidVoid"));
  bremScrapper->insertInCell
    ("Payload",System,monoBremTube->getCell("MidVoid"));
  
  
  bremCollB->addInsertCell(monoBremTube->getCell("Void"));
  bremCollB->createAll(System,*monoBremTube,0);

  
  hpJawsA->setFlangeJoin();
  constructSystem::constructUnit
    (System,buildZone,*monoBremTube,"back",*hpJawsA);

  constructSystem::constructUnit
    (System,buildZone,*hpJawsA,"back",*bellowI);

  int outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowI,"back",*viewTubeB);

  cooledScreenB->setBeamAxis(*viewTubeB,1);
  cooledScreenB->createAll(System,*viewTubeB,4);
  cooledScreenB->insertInCell("Outer",System,outerCell);
  cooledScreenB->insertInCell("Connect",System,viewTubeB->getCell("Plate"));
  cooledScreenB->insertInCell("Connect",System,viewTubeB->getCell("Void"));
  cooledScreenB->insertInCell("Payload",System,viewTubeB->getCell("Void"));
  
  constructSystem::constructUnit
    (System,buildZone,*viewTubeB,"back",*gateTubeD);


  return;
}


void
micromaxOpticsLine::constructCRL(Simulation& System,
				   const attachSystem::FixedComp& initFC, 
				   const std::string& sideName)
/*!
    Sub build of the post first mono system.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructCRL");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*crlPipeA);

  constructSystem::constructUnit
    (System,buildZone,*crlPipeA,"back",*crlTubeA);

  constructSystem::constructUnit
    (System,buildZone,*crlTubeA,"back",*crlPipeB);

  constructSystem::constructUnit
    (System,buildZone,*crlPipeB,"back",*crlPipeC);

  constructSystem::constructUnit
    (System,buildZone,*crlPipeC,"back",*crlTubeB);

  constructSystem::constructUnit
    (System,buildZone,*crlTubeB,"back",*crlPipeD);

  return;
}

void
micromaxOpticsLine::constructDiag3(Simulation& System,
				   const attachSystem::FixedComp& initFC, 
				   const std::string& sideName)
/*!
    Sub build of the post first mono system.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructDiag3");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*longPipeA);
  constructSystem::constructUnit
    (System,buildZone,*longPipeA,"back",*longPipeB);

  constructSystem::constructUnit
    (System,buildZone,*longPipeB,"back",*gateTubeE);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeE,"back",*bellowJ);

  int outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowJ,"back",*viewTubeC);

  cooledScreenC->setBeamAxis(*viewTubeC,1);
  cooledScreenC->createAll(System,*viewTubeC,4);
  cooledScreenC->insertInCell("Outer",System,outerCell);
  cooledScreenC->insertInCell("Connect",System,viewTubeC->getCell("Plate"));
  cooledScreenC->insertInCell("Connect",System,viewTubeC->getCell("Void"));
  cooledScreenC->insertInCell("Payload",System,viewTubeC->getCell("Void"));

  constructSystem::constructUnit
    (System,buildZone,*viewTubeC,"back",*bellowK);

  constructSystem::constructUnit
    (System,buildZone,*bellowK,"back",*hpJawsB);

  crlBremTube->setSideVoid();
  constructSystem::constructUnit
    (System,buildZone,*hpJawsB,"back",*crlBremTube);

  bremCollC->addInsertCell(crlBremTube->getCell("Void"));
  bremCollC->createAll(System,*crlBremTube,0);

  return;
}


void
micromaxOpticsLine::constructMonoShutter(Simulation& System,
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
  ELog::RegMethod RegA("micromaxOpticsLine","constructMonoShutter");

  constructSystem::constructUnit
    (System,buildZone,FC,linkName,*bellowL);

  int outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowL,"back",*monoShutter);

  monoShutter->splitObject(System,"-TopPlate",outerCell);
  monoShutter->splitObject(System,"MidCutB",outerCell);
  
  return;
}

void
micromaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","buildObjects");

  int outerCell;
  
  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*pipeA);

  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*bellowA);
  
  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*whiteCollA);

  constructSystem::constructUnit
    (System,buildZone,*whiteCollA,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*bremHolderA);

  bremCollA->addInsertCell(bremHolderA->getCell("Void"));
  bremCollA->createAll(System,*bremHolderA,0);

  constructSystem::constructUnit
    (System,buildZone,*bremHolderA,"back",*bellowC);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*viewTubeA);

  cooledScreenA->setBeamAxis(*viewTubeA,1);
  cooledScreenA->createAll(System,*viewTubeA,4);
  cooledScreenA->insertInCell("Outer",System,outerCell);
  cooledScreenA->insertInCell("Connect",System,viewTubeA->getCell("Plate"));
  cooledScreenA->insertInCell("Connect",System,viewTubeA->getCell("Void"));
  cooledScreenA->insertInCell("Payload",System,viewTubeA->getCell("Void"));

  constructSystem::constructUnit
    (System,buildZone,*viewTubeA,"back",*bellowD);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*attnTube);
  attnUnit->addInsertCell(attnTube->getCell("Void"));
  attnUnit->addInsertCell(attnTube->getCell("PipeVoid"));
  attnUnit->setCutSurf("Flange",*attnTube,"TopFlange");
  attnUnit->createAll(System,*attnTube,"Origin");

  attnTube->addCell("OuterVoid",outerCell);
    
  constructSystem::constructUnit
    (System,buildZone,*attnTube,"back",*bellowE);

  //  tableA->addHole(*viewTubeA,"Origin","OuterRadius");
  tableA->addHole(*attnTube,"VertCentre","VertOuterWall");
  tableA->addHole(*bremHolderA,"Origin","OuterRadius");
  tableA->createAll(System,*bellowA,0);
  tableA->insertAllInCells(System,buildZone.getCells());
  tableA->insertInCell("Plate",System,bremHolderA->getCell("VertOuter"));
  tableA->insertInCell("Plate",System,bremHolderA->getCell("MainOuter"));
  tableA->insertInCell("Plate",System,attnTube->getCell("BlockVoid"));
  tableA->insertInCell("Plate",System,attnTube->getCell("VerticalVoid"));


  constructHDMM(System,*bellowE,"back");

  constructSystem::constructUnit
    (System,buildZone,*dmmVessel,"back",*bellowF);

  constructSystem::constructUnit
    (System,buildZone,*bellowF,"back",*gateTubeB);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeB,"back",*bellowG);

  constructHDCM(System,*bellowG,"back");

  constructSystem::constructUnit
    (System,buildZone,*dcmVessel,"back",*bellowH);
  constructSystem::constructUnit
    (System,buildZone,*bellowH,"back",*pipeB);
  constructSystem::constructUnit
    (System,buildZone,*pipeB,"back",*gateTubeC);

  
  constructDiag2(System,*gateTubeC,"back");

  constructCRL(System,*gateTubeD,"back");

  constructDiag3(System,*crlPipeD,"back");

  constructMonoShutter(System,*crlBremTube,"back");

  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=monoShutter;

  return;
}

void
micromaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("micromaxOpticsLine","createLinks");
  
  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
   
void 
micromaxOpticsLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("micromaxOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

