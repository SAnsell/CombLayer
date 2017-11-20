/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/ODIN.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <utility>
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
#include "stringCombine.h"
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
#include "Simulation.h"
#include "debugMethod.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "VacuumPipe.h"
#include "SingleChopper.h"
#include "TwinBase.h"
#include "TwinChopper.h"
#include "Jaws.h"
#include "LineShield.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "Hut.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"
#include "PinHole.h"
#include "RentrantBS.h"
#include "ODIN.h"

namespace essSystem
{

ODIN::ODIN(const std::string& keyName) :
  attachSystem::CopiedComp("odin",keyName),
  stopPoint(0),
  odinAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperAA(new constructSystem::SingleChopper(newName+"ChopperAA")),
  DiskAA(new constructSystem::DiskChopper(newName+"DiskAA")),
  ChopperAB(new constructSystem::SingleChopper(newName+"ChopperAB")),
  DiskAB(new constructSystem::DiskChopper(newName+"DiskAB")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  ChopperB(new constructSystem::SingleChopper(newName+"ChopperB")),
  T0Disk(new constructSystem::DiskChopper(newName+"T0Disk")),
  
  ChopperFOC1(new constructSystem::SingleChopper(newName+"ChopperFOC1")),
  FOC1Disk(new constructSystem::DiskChopper(newName+"FOC1Blade")),
  
  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperFOC2(new constructSystem::SingleChopper(newName+"ChopperFOC2")),
  FOC2Disk(new constructSystem::DiskChopper(newName+"FOC2Blade")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  ChopperFOC3(new constructSystem::SingleChopper(newName+"ChopperFOC3")),
  FOC3Disk(new constructSystem::DiskChopper(newName+"FOC3Blade")),

  VPipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  FocusG(new beamlineSystem::GuideLine(newName+"FG")),

  ChopperFOC4(new constructSystem::SingleChopper(newName+"ChopperFOC4")),
  FOC4Disk(new constructSystem::DiskChopper(newName+"FOC4Blade")),

  VPipeH(new constructSystem::VacuumPipe(newName+"PipeH")),
  FocusH(new beamlineSystem::GuideLine(newName+"FH")),

  BInsert(new BunkerInsert(keyName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(keyName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"OutFA")),
  
  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACut(new constructSystem::HoleShape(newName+"OutACut")),
  OutBCut(new constructSystem::HoleShape(newName+"OutBCut")),
  ChopOutFOC5(new constructSystem::SingleChopper(newName+"ChopOutFOC5")),
  FOC5Disk(new constructSystem::DiskChopper(newName+"FOC5Blade")),
  
  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"OutFB")),

  Cave(new essSystem::Hut(newName+"Cave")),
  CaveCut(new constructSystem::HoleShape(newName+"CaveCut")),
  CaveMidCut(new constructSystem::HoleShape(newName+"CaveMidCut")),

  VPipeCaveA(new constructSystem::VacuumPipe(newName+"PipeCaveA")),
  FocusCaveA(new beamlineSystem::GuideLine(newName+"CaveFA")),

  PinA(new constructSystem::PinHole(newName+"Pin")),

  BeamStop(new RentrantBS(newName+"BeamStop"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.cell(newName+"Axis");
  OR.addObject(odinAxis);

  OR.addObject(FocusA);
  
  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(ChopperAA);
  OR.addObject(DiskAA);
  OR.addObject(ChopperAB);
  OR.addObject(DiskAB);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(ChopperB);
  OR.addObject(T0Disk);

  OR.addObject(ChopperFOC1);
  OR.addObject(FOC1Disk);

  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(ChopperFOC2);
  OR.addObject(FOC2Disk);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);
  
  OR.addObject(ChopperFOC3);
  OR.addObject(FOC3Disk);
  
  OR.addObject(VPipeG);
  OR.addObject(FocusG);

  OR.addObject(ChopperFOC4);
  OR.addObject(FOC4Disk);

  OR.addObject(FocusWall);
  OR.addObject(BInsert);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
  
  OR.addObject(OutPitA);
  OR.addObject(OutACut);
  OR.addObject(ChopOutFOC5);
  OR.addObject(FOC5Disk);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);

  OR.addObject(Cave);
  OR.addObject(CaveCut);
  OR.addObject(CaveMidCut);

  OR.addObject(VPipeCaveA);
  OR.addObject(FocusCaveA);

  OR.addObject(PinA);
  OR.addObject(BeamStop);
}

ODIN::~ODIN()
  /*!
    Destructor
  */
{}

void
ODIN::buildBunkerUnits(Simulation& System,
                       const attachSystem::FixedComp& FA,
                       const long int startIndex,
                       const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FA :: Fixed component to start build from [Mono guide]
    \param startIndex :: Fixed component link point
    \param bunkerVoid :: cell to place objects in
  */
{
  ELog::RegMethod RegA("ODIN","buildBunkerUnits");

  VPipeB->addInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);  
  
  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);
  
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  ChopperAA->addInsertCell(bunkerVoid);
  ChopperAA->createAll(System,FocusC->getKey("Guide0"),2);

  DiskAA->addInsertCell(ChopperAA->getCell("Void"));
  DiskAA->createAll(System,ChopperAA->getKey("Main"),0);
  ChopperAA->insertAxle(System,*DiskAA);

  ChopperAB->addInsertCell(bunkerVoid);
  ChopperAB->createAll(System,ChopperAA->getKey("Beam"),2);

  DiskAB->addInsertCell(ChopperAB->getCell("Void"));
  DiskAB->createAll(System,ChopperAB->getKey("Main"),0);
  ChopperAB->insertAxle(System,*DiskAB);

  VPipeD->addInsertCell(bunkerVoid);
  VPipeD->createAll(System,ChopperAB->getKey("Beam"),2);
  
  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // T0 disk chopper
  ChopperB->addInsertCell(bunkerVoid);
  ChopperB->createAll(System,FocusD->getKey("Guide0"),2);
  T0Disk->addInsertCell(ChopperB->getCell("Void"));
  T0Disk->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*T0Disk);
  
  // FOC1 disk chopper
  ChopperFOC1->addInsertCell(bunkerVoid);
  ChopperFOC1->createAll(System,ChopperB->getKey("Beam"),2);
  FOC1Disk->addInsertCell(ChopperFOC1->getCell("Void"));
  FOC1Disk->createAll(System,ChopperFOC1->getKey("Main"),0);
  ChopperFOC1->insertAxle(System,*FOC1Disk);
  
  VPipeE->addInsertCell(bunkerVoid);
  VPipeE->createAll(System,ChopperFOC1->getKey("Beam"),2);
  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // FOC2 disk chopper
  ChopperFOC2->addInsertCell(bunkerVoid);
  ChopperFOC2->createAll(System,FocusE->getKey("Guide0"),2);
  FOC2Disk->addInsertCell(ChopperFOC2->getCell("Void"));
  FOC2Disk->createAll(System,ChopperFOC2->getKey("Main"),0);
  ChopperFOC2->insertAxle(System,*FOC2Disk);
  
  VPipeF->addInsertCell(bunkerVoid);
  VPipeF->createAll(System,ChopperFOC2->getKey("Beam"),2);
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

  // FOC3 disk chopper [16.49m]
  ChopperFOC3->addInsertCell(bunkerVoid);
  ChopperFOC3->createAll(System,FocusF->getKey("Guide0"),2);
  FOC3Disk->addInsertCell(ChopperFOC3->getCell("Void"));
  FOC3Disk->createAll(System,ChopperFOC3->getKey("Main"),0);
  ChopperFOC3->insertAxle(System,*FOC3Disk);
  
  VPipeG->addInsertCell(bunkerVoid);
  VPipeG->createAll(System,ChopperFOC3->getKey("Beam"),2);
  FocusG->addInsertCell(VPipeG->getCells("Void"));
  FocusG->createAll(System,*VPipeG,0,*VPipeG,0);

  // FOC4 disk chopper [22.97m]
  ChopperFOC4->addInsertCell(bunkerVoid);
  ChopperFOC4->createAll(System,FocusG->getKey("Guide0"),2);
  FOC4Disk->addInsertCell(ChopperFOC4->getCell("Void"));
  FOC4Disk->createAll(System,ChopperFOC4->getKey("Main"),0);
  ChopperFOC4->insertAxle(System,*FOC4Disk);
  
  VPipeH->addInsertCell(bunkerVoid);
  VPipeH->createAll(System,ChopperFOC4->getKey("Beam"),2);
  FocusH->addInsertCell(VPipeH->getCells("Void"));
  FocusH->createAll(System,*VPipeH,0,*VPipeH,0);
  
  return;
}

void
ODIN::buildOutGuide(Simulation& System,
			const attachSystem::FixedComp& FA,
			const long int startIndex,
			const int voidCell)
  /*!
    Build the section out of the bunker wall
    \param System :: Simulation ot use
    \param FA :: Fixed point for guide
    \param startIndex :: Fixed point link point 
    \param voidCell :: void cell for constuction 
   */
{
  ELog::RegMethod RegA("ODIN","buildOutGuide");

  ShieldA->addInsertCell(voidCell);
  ShieldA->createAll(System,FA,startIndex);

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FA,startIndex);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,*ShieldA,2);

  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->insertObjects(System);
  
  
  OutACut->addInsertCell(OutPitA->getCells("MidLayerFront"));
  OutACut->setFaces(OutPitA->getKey("Inner").getFullRule(1),
                    OutPitA->getKey("Mid").getFullRule(-1));
  OutACut->createAll(System,OutPitA->getKey("Inner"),1);

  OutBCut->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutBCut->addInsertCell(OutPitA->getCells("Collet"));
  OutBCut->setFaces(OutPitA->getKey("Inner").getFullRule(2),
                    OutPitA->getKey("Mid").getFullRule(-2));
  OutBCut->createAll(System,OutPitA->getKey("Inner"),2);
  
    
  // 15m WBC chopper
  ChopOutFOC5->addInsertCell(OutPitA->getCell("Void"));
  ChopOutFOC5->createAll(System,FocusOutA->getKey("Guide0"),2);
  // Double disk chopper
  FOC5Disk->addInsertCell(ChopOutFOC5->getCell("Void"));
  FOC5Disk->createAll(System,ChopOutFOC5->getKey("Main"),0);
  ChopOutFOC5->insertAxle(System,*FOC5Disk);
  
  ShieldB->addInsertCell(voidCell);
  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->setFront(OutPitA->getKey("Mid"),2);
  ShieldB->createAll(System,OutPitA->getKey("Inner"),0);
  
  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,ChopOutFOC5->getKey("Beam"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  
  return;
}

void
ODIN::buildCave(Simulation& System,
                const attachSystem::FixedComp& FA,
                const long int startIndex,
                const int voidCell)
  /*!
    Build the cave system
    \param System :: Simulation ot use
    \param FA :: Fixed point for guide
    \param startIndex :: Fixed point link point 
    \param voidCell :: void cell for constuction 
  */
{
  ELog::RegMethod RegA("ODIN","buildCave");


  Cave->addInsertCell(voidCell);
  Cave->createAll(System,FA,startIndex);

  ShieldB->addInsertCell(Cave->getCells("ConcNoseFront"));
  ShieldB->insertObjects(System);

  CaveCut->addInsertCell(Cave->getCells("FeNose"));
  CaveCut->setFaces(Cave->getKey("Outer").getFullRule(-1),
                    Cave->getKey("Inner").getFullRule(1));
  CaveCut->createAll(System,Cave->getKey("Inner"),-1);

  VPipeCaveA->addInsertCell(Cave->getCells("VoidNose"));
  VPipeCaveA->createAll(System,Cave->getKey("Inner"),-1);

  FocusCaveA->addInsertCell(VPipeCaveA->getCells("Void"));
  FocusCaveA->createAll(System,*VPipeCaveA,0,*VPipeCaveA,0);

  CaveMidCut->addInsertCell(Cave->getCells("InnerWall"));
  CaveMidCut->setFaces(Cave->getSurf("InnerWallFront"),
                       -Cave->getSurf("InnerWallBack"));
  CaveMidCut->createAll(System,Cave->getKey("Inner"),-1);

  PinA->addInsertCell(Cave->getCell("VoidNose"));
  PinA->createAll(System,FocusCaveA->getKey("Guide0"),2);

  BeamStop->addInsertCell(Cave->getCell("VoidMain"));
  BeamStop->createAll(System,FocusCaveA->getKey("Guide0"),2);

  return;
}
  

void 
ODIN::build(Simulation& System,
            const GuideItem& GItem,
	    const Bunker& bunkerObj,const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("ODIN","build");
  ELog::debugMethod DA;

  ELog::EM<<"\nBuilding ODIN on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;

  essBeamSystem::setBeamAxis(*odinAxis,Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*odinAxis,-3,*odinAxis,-3); 

  if (stopPoint==1) return;

  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

    // First collimator [In WALL]
  BInsert->createAll(System,FocusH->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);
      // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);

  if (stopPoint==3) return;

  ShieldA->setFront(bunkerObj,2);
  buildOutGuide(System,FocusWall->getKey("Guide0"),2,voidCell);

  buildCave(System,*ShieldB,2,voidCell);

  return;


  

 
  return;
}

}   // NAMESPACE essSystem

