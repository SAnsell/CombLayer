/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/dream/DREAM.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "CompBInsert.h"
#include "SingleChopper.h"
#include "EssHut.h"
#include "LineShield.h"
#include "PipeCollimator.h"

#include "DREAM.h"

namespace essSystem
{

DREAM::DREAM(const std::string& keyName) :
  attachSystem::CopiedComp("dream",keyName),
  stopPoint(0),
  dreamAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),
 
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::PlateUnit(newName+"FB")),

  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  DDisk(new essConstruct::DiskChopper(newName+"DBlade")),
  SDisk(new essConstruct::DiskChopper(newName+"SBlade")),

  CollimA(new constructSystem::PipeCollimator(newName+"CollimA")),
  CollimB(new constructSystem::PipeCollimator(newName+"CollimB")),
  CollimC(new constructSystem::PipeCollimator(newName+"CollimC")),

  VPipeC0(new constructSystem::VacuumPipe(newName+"PipeC0")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::PlateUnit(newName+"FC")),
  
  ChopperB(new essConstruct::SingleChopper(newName+"ChopperB")),
  BandADisk(new essConstruct::DiskChopper(newName+"BandADisk")),  

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::PlateUnit(newName+"FD")),
  
  ChopperC(new essConstruct::SingleChopper(newName+"ChopperC")), 
  T0DiskA(new essConstruct::DiskChopper(newName+"T0DiskA")),

  VPipeE1(new constructSystem::VacuumPipe(newName+"PipeE1")),
  FocusE1(new beamlineSystem::PlateUnit(newName+"FE1")),

  VPipeE2(new constructSystem::VacuumPipe(newName+"PipeE2")),
  FocusE2(new beamlineSystem::PlateUnit(newName+"FE2")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::PlateUnit(newName+"FF")),
  VPipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  FocusG(new beamlineSystem::PlateUnit(newName+"FG")),
  
  BInsertA(new CompBInsert(newName+"BInsertA")),
  BInsertB(new CompBInsert(newName+"BInsertB")),
  FocusWallA(new beamlineSystem::PlateUnit(newName+"FWallA")),


  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::PlateUnit(newName+"FOutA")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::PlateUnit(newName+"FOutB")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::PlateUnit(newName+"FOutC")),

  Cave(new EssHut(newName+"Cave")),

  VPipeCaveA(new constructSystem::VacuumPipe(newName+"PipeCaveA")),
  FocusCaveA(new beamlineSystem::PlateUnit(newName+"FCaveA")),

  FocusCaveB(new beamlineSystem::PlateUnit(newName+"FCaveB"))

 /*!
    Constructor
    \param keyName :: main instrument name
 */
{
  ELog::RegMethod RegA("DREAM","DREAM");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.addObject(dreamAxis);

  OR.addObject(FocusA);

  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(ChopperA);
  OR.addObject(DDisk);  
  OR.addObject(SDisk);
 
  OR.addObject(CollimA);
  OR.addObject(CollimB);
  OR.addObject(CollimC);
  
  OR.addObject(VPipeC0);
  OR.addObject(VPipeC);
  OR.addObject(FocusC);
  OR.addObject(ChopperB);
  OR.addObject(BandADisk);
  
  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(ChopperC);
  OR.addObject(T0DiskA);

  OR.addObject(VPipeE1);
  OR.addObject(FocusE1);
  OR.addObject(VPipeE2);
  OR.addObject(FocusE2);

  OR.addObject(VPipeG);
  OR.addObject(FocusG);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(BInsertA);
  OR.addObject(BInsertB);
  OR.addObject(FocusWallA);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
  
  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);

  OR.addObject(ShieldC);
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);

  OR.addObject(Cave);
  
  OR.addObject(VPipeCaveA);
  OR.addObject(FocusCaveA);
  OR.addObject(FocusCaveB);
}
  
void 
DREAM::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("DREAM","build");

  ELog::EM<<"\nBuilding DREAM on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());  // CONTROL modified
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt("front")
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*dreamAxis,Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,GItem.getKey("Beam"),-1);

  if (stopPoint==1) return;                      // STOP At monolith edge

  VPipeB->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,GItem.getKey("Beam"),2);
  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0);
  
  // NEW TEST SECTION:
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,*FocusB,2);
  
  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setOffsetFlag(1);  // centre flag
  DDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDisk);

  // Double disk chopper
  SDisk->addInsertCell(ChopperA->getCell("Void"));
  SDisk->setOffsetFlag(1);  // Centre offset control
  SDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*SDisk);
  
  VPipeC0->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC0->createAll(System,ChopperA->getKey("Beam"),2);

  VPipeC->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,*VPipeC0,2);
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0);
  

  //  CollimA->setOuter(VPipeC->getFullRule(-6));
  //  CollimA->setInner(FocusC->getXSection(0,0));
  CollimA->setCutSurf("Outer",*VPipeC,-6);
  CollimA->setCutSurf("Inner",FocusC->getOuterSurf());
  CollimA->addInsertCell(VPipeC->getCells("Void"));
  CollimA->createAll(System,*VPipeC,-1);

  // NEW TEST SECTION:
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,*VPipeC,2);
  BandADisk->addInsertCell(ChopperB->getCell("Void"));
  BandADisk->setOffsetFlag(1);  // Centre offset control
  BandADisk->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*BandADisk);
    
  VPipeD->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0);

  // NEW TEST SECTION:
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,*VPipeD,2);

  // First disk of a T0 chopper
  T0DiskA->addInsertCell(ChopperC->getCell("Void",0));
  T0DiskA->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*T0DiskA);
  
  VPipeE1->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE1->createAll(System,ChopperC->getKey("Beam"),2);
  FocusE1->addInsertCell(VPipeE1->getCells("Void"));
  FocusE1->createAll(System,*VPipeE1,0);

  //  CollimB->setOuter(VPipeE1->getFullRule(-6));
  //CollimB->setInner(FocusE1->getXSection(0,0))
  CollimB->setCutSurf("Outer",*VPipeE1,-6);
  CollimB->setCutSurf("Inner",FocusE1->getOuterSurf());
  CollimB->addInsertCell(VPipeE1->getCells("Void"));
  CollimB->createAll(System,*VPipeE1,-1);

  VPipeE2->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE2->createAll(System,*VPipeE1,2);
  FocusE2->addInsertCell(VPipeE2->getCells("Void"));
  FocusE2->createAll(System,*VPipeE2,0);
  
  //  CollimC->setOuter(VPipeE2->getFullRule(-6));
  //  CollimC->setInner(FocusE2->getXSection(0,0));
  CollimC->setCutSurf("Outer",*VPipeE2,-6);
  CollimC->setCutSurf("Inner",FocusE2->getOuterSurf());
  CollimC->addInsertCell(VPipeE2->getCells("Void"));
  CollimC->createAll(System,*VPipeE2,-2);

  // move guide
  VPipeF->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,*VPipeE2,2);
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0);

  VPipeG->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeG->createAll(System,*VPipeF,2);
  FocusG->addInsertCell(VPipeG->getCells("Void"));
  FocusG->createAll(System,*VPipeG,0);

  if (stopPoint==2) return;      

  // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  //  BInsert->createAll(System,FocusF->getKey("Guide0"),2,bunkerObj);
  //  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);

  BInsertA->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsertA->createAll(System,*VPipeG,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertA);
  

  FocusWallA->createAll(System,*BInsertA,-1);

  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->createAll(System,*BInsertA,2);
  BInsertA->addInsertCell(ShieldA->getCell("Void"));
  BInsertA->insertObjects(System);

  BInsertB->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsertB->addInsertCell(ShieldA->getCell("Void"));
  BInsertB->addInsertCell(voidCell);
  BInsertB->createAll(System,*BInsertA,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertB);

  FocusWallA->insertInCell(System,BInsertA->getCell("Item"));
  FocusWallA->insertInCell(System,BInsertB->getCell("Item"));
  FocusWallA->insertInCell(System,ShieldA->getCell("Void"));


  if (stopPoint==3) return;                      // STOP At bunker edge
  
  // Section up to 41.35 m 
  VPipeOutA->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->setBack(*ShieldA,-2);
  VPipeOutA->createAll(System,*FocusWallA,2);
      
  FocusOutA->addInsertCell(VPipeOutA->getCell("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0);

  // Section to 58.95m
  ShieldB->addInsertCell(voidCell);
  ShieldB->createAll(System,*ShieldA,2);

  VPipeOutB->addAllInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->setFront(*ShieldB,-1);
  VPipeOutB->setBack(*ShieldB,-2);
  VPipeOutB->createAll(System,*ShieldB,-1);

  FocusOutA->insertInCell(System,VPipeOutB->getCell("Void"));
  FocusOutB->setFront(*FocusOutA,"back");
  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0);

  // Section to 70.8m
  ShieldC->addInsertCell(voidCell);
  ShieldC->createAll(System,*ShieldB,2);

  VPipeOutC->addAllInsertCell(ShieldC->getCell("Void"));
  VPipeOutC->setFront(*ShieldC,-1);
  VPipeOutC->setBack(*ShieldC,-2);
  VPipeOutC->createAll(System,*ShieldC,-1);

  FocusOutC->addInsertCell(VPipeOutC->getCell("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0);

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldC,2);
  Cave->insertComponent(System,"FrontWall",*ShieldC);

  VPipeCaveA->addAllInsertCell(Cave->getCell("FrontWall"));
  VPipeCaveA->addAllInsertCell(Cave->getCell("Void"));
  VPipeCaveA->setFront(*VPipeOutC,2);
  VPipeCaveA->createAll(System,*VPipeOutC,2);
  FocusCaveA->addInsertCell(VPipeCaveA->getCell("Void"));
  FocusCaveA->createAll(System,*VPipeCaveA,7);
    
  FocusCaveB->addInsertCell(Cave->getCell("Void"));
  FocusCaveB->createAll(System,*FocusCaveA,2);         
  
  return;
}


}   // NAMESPACE essSystem

