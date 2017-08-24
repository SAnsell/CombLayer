/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/skadi/SKADI.cxx
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
#include "ChopperUnit.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "ChopperPit.h"
#include "HoleShape.h"
#include "LineShield.h"
#include "PipeCollimator.h"
#include "Aperture.h"

#include "SkadiHut.h"

#include "SKADI.h"

namespace essSystem
{
  
SKADI::SKADI(const std::string& keyName):
  attachSystem::CopiedComp("skadi",keyName),
  stopPoint(0),
  skadiAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  BendA(new beamlineSystem::GuideLine(newName+"BA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeInA(new constructSystem::VacuumPipe(newName+"PipeInA")),
  BendInA(new beamlineSystem::GuideLine(newName+"BInA")),

  VPipeInB(new constructSystem::VacuumPipe(newName+"PipeInB")),
  BendInB(new beamlineSystem::GuideLine(newName+"BInB")),

  VPipeInC(new constructSystem::VacuumPipe(newName+"PipeInC")),
  BendInC(new beamlineSystem::GuideLine(newName+"BInC")),

  VPipeInD(new constructSystem::VacuumPipe(newName+"PipeInD")),
  GuideInD(new beamlineSystem::GuideLine(newName+"GInD")),

  CollimA(new constructSystem::PipeCollimator(newName+"CollimA")),
  CollimB(new constructSystem::PipeCollimator(newName+"CollimB")),
  CollimC(new constructSystem::PipeCollimator(newName+"CollimC")),

  BInsert(new CompBInsert(newName+"BInsert")),
  FocusWallA(new beamlineSystem::GuideLine(newName+"FWallA")),
  
  CInsert(new CompBInsert(newName+"CInsert")),
  FocusWallB(new beamlineSystem::GuideLine(newName+"FWallB")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  ShieldA1(new constructSystem::LineShield(newName+"ShieldA1")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  GuideOutA(new beamlineSystem::GuideLine(newName+"GOutA")),  
  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  PitACutFront(new
  constructSystem::HoleShape(newName+"PitACutFront")),
  PitACutBack(new constructSystem::HoleShape(newName+"PitACutBack")),
  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DiskA(new constructSystem::DiskChopper(newName+"ADisk")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  ShieldB1(new constructSystem::LineShield(newName+"ShieldB1")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  GuideOutB(new beamlineSystem::GuideLine(newName+"GOutB")),
  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  PitBCutFront(new
  constructSystem::HoleShape(newName+"PitBCutFront")),
  PitBCutBack(new constructSystem::HoleShape(newName+"PitBCutBack")),
  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  DiskB(new constructSystem::DiskChopper(newName+"BDisk")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  ShieldC1(new constructSystem::LineShield(newName+"ShieldC1")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  GuideOutC(new beamlineSystem::GuideLine(newName+"GOutC")),
  PitC(new constructSystem::ChopperPit(newName+"PitC")),
  PitCCutFront(new
  constructSystem::HoleShape(newName+"PitCCutFront")),
  PitCCutBack(new constructSystem::HoleShape(newName+"PitCCutBack")),
  ChopperC1(new constructSystem::ChopperUnit(newName+"ChopperC1")),
  DiskC1(new constructSystem::DiskChopper(newName+"C1Disk")),
  ChopperC2(new constructSystem::ChopperUnit(newName+"ChopperC2")),
  DiskC2(new constructSystem::DiskChopper(newName+"C2Disk")),

  ShieldD(new constructSystem::LineShield(newName+"ShieldD")),
  ShieldD1(new constructSystem::LineShield(newName+"ShieldD1")),
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  GuideOutD(new beamlineSystem::GuideLine(newName+"GOutD")),

  GuideOutE(new beamlineSystem::GuideLine(newName+"GOutE")),
  Cave(new SkadiHut(newName+"Cave")),
  CaveFrontCut(new constructSystem::HoleShape(newName+"CaveFrontCut")),
  
  ShieldF(new constructSystem::LineShield(newName+"ShieldF"))
 
{
  ELog::RegMethod RegA("SKADI","SKADI");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.cell(newName+"Axis");

  OR.addObject(skadiAxis);

  OR.addObject(BendA);

  OR.addObject(VPipeB);
  OR.addObject(BendB);

  OR.addObject(VPipeInA);
  OR.addObject(BendInA);
  
  OR.addObject(VPipeInB);
  OR.addObject(BendInB);  

  OR.addObject(VPipeInC);
  OR.addObject(BendInC);  

  OR.addObject(VPipeInD);
  OR.addObject(GuideInD);

  OR.addObject(CollimA);
  OR.addObject(CollimB);
  OR.addObject(CollimC);

  OR.addObject(BInsert);
  OR.addObject(FocusWallA);

  OR.addObject(CInsert);
  OR.addObject(FocusWallB);

  OR.addObject(ShieldA);
  OR.addObject(ShieldA1);
  OR.addObject(VPipeOutA);
  OR.addObject(GuideOutA);
  OR.addObject(PitA);
  OR.addObject(PitACutBack);
  OR.addObject(PitACutFront);
  OR.addObject(ChopperA);
  OR.addObject(DiskA);

  OR.addObject(ShieldB);
  OR.addObject(ShieldB1);
  OR.addObject(VPipeOutB);
  OR.addObject(GuideOutB);
  OR.addObject(PitB);
  OR.addObject(PitBCutFront);
  OR.addObject(PitBCutBack);
  OR.addObject(ChopperB);
  OR.addObject(DiskB);

  OR.addObject(ShieldC);
  OR.addObject(ShieldC1);
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
  OR.addObject(ShieldD1);
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
    ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());

  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  
  essBeamSystem::setBeamAxis(*skadiAxis,Control,GItem,1);

  /// Inside the Monolith
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*skadiAxis,-3,*skadiAxis,-3);
  
  if (stopPoint==1) return;          // Stop at Monolith

  /// Pipe+Guide at Light shutter position
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,BendA->getKey("Guide0"),2);
  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);

  VPipeInA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeInA->createAll(System,BendB->getKey("Guide0"),2);
  BendInA->addInsertCell(VPipeInA->getCells("Void"));
  BendInA->createAll(System,*VPipeInA,0,*VPipeInA,0);

  CollimA->setOuter(VPipeInA->getSignedFullRule(-6));
  CollimA->setInner(BendInA->getXSection(0,0)); 
  CollimA->addInsertCell(VPipeInA->getCell("Void"));
  CollimA->createAll(System,*VPipeInA,-1);
  
  VPipeInB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeInB->createAll(System,BendInA->getKey("Guide0"),2);
  BendInB->addInsertCell(VPipeInB->getCells("Void"));
  BendInB->createAll(System,BendInA->getKey("Guide0"),2,
		     BendInA->getKey("Guide0"),2);
  
  CollimB->setOuter(VPipeInB->getSignedFullRule(-6));
  CollimB->setInner(BendInB->getXSection(0,0)); 
  CollimB->addInsertCell(VPipeInB->getCell("Void"));
  CollimB->createAll(System,*VPipeInB,-1);

  
  VPipeInC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeInC->createAll(System,BendInB->getKey("Guide0"),2);
  BendInC->addInsertCell(VPipeInC->getCells("Void"));
  BendInC->createAll(System,BendInB->getKey("Guide0"),2,
		     BendInB->getKey("Guide0"),2);

  VPipeInD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeInD->createAll(System,BendInC->getKey("Guide0"),2);
  GuideInD->addInsertCell(VPipeInD->getCells("Void"));
  GuideInD->createAll(System,*VPipeInD,0,*VPipeInD,0);

  CollimC->setOuter(VPipeInD->getSignedFullRule(-6)); 
  CollimC->setInner(GuideInD->getXSection(0,0)); 
  CollimC->addInsertCell(VPipeInD->getCell("Void"));
  CollimC->createAll(System,*VPipeInD,-1);
  
  if (stopPoint==2) return;         // Stop at last pipe in Bunker
  
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->createAll(System,*VPipeInD,2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);
  FocusWallA->addInsertCell(BInsert->getCells("Item"));
  FocusWallA->createAll(System,*BInsert,0,*BInsert,0);
  
  if (stopPoint==3) return;

  PitA->addInsertCell(voidCell); //Chopper I pit
  PitA->createAll(System,FocusWallA->getKey("Guide0"),2);

  PitACutFront->addInsertCell(PitA->getCells("MidLayerFront"));
  PitACutFront->setFaces(PitA->getKey("Mid").getSignedFullRule(-1),
			 PitA->getKey("Inner").getSignedFullRule(1));
  PitACutFront->createAll(System,PitA->getKey("Inner"),-1);
  PitACutBack->addInsertCell(PitA->getCells("MidLayerBack"));
  PitACutBack->addInsertCell(PitA->getCells("Collet"));
  PitACutBack->setFaces(PitA->getKey("Mid").getSignedFullRule(-2),
			 PitA->getKey("Inner").getSignedFullRule(2));
  PitACutBack->createAll(System,PitA->getKey("Inner"),2);
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(PitA->getCells("Outer"));
  ShieldA->addInsertCell(PitA->getCells("MidLayer"));
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setBack(PitA->getKey("Mid"),1);  
  ShieldA->createAll(System,FocusWallA->getKey("Guide0"),2);

  ShieldA1->addInsertCell(ShieldA->getCell("Void"));
  ShieldA1->addInsertCell(PitA->getCells("Outer"));
  ShieldA1->addInsertCell(PitA->getCells("MidLayer"));
  ShieldA1->setBack(PitA->getKey("Mid"),1);
  ShieldA1->setFront(bunkerObj,2);
  ShieldA1->createAll(System,*ShieldA,-1);

  CInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  CInsert->addInsertCell(voidCell);
  CInsert->addInsertCell(ShieldA1->getCell("Void"));
  CInsert->createAll(System,*BInsert,2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*CInsert);
  
  FocusWallB->addInsertCell(CInsert->getCells("Item"));
  FocusWallB->createAll(System,*CInsert,-1,*CInsert,-1);

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->createAll(System,*PitA,0);
  DiskA->addInsertCell(ChopperA->getCell("Void"));
  DiskA->setCentreFlag(3);
  DiskA->setOffsetFlag(1);
  DiskA->createAll(System,ChopperA->getKey("Beam"),0);
  
  PitB->addInsertCell(voidCell); //Chopper II pit
  PitB->createAll(System,PitA->getKey("Outer"),2);
  PitBCutFront->addInsertCell(PitB->getCells("MidLayerFront"));
  PitBCutFront->setFaces(PitB->getKey("Mid").getSignedFullRule(-1),
			 PitB->getKey("Inner").getSignedFullRule(1));
  PitBCutFront->createAll(System,PitB->getKey("Inner"),-1);
  PitBCutBack->addInsertCell(PitB->getCells("MidLayerBack"));
  PitBCutBack->addInsertCell(PitB->getCells("Collet"));
  PitBCutBack->setFaces(PitB->getKey("Mid").getSignedFullRule(-2),
			 PitB->getKey("Inner").getSignedFullRule(2));
  PitBCutBack->createAll(System,PitB->getKey("Inner"),2);

  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->createAll(System,*PitB,0);
  DiskB->addInsertCell(ChopperB->getCell("Void"));
  DiskB->setCentreFlag(3);
  DiskB->setOffsetFlag(1);
  DiskB->createAll(System,ChopperB->getKey("Beam"),0);
  
  ShieldB->addInsertCell(voidCell);
  ShieldB->addInsertCell(PitA->getCells("Outer"));
  ShieldB->addInsertCell(PitA->getCells("MidLayer"));
  ShieldB->setFront(PitA->getKey("Mid"),2);
  ShieldB->addInsertCell(PitB->getCells("Outer"));
  ShieldB->addInsertCell(PitB->getCells("MidLayer"));
  ShieldB->setBack(PitB->getKey("Mid"),1);  
  ShieldB->createAll(System,PitA->getKey("Mid"),2);
  
  ShieldB1->addInsertCell(ShieldB->getCell("Void"));
  ShieldB1->addInsertCell(PitA->getCells("Outer"));
  ShieldB1->addInsertCell(PitA->getCells("MidLayer"));
  ShieldB1->setFront(PitA->getKey("Mid"),2);
  ShieldB1->addInsertCell(PitB->getCells("Outer"));
  ShieldB1->addInsertCell(PitB->getCells("MidLayer"));
  ShieldB1->setBack(PitB->getKey("Mid"),1);  
  ShieldB1->createAll(System,*ShieldB,-1);

  VPipeOutA->addInsertCell(ShieldB1->getCell("Void"));
  VPipeOutA->createAll(System,PitA->getKey("Mid"),2);
  GuideOutA->addInsertCell(VPipeOutA->getCells("Void"));
  GuideOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  VPipeOutB->addInsertCell(ShieldB1->getCell("Void"));
  VPipeOutB->createAll(System,*VPipeOutA,2);
  VPipeOutB->addInsertCell(PitB->getCells("Outer"));
  VPipeOutB->addInsertCell(PitB->getCells("MidLayer"));
  VPipeOutB->setBack(PitB->getKey("Mid"),1); 
  GuideOutB->addInsertCell(VPipeOutB->getCells("Void"));
  GuideOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);
  
  PitC->addInsertCell(voidCell); //Chopper III & IV pit
  PitC->createAll(System,PitB->getKey("Outer"),2);
  PitCCutFront->addInsertCell(PitC->getCells("MidLayerFront"));
  PitCCutFront->setFaces(PitC->getKey("Mid").getSignedFullRule(-1),
			 PitC->getKey("Inner").getSignedFullRule(1));
  PitCCutFront->createAll(System,PitC->getKey("Inner"),-1);
  PitCCutBack->addInsertCell(PitC->getCells("MidLayerBack"));
  PitCCutBack->addInsertCell(PitC->getCells("Collet"));
  PitCCutBack->setFaces(PitC->getKey("Mid").getSignedFullRule(-2),
			 PitC->getKey("Inner").getSignedFullRule(2));
  PitCCutBack->createAll(System,PitC->getKey("Inner"),2);

  ChopperC1->addInsertCell(PitC->getCell("Void"));
  ChopperC1->createAll(System,*PitC,0);
  DiskC1->addInsertCell(ChopperC1->getCell("Void"));
  DiskC1->setCentreFlag(3);
  DiskC1->setOffsetFlag(1);
  DiskC1->createAll(System,ChopperC1->getKey("Beam"),0);
  
  ChopperC2->addInsertCell(PitC->getCell("Void"));
  ChopperC2->createAll(System,*PitC,0);
  DiskC2->addInsertCell(ChopperC2->getCell("Void"));
  DiskC2->setCentreFlag(3);
  DiskC2->setOffsetFlag(1);
  DiskC2->createAll(System,ChopperC2->getKey("Beam"),0);
  
  ShieldC->addInsertCell(voidCell);
  ShieldC->addInsertCell(PitB->getCells("Outer"));
  ShieldC->addInsertCell(PitB->getCells("MidLayer"));
  ShieldC->setFront(PitB->getKey("Mid"),2);
  ShieldC->addInsertCell(PitC->getCells("Outer"));
  ShieldC->addInsertCell(PitC->getCells("MidLayer"));
  ShieldC->setBack(PitC->getKey("Mid"),1);  
  ShieldC->createAll(System,*VPipeOutB,2);

  ShieldC1->addInsertCell(ShieldC->getCell("Void"));
  ShieldC1->addInsertCell(PitB->getCells("Outer"));
  ShieldC1->addInsertCell(PitB->getCells("MidLayer"));
  ShieldC1->setFront(PitB->getKey("Mid"),2);
  ShieldC1->addInsertCell(PitC->getCells("Outer"));
  ShieldC1->addInsertCell(PitC->getCells("MidLayer"));
  ShieldC1->setBack(PitC->getKey("Mid"),1);  
  ShieldC1->createAll(System,*ShieldC,-1);

  VPipeOutC->addInsertCell(ShieldC1->getCell("Void"));
  VPipeOutC->createAll(System,*ShieldC1,-1);
  GuideOutC->addInsertCell(VPipeOutC->getCells("Void"));
  GuideOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  ShieldD->addInsertCell(voidCell);
  ShieldD->addInsertCell(PitC->getCells("Outer"));
  ShieldD->addInsertCell(PitC->getCells("MidLayer"));
  ShieldD->setFront(PitC->getKey("Mid"),2);
  ShieldD->createAll(System,*VPipeOutC,2);

  ShieldD1->addInsertCell(ShieldD->getCell("Void"));
  ShieldD1->addInsertCell(PitC->getCells("Outer"));
  ShieldD1->addInsertCell(PitC->getCells("MidLayer"));
  ShieldD1->setFront(PitC->getKey("Mid"),2);
  ShieldD1->setBack(*ShieldD,-2);
  ShieldD1->createAll(System,*ShieldD,-1);

  VPipeOutD->addInsertCell(ShieldD1->getCell("Void"));
  VPipeOutD->setBack(*ShieldD,-2);
  VPipeOutD->createAll(System,*ShieldD1,-1);
  GuideOutD->addInsertCell(VPipeOutD->getCells("Void"));
  GuideOutD->createAll(System,*VPipeOutD,0,*VPipeOutD,0);

  ShieldF->addInsertCell(voidCell);  
  ShieldF->createAll(System,*ShieldD,2);

  Cave->addInsertCell(voidCell);
  Cave->addInsertCell(ShieldF->getCell("Void"));
  Cave->createAll(System,*ShieldD,2);
  
  CaveFrontCut->addInsertCell(Cave->getCells("FrontWall"));
  CaveFrontCut->setFaces(Cave->getKey("Outer").getSignedFullRule(-1),
			 Cave->getKey("Inner").getSignedFullRule(1));
  CaveFrontCut->createAll(System,*ShieldD,2);
  
  GuideOutE->addInsertCell(voidCell);
  GuideOutE->addInsertCell(CaveFrontCut->getCells("Void"));
  GuideOutE->addInsertCell(Cave->getCell("Void"));
  GuideOutE->createAll(System,*ShieldD,2,*ShieldD,2);
  

  return;  
}

} // NAMESPACE essSystem

