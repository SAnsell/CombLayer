/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/skadi/SKADI.cxx
 *
 * Copyright (c) 2004-2022 by Tsitohaina Randriamalala/Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
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
#include "BenderUnit.h"
#include "PlateUnit.h"
#include "DiskChopper.h"
#include "Motor.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "SingleChopper.h"
#include "Bunker.h"
#include "CompBInsert.h"
#include "ChopperPit.h"
#include "HoleShape.h"
#include "LineShield.h"
#include "PipeCollimator.h"

#include "SkadiHut.h"

#include "SKADI.h"

namespace essSystem
{
  
SKADI::SKADI(const std::string& keyName):
  attachSystem::CopiedComp("skadi",keyName),
  stopPoint(0),
  skadiAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  BendA(new beamlineSystem::BenderUnit(newName+"BA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::BenderUnit(newName+"BB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::BenderUnit(newName+"BC")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::BenderUnit(newName+"BD")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  BendE(new beamlineSystem::BenderUnit(newName+"BE")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::PlateUnit(newName+"FocusF")),

  CollA(new constructSystem::PipeCollimator(newName+"CollA")),
  CollB(new constructSystem::PipeCollimator(newName+"CollB")),
  CollC(new constructSystem::PipeCollimator(newName+"CollC")),

  BInsert(new CompBInsert(newName+"BInsert")),
  FocusWallA(new beamlineSystem::PlateUnit(newName+"FWallA")),
  
  CInsert(new CompBInsert(newName+"CInsert")),
  FocusWallB(new beamlineSystem::PlateUnit(newName+"FWallB")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  GuideOutA(new beamlineSystem::PlateUnit(newName+"GOutA")),  
  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  PitACutFront(new constructSystem::HoleShape(newName+"PitACutFront")),
  
  PitACutBack(new constructSystem::HoleShape(newName+"PitACutBack")),
  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  ChopAMotor(new essConstruct::Motor(newName+"ChopAMotor")),
  DiskA(new essConstruct::DiskChopper(newName+"ADisk")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  GuideOutB(new beamlineSystem::PlateUnit(newName+"GOutB")),
  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  PitBCutFront(new constructSystem::HoleShape(newName+"PitBCutFront")),
  PitBCutBack(new constructSystem::HoleShape(newName+"PitBCutBack")),
  ChopperB(new essConstruct::SingleChopper(newName+"ChopperB")),
  DiskB(new essConstruct::DiskChopper(newName+"BDisk")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  GuideOutC(new beamlineSystem::PlateUnit(newName+"GOutC")),
  PitC(new constructSystem::ChopperPit(newName+"PitC")),
  PitCCutFront(new
  constructSystem::HoleShape(newName+"PitCCutFront")),
  PitCCutBack(new constructSystem::HoleShape(newName+"PitCCutBack")),
  ChopperC1(new essConstruct::SingleChopper(newName+"ChopperC1")),
  DiskC1(new essConstruct::DiskChopper(newName+"C1Disk")),
  ChopperC2(new essConstruct::SingleChopper(newName+"ChopperC2")),
  DiskC2(new essConstruct::DiskChopper(newName+"C2Disk")),

  ShieldD(new constructSystem::LineShield(newName+"ShieldD")),
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  GuideOutD(new beamlineSystem::PlateUnit(newName+"GOutD")),

  GuideOutE(new beamlineSystem::PlateUnit(newName+"GOutE")),
  Cave(new SkadiHut(newName+"Cave")),
  CaveFrontCut(new constructSystem::HoleShape(newName+"CaveFrontCut")),
  
  ShieldF(new constructSystem::LineShield(newName+"ShieldF"))
  
{
  ELog::RegMethod RegA("SKADI","SKADI");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(skadiAxis);

  OR.addObject(BendA);

  OR.addObject(VPipeB);
  OR.addObject(BendB);
  
  OR.addObject(VPipeC);
  OR.addObject(BendC);  

  OR.addObject(VPipeD);
  OR.addObject(BendD);  

  OR.addObject(VPipeE);
  OR.addObject(BendE);  

  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(CollA);
  OR.addObject(CollB);
  OR.addObject(CollC);

  OR.addObject(BInsert);
  OR.addObject(FocusWallA);

  OR.addObject(CInsert);
  OR.addObject(FocusWallB);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(GuideOutA);
  OR.addObject(PitA);
  OR.addObject(PitACutBack);
  OR.addObject(PitACutFront);
  OR.addObject(ChopperA);
  OR.addObject(DiskA);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(GuideOutB);
  OR.addObject(PitB);
  OR.addObject(PitBCutFront);
  OR.addObject(PitBCutBack);
  OR.addObject(ChopperB);
  OR.addObject(DiskB);

  OR.addObject(ShieldC);
  OR.addObject(VPipeOutC);
  OR.addObject(GuideOutC);
  OR.addObject(PitC);
  OR.addObject(PitCCutFront);
  OR.addObject(PitCCutBack);
  OR.addObject(ChopperC1);
  OR.addObject(DiskC1);
  OR.addObject(ChopperC2);
  OR.addObject(DiskC2);

  OR.addObject(ShieldD);
  OR.addObject(VPipeOutD);
  OR.addObject(GuideOutD);  

  OR.addObject(GuideOutE);

  OR.addObject(Cave);
  OR.addObject(CaveFrontCut); 

  OR.addObject(ShieldF);
}

SKADI::~SKADI()
{}



void
SKADI::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("SKADI","buildBunkerUnits");
  /// Pipe+Guide at Light shutter position

  VPipeB->addAllInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);
  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0);

  VPipeC->addAllInsertCell(bunkerVoid);
  VPipeC->createAll(System,*BendB,"back");
  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,0);

  CollA->setCutSurf("Outer",*VPipeC,-6);
  CollA->setCutSurf("Inner",BendC->getOuterSurf());   // remove front/ back
  CollA->addInsertCell(VPipeC->getCell("Void"));
  CollA->createAll(System,*VPipeC,"#front");

  VPipeD->addAllInsertCell(bunkerVoid);
  VPipeD->createAll(System,*BendC,"back");
  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*BendC,"back");
  
  CollB->setCutSurf("Outer",*VPipeD,-6);
  CollB->setCutSurf("Inner",BendD->getOuterSurf());   // remove front/ back
  CollB->addInsertCell(VPipeD->getCell("Void"));
  CollB->createAll(System,*VPipeD,"#front");

  
  VPipeE->addAllInsertCell(bunkerVoid);
  VPipeE->createAll(System,*BendD,"back");
  BendE->addInsertCell(VPipeE->getCells("Void"));
  BendE->createAll(System,*BendD,"back");
 

  VPipeF->addAllInsertCell(bunkerVoid);
  VPipeF->createAll(System,*BendE,2);
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0);

  CollC->setCutSurf("Outer",*VPipeF,-6);
  CollC->setCutSurf("Inner",FocusF->getOuterSurf());   // remove front/ back
  CollC->addInsertCell(VPipeF->getCell("Void"));
  CollC->createAll(System,*VPipeF,-1);


  return;
}

  
void
SKADI::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param bunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  ELog::RegMethod RegA("SKADI", "build");
  
  ELog::EM<<"\n Building SKADI on:"<<GItem.getKeyName()<<ELog::endDiag;

  ELog::EM<<"\n First Cell w/ SKADI : "<<GItem.getCell("Void")<<ELog::endDiag;
    ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());

  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  
  essBeamSystem::setBeamAxis(*skadiAxis,Control,GItem,1);

  /// Inside the Monolith
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*skadiAxis,-3);
  
  if (stopPoint==1) return;          // Stop at Monolith

  buildBunkerUnits(System,*BendA,2,
                   bunkerObj.getCell("MainVoid"));


  if (stopPoint==2) return;         // Stop at last pipe in Bunker
  
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->createAll(System,*FocusF,"back");

  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);

  FocusWallA->addInsertCell(BInsert->getCells("Item"));
  FocusWallA->createAll(System,*BInsert,0);
  
  if (stopPoint==3) return;

  PitA->addInsertCell(voidCell); //Chopper I pit
  PitA->createAll(System,*FocusWallA,2);

  PitACutFront->addInsertCell(PitA->getCells("MidLayerFront"));
  PitACutFront->setFaces(PitA->getKey("Mid").getFullRule(-1),
			 PitA->getKey("Inner").getFullRule(1));
  PitACutFront->createAll(System,PitA->getKey("Inner"),-1);
  PitACutBack->addInsertCell(PitA->getCells("MidLayerBack"));
  PitACutBack->addInsertCell(PitA->getCells("Collet"));
  PitACutBack->setFaces(PitA->getKey("Mid").getFullRule(-2),
			 PitA->getKey("Inner").getFullRule(2));
  PitACutBack->createAll(System,PitA->getKey("Inner"),2);
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(PitA->getCells("Outer"));
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setBack(PitA->getKey("Mid"),1);  
  ShieldA->createAll(System,*FocusWallA,2);

  CInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  CInsert->addInsertCell(voidCell);
  CInsert->addInsertCell(ShieldA->getCell("Void"));
  CInsert->createAll(System,*BInsert,2);

  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*CInsert);
  
  FocusWallB->addInsertCell(CInsert->getCells("Item"));
  FocusWallB->createAll(System,*CInsert,-1);

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->createAll(System,*PitA,0);

  //  ChopAMotor->addInsertCell(PitA->getCell("Void"));
  //  ChopAMotor->createAll(System,ChopperA->getKey("Main"),1);
  
  DiskA->addInsertCell(ChopperA->getCell("Void"));
  DiskA->setOffsetFlag(1);
  DiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DiskA);
  
  PitB->addInsertCell(voidCell); //Chopper II pit
  PitB->createAll(System,PitA->getKey("Outer"),2);
  PitBCutFront->addInsertCell(PitB->getCells("MidLayerFront"));
  PitBCutFront->setFaces(PitB->getKey("Mid").getFullRule(-1),
			 PitB->getKey("Inner").getFullRule(1));
  PitBCutFront->createAll(System,PitB->getKey("Inner"),-1);
  PitBCutBack->addInsertCell(PitB->getCells("MidLayerBack"));
  PitBCutBack->addInsertCell(PitB->getCells("Collet"));
  PitBCutBack->setFaces(PitB->getKey("Mid").getFullRule(-2),
			 PitB->getKey("Inner").getFullRule(2));
  PitBCutBack->createAll(System,PitB->getKey("Inner"),2);

  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->createAll(System,*PitB,0);
  DiskB->addInsertCell(ChopperB->getCell("Void"));
  DiskB->setOffsetFlag(1);
  DiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*DiskB);
  
  ShieldB->addInsertCell(voidCell);
  ShieldB->addInsertCell(PitA->getCells("Outer"));
  ShieldB->setFront(PitA->getKey("Mid"),2);
  ShieldB->addInsertCell(PitB->getCells("Outer"));
  ShieldB->setBack(PitB->getKey("Mid"),1);  
  ShieldB->createAll(System,PitA->getKey("Mid"),2);
  

  VPipeOutA->addAllInsertCell(ShieldB->getCell("Void"));
  VPipeOutA->createAll(System,PitA->getKey("Mid"),2);
  GuideOutA->addInsertCell(VPipeOutA->getCells("Void"));
  GuideOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  //  VPipeOutB->addInsertCell(ShieldB1->getCell("Void"));
  VPipeOutB->addAllInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,*VPipeOutA,2);

  GuideOutB->addInsertCell(VPipeOutB->getCells("Void"));
  GuideOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  
  PitC->addInsertCell(voidCell); //Chopper III & IV pit
  PitC->createAll(System,PitB->getKey("Outer"),2);

  PitCCutFront->addInsertCell(PitC->getCells("MidLayerFront"));
  PitCCutFront->setFaces(PitC->getKey("Mid").getFullRule(-1),
			 PitC->getKey("Inner").getFullRule(1));
  PitCCutFront->createAll(System,PitC->getKey("Inner"),-1);

  PitCCutBack->addInsertCell(PitC->getCells("MidLayerBack"));
  PitCCutBack->addInsertCell(PitC->getCells("Collet"));
  PitCCutBack->setFaces(PitC->getKey("Mid").getFullRule(-2),
			 PitC->getKey("Inner").getFullRule(2));
  PitCCutBack->createAll(System,PitC->getKey("Inner"),2);

  ChopperC1->addInsertCell(PitC->getCell("Void"));
  ChopperC1->createAll(System,*PitC,0);

  DiskC1->addInsertCell(ChopperC1->getCell("Void"));
  DiskC1->createAll(System,ChopperC1->getKey("Main"),0);
  ChopperC1->insertAxle(System,*DiskC1);
    
  ChopperC2->addInsertCell(PitC->getCell("Void"));
  ChopperC2->createAll(System,*PitC,0);
  DiskC2->addInsertCell(ChopperC2->getCell("Void"));
  DiskC2->createAll(System,ChopperC2->getKey("Main"),0);
  ChopperC2->insertAxle(System,*DiskC2);
    
  ShieldC->addInsertCell(voidCell);
  ShieldC->addInsertCell(PitB->getCells("Outer"));
  ShieldC->addInsertCell(PitC->getCells("Outer"));
  ShieldC->setFront(PitB->getKey("Mid"),2);
  ShieldC->setBack(PitC->getKey("Mid"),1);  
  ShieldC->createAll(System,*VPipeOutB,2);

  VPipeOutC->addAllInsertCell(ShieldC->getCell("Void"));
  VPipeOutC->createAll(System,*ShieldC,-1);
  
  GuideOutC->addInsertCell(VPipeOutC->getCells("Void"));
  GuideOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  ShieldD->addInsertCell(voidCell);
  ShieldD->addInsertCell(PitC->getCells("Outer"));
  ShieldD->addInsertCell(PitC->getCells("MidLayer"));
  ShieldD->setFront(PitC->getKey("Mid"),2);
  ShieldD->createAll(System,*VPipeOutC,2);

  VPipeOutD->addAllInsertCell(ShieldD->getCell("Void"));
  VPipeOutD->setBack(*ShieldD,-2);
  VPipeOutD->createAll(System,*ShieldD,-1);
  GuideOutD->addInsertCell(VPipeOutD->getCells("Void"));
  GuideOutD->createAll(System,*VPipeOutD,0,*VPipeOutD,0);

  ShieldF->addInsertCell(voidCell);  
  ShieldF->createAll(System,*ShieldD,2);

  Cave->addInsertCell(voidCell);
  Cave->addInsertCell(ShieldF->getCell("Void"));
  Cave->createAll(System,*ShieldD,2);
  
  CaveFrontCut->addInsertCell(Cave->getCells("FrontWall"));
  CaveFrontCut->setFaces(Cave->getKey("Outer").getFullRule(-1),
			 Cave->getKey("Inner").getFullRule(1));
  CaveFrontCut->createAll(System,*ShieldD,2);
  
  GuideOutE->addInsertCell(voidCell);
  GuideOutE->addInsertCell(CaveFrontCut->getCells("Void"));
  GuideOutE->addInsertCell(Cave->getCell("Void"));
  GuideOutE->createAll(System,*ShieldD,2,*ShieldD,2);
  

  return;  
}

} // NAMESPACE essSystem

