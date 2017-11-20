/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trex/TREX.cxx
 *
 * Copyright (c) 2004-2017 by Tsitohaina Randriamalala/Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "debugMethod.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "SecondTrack.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "SingleChopper.h"
#include "Bunker.h"
#include "BunkerInsert.h"
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
  nC(7),nF(8),stopPoint(0),
  trexAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusMono(new beamlineSystem::GuideLine(newName+"FMono")),
  VPipeBridge(new constructSystem::VacuumPipe(newName+"PipeBridge")),
  FocusBridge(new beamlineSystem::GuideLine(newName+"FBridge")),
  
  VPipeInA(new constructSystem::VacuumPipe(newName+"PipeInA")),
  BendInA(new beamlineSystem::GuideLine(newName+"BInA")),

  VPipeInB(new constructSystem::VacuumPipe(newName+"PipeInB")),
  BendInB(new beamlineSystem::GuideLine(newName+"BInB")),

  CollimA(new constructSystem::PipeCollimator(newName+"CollimA")),
  CollimB(new constructSystem::PipeCollimator(newName+"CollimB")),
  CollimC(new constructSystem::PipeCollimator(newName+"CollimC")),

  VPipeInC(new constructSystem::VacuumPipe(newName+"PipeInC")),
  BendInC(new beamlineSystem::GuideLine(newName+"BInC")),

  BInsertA(new CompBInsert(newName+"BInsertA")),
  FocusWallA(new beamlineSystem::GuideLine(newName+"FWallA")),

  BInsertB(new CompBInsert(newName+"BInsertB")),
  FocusWallB(new beamlineSystem::GuideLine(newName+"FWallB")),

  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  PitACutFront(new constructSystem::HoleShape(newName+"PitACutFront")),
  PitACutBack(new constructSystem::HoleShape(newName+"PitACutBack")),
  ChopperA(new constructSystem::SingleChopper(newName+"ChopperA")),
  DiskA(new constructSystem::DiskChopper(newName+"ADisk")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  BendOutA(new beamlineSystem::GuideLine(newName+"BOutA")),
  
  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  PitBCutFront(new constructSystem::HoleShape(newName+"PitBCutFront")),
  PitBCutBack(new constructSystem::HoleShape(newName+"PitBCutBack")),
  ChopperB(new constructSystem::SingleChopper(newName+"ChopperB")),
  DiskB(new constructSystem::DiskChopper(newName+"BDisk")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  BendOutB(new beamlineSystem::GuideLine(newName+"BOutB")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  PitC(new constructSystem::ChopperPit(newName+"PitC")),
  PitCCutFront(new constructSystem::HoleShape(newName+"PitCCutFront")),
  PitCCutBack(new constructSystem::HoleShape(newName+"PitCCutBack")),
  ChopperC(new constructSystem::SingleChopper(newName+"ChopperC")),
  DiskC(new constructSystem::DiskChopper(newName+"CDisk")),

  ShieldD(new constructSystem::LineShield(newName+"ShieldD")),
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  BendOutD(new beamlineSystem::GuideLine(newName+"BOutD")),

  PitE(new constructSystem::ChopperPit(newName+"PitE")),
  PitECutFront(new constructSystem::HoleShape(newName+"PitECutFront")),
  PitECutBack(new constructSystem::HoleShape(newName+"PitECutBack")),
  ChopperE(new constructSystem::SingleChopper(newName+"ChopperE")),
  DiskE1(new constructSystem::DiskChopper(newName+"E1Disk")),
  DiskE2(new constructSystem::DiskChopper(newName+"E2Disk")),
  ShieldE(new constructSystem::LineShield(newName+"ShieldE")),
  VPipeOutE(new constructSystem::VacuumPipe(newName+"PipeOutE")),
  GuideOutE(new beamlineSystem::GuideLine(newName+"GOutE")),

  ShieldF(new constructSystem::LineShield(newName+"ShieldF")),

  Cave(new TrexHut(newName+"Cave")),
  CaveFrontCut(new constructSystem::HoleShape(newName+"CaveFrontCut")),

  VPipeOutG(new constructSystem::VacuumPipe(newName+"PipeOutG")),
  GuideOutG(new beamlineSystem::GuideLine(newName+"GOutG")),
  
  ChopperG(new constructSystem::SingleChopper(newName+"ChopperG")),
  DiskG(new constructSystem::DiskChopper(newName+"GDisk")),
  
  VPipeOutH(new constructSystem::VacuumPipe(newName+"PipeOutH")),
  GuideOutH(new beamlineSystem::GuideLine(newName+"GOutH")),
  
  ChopperH(new constructSystem::SingleChopper(newName+"ChopperH")),
  DiskH1(new constructSystem::DiskChopper(newName+"H1Disk")),
  DiskH2(new constructSystem::DiskChopper(newName+"H2Disk")),

  GuideOutI(new beamlineSystem::GuideLine(newName+"GOutI"))

{
  ELog::RegMethod RegA("TREX","TREX");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.cell(newName+"Axis");

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
      const std::string strNum(StrFunc::makeString(i));
      VPipeOutCs[i]=std::shared_ptr<constructSystem::VacuumPipe>
	(new constructSystem::VacuumPipe(newName+"PipeOutC"+strNum));
      BendOutCs[i]=std::shared_ptr<beamlineSystem::GuideLine>
	(new beamlineSystem::GuideLine(newName+"BOutC"+strNum));
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
      const std::string strNum(StrFunc::makeString(i));

      VPipeOutFs[i]=std::shared_ptr<constructSystem::VacuumPipe>
	(new constructSystem::VacuumPipe(newName+"PipeOutF"+strNum));

      GuideOutFs[i]=std::shared_ptr<beamlineSystem::GuideLine>
	(new beamlineSystem::GuideLine(newName+"GOutF"+strNum));

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
  VPipeBridge->addInsertCell(bunkerVoid); 
  VPipeBridge->createAll(System,FA,startIndex);
  FocusBridge->addInsertCell(VPipeBridge->getCells("Void"));
  FocusBridge->createAll(System,*VPipeBridge,0,*VPipeBridge,0);
    
  /// Start Curve
  VPipeInA->addInsertCell(bunkerVoid);
  VPipeInA->createAll(System,*VPipeBridge,2);
  BendInA->addInsertCell(VPipeInA->getCells("Void"));
  BendInA->createAll(System,*VPipeInA,0,*VPipeInA,0);
  
  CollimA->setOuter(VPipeInA->getFullRule(-6));
  CollimA->setInner(BendInA->getXSection(0,0)); 
  CollimA->addInsertCell(VPipeInA->getCells("Void"));
  CollimA->createAll(System,*VPipeInA,-1);

  VPipeInB->addInsertCell(bunkerVoid);
  VPipeInB->createAll(System,BendInA->getKey("Guide0"),2);
  BendInB->addInsertCell(VPipeInB->getCells("Void"));
  BendInB->createAll(System,*VPipeInB,0,*VPipeInB,0);

  CollimB->setOuter(VPipeInB->getFullRule(-6));
  CollimB->setInner(BendInB->getXSection(0,0)); 
  CollimB->addInsertCell(VPipeInB->getCells("Void"));
  CollimB->createAll(System,*VPipeInB,-1);

  VPipeInC->addInsertCell(bunkerVoid);
  VPipeInC->createAll(System,BendInB->getKey("Guide0"),2);
  BendInC->addInsertCell(VPipeInC->getCells("Void"));
  BendInC->createAll(System,*VPipeInC,0,*VPipeInC,0);

  CollimC->setOuter(VPipeInC->getFullRule(-6));
  CollimC->setInner(BendInC->getXSection(0,0)); 
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
  BInsertA->createAll(System,FA,startIndex,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertA);

  FocusWallA->addInsertCell(BInsertA->getCells("Item"));
  FocusWallA->createAll(System,*BInsertA,0,*BInsertA,0);

  BInsertB->createAll(System,*BInsertA,2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsertB);
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
  FocusMono->createAll(System,*trexAxis,-3,*trexAxis,-3);
  
  
  if (stopPoint==1) return;                      // STOP At monolith

  buildBunkerUnits(System,FocusMono->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  
  if (stopPoint==2) return;       // STOP at the bunker edge

  buildBunkerWallUnits(System,bunkerObj,
		       BendInC->getKey("Guide0"),2,
		       bunkerObj.getCell("MainVoid"));


  if (stopPoint==3)
    {
      ShieldA->insertComponent(System,"Void",*BInsertB);
      BInsertB->insertInCell(System,voidCell);
      return;       // STOP at the outside of the bunker
    }
    
  PitA->addInsertCell(voidCell);  // First pit wrt bunker insert
  PitA->createAll(System,*VPipeBridge,2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(PitA->getCells("Outer"));
  ShieldA->addInsertCell(PitA->getCells("MidLayer"));
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setBack(PitA->getKey("Mid"),1);  
  ShieldA->createAll(System,FocusWallA->getKey("Guide0"),2);
  ShieldA->insertComponent(System,"Void",*BInsertB);
  
  FocusWallB->addInsertCell(BInsertB->getCells("Item"));
  FocusWallB->createAll(System,*BInsertB,0,*BInsertB,0);

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->createAll(System,*PitA,0);
  
  DiskA->addInsertCell(ChopperA->getCell("Void"));
  DiskA->setOffsetFlag(1);
  DiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DiskA);
  
  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FocusWallB->getKey("Guide0"),2);
  BendOutA->addInsertCell(VPipeOutA->getCells("Void"));
  BendOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);
  
  PitACutFront->addInsertCell(PitA->getCells("MidLayerFront"));
  PitACutFront->setFaces(PitA->getKey("Mid").getFullRule(-1),
			 PitA->getKey("Inner").getFullRule(1));
  PitACutFront->createAll(System,BendOutA->getKey("Guide0"),2);

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
  ShieldB->setFront(PitA->getKey("Mid"),2);
  ShieldB->addInsertCell(PitB->getCells("Outer"));
  ShieldB->addInsertCell(PitB->getCells("MidLayer"));
  ShieldB->setBack(PitB->getKey("Mid"),1);
  ShieldB->createAll(System,PitA->getKey("Mid"),2);

  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,*ShieldB,-1);
  BendOutB->addInsertCell(VPipeOutB->getCells("Void"));
  BendOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  
  PitACutBack->addInsertCell(PitA->getCells("MidLayerBack"));
  PitACutBack->addInsertCell(PitA->getCells("Collet"));  
  PitACutBack->setFaces(PitA->getKey("Inner").getFullRule(2),
			 PitA->getKey("Mid").getFullRule(-2));
  PitACutBack->createAll(System,BendOutB->getKey("Guide0"),1);

  PitBCutFront->addInsertCell(PitB->getCells("MidLayerFront"));
  PitBCutFront->setFaces(PitB->getKey("Mid").getFullRule(-1),
			 PitB->getKey("Inner").getFullRule(1));
  PitBCutFront->createAll(System,BendOutB->getKey("Guide0"),2);
  
  
  if (stopPoint==4) return; // Up to BW2 Chopper Pit

  PitC->addInsertCell(voidCell);
  PitC->createAll(System,*VPipeBridge,2);

  ChopperC->addInsertCell(PitC->getCell("Void"));
  ChopperC->createAll(System,*PitC,0);
  DiskC->addInsertCell(ChopperC->getCell("Void"));
  DiskC->setCentreFlag(3);
  DiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*DiskC);
  
  ShieldC->addInsertCell(voidCell);
  ShieldC->addInsertCell(PitB->getCells("Outer"));
  ShieldC->addInsertCell(PitB->getCells("MidLayer"));
  ShieldC->setFront(PitB->getKey("Mid"),2);
  ShieldC->addInsertCell(PitC->getCells("MidLayer"));
  ShieldC->addInsertCell(PitC->getCells("Outer"));
  ShieldC->setBack(PitC->getKey("Mid"),1);
  ShieldC->createAll(System,PitB->getKey("Mid"),2);

  VPipeOutCs[0]->addInsertCell(ShieldC->getCell("Void"));
  VPipeOutCs[0]->addInsertCell(PitB->getCells("MidLayer"));
  VPipeOutCs[0]->createAll(System,*ShieldC,-1);
  BendOutCs[0]->addInsertCell(VPipeOutCs[0]->getCells("Void"));
  BendOutCs[0]->createAll(System,*VPipeOutCs[0],0,*VPipeOutCs[0],0);
  
  PitBCutBack->addInsertCell(PitB->getCells("MidLayerBack"));
  PitBCutBack->addInsertCell(PitB->getCells("Collet"));  
  PitBCutBack->setFaces(PitB->getKey("Inner").getFullRule(2),
			 PitB->getKey("Mid").getFullRule(-2));
  PitBCutBack->createAll(System,BendOutCs[0]->getKey("Guide0"),1);
  
  const attachSystem::FixedComp* LinkPtr= &BendOutCs[0]->getKey("Guide0");
      
  for(size_t i=1;i<nC-1;i++)
    {      
      VPipeOutCs[i]->addInsertCell(ShieldC->getCell("Void"));
      VPipeOutCs[i]->createAll(System,*VPipeOutCs[i-1],2);
      BendOutCs[i]->addInsertCell(VPipeOutCs[i]->getCells("Void"));
      BendOutCs[i]->createAll(System,*VPipeOutCs[i],0,*VPipeOutCs[i],0);
      LinkPtr= &BendOutCs[i]->getKey("Guide0");
    }
  
  VPipeOutCs[6]->addInsertCell(ShieldC->getCell("Void"));
  VPipeOutCs[6]->addInsertCell(PitC->getCells("MidLayer"));
  VPipeOutCs[6]->addInsertCell(PitC->getCells("Outer"));
  VPipeOutCs[6]->createAll(System,*VPipeOutCs[5],2);

  BendOutCs[6]->addInsertCell(VPipeOutCs[6]->getCells("Void"));
  BendOutCs[6]->createAll(System,*VPipeOutCs[6],0,*VPipeOutCs[6],0);

  LinkPtr= &BendOutCs[6]->getKey("Guide0");

  PitCCutFront->addInsertCell(PitC->getCells("MidLayerFront"));
  PitCCutFront->setFaces(PitC->getKey("Mid").getFullRule(-1),
  			 PitC->getKey("Inner").getFullRule(1));
  PitCCutFront->createAll(System,*LinkPtr,2);

  
  if (stopPoint==5) return; // Up to T0 Chopper Pit      

  ShieldD->addInsertCell(voidCell);
  ShieldD->addInsertCell(PitC->getCells("Outer"));
  ShieldD->addInsertCell(PitC->getCells("MidLayer"));
  ShieldD->setFront(PitC->getKey("Mid"),2);
  ShieldD->createAll(System,PitC->getKey("Mid"),2);

  VPipeOutD->addInsertCell(ShieldD->getCell("Void"));
  VPipeOutD->createAll(System,*ShieldD,-1);
  BendOutD->addInsertCell(VPipeOutD->getCells("Void"));
  BendOutD->createAll(System,*VPipeOutD,0,*VPipeOutD,0);

  PitCCutBack->addInsertCell(PitC->getCells("MidLayerBack"));
  PitCCutBack->addInsertCell(PitC->getCells("Collet"));  
  PitCCutBack->setFaces(PitC->getKey("Inner").getFullRule(2),
			 PitC->getKey("Mid").getFullRule(-2));
  PitCCutBack->createAll(System,BendOutD->getKey("Guide0"),1);

 
  if (stopPoint==6) return; // END of LOS
 
  /// End  Curve
  
  /// Start straight up to the next chopper pit
  PitE->addInsertCell(voidCell);
  PitE->createAll(System,BendOutD->getKey("Guide0"),2);
  
  ChopperE->addInsertCell(PitE->getCell("Void")); 
  ChopperE->createAll(System,*PitE,0);
  DiskE1->addInsertCell(ChopperE->getCell("Void"));
  DiskE1->setOffsetFlag(1);

  DiskE1->createAll(System,ChopperE->getKey("Beam"),0);
  DiskE2->addInsertCell(ChopperE->getCell("Void"));
  DiskE2->setOffsetFlag(1);
  
  DiskE2->createAll(System,ChopperE->getKey("Beam"),0);
  ChopperE->insertAxle(System,*DiskE1);
  ChopperE->insertAxle(System,*DiskE2);

  
  PitECutFront->addInsertCell(PitE->getCells("MidLayerFront"));
  PitECutFront->setFaces(PitE->getKey("Mid").getFullRule(-1),
			 PitE->getKey("Inner").getFullRule(1));
  PitECutFront->createAll(System,PitE->getKey("Inner"),-1);
  PitECutBack->addInsertCell(PitE->getCells("MidLayerBack"));
  PitECutBack->addInsertCell(PitE->getCells("Collet"));  
  PitECutBack->setFaces(PitE->getKey("Inner").getFullRule(2),
			 PitE->getKey("Mid").getFullRule(-2));
  PitECutBack->createAll(System,PitE->getKey("Inner"),2);
  
  ShieldE->addInsertCell(voidCell);
  ShieldE->setFront(*ShieldD,2);
  ShieldE->addInsertCell(PitE->getCells("Outer"));
  ShieldE->addInsertCell(PitE->getCells("MidLayer"));
  ShieldE->setBack(PitE->getKey("Mid"),1);
  ShieldE->createAll(System,BendOutD->getKey("Guide0"),2);
  
  VPipeOutE->addInsertCell(ShieldE->getCell("Void"));
  VPipeOutE->createAll(System,*ShieldE,-1);

  GuideOutE->addInsertCell(VPipeOutE->getCells("Void"));
  GuideOutE->createAll(System,*VPipeOutE,0,*VPipeOutE,0);
  
  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*PitECutBack,2);

  ShieldF->addInsertCell(voidCell);
  ShieldF->addInsertCell(PitE->getCells("Outer"));
  ShieldF->addInsertCell(PitE->getCells("MidLayer"));
  ShieldF->addInsertCell(Cave->getCell("L3Front"));
  ShieldF->createAll(System,PitE->getKey("Mid"),2);
   
  VPipeOutFs[0]->addInsertCell(ShieldF->getCell("Void"));
  VPipeOutFs[0]->createAll(System,*ShieldF,-1);
  
  GuideOutFs[0]->addInsertCell(VPipeOutFs[0]->getCells("Void"));
  GuideOutFs[0]->createAll(System,*VPipeOutFs[0],0,*VPipeOutFs[0],0);
  
  for(size_t i=1;i<nF-1;i++)
    {
      VPipeOutFs[i]->addInsertCell(ShieldF->getCell("Void"));
      VPipeOutFs[i]->createAll(System,*VPipeOutFs[i-1],2);
      
      GuideOutFs[i]->addInsertCell(VPipeOutFs[i]->getCells("Void"));
      GuideOutFs[i]->createAll(System,*VPipeOutFs[i],0,*VPipeOutFs[i],0);
    }
  
  CaveFrontCut->addInsertCell(Cave->getCell("L1Front"));
  CaveFrontCut->addInsertCell(Cave->getCell("L2Front"));
  CaveFrontCut->setFaces(Cave->getKey("Mid").getFullRule(-1),
  			 Cave->getKey("Inner").getFullRule(1));
  CaveFrontCut->createAll(System,Cave->getKey("Mid"),-1);
  
  VPipeOutFs[7]->addInsertCell(ShieldF->getCell("Void"));
  VPipeOutFs[7]->addInsertCell(CaveFrontCut->getCells("Void"));
  VPipeOutFs[7]->createAll(System,*VPipeOutFs[6],2);
  
  GuideOutFs[7]->addInsertCell(VPipeOutFs[7]->getCells("Void"));
  GuideOutFs[7]->createAll(System,*VPipeOutFs[7],0,*VPipeOutFs[7],0);
  
  GuideOutG->addInsertCell(CaveFrontCut->getCells("Void"));
  GuideOutG->addInsertCell(Cave->getCell("Void"));
  GuideOutG->createAll(System,*VPipeOutFs[7],2,*VPipeOutFs[7],2);
  
  ChopperG->addInsertCell(Cave->getCell("Void"));
  ChopperG->createAll(System,GuideOutG->getKey("Guide0"),2);
  DiskG->addInsertCell(ChopperG->getCell("Void"));
  DiskG->setOffsetFlag(1);
  DiskG->createAll(System,ChopperG->getKey("Main"),0);
  ChopperG->insertAxle(System,*DiskG);
  
  VPipeOutH->addInsertCell(Cave->getCell("Void"));
  VPipeOutH->createAll(System,ChopperG->getKey("Beam"),2);
  GuideOutH->addInsertCell(VPipeOutH->getCells("Void"));
  GuideOutH->createAll(System,*VPipeOutH,0,*VPipeOutH,0);
  
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
  GuideOutI->createAll(System,ChopperH->getKey("Beam"),2,
		       ChopperH->getKey("Beam"),2);
  
  return;  
}

} // NAMESPACE essSystem

