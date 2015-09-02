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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LayerComp.h"
#include "CellMap.h"
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

ODIN::ODIN() :
  odinAxis(new attachSystem::FixedComp("odinAxis",4)),
  BladeChopper(new constructSystem::DiskChopper("odinBlade")),
  GuideA(new beamlineSystem::GuideLine("odinGA")),
  T0Chopper(new constructSystem::DiskChopper("odinTZero")),
  GuideB(new beamlineSystem::GuideLine("odinGB")),
  BInsert(new BunkerInsert("odinBInsert")),
  GuideC(new beamlineSystem::GuideLine("odinGC")),
  GuideD(new beamlineSystem::GuideLine("odinGD")),

  PitA(new constructSystem::ChopperPit("odinPitA")),
  GuidePitAFront(new beamlineSystem::GuideLine("odinGPitAFront")),
  GuidePitABack(new beamlineSystem::GuideLine("odinGPitABack")),
  ChopperA(new constructSystem::DiskChopper("odinChopperA")),

  GuideE(new beamlineSystem::GuideLine("odinGE")),

  PitB(new constructSystem::ChopperPit("odinPitB")),
  GuidePitBFront(new beamlineSystem::GuideLine("odinGPitBFront")),
  GuidePitBBack(new beamlineSystem::GuideLine("odinGPitBBack")),
  GuideF(new beamlineSystem::GuideLine("odinGF")),

  PitC(new constructSystem::ChopperPit("odinPitC")),
  GuidePitCFront(new beamlineSystem::GuideLine("odinGPitCFront")),
  GuidePitCBack(new beamlineSystem::GuideLine("odinGPitCBack")),
  GuideG(new beamlineSystem::GuideLine("odinGG")),

  Cave(new essSystem::Hut("odinCave")),
  GuideH(new beamlineSystem::GuideLine("odinGH")),
  PinA(new PinHole("odinPin")),

  BeamStop(new RentrantBS("odinBeamStop"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.cell("odinAxis");
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
  ELog::EM<<"REVERSE Z on axis"<<ELog::endDebug;
  setBeamAxis(GItem,1);

  BladeChopper->addInsertCell(bunkerObj.getCell("MainVoid"));
  BladeChopper->setCentreFlag(3);  // Z direction
  // beam direction from GIetm
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

  // Make bunker insert
  const attachSystem::FixedComp& GFC(GuideB->getKey("Guide0"));
  const std::string BSector=
    bunkerObj.calcSegment(System,GFC.getSignedLinkPt(2),
			  GFC.getSignedLinkAxis(2));  
  BInsert->setInsertCell(bunkerObj.getCells(BSector));
  BInsert->createAll(System,GuideB->getKey("Guide0"),2,bunkerObj);

  // Guide in the bunker insert
  GuideC->addInsertCell(BInsert->getCell("Void"));
  GuideC->addEndCut(bunkerObj.getSignedLinkString(-2));
  GuideC->createAll(System,*BInsert,-1,*BInsert,-1);

  GuideD->addInsertCell(voidCell);
  GuideD->createAll(System,*BInsert,2,GuideC->getKey("Guide0"),2);

  //
  // First chopper pit out of bunker
  // Guide guide String
  HeadRule GuideCut=
    attachSystem::unionLink(GuideD->getKey("Shield"),{2,3,4,5,6});
  PitA->addInsertCell(voidCell);
  PitA->createAll(System,GuideD->getKey("Guide0"),2,GuideCut.display());

  ELog::EM<<"PitA == "<<PitA->getCentre()
	  <<" :: "<<PitA->getCentre().abs()<<ELog::endDebug;
  
  GuidePitAFront->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitAFront->addEndCut(PitA->getKey("Inner").getSignedLinkString(1));
  GuidePitAFront->createAll(System,GuideD->getKey("Guide0"),2,
			    GuideD->getKey("Guide0"),2);

  ChopperA->addInsertCell(PitA->getCell("Void"));
  ChopperA->setCentreFlag(-3);  // -Z direction
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
  PitB->createAll(System,GuideE->getKey("Guide0"),2,GuideCut.display());

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
  GuideCut.reset();
  for(size_t i=1;i<6;i++)
    GuideCut.addUnion(GOuterC.getLinkString(i));
  PitC->addInsertCell(voidCell);
  PitC->createAll(System,GuideF->getKey("Guide0"),2,GuideCut.display());

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

