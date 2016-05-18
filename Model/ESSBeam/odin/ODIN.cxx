/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/ODIN.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "World.h"
#include "AttachSupport.h"
#include "Jaws.h"
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
  odinAxis(new attachSystem::FixedComp(newName+"Axis",4)),
  BladeChopper(new constructSystem::DiskChopper(newName+"Blade")),
  GuideA(new beamlineSystem::GuideLine(newName+"GA")),
  T0Chopper(new constructSystem::DiskChopper(newName+"TZero")),
  GuideB(new beamlineSystem::GuideLine(newName+"GB")),
  BInsert(new BunkerInsert(newName+"BInsert")),
  GuideC(new beamlineSystem::GuideLine(newName+"GC")),
  GuideD(new beamlineSystem::GuideLine(newName+"GD")),

  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  GuidePitAFront(new beamlineSystem::GuideLine(newName+"GPitAFront")),
  GuidePitABack(new beamlineSystem::GuideLine(newName+"GPitABack")),
  ChopperA(new constructSystem::DiskChopper(newName+"ChopperA")),

  GuideE(new beamlineSystem::GuideLine(newName+"GE")),

  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  GuidePitBFront(new beamlineSystem::GuideLine(newName+"GPitBFront")),
  GuidePitBBack(new beamlineSystem::GuideLine(newName+"GPitBBack")),
  ChopperB(new constructSystem::DiskChopper(newName+"ChopperB")),
  GuideF(new beamlineSystem::GuideLine(newName+"GF")),

  PitC(new constructSystem::ChopperPit(newName+"PitC")),
  GuidePitCFront(new beamlineSystem::GuideLine(newName+"GPitCFront")),
  GuidePitCBack(new beamlineSystem::GuideLine(newName+"GPitCBack")),
  GuideG(new beamlineSystem::GuideLine(newName+"GG")),

  Cave(new essSystem::Hut(newName+"Cave")),
  GuideH(new beamlineSystem::GuideLine(newName+"GH")),
  PinA(new PinHole(newName+"Pin")),

  BeamStop(new RentrantBS(newName+"BeamStop"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.cell(newName+"Axis");
  OR.addObject(odinAxis);
  
  OR.addObject(BladeChopper);
  OR.addObject(GuideA);
  OR.addObject(T0Chopper);
  OR.addObject(GuideB);
  OR.addObject(BInsert);
  
  OR.addObject(GuideC);
  OR.addObject(GuideD);

  OR.addObject(PitA);
  OR.addObject(GuidePitAFront);
  OR.addObject(GuidePitABack);
  OR.addObject(ChopperA);
    
  OR.addObject(GuideE);

  OR.addObject(PitB);
  OR.addObject(GuidePitBFront);
  OR.addObject(GuidePitBBack);
  OR.addObject(ChopperB);
  OR.addObject(GuideF);

  OR.addObject(PitC);
  OR.addObject(GuidePitCFront);
  OR.addObject(GuidePitCBack);
  OR.addObject(GuideG);

  OR.addObject(Cave);
  OR.addObject(GuideH);
  OR.addObject(PinA);
  OR.addObject(BeamStop);
  
}

ODIN::~ODIN()
  /*!
    Destructor
  */
{}

void
ODIN::setBeamAxis(const attachSystem::FixedGroup& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Primary beam object
    \param reverseZ :: Reverse Z direction
  */
{
  ELog::RegMethod RegA("ODIN","setBeamAxis");

  odinAxis->createUnitVector(GItem);
  odinAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  odinAxis->setLinkCopy(1,GItem.getKey("Main"),1);

  odinAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  odinAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    odinAxis->reverseZ();
  return;
}

void 
ODIN::build(Simulation& System,const attachSystem::FixedGroup& GItem,
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
  setBeamAxis(GItem,1);

  if (stopPoint==1) return;
    
  BladeChopper->addInsertCell(bunkerObj.getCell("MainVoid"));
  BladeChopper->setCentreFlag(3);  // Z direction
  // beam direction from GItem
  BladeChopper->createAll(System,*odinAxis,4);

  GuideA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideA->createAll(System,BladeChopper->getKey("Beam"),2,
		    BladeChopper->getKey("Beam"),2);

  T0Chopper->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0Chopper->setCentreFlag(3);  // Z direction
  T0Chopper->createAll(System,GuideA->getKey("Guide0"),2);

  GuideB->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideB->addEndCut(bunkerObj.getSignedLinkString(8));
  GuideB->createAll(System,T0Chopper->getKey("Main"),2,
		    T0Chopper->getKey("Beam"),2);

  if (stopPoint==2) return;
  
  // Make bunker insert
  const attachSystem::FixedComp& GFC(GuideB->getKey("Guide0"));
  BInsert->createAll(System,GFC,2,bunkerObj);   // changed from -1
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  


  // Guide in the bunker insert
  GuideC->addInsertCell(BInsert->getCell("Void"));
  GuideC->addEndCut(bunkerObj.getSignedLinkString(-2));
  GuideC->createAll(System,*BInsert,-1,*BInsert,-1);

  GuideD->addInsertCell(voidCell);
  GuideD->createAll(System,*BInsert,2,GuideC->getKey("Guide0"),2);

  if (stopPoint==3) return;
  //
  // First chopper pit out of bunker
  // Guide guide String
  HeadRule GuideCut=
    attachSystem::unionLink(GuideD->getKey("Shield"),{2,3,4,5,6});
  PitA->addInsertCell(voidCell);
  PitA->createAll(System,GuideD->getKey("Guide0"),2);
  PitA->insertComponent(System,"Outer",GuideCut);

  
  GuidePitAFront->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitAFront->addEndCut(PitA->getKey("Inner").getSignedLinkString(1));
  GuidePitAFront->createAll(System,GuideD->getKey("Guide0"),2,
			    GuideD->getKey("Guide0"),2);

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->setCentreFlag(3);  // -Z direction
  ChopperA->createAll(System,*PitA,0);
  
  GuideE->addInsertCell(voidCell);
  GuideE->addInsertCell(PitA->getCells("MidLayer"));
  GuideE->addInsertCell(PitA->getCell("Outer"));
  GuideE->createAll(System,PitA->getKey("Mid"),2,PitA->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitABack->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitABack->addInsertCell(PitA->getCells("Collet"));
  GuidePitABack->addEndCut(PitA->getKey("Inner").getSignedLinkString(2));
  GuidePitABack->createAll(System,GuideE->getKey("Guide0"),-1,
			    GuideE->getKey("Guide0"),-1);

  ELog::EM<<"GuideE exit point == "<<
    GuideE->getKey("Guide0").getSignedLinkPt(2).abs()<<ELog::endDebug;

  // SECOND CHOPPER PIT:
  // First chopper pit out of bunker
  // Guide guide String
  GuideCut=attachSystem::unionLink(GuideE->getKey("Shield"),{2,3,4,5,6});
  PitB->addInsertCell(voidCell);
  PitB->createAll(System,GuideE->getKey("Guide0"),2);
  PitB->insertComponent(System,"Outer",GuideCut);
  
  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->setCentreFlag(3);  // -Z direction
  ChopperB->createAll(System,*PitB,0);

  ELog::EM<<"PitB == "<<PitB->getCentre()
	  <<" :: "<<PitB->getCentre().abs()<<ELog::endDebug;

  GuidePitBFront->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBFront->addEndCut(PitB->getKey("Inner").getSignedLinkString(1));
  GuidePitBFront->createAll(System,GuideE->getKey("Guide0"),2,
			    GuideE->getKey("Guide0"),2);

  GuideF->addInsertCell(voidCell);
  GuideF->addInsertCell(PitB->getCells("MidLayer"));
  GuideF->addInsertCell(PitB->getCell("Outer"));
  GuideF->createAll(System,PitB->getKey("Mid"),2,PitB->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitBBack->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBBack->addInsertCell(PitB->getCells("Collet"));
  GuidePitBBack->addEndCut(PitB->getKey("Inner").getSignedLinkString(2));
  GuidePitBBack->createAll(System,GuideF->getKey("Guide0"),-1,
			    GuideF->getKey("Guide0"),-1);

  //
  // THIRD CHOPPER PIT:
  //
  const attachSystem::FixedComp& GOuterC=GuideF->getKey("Shield");
  GuideCut=
    attachSystem::unionLink(GOuterC,{2,3,4,5,6});
  PitC->addInsertCell(voidCell);
  PitC->createAll(System,GuideF->getKey("Guide0"),2);
  PitC->insertComponent(System,"Outer",GuideCut);
  
  ELog::EM<<"PitC == "<<PitC->getCentre()
	  <<" :: "<<PitC->getCentre().abs()<<ELog::endDebug;

  GuidePitCFront->addInsertCell(PitC->getCell("MidLayer"));
  GuidePitCFront->addEndCut(PitC->getKey("Inner").getSignedLinkString(1));
  GuidePitCFront->createAll(System,GuideF->getKey("Guide0"),2,
			    GuideF->getKey("Guide0"),2);


  GuideG->addInsertCell(voidCell);
  GuideG->addInsertCell(PitC->getCell("MidLayer"));
  GuideG->addInsertCell(PitC->getCell("Outer"));
  GuideG->createAll(System,PitC->getKey("Mid"),2,PitC->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitCBack->addInsertCell(PitC->getCell("MidLayer"));
  GuidePitCBack->addInsertCell(PitC->getCells("Collet"));
  GuidePitCBack->addEndCut(PitC->getKey("Inner").getSignedLinkString(2));
  GuidePitCBack->createAll(System,GuideG->getKey("Guide0"),-1,
			    GuideG->getKey("Guide0"),-1);
  

  GuideCut=attachSystem::unionLink(GuideG->getKey("Shield"),{3,4,5,6});
  Cave->addInsertCell(voidCell);  
  Cave->createAll(System,GuideG->getKey("Guide0"),2,GuideCut.display());

  // runs through wall and into void 
  GuideH->addInsertCell(Cave->getCell("VoidNose"));
  GuideH->addInsertCell(Cave->getCell("FeNose"));
  GuideH->createAll(System,GuideG->getKey("Guide0"),2,
		    GuideG->getKey("Guide0"),2);


  PinA->addInsertCell(Cave->getCell("VoidNose"));
  PinA->createAll(System,GuideH->getKey("Guide0"),2);

  BeamStop->addInsertCell(Cave->getCell("VoidMain"));
  BeamStop->createAll(System,GuideH->getKey("Guide0"),2);

  return;
}

}   // NAMESPACE essSystem

