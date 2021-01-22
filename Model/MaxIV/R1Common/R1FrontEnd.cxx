/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: R1Common/R1FrontEnd.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedGroup.h"
#include "FixedOffset.h"
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
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "OffsetFlangePipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "insertPlate.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "CrossPipe.h"
#include "UTubePipe.h"
#include "Undulator.h"
#include "SquareFMask.h"
#include "FlangeMount.h"
#include "BeamMount.h"
#include "HeatDump.h"
#include "BremBlock.h"
#include "Quadrupole.h"
#include "QuadUnit.h"
#include "DipoleChamber.h"
#include "LCollimator.h"
#include "MagnetBlock.h"

#include "R1FrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
R1FrontEnd::R1FrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),

  buildZone(*this,cellIndex),

  elecGateA(new constructSystem::GateValveCube(newName+"ElecGateA")),
  magnetBlock(new xraySystem::MagnetBlock(newName+"MagnetBlock")),
  dipolePipe(new constructSystem::VacuumPipe(newName+"DipolePipe")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  collA(new xraySystem::SquareFMask(newName+"CollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  heatPipe(new constructSystem::VacuumPipe(newName+"HeatPipe")),
  heatBox(new constructSystem::PipeTube(newName+"HeatBox")),
  heatDump(new xraySystem::HeatDump(newName+"HeatDump")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  ionPB(new constructSystem::CrossPipe(newName+"IonPB")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  aperturePipe(new constructSystem::VacuumPipe(newName+"AperturePipe")),
  moveCollA(new xraySystem::LCollimator(newName+"MoveCollA")),  
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  ionPC(new constructSystem::CrossPipe(newName+"IonPC")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  aperturePipeB(new constructSystem::VacuumPipe(newName+"AperturePipeB")),
  moveCollB(new xraySystem::LCollimator(newName+"MoveCollB")),  
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  gateTubeB(new xraySystem::CylGateValve(newName+"GateTubeB")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  shutterBox(new constructSystem::PipeTube(newName+"ShutterBox")),
  shutters({
      std::make_shared<xraySystem::BeamMount>(newName+"Shutter0"),
	std::make_shared<xraySystem::BeamMount>(newName+"Shutter1")
	}),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  bremBlock(new xraySystem::BremBlock(newName+"BremBlock")),  
  bellowK(new constructSystem::Bellows(newName+"BellowK")) 
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(elecGateA);
  OR.addObject(magnetBlock);
  OR.addObject(dipolePipe);
  OR.addObject(bellowA);
  OR.addObject(collA);
  OR.addObject(bellowB);
  OR.addObject(ionPA);
  OR.addObject(bellowC);
  OR.addObject(heatPipe);
  OR.addObject(heatBox);
  OR.addObject(heatDump);
  OR.addObject(bellowD);
  OR.addObject(gateTubeA);
  OR.addObject(ionPB);
  OR.addObject(pipeB);
  OR.addObject(bellowE);
  OR.addObject(aperturePipe);
  OR.addObject(moveCollA);
  OR.addObject(bellowF);
  OR.addObject(ionPC);
  OR.addObject(bellowG);
  OR.addObject(aperturePipeB);
  OR.addObject(moveCollB);
  OR.addObject(bellowH);
  OR.addObject(pipeC);
  OR.addObject(gateA);
  OR.addObject(bellowI);
  OR.addObject(florTubeA);
  OR.addObject(bellowJ);
  OR.addObject(gateTubeB);
  OR.addObject(offPipeA);
  OR.addObject(shutterBox);
  OR.addObject(shutters[0]);
  OR.addObject(shutters[1]);
  OR.addObject(offPipeB);
  OR.addObject(bremBlock);
  OR.addObject(bellowK);

}
  
R1FrontEnd::~R1FrontEnd()
  /*!
    Destructor
   */
{}

void
R1FrontEnd::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: DataBase
   */
{
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  return;
}


void
R1FrontEnd::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("R1FrontEnd","createSurfaces");

  if (outerLeft>Geometry::zeroTol &&  isActive("Floor"))
    {
      std::string Out;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("Floor"));
      buildZone.setSurround(HR);
    }

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*150.0,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  return;
}


   
void
R1FrontEnd::buildHeatTable(Simulation& System,
				MonteCarlo::Object* masterCell,
				const attachSystem::FixedComp& preFC,
				const long int preSideIndex)

  /*!
    Build the heatDump table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildHeatTable");

  int outerCell;
  // FAKE insertcell:
  heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  heatBox->createAll(System,preFC,preSideIndex);

  const constructSystem::portItem& PIA=heatBox->getPort(1);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
					 PIA,PIA.getSideIndex("OuterPlate"));
  heatBox->insertAllInCell(System,outerCell);
    
  // cant use heatbox here because of port rotation
  
  heatDump->addInsertCell("Inner",heatBox->getCell("Void"));
  heatDump->addInsertCell("Outer",outerCell);
  heatDump->createAll(System,PIA,0,*heatBox,2);


  //  const constructSystem::portItem& PI=heatBox->getPort(1);  
  bellowD->createAll(System,PIA,PIA.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);

  gateTubeA->createAll(System,*bellowD,2);  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateTubeA,2);
  gateTubeA->insertInCell(System,outerCell);
  
  ionPB->createAll(System,*gateTubeA,"back");
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPB,2);
  ionPB->insertInCell(System,outerCell);

  //  insertFlanges(System,*gateTubeA);
  
  pipeB->createAll(System,*ionPB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertAllInCell(System,outerCell);
  
  return;
  
}

void
R1FrontEnd::buildApertureTable(Simulation& System,
			       MonteCarlo::Object* masterCell,
			       const attachSystem::FixedComp& preFC,
			       const long int preSideIndex)
  
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildApertureTable");

  int outerCell;
  // NOTE order for master cell [Next 4 object
  aperturePipe->createAll(System,preFC,preSideIndex);
  moveCollA->addInsertCell(aperturePipe->getCell("Void"));
  moveCollA->createAll(System,*aperturePipe,0);
  
  // bellows AFTER movable aperture pipe
  bellowE->setFront(preFC,preSideIndex);
  bellowE->setBack(*aperturePipe,1);
  bellowE->createAll(System,preFC,preSideIndex);

  ionPC->createAll(System,preFC,preSideIndex);

  // bellows AFTER aperature ionpump and ion pump
  bellowF->setFront(*aperturePipe,2);
  bellowF->setBack(*ionPC,1);
  bellowF->createAll(System,preFC,preSideIndex);

  // now do insert:
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);
    
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*aperturePipe,2);
  aperturePipe->insertAllInCell(System,outerCell);
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowF,2);
  bellowF->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPC,2);
  ionPC->insertInCell(System,outerCell);


  // Next 4 objects need to be build before insertion
  aperturePipeB->createAll(System,*ionPC,2);
  moveCollB->addInsertCell(aperturePipeB->getCell("Void"));
  moveCollB->createAll(System,*aperturePipeB,0);

  // bellows AFTER movable aperture pipe
  bellowG->setFront(*ionPC,2);
  bellowG->setBack(*aperturePipeB,1);
  bellowG->createAll(System,*ionPC,2);

  pipeC->createAll(System,*ionPC,2);

  // bellows AFTER movable aperture pipe
  bellowH->setFront(*aperturePipeB,2);
  bellowH->setBack(*pipeC,1);
  bellowH->createAll(System,*ionPC,2);


  // now do insert:
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowG,2);
  bellowG->insertInCell(System,outerCell);
    
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*aperturePipeB,2);
  aperturePipeB->insertAllInCell(System,outerCell);
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowH,2);
  bellowH->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  pipeC->insertAllInCell(System,outerCell);

  
  return;
}

void
R1FrontEnd::buildShutterTable(Simulation& System,
				   MonteCarlo::Object* masterCell,
				   const attachSystem::FixedComp& preFC,
				   const long int preSideIndex)

  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell for insertion
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildShutterTable");
  int outerCell;
  
  gateA->createAll(System,preFC,preSideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  // bellows 
  bellowI->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowI,2);
  bellowI->insertInCell(System,outerCell);

  // FAKE insertcell:

  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,2);
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertAllInCell(System,outerCell);

  // bellows 
  bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowJ,2);
  bellowJ->insertInCell(System,outerCell);

  insertFlanges(System,*florTubeA);
  
  gateTubeB->createAll(System,*bellowJ,"back");  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateTubeB,2);
  gateTubeB->insertInCell(System,outerCell);

  offPipeA->createAll(System,*gateTubeB,"back");
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);
  
  shutterBox->createAll(System,*offPipeA,
			offPipeA->getSideIndex("FlangeBCentre"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*shutterBox,2);
  shutterBox->insertAllInCell(System,outerCell);
  
  cellIndex=shutterBox->splitVoidPorts(System,"SplitVoid",1001,
				       shutterBox->getCell("Void"),
				       {0,1});
  cellIndex=
    shutterBox->splitVoidPorts(System,"SplitOuter",2001,
			       outerCell,{0,1});
  shutterBox->addAllInsertCell(outerCell);
  //  shutterBox->createPorts(Systsem);

  for(size_t i=0;i<shutters.size();i++)
    {
      const constructSystem::portItem& PI=shutterBox->getPort(i);
      shutters[i]->addInsertCell("Support",PI.getCell("Void"));
      shutters[i]->addInsertCell("Support",shutterBox->getCell("SplitVoid",i));
      shutters[i]->addInsertCell("Block",shutterBox->getCell("SplitVoid",i));

      shutters[i]->createAll(System,*offPipeA,
			     offPipeA->getSideIndex("FlangeACentre"),
			     PI,PI.getSideIndex("InnerPlate"));
    }

  offPipeB->createAll(System,*shutterBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);

  bremBlock->addInsertCell(offPipeB->getCell("Void"));
  bremBlock->setFront(*offPipeB,-1);
  bremBlock->setBack(*offPipeB,-2);
  bremBlock->createAll(System,*offPipeB,0);
    
  // bellows 
  bellowK->createAll(System,*offPipeB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowK,2);
  bellowK->insertInCell(System,outerCell);
  
  return;
}

void
R1FrontEnd::insertFlanges(Simulation& System,
			  const constructSystem::PipeTube& PT)
  /*!
    Boilerplate function to insert the flanges from pipetubes
    that extend past the linkzone in to ther neighbouring regions.
    \param System :: Simulation to use
    \param PT :: PipeTube
   */
{
  ELog::RegMethod RegA("R1FrontEnd","insertFlanges");
  
  const size_t voidN=this->getNItems("OuterVoid")-3;

  this->insertComponent(System,"OuterVoid",voidN,
			PT.getFullRule("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN,
			PT.getFullRule("FlangeB"));
  this->insertComponent(System,"OuterVoid",voidN+2,
			PT.getFullRule("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN+2,
			PT.getFullRule("FlangeB"));
  return;
}
  
void
R1FrontEnd::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildObjects");

  int outerCell;
  buildZone.setFront(getFrontRule());
  buildZone.setBack(getBackRule());
  buildZone.setInsertCells(this->getInsertCells());
  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System);

  const attachSystem::FixedComp& undulatorFC=
    buildUndulator(System,masterCell,*this,0);

  constructSystem::constructUnit
    (System,buildZone,masterCell,undulatorFC,"back",*elecGateA);

  magnetBlock->setStopPoint(stopPoint);
  magnetBlock->createAll(System,*elecGateA,2);
  // UGLY insert into main ring
  for(const int CN : magnetCells)
    magnetBlock->insertInCell("Magnet",System,CN);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*magnetBlock,2);

  magnetBlock->insertInCell("Magnet",System,outerCell);
  magnetBlock->insertInCell("Dipole",System,outerCell);

  if (stopPoint=="Quadrupole")
    {
      setCell("MasterVoid",masterCell->getName());
      lastComp=magnetBlock;
      return;
    }
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*magnetBlock,3);

  magnetBlock->insertInCell("Magnet",System,outerCell);
  magnetBlock->insertInCell("Photon",System,outerCell);

  if (stopPoint=="Dipole" || stopPoint=="Quadrupole")
    {
      setCell("MasterVoid",masterCell->getName());
      lastComp=magnetBlock;
      return;
    }
  // FM1 Built relateive to MASTER coordinate
  collA->createAll(System,*this,0);
  bellowA->createAll(System,*collA,1);

  dipolePipe->setFront(*magnetBlock,"Photon");
  dipolePipe->setBack(*bellowA,2);
  dipolePipe->createAll(System,*magnetBlock,"Photon");
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*dipolePipe,2);
  magnetBlock->insertInCell("Magnet",System,outerCell);  
  dipolePipe->insertAllInCell(System,outerCell);
  //  setCell("MasterVoid",masterCell->getName());
  //  lastComp=magnetBlock;
  //  return;

  // note : bellowA is reversed
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowA,1);
  bellowA->insertInCell(System,outerCell);
  magnetBlock->insertInCell("Magnet",System,outerCell);
  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*collA,2);
  collA->insertInCell(System,outerCell);

  if (stopPoint=="Dipole")
    {
      lastComp=magnetBlock;
      return;
    }
  setCell("MasterVoid",masterCell->getName());
  lastComp=magnetBlock;
  
  bellowB->createAll(System,*collA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPA,2);
  ionPA->insertInCell(System,outerCell);

  bellowC->createAll(System,*ionPA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);


  heatPipe->createAll(System,*bellowC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*heatPipe,2);
  heatPipe->insertAllInCell(System,outerCell);

  if (stopPoint=="Heat")
    {
      lastComp=heatPipe;
      return;
    }

  
  buildHeatTable(System,masterCell,*heatPipe,2);  
  buildApertureTable(System,masterCell,*pipeB,2);
  buildShutterTable(System,masterCell,*pipeC,2);


  setCell("MasterVoid",masterCell->getName());
  lastComp=bellowK;
  
  return;
}
  
void 
R1FrontEnd::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  ELog::RegMethod RControl("R1FrontEnd","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();

  std::string Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6 ");
  Out+=frontRule()+backRule()+getRuleStr("Floor");

  addOuterSurf(Out);
  insertObjects(System);

  return;
}

}   // NAMESPACE xraySystem

