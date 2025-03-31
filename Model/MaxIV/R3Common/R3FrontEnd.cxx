/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: R3Common/R3FrontEnd.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "Exception.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateGroup.h"
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

#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "CornerPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "Bellows.h"
#include "LCollimator.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "OffsetFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "CrossPipe.h"
#include "SquareFMask.h"
#include "BeamMount.h"
#include "HeatDump.h"
#include "EPSeparator.h"
#include "R3ChokeChamber.h"
#include "R3ChokeInsert.h"
#include "EntryPipe.h"
#include "MagnetM1.h"
#include "MagnetU1.h"
#include "CleaningMagnet.h"
#include "HeatAbsorberToyama.h"

#include "R3FrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:

R3FrontEnd::R3FrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),

  buildZone(Key+"R3FrontEnd"),

  transPipe(new constructSystem::VacuumPipe(newName+"TransPipe")),
  magBlockM1(new xraySystem::MagnetM1(newName+"M1Block")),
  magBlockU1(new xraySystem::MagnetU1(newName+"U1Block")),
  epSeparator(new xraySystem::EPSeparator(newName+"EPSeparator")),
  chokeChamber(new xraySystem::R3ChokeChamber(newName+"ChokeChamber")),
  chokeInsert(new xraySystem::R3ChokeInsert(newName+"ChokeInsert")),
  dipolePipe(new constructSystem::CornerPipe(newName+"DipolePipe")),
  eTransPipe(new constructSystem::VacuumPipe(newName+"ETransPipe")),
  eCutDisk(new insertSystem::insertCylinder(newName+"ECutDisk")),
  eCutMagDisk(new insertSystem::insertCylinder(newName+"ECutMagDisk")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  collA(new xraySystem::SquareFMask(newName+"CollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  collABPipe(new constructSystem::VacuumPipe(newName+"CollABPipe")),
  pMag(std::make_shared<tdcSystem::CleaningMagnet>(newName+"PermanentMagnet")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  collB(new xraySystem::SquareFMask(newName+"CollB")),
  collC(new xraySystem::SquareFMask(newName+"CollC")),
  collExitPipe(new constructSystem::VacuumPipe(newName+"CollExitPipe")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  ionPB(new constructSystem::CrossPipe(newName+"IonPB")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  // aperture table
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  aperturePipeA(new constructSystem::VacuumPipe(newName+"AperturePipeA")),
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
  collFM3Active(true)

  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(transPipe);
  OR.addObject(magBlockM1);
  OR.addObject(magBlockU1);
  OR.addObject(epSeparator);
  OR.addObject(chokeChamber);
  OR.addObject(chokeInsert);

  OR.addObject(dipolePipe);
  OR.addObject(eTransPipe);
  OR.addObject(bellowA);
  OR.addObject(collA);
  OR.addObject(bellowB);
  OR.addObject(collABPipe);
  OR.addObject(pMag);
  OR.addObject(bellowC);
  OR.addObject(collB);
  OR.addObject(collC);
  OR.addObject(eCutDisk);
  OR.addObject(eCutMagDisk);
  OR.addObject(collExitPipe);

  OR.addObject(pipeB);
  OR.addObject(bellowE);
  OR.addObject(aperturePipeA);
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

}

R3FrontEnd::~R3FrontEnd()
  /*!
    Destructor
   */
{}

void
R3FrontEnd::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: DataBase
   */
{
  FixedRotate::populate(Control);
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  frontOffset=Control.EvalDefVar<double>(keyName+"FrontOffset",0.0);
  return;
}


void
R3FrontEnd::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("R3FrontEnd","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
      buildZone.setSurround(HeadRule(-SMap.realSurf(buildIndex+7)));
    }

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*frontOffset,Y);
      buildZone.initFront(HeadRule(SMap.realSurf(buildIndex+1)));
      setFront(-SMap.realSurf(buildIndex+1));
    }
  else
    buildZone.setFront(getFrontRule());

  return;
}

void
R3FrontEnd::insertFlanges(Simulation& System,
			  const constructSystem::PipeTube& PT,
			  const size_t offset)
  /*!
    Boilerplate function to insert the flanges from pipetubes
    that extend past the linkzone in to ther neighbouring regions.
    \param System :: Simulation to use
    \param PT :: PipeTube
   */
{
  ELog::RegMethod RegA("R3FrontEnd","insertFlanges");

  size_t voidN=buildZone.getNItems("Unit");
  if (voidN<offset)
    throw ColErr::InContainerError<size_t>
      (offset, "Offset to large for buildZone cells:"+std::to_string(voidN));
  voidN-=offset;

  buildZone.insertComponent(System,"Unit",voidN,
			PT.getFullRule("FlangeA"));
  buildZone.insertComponent(System,"Unit",voidN,
			PT.getFullRule("FlangeB"));
  buildZone.insertComponent(System,"Unit",voidN+2,
			PT.getFullRule("FlangeA"));
  buildZone.insertComponent(System,"Unit",voidN+2,
			PT.getFullRule("FlangeB"));
  return;
}

void
R3FrontEnd::buildHeatTable(Simulation& System)
  /*!
    Build the heatDump table
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3FrontEnd","buildHeatTable");

  // int outerCell;

  // if (!true) { // FMB/B (/B is Berlin)
  //   heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  //   heatBox->createAll(System,*this,0);

  //   const constructSystem::portItem& PIA=heatBox->getPort(0);
  //   const constructSystem::portItem& PIB=heatBox->getPort(1);

  //   // cant use heatbox here because of port rotation
  //   heatDump->addInsertCell("Inner",heatBox->getCell("Void"));
  //   heatDump->createAll(System,PIB,0,*heatBox,2);

  //   // Built after heatDump
  //   collExitPipe->setBack(PIA,"OuterPlate");
  //   collExitPipe->createAll(System,*collB,2);
  //   outerCell=buildZone.createUnit(System,*collExitPipe,2);
  //   collExitPipe->insertAllInCell(System,outerCell);

  //   outerCell=buildZone.createUnit(System,PIB,"OuterPlate");
  //   heatBox->insertAllInCell(System,outerCell);
  //   heatDump->insertInCell("Outer",System,outerCell);

  //   constructSystem::constructUnit
  //     (System,buildZone,PIB,"OuterPlate",*bellowD);
  // } else {
  //   haToyama->createAll(System,*this,0);
  //   collExitPipe->setBack(*haToyama,"front");
  //   collExitPipe->createAll(System,*collB,2);
  //   outerCell=buildZone.createUnit(System,*collExitPipe,2);
  //   collExitPipe->insertAllInCell(System,outerCell);

  //   outerCell=buildZone.createUnit(System,*haToyama,"back");
  //   haToyama->insertInCell(System,outerCell);
  //   constructSystem::constructUnit(System,buildZone,*haToyama,"back",*bellowD);
  // }

  // constructSystem::constructUnit
  //   (System,buildZone,*bellowD,"back",*gateTubeA);

  // constructSystem::constructUnit
  //   (System,buildZone,*gateTubeA,"back",*ionPB);

  // constructSystem::constructUnit
  //   (System,buildZone,*ionPB,"back",*pipeB);

  return;

}

void
R3FrontEnd::buildApertureTable(Simulation& System,
				   const attachSystem::FixedComp& preFC,
				   const long int preSideIndex)

  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
  */
{
  ELog::RegMethod RegA("R3FrontEnd","buildApertureTable");

  // int outerCell;
  // // NOTE order for master cell [Next 4 objects]
  // aperturePipeA->createAll(System,preFC,preSideIndex);  // pipeB
  // moveCollA->addInsertCell(aperturePipeA->getCell("Void"));
  // moveCollA->createAll(System,*aperturePipeA,"midPoint");

  // // bellows AFTER movable aperture pipe
  // bellowE->setFront(preFC,preSideIndex);
  // bellowE->setBack(*aperturePipeA,1);
  // bellowE->createAll(System,preFC,preSideIndex);

  // ionPC->createAll(System,preFC,preSideIndex);

  // // bellows AFTER aperature ionpump and ion pump
  // bellowF->setFront(*aperturePipeA,2);
  // bellowF->setBack(*ionPC,1);
  // bellowF->createAll(System,preFC,preSideIndex);

  // // now do insert:
  // outerCell=buildZone.createUnit(System,*bellowE,2);
  // bellowE->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*aperturePipeA,2);
  // aperturePipeA->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*bellowF,2);
  // bellowF->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*ionPC,2);
  // ionPC->insertInCell(System,outerCell);


  // // Next 4 objects need to be build before insertion
  // aperturePipeB->createAll(System,*ionPC,2);
  // moveCollB->addInsertCell(aperturePipeB->getCell("Void"));
  // moveCollB->createAll(System,*aperturePipeB,"midPoint");

  // // bellows AFTER movable aperture pipe
  // bellowG->setFront(*ionPC,2);
  // bellowG->setBack(*aperturePipeB,1);
  // bellowG->createAll(System,*ionPC,2);

  // pipeC->createAll(System,*ionPC,2);

  // // bellows AFTER movable aperture pipe
  // bellowH->setFront(*aperturePipeB,2);
  // bellowH->setBack(*pipeC,1);
  // bellowH->createAll(System,*ionPC,2);


  // // now do insert:
  // outerCell=buildZone.createUnit(System,*bellowG,2);
  // bellowG->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*aperturePipeB,2);
  // aperturePipeB->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*bellowH,2);
  // bellowH->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*pipeC,2);
  // pipeC->insertAllInCell(System,outerCell);


  return;
}

void
R3FrontEnd::buildShutterTable(Simulation& System,
			      const attachSystem::FixedComp& preFC,
			      const std::string& preSide)
  /*!
    Build the shutter block table
    \param System :: Simulation to use
    \param preFC :: initial Fixedcomp
    \param preSide :: link point on initial FC
  */
{
  ELog::RegMethod RegA("R3FrontEnd","buildShutterTable");
  int outerCell;

  // constructSystem::constructUnit
  //   (System,buildZone,preFC,preSide,*gateA);

  // // gateA->createAll(System,preFC,preSideIndex);
  // // outerCell=buildZone.createUnit(System,*gateA,2);
  // // gateA->insertInCell(System,outerCell);

  // // bellows
  // constructSystem::constructUnit
  //   (System,buildZone,*gateA,"back",*bellowI);

  // florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  // florTubeA->createAll(System,*bellowI,2);
  // const constructSystem::portItem& FPI=florTubeA->getPort(1);
  // outerCell=buildZone.createUnit(System,
  // 				 FPI,FPI.getSideIndex("OuterPlate"));

  // florTubeA->insertAllInCell(System,outerCell);

  // // bellows
  // bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  // outerCell=buildZone.createUnit(System,*bellowJ,2);
  // bellowJ->insertAllInCell(System,outerCell);

  // insertFlanges(System,*florTubeA,3);

  // constructSystem::constructUnit
  //   (System,buildZone,*bellowJ,"back",*gateTubeB);

  // constructSystem::constructUnit
  //   (System,buildZone,*gateTubeB,"back",*offPipeA);

  // //  insertFlanges(System,*gateTubeB);

  // shutterBox->createAll(System,*offPipeA,"FlangeBCentre");
  // outerCell=buildZone.createUnit(System,*shutterBox,"back");

  // shutterBox->splitVoidPorts
  //   (System,"SplitVoid",1001,shutterBox->getCell("Void"),{0,1});


  // shutterBox->splitVoidPorts(System,"SplitOuter",2001,
  // 			       outerCell,{0,1});
  // shutterBox->insertMainInCell(System,shutterBox->getCells("SplitOuter"));
  // shutterBox->insertPortInCell(System,0,shutterBox->getCell("SplitOuter",0));
  // shutterBox->insertPortInCell(System,1,shutterBox->getCell("SplitOuter",1));



  // for(size_t i=0;i<shutters.size();i++)
  //   {
  //     const constructSystem::portItem& PI=shutterBox->getPort(i);
  //     shutters[i]->addInsertCell("Support",PI.getCell("Void"));
  //     shutters[i]->addInsertCell("Support",shutterBox->getCell("SplitVoid",i));
  //     shutters[i]->addInsertCell("Block",shutterBox->getCell("SplitVoid",i));

  //     shutters[i]->createAll(System,*offPipeA,
  // 			     offPipeA->getSideIndex("FlangeACentre"),
  // 			     PI,PI.getSideIndex("InnerPlate"));
  //   }

  // constructSystem::constructUnit
  //   (System,buildZone,*shutterBox,"back",*offPipeB);

  return;
}

void
R3FrontEnd::createAll(Simulation& System,
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
  ELog::RegMethod RControl("R3FrontEnd","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();

  return;
}


}   // namespace xraySystem
