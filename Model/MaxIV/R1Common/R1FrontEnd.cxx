/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: R1Common/R1FrontEnd.cxx
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
#include "BlockZone.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "OffsetFlangePipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "CrossPipe.h"
#include "SquareFMask.h"
#include "BeamMount.h"
#include "HeatDump.h"
#include "BremBlock.h"
#include "LCollimator.h"
#include "MagnetBlock.h"

#include "R1FrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
R1FrontEnd::R1FrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),

  buildZone(Key+"R3FrontEnd"),

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
  FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerFront=Control.EvalDefVar<double>(keyName+"OuterFront",0.0);

  return;
}


void
R1FrontEnd::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("R1FrontEnd","createSurfaces");

  if (outerLeft>Geometry::zeroTol &&
      isActive("Floor") && isActive("Roof"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*outerRight,X);

      const HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");
      buildZone.setSurround(HR*getRule("Floor")*getRule("Roof"));
    }

  if (!isActive("front"))
    {
      ELog::EM<<"Building Front "<<outerFront<<ELog::endDiag;
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*outerFront,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  else
    ELog::EM<<"Front == "<<getRule("front")<<ELog::endDiag;
  
  buildZone.setFront(getRule("front"));
  
  return;
}


   
void
R1FrontEnd::buildHeatTable(Simulation& System,
			   const attachSystem::FixedComp& preFC,
			   const std::string& preSide)
  /*!
    Build the heatDump table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSide :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildHeatTable");

  int outerCell;

  heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  heatBox->createAll(System,preFC,preSide);

  const constructSystem::portItem& PIA=heatBox->getPort(1);
  outerCell=buildZone.createUnit(System,PIA,"OuterPlate");
  heatBox->insertAllInCell(System,outerCell);
    
  // cant use heatbox here because of port rotation and
  // using PIA centre point
  
  heatDump->addInsertCell("Inner",heatBox->getCell("Void"));
  heatDump->addInsertCell("Outer",outerCell);
  heatDump->createAll(System,PIA,0,*heatBox,2);

  constructSystem::constructUnit
    (System,buildZone,PIA,"OuterPlate",*bellowD);

  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*ionPB);

  constructSystem::constructUnit
    (System,buildZone,*ionPB,"back",*pipeB);

  return;
}

void
R1FrontEnd::buildApertureTable(Simulation& System,
			       const attachSystem::FixedComp& preFC,
			       const std::string& preSide)
  
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSide :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildApertureTable");

  int outerCell;
  // NOTE order for master cell [Next 4 object
  aperturePipe->createAll(System,preFC,preSide);
  
  moveCollA->addInsertCell(aperturePipe->getCell("Void"));
  moveCollA->createAll(System,*aperturePipe,0);
  
  // bellows AFTER movable aperture pipe
  bellowE->setFront(preFC,preSide);
  bellowE->setBack(*aperturePipe,1);
  bellowE->createAll(System,preFC,preSide);

  ionPC->createAll(System,preFC,preSide);

  // bellows AFTER aperature ionpump and ion pump
  bellowF->setFront(*aperturePipe,2);
  bellowF->setBack(*ionPC,1);
  bellowF->createAll(System,preFC,preSide);

  // now do insert:
  outerCell=buildZone.createUnit(System,*bellowE,"back");
  bellowE->insertInCell(System,outerCell);
    
  outerCell=buildZone.createUnit(System,*aperturePipe,"back");
  aperturePipe->insertAllInCell(System,outerCell);
  
  outerCell=buildZone.createUnit(System,*bellowF,"back");
  bellowF->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*ionPC,"back");
  ionPC->insertInCell(System,outerCell);

  // Next 4 objects need to be build before insertion
  aperturePipeB->createAll(System,*ionPC,"back");
  moveCollB->addInsertCell(aperturePipeB->getCell("Void"));
  moveCollB->createAll(System,*aperturePipeB,0);

  // bellows AFTER movable aperture pipe
  bellowG->setFront(*ionPC,2);
  bellowG->setBack(*aperturePipeB,1);
  bellowG->createAll(System,*ionPC,"back");

  pipeC->createAll(System,*ionPC,"back");

  // bellows AFTER movable aperture pipe
  bellowH->setFront(*aperturePipeB,"back");
  bellowH->setBack(*pipeC,1);
  bellowH->createAll(System,*ionPC,"back");


  // now do insert:
  outerCell=buildZone.createUnit(System,*bellowG,"back");
  bellowG->insertInCell(System,outerCell);
    
  outerCell=buildZone.createUnit(System,*aperturePipeB,"back");
  aperturePipeB->insertAllInCell(System,outerCell);
  
  outerCell=buildZone.createUnit(System,*bellowH,"back");
  bellowH->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*pipeC,"back");
  pipeC->insertAllInCell(System,outerCell);

  return;
}

void
R1FrontEnd::buildShutterTable(Simulation& System,
			      const attachSystem::FixedComp& preFC,
			      const std::string& preSide)

  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell for insertion
    \param preFC :: Initial cell
    \param preSide :: Initial side index
  */
{
  ELog::RegMethod RegA("R1FrontEnd","buildShutterTable");
  int outerCell;


  constructSystem::constructUnit
    (System,buildZone,preFC,preSide,*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*bellowI);

  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,"back");
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createUnit(System,FPI,"OuterPlate");
  florTubeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,FPI,"OuterPlate",*bellowJ);

  //  insertFlanges(System,*florTubeA);

  constructSystem::constructUnit
    (System,buildZone,*bellowJ,"back",*gateTubeB);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeB,"back",*offPipeA);

 
  shutterBox->createAll(System,*offPipeA,"FlangeBCentre");
  outerCell=buildZone.createUnit(System,*shutterBox,"back");

  shutterBox->splitVoidPorts
    (System,"SplitVoid",1001,shutterBox->getCell("Void"),{0,1});
  
  
  shutterBox->splitVoidPorts(System,"SplitOuter",2001,
			       outerCell,{0,1});
  shutterBox->insertMainInCell(System,shutterBox->getCells("SplitOuter"));
  shutterBox->insertPortInCell(System,0,shutterBox->getCell("SplitOuter",0));
  shutterBox->insertPortInCell(System,1,shutterBox->getCell("SplitOuter",1));



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

  constructSystem::constructUnit
    (System,buildZone,*shutterBox,"back",*offPipeB);
  
  bremBlock->addInsertCell(offPipeB->getCell("Void"));
  bremBlock->setFront(*offPipeB,-1);
  bremBlock->setBack(*offPipeB,-2);
  bremBlock->createAll(System,*offPipeB,0);

  constructSystem::constructUnit
    (System,buildZone,*offPipeB,"back",*bellowK);

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

  buildZone.addInsertCells(this->getInsertCells());


  const attachSystem::FixedComp& undulatorFC=
    buildUndulator(System,*this,"Origin");

  constructSystem::constructUnit
    (System,buildZone,undulatorFC,"back",*elecGateA);

  magnetBlock->setStopPoint(stopPoint);
  magnetBlock->createAll(System,*elecGateA,2);
  
  outerCell=buildZone.createUnit(System,*magnetBlock,2);

  magnetBlock->insertInCell("Magnet",System,outerCell);
  magnetBlock->insertInCell("Dipole",System,outerCell);

  if (stopPoint=="Quadrupole")
    {
      processEnd(System,magnetBlock);
      return;
    }
  
  outerCell=buildZone.createUnit(System,*magnetBlock,3);
  magnetBlock->insertInCell("Magnet",System,outerCell);
  magnetBlock->insertInCell("Photon",System,outerCell);

  
  if (stopPoint=="Dipole")
    {
      processEnd(System,magnetBlock);
      return;
    }

  // FM1 Built relateive to MASTER coordinate
  collA->createAll(System,*this,0);
  bellowA->createAll(System,*collA,1);

  dipolePipe->setFront(*magnetBlock,"Photon");
  dipolePipe->setBack(*bellowA,2);
  dipolePipe->createAll(System,*magnetBlock,"Photon");
  outerCell=buildZone.createUnit(System,*dipolePipe,2);

  magnetBlock->insertInCell("Magnet",System,outerCell);  
  dipolePipe->insertAllInCell(System,outerCell);

  // note : bellowA is reversed
  outerCell=buildZone.createUnit(System,*bellowA,1);
  bellowA->insertInCell(System,outerCell);
  magnetBlock->insertInCell("Magnet",System,outerCell);
  
  outerCell=buildZone.createUnit(System,*collA,2);
  collA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*collA,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*ionPA);

  constructSystem::constructUnit
    (System,buildZone,*ionPA,"back",*bellowC);

  constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*heatPipe);


  if (stopPoint=="Heat")
    {
      processEnd(System,heatPipe);
      return;
    }

  buildHeatTable(System,*heatPipe,"back");  
  buildApertureTable(System,*pipeB,"back");
  buildShutterTable(System,*pipeC,"back");  


  processEnd(System,bellowK);

  return;
}

void
R1FrontEnd::processEnd(Simulation& System,
                       std::shared_ptr<attachSystem::FixedComp> lUnit)
  /*!
    Process the end point
    \param System :: Simulation to use
    \param lUnit :: last unit
  */
{
  ELog::RegMethod RegA("R1FrontEnd","processEnd");

  if (ExternalCut::isActive("REWall"))
    {
      buildZone.setMaxExtent(getRule("REWall"));
      buildZone.createUnit(System);
    }

  buildZone.rebuildInsertCells(System);

  for(const int CN : magnetCells)
    magnetBlock->insertInCell("Magnet",System,CN);
  setCell("MasterVoid",buildZone.getLastCell("Unit"));
  lastComp=lUnit;
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

  return;
}

}   // NAMESPACE xraySystem

