/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trex/TREX.cxx
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
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "SingleChopper.h"
#include "Bunker.h"
#include "CompBInsert.h"
#include "ChopperPit.h"
#include "HoleShape.h"
#include "LineShield.h"
#include "PipeCollimator.h"

#include "TrexHut.h"

#include "TREX.h"

namespace essSystem
{
  
TREX::TREX(const std::string& keyName):
  attachSystem::CopiedComp("trex",keyName),
  stopPoint(0),
  trexAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  FocusMono(new beamlineSystem::PlateUnit(newName+"FMono")),
  VPipeBridge(new constructSystem::VacuumPipe(newName+"PipeBridge")),
  FocusBridge(new beamlineSystem::PlateUnit(newName+"FBridge")),
  
  VPipeInA(new constructSystem::VacuumPipe(newName+"PipeInA")),
  BendInA(new beamlineSystem::BenderUnit(newName+"BInA")),

  VPipeInB(new constructSystem::VacuumPipe(newName+"PipeInB")),
  BendInB(new beamlineSystem::BenderUnit(newName+"BInB")),

  CollimA(new constructSystem::PipeCollimator(newName+"CollimA")),
  CollimB(new constructSystem::PipeCollimator(newName+"CollimB")),
  CollimC(new constructSystem::PipeCollimator(newName+"CollimC")),

  VPipeInC(new constructSystem::VacuumPipe(newName+"PipeInC")),
  BendInC(new beamlineSystem::BenderUnit(newName+"BInC")),

  BInsertA(new CompBInsert(newName+"BInsertA")),
  FocusWallA(new beamlineSystem::BenderUnit(newName+"FWallA")),

  BInsertB(new CompBInsert(newName+"BInsertB")),
  FocusWallB(new beamlineSystem::BenderUnit(newName+"FWallB")),

  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  PitACutFront(new constructSystem::HoleShape(newName+"PitACutFront")),
  PitACutBack(new constructSystem::HoleShape(newName+"PitACutBack")),
  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  DiskA(new essConstruct::DiskChopper(newName+"ADisk")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  BendOutA(new beamlineSystem::BenderUnit(newName+"BOutA")),
  
  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  PitBCutFront(new constructSystem::HoleShape(newName+"PitBCutFront")),
  PitBCutBack(new constructSystem::HoleShape(newName+"PitBCutBack")),
  ChopperB(new essConstruct::SingleChopper(newName+"ChopperB")),
  DiskB(new essConstruct::DiskChopper(newName+"BDisk")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  BendOutB(new beamlineSystem::BenderUnit(newName+"BOutB")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  PitC(new constructSystem::ChopperPit(newName+"PitC")),
  PitCCutFront(new constructSystem::HoleShape(newName+"PitCCutFront")),
  PitCCutBack(new constructSystem::HoleShape(newName+"PitCCutBack")),
  ChopperC(new essConstruct::SingleChopper(newName+"ChopperC")),
  DiskC(new essConstruct::DiskChopper(newName+"CDisk")),

  ShieldD(new constructSystem::LineShield(newName+"ShieldD")),
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  BendOutD(new beamlineSystem::BenderUnit(newName+"BOutD")),

  PitE(new constructSystem::ChopperPit(newName+"PitE")),
  PitECutFront(new constructSystem::HoleShape(newName+"PitECutFront")),
  PitECutBack(new constructSystem::HoleShape(newName+"PitECutBack")),
  ChopperE(new essConstruct::SingleChopper(newName+"ChopperE")),
  DiskE1(new essConstruct::DiskChopper(newName+"E1Disk")),
  DiskE2(new essConstruct::DiskChopper(newName+"E2Disk")),
  ShieldE(new constructSystem::LineShield(newName+"ShieldE")),
  VPipeOutE(new constructSystem::VacuumPipe(newName+"PipeOutE")),
  GuideOutE(new beamlineSystem::PlateUnit(newName+"GOutE")),

  ShieldF(new constructSystem::LineShield(newName+"ShieldF")),

  Cave(new TrexHut(newName+"Cave")),
  CaveFrontCut(new constructSystem::HoleShape(newName+"CaveFrontCut")),

  VPipeOutG(new constructSystem::VacuumPipe(newName+"PipeOutG")),
  GuideOutG(new beamlineSystem::PlateUnit(newName+"GOutG")),
  
  ChopperG(new essConstruct::SingleChopper(newName+"ChopperG")),
  DiskG(new essConstruct::DiskChopper(newName+"GDisk")),
  
  VPipeOutH(new constructSystem::VacuumPipe(newName+"PipeOutH")),
  GuideOutH(new beamlineSystem::PlateUnit(newName+"GOutH")),
  
  ChopperH(new essConstruct::SingleChopper(newName+"ChopperH")),
  DiskH1(new essConstruct::DiskChopper(newName+"H1Disk")),
  DiskH2(new essConstruct::DiskChopper(newName+"H2Disk")),

  GuideOutI(new beamlineSystem::PlateUnit(newName+"GOutI"))

{
  ELog::RegMethod RegA("TREX","TREX");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(trexAxis);
  
  OR.addObject(FocusMono);

  OR.addObject(VPipeBridge);
  OR.addObject(FocusBridge);
  
  OR.addObject(VPipeInA);
  OR.addObject(BendInA);

  OR.addObject(VPipeInB);
  OR.addObject(BendInB);

  OR.addObject(CollimA);
  OR.addObject(CollimB);
  OR.addObject(CollimC);

  OR.addObject(VPipeInC);
  OR.addObject(BendInC);

  OR.addObject(BInsertA);
  OR.addObject(FocusWallA);

  OR.addObject(BInsertB);
  OR.addObject(FocusWallB);

  OR.addObject(PitA);
  OR.addObject(PitACutFront);
  OR.addObject(PitACutBack);
  OR.addObject(ChopperA);
  OR.addObject(DiskA);
  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(BendOutA);
  
  OR.addObject(PitB);
  OR.addObject(PitBCutFront);
  OR.addObject(PitBCutBack);
  OR.addObject(ChopperB);
  OR.addObject(DiskB);
  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(BendOutB);
  
  OR.addObject(PitC);
  OR.addObject(PitCCutFront);
  OR.addObject(PitCCutBack);
  OR.addObject(ChopperC);
  OR.addObject(DiskC);
  OR.addObject(ShieldC);
  
  for(size_t i=0;i<nC;i++)
    {
      const std::string strNum(std::to_string(i));
      VPipeOutCs[i]=std::shared_ptr<constructSystem::VacuumPipe>
	(new constructSystem::VacuumPipe(newName+"PipeOutC"+strNum));
      BendOutCs[i]=std::shared_ptr<beamlineSystem::BenderUnit>
	(new beamlineSystem::BenderUnit(newName+"BOutC"+strNum));
      OR.addObject(VPipeOutCs[i]);
      OR.addObject(BendOutCs[i]);
    }
  
  OR.addObject(ShieldD);
  OR.addObject(VPipeOutD);
  OR.addObject(BendOutD);
  
  OR.addObject(PitE);
  OR.addObject(PitECutFront);
  OR.addObject(PitECutBack);  
  OR.addObject(ChopperE);
  OR.addObject(DiskE1);
  OR.addObject(DiskE2);

  OR.addObject(ShieldE);

  OR.addObject(VPipeOutE);
  OR.addObject(GuideOutE);
  
  OR.addObject(ShieldF);
  for(size_t i=0;i<nF;i++)
    {
      const std::string strNum(std::to_string(i));

      VPipeOutFs[i]=std::shared_ptr<constructSystem::VacuumPipe>
	(new constructSystem::VacuumPipe(newName+"PipeOutF"+strNum));

      GuideOutFs[i]=std::shared_ptr<beamlineSystem::PlateUnit>
	(new beamlineSystem::PlateUnit(newName+"GOutF"+strNum));

      OR.addObject(VPipeOutFs[i]);
      OR.addObject(GuideOutFs[i]);
    }
  
  OR.addObject(Cave);
  OR.addObject(CaveFrontCut); 
  OR.addObject(VPipeOutG);
  OR.addObject(GuideOutG);
  OR.addObject(ChopperG);
  OR.addObject(DiskG);

  OR.addObject(VPipeOutH);
  OR.addObject(GuideOutH);
  OR.addObject(ChopperH);
  OR.addObject(DiskH1);
  OR.addObject(DiskH2);

  OR.addObject(GuideOutI);
}


TREX::~TREX()
  /*!
    Destructor
   */
{}

void
TREX::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("TREX","buildBunkerUnits");

  /// Brigde Guide 
  VPipeBridge->addAllInsertCell(bunkerVoid); 
  VPipeBridge->createAll(System,FA,startIndex);
  FocusBridge->addInsertCell(VPipeBridge->getCells("Void"));
  FocusBridge->createAll(System,*VPipeBridge,0);
    
  /// Start Curve
  VPipeInA->addAllInsertCell(bunkerVoid);
  VPipeInA->createAll(System,*VPipeBridge,2);
  BendInA->addInsertCell(VPipeInA->getCells("Void"));
  BendInA->createAll(System,*VPipeInA,0);

  //  CollimA->setOuter(VPipeInA->getFullRule(-6));
  //CollimA->setInner(BendInA->getXSection(0,0)); 
  CollimA->setCutSurf("Outer",*VPipeInA,-6);
  CollimA->setCutSurf("Inner",BendInA->getOuterSurf()); 
  CollimA->addInsertCell(VPipeInA->getCells("Void"));
  CollimA->createAll(System,*VPipeInA,-1);

  VPipeInB->addAllInsertCell(bunkerVoid);
  VPipeInB->createAll(System,*BendInA,2);
  BendInB->addInsertCell(VPipeInB->getCells("Void"));
  BendInB->createAll(System,*VPipeInB,0);
  
  CollimB->setCutSurf("Outer",*VPipeInB,-6);
  CollimB->setCutSurf("Inner",BendInB->getOuterSurf()); 
  CollimB->addInsertCell(VPipeInB->getCells("Void"));
  CollimB->createAll(System,*VPipeInB,-1);


  VPipeInC->addAllInsertCell(bunkerVoid);
  VPipeInC->createAll(System,*BendInB,2);
  BendInC->addInsertCell(VPipeInC->getCells("Void"));
  BendInC->createAll(System,*VPipeInC,0);

  //  CollimC->setOuter(VPipeInC->getFullRule(-6));
  //  CollimC->setInner(BendInC->getXSection(0,0)); 
  CollimC->setCutSurf("Outer",*VPipeInC,-6);
  CollimC->setCutSurf("Inner",BendInC->getOuterSurf()); 
  CollimC->addInsertCell(VPipeInC->getCells("Void"));
  CollimC->createAll(System,*VPipeInC,-2);

  return;  
}

void
TREX::buildBunkerWallUnits(Simulation& System,
			   const Bunker& bunkerObj,
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
  ELog::RegMethod RegA("TREX","buildBunkerWallUnits");
  
  BInsertA->addInsertCell(bunkerVoid);
  BInsertA->setFront(bunkerObj,-1);
  BInsertA->createAll(System,FA,startIndex);
  
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertA);

  FocusWallA->addInsertCell(BInsertA->getCells("Item"));
  FocusWallA->setFront(bunkerObj,-1);
  FocusWallA->setBack(*BInsertA,-2);
  FocusWallA->createAll(System,*BInsertA,0);

  BInsertB->setFront(*BInsertA,2);
  BInsertB->setBack(bunkerObj,-2);
  BInsertB->createAll(System,*BInsertA,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertB);

  FocusWallB->setFront(*FocusWallA,"back");
  FocusWallB->setBack(bunkerObj,-2);
  FocusWallB->addInsertCell(BInsertB->getCells("Item"));
  FocusWallB->createAll(System,*BInsertB,0);
  return;
}

void
TREX::build(Simulation& System,
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
  ELog::RegMethod RegA("TREX", "build");
  
  ELog::EM<<"\n Building TREX on:"<<GItem.getKeyName()<<ELog::endDiag;
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  
  essBeamSystem::setBeamAxis(*trexAxis,Control,GItem,1);
  
  /// Inside the Monolith
  FocusMono->addInsertCell(GItem.getCells("Void"));
  FocusMono->setFront(GItem.getKey("Beam"),-1);
  FocusMono->setBack(GItem.getKey("Beam"),-2);
  FocusMono->createAll(System,*trexAxis,-3);
  
  
  if (stopPoint==1) return;                      // STOP At monolith

  buildBunkerUnits(System,*FocusMono,2,
                   bunkerObj.getCell("MainVoid"));


  if (stopPoint==2) return;       // STOP at the bunker edge

  buildBunkerWallUnits(System,bunkerObj,
		       *BendInC,2,
		       bunkerObj.getCell("MainVoid"));

  if (stopPoint==3) return;    // End of bunker

  PitA->addInsertCell(voidCell);  // First pit wrt bunker insert
  PitA->createAll(System,*VPipeBridge,2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(PitA->getCells("Outer"));
  ShieldA->addInsertCell(PitA->getCells("MidLayer"));
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setBack(*PitA,"midFront");  
  ShieldA->createAll(System,*FocusWallA,2);
  ShieldA->insertComponent(System,"Void",*BInsertB);
  
  //  FocusWallB->addInsertCell(BInsertB->getCells("Item"));

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->createAll(System,*PitA,0);
  
  DiskA->addInsertCell(ChopperA->getCell("Void"));
  DiskA->setOffsetFlag(1);
  DiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DiskA);

  
  VPipeOutA->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,*FocusWallB,2);
  BendOutA->addInsertCell(VPipeOutA->getCells("Void"));
  BendOutA->createAll(System,*VPipeOutA,0);
  
  PitACutFront->addInsertCell(PitA->getCells("MidLayerFront"));
  PitACutFront->setCutSurf("front",*PitA,"#midFront");
  PitACutFront->setCutSurf("back",*PitA,"innerFront");
  PitACutFront->createAll(System,*BendOutA,2);

  if (stopPoint==4) return;    // Up to BW1 Chopper pit

  PitB->addInsertCell(voidCell);
  PitB->createAll(System,*VPipeBridge,2);
  
  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->createAll(System,*PitB,0);
  DiskB->addInsertCell(ChopperB->getCell("Void"));
  DiskB->setOffsetFlag(1);
  DiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*DiskB);
    
  ShieldB->addInsertCell(voidCell);
  ShieldB->addInsertCell(PitA->getCells("Outer"));
  ShieldB->addInsertCell(PitA->getCells("MidLayer"));
  ShieldB->setFront(*PitA,"midBack");
  ShieldB->addInsertCell(PitB->getCells("Outer"));
  ShieldB->addInsertCell(PitB->getCells("MidLayer"));
  ShieldB->setBack(*PitB,"midFront");
  ShieldB->createAll(System,*PitA,"midBack");

  VPipeOutB->addAllInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,*ShieldB,-1);
  BendOutB->addInsertCell(VPipeOutB->getCells("Void"));
  BendOutB->createAll(System,*VPipeOutB,0);
  
  PitACutBack->addInsertCell(PitA->getCells("MidLayerBack"));
  PitACutBack->addInsertCell(PitA->getCells("Collet"));
  PitACutBack->setCutSurf("front",*PitA,"innerBack");
  PitACutBack->setCutSurf("back",*PitA,"#midBack");
  PitACutBack->createAll(System,*BendOutB,1);

  PitBCutFront->addInsertCell(PitB->getCells("MidLayerFront"));
  PitBCutFront->setCutSurf("front",*PitB,"#midFront");
  PitBCutFront->setCutSurf("back",*PitB,"innerFront");  
  PitBCutFront->createAll(System,*BendOutB,2);


  if (stopPoint==5) return; // Up to BW2 Chopper Pit

  PitC->addInsertCell(voidCell);
  PitC->createAll(System,*VPipeBridge,2);

  ChopperC->addInsertCell(PitC->getCell("Void"));
  ChopperC->createAll(System,*PitC,0);
  /*
  DiskC->addInsertCell(ChopperC->getCell("Void"));
  DiskC->setCentreFlag(3);
  DiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*DiskC);
  */
  ShieldC->addInsertCell(voidCell);
  ShieldC->addInsertCell(PitB->getCells("Outer"));
  ShieldC->addInsertCell(PitB->getCells("MidLayer"));
  ShieldC->setFront(*PitB,"midBack");
  ShieldC->addInsertCell(PitC->getCells("MidLayer"));
  ShieldC->addInsertCell(PitC->getCells("Outer"));
  ShieldC->setBack(*PitC,"midFront");
  ShieldC->createAll(System,*PitB,"midBack");

  VPipeOutCs[0]->addAllInsertCell(ShieldC->getCell("Void"));
  VPipeOutCs[0]->addAllInsertCell(PitB->getCells("MidLayer"));
  VPipeOutCs[0]->createAll(System,*ShieldC,-1);
  BendOutCs[0]->addInsertCell(VPipeOutCs[0]->getCells("Void"));
  BendOutCs[0]->createAll(System,*VPipeOutCs[0],0);
  
  PitBCutBack->addInsertCell(PitB->getCells("MidLayerBack"));
  PitBCutBack->addInsertCell(PitB->getCells("Collet"));
  PitBCutBack->setCutSurf("front",*PitB,"innerBack");
  PitBCutBack->setCutSurf("back",*PitB,"#midBack");  
  PitBCutBack->createAll(System,*BendOutCs[0],1);

  for(size_t i=1;i<nC-1;i++)
    {      
      VPipeOutCs[i]->addAllInsertCell(ShieldC->getCell("Void"));
      VPipeOutCs[i]->createAll(System,*VPipeOutCs[i-1],2);
      BendOutCs[i]->addInsertCell(VPipeOutCs[i]->getCells("Void"));
      BendOutCs[i]->createAll(System,*VPipeOutCs[i],0);
    }
  
  VPipeOutCs[6]->addAllInsertCell(ShieldC->getCell("Void"));
  VPipeOutCs[6]->addAllInsertCell(PitC->getCells("MidLayer"));
  VPipeOutCs[6]->addAllInsertCell(PitC->getCells("Outer"));
  VPipeOutCs[6]->createAll(System,*VPipeOutCs[5],2);

  BendOutCs[6]->addInsertCell(VPipeOutCs[6]->getCells("Void"));
  BendOutCs[6]->createAll(System,*VPipeOutCs[6],0);

  PitCCutFront->addInsertCell(PitC->getCells("MidLayerFront"));
  PitCCutFront->setCutSurf("front",*PitC,"#midFront");
  PitCCutFront->setCutSurf("back",*PitC,"innerFront");  
  PitCCutFront->createAll(System,*BendOutCs[6],2);


  if (stopPoint==6) return; // Up to T0 Chopper Pit      

  ShieldD->addInsertCell(voidCell);
  ShieldD->addInsertCell(PitC->getCells("Outer"));
  ShieldD->addInsertCell(PitC->getCells("MidLayer"));
  ShieldD->setFront(*PitC,"midBack");
  ShieldD->createAll(System,*PitC,"midBack");

  VPipeOutD->addAllInsertCell(ShieldD->getCell("Void"));
  VPipeOutD->createAll(System,*ShieldD,-1);
  BendOutD->addInsertCell(VPipeOutD->getCells("Void"));
  BendOutD->createAll(System,*VPipeOutD,0);

  PitCCutBack->addInsertCell(PitC->getCells("MidLayerBack"));
  PitCCutBack->addInsertCell(PitC->getCells("Collet"));
  PitCCutBack->setCutSurf("front",*PitC,"innerBack");
  PitCCutBack->setCutSurf("back",*PitC,"#midBack");  
  PitCCutBack->createAll(System,*BendOutD,1);
 
  if (stopPoint==7) return; // END of LOS

  /// End  Curve
  
  /// Start straight up to the next chopper pit
  PitE->addInsertCell(voidCell);
  PitE->createAll(System,*BendOutD,2);
  
  ChopperE->addInsertCell(PitE->getCell("Void")); 
  ChopperE->createAll(System,*PitE,0);

  DiskE1->addInsertCell(ChopperE->getCell("Void"));
  DiskE1->setOffsetFlag(1);
  DiskE1->createAll(System,ChopperE->getKey("Main"),0,
		    ChopperE->getKey("Beam"),0);

  DiskE2->addInsertCell(ChopperE->getCell("Void"));
  DiskE2->setOffsetFlag(1);
  DiskE2->createAll(System,ChopperE->getKey("Main"),0,
		    ChopperE->getKey("Beam"),0);
  
  ChopperE->insertAxle(System,*DiskE1);
  ChopperE->insertAxle(System,*DiskE2);

  
  PitECutFront->addInsertCell(PitE->getCells("MidLayerFront"));
  PitECutFront->setCutSurf("front",*PitE,"#midFront");
  PitECutFront->setCutSurf("back",*PitE,"innerFront");  
  PitECutFront->createAll(System,*PitE,"#innerFront");

  PitECutBack->addInsertCell(PitE->getCells("MidLayerBack"));
  PitECutBack->addInsertCell(PitE->getCells("Collet"));
  PitECutBack->setCutSurf("front",*PitE,"innerBack");
  PitECutBack->setCutSurf("back",*PitE,"#midBack");  
  PitECutBack->createAll(System,*PitE,"innerBack");


  ShieldE->addInsertCell(voidCell);
  ShieldE->setFront(*ShieldD,2);
  ShieldE->addInsertCell(PitE->getCells("Outer"));
  ShieldE->addInsertCell(PitE->getCells("MidLayer"));
  ShieldE->setBack(*PitE,"midFront");
  ShieldE->createAll(System,*BendOutD,2);
  
  VPipeOutE->addAllInsertCell(ShieldE->getCell("Void"));
  VPipeOutE->createAll(System,*ShieldE,-1);


  GuideOutE->addInsertCell(VPipeOutE->getCells("Void"));
  GuideOutE->createAll(System,*VPipeOutE,0);
  
  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*PitECutBack,2);

  ShieldF->addInsertCell(voidCell);
  ShieldF->addInsertCell(PitE->getCells("Outer"));
  ShieldF->addInsertCell(PitE->getCells("MidLayer"));
  ShieldF->addInsertCell(Cave->getCell("L3Front"));
  ShieldF->createAll(System,*PitE,"midBack");

  VPipeOutFs[0]->addAllInsertCell(ShieldF->getCell("Void"));
  VPipeOutFs[0]->createAll(System,*ShieldF,-1);
  
  GuideOutFs[0]->addInsertCell(VPipeOutFs[0]->getCells("Void"));
  GuideOutFs[0]->createAll(System,*VPipeOutFs[0],0);


  for(size_t i=1;i<nF-1;i++)
    {
      VPipeOutFs[i]->addAllInsertCell(ShieldF->getCell("Void"));
      VPipeOutFs[i]->createAll(System,*VPipeOutFs[i-1],2);
      
      GuideOutFs[i]->addInsertCell(VPipeOutFs[i]->getCells("Void"));
      GuideOutFs[i]->createAll(System,*VPipeOutFs[i],0);
    }
  
  CaveFrontCut->addInsertCell(Cave->getCell("L1Front"));
  CaveFrontCut->addInsertCell(Cave->getCell("L2Front"));
  CaveFrontCut->setCutSurf("front",*Cave,"#midFront");
  CaveFrontCut->setCutSurf("back",*Cave,"innerFront");  
  CaveFrontCut->createAll(System,*Cave,"#midFront");
  return;
  VPipeOutFs[7]->addAllInsertCell(ShieldF->getCell("Void"));
  VPipeOutFs[7]->addAllInsertCell(CaveFrontCut->getCells("Void"));
  VPipeOutFs[7]->createAll(System,*VPipeOutFs[6],2);
  
  GuideOutFs[7]->addInsertCell(VPipeOutFs[7]->getCells("Void"));
  GuideOutFs[7]->createAll(System,*VPipeOutFs[7],0);

  GuideOutG->addInsertCell(CaveFrontCut->getCells("Void"));
  GuideOutG->addInsertCell(Cave->getCell("Void"));
  GuideOutG->createAll(System,*VPipeOutFs[7],2);
  
  ChopperG->addInsertCell(Cave->getCell("Void"));
  ChopperG->createAll(System,*GuideOutG,2);
  
  DiskG->addInsertCell(ChopperG->getCell("Void"));
  DiskG->setOffsetFlag(1);
  DiskG->createAll(System,ChopperG->getKey("Main"),0);

  ChopperG->insertAxle(System,*DiskG);

  VPipeOutH->addAllInsertCell(Cave->getCell("Void"));
  VPipeOutH->createAll(System,ChopperG->getKey("Beam"),2);
  GuideOutH->addInsertCell(VPipeOutH->getCells("Void"));
  GuideOutH->createAll(System,*VPipeOutH,0);
  
  ChopperH->addInsertCell(Cave->getCell("Void"));
  ChopperH->createAll(System,*CaveFrontCut,2);
  DiskH1->addInsertCell(ChopperH->getCell("Void"));
  DiskH1->setOffsetFlag(1);
  DiskH1->createAll(System,ChopperH->getKey("Main"),0);
  DiskH2->addInsertCell(ChopperH->getCell("Void"));
  DiskH2->setOffsetFlag(1);
  DiskH2->createAll(System,ChopperH->getKey("Main"),0);

  ChopperH->insertAxle(System,*DiskH1);
  ChopperH->insertAxle(System,*DiskH2);

  
  GuideOutI->addInsertCell(Cave->getCell("Void"));
  GuideOutI->createAll(System,ChopperH->getKey("Beam"),2);
  
  return;  
}

} // NAMESPACE essSystem

