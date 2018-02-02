/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/shortOdin/shortODIN.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "Hut.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"
#include "PinHole.h"
#include "RentrantBS.h"
#include "shortODIN.h"

namespace essSystem
{

shortODIN::shortODIN(const std::string& keyName) :
  attachSystem::CopiedComp("shortOdin",keyName),
  stopPoint(0),
  odinAxis(new attachSystem::FixedComp(newName+"Axis",4)),
  VacBoxA(new constructSystem::VacuumBox(newName+"VacA")),
  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),
  BladeChopper(new constructSystem::DiskChopper(newName+"Blade")),
  
  VacBoxB(new constructSystem::VacuumBox(newName+"VacB")),
  T0House(new constructSystem::ChopperHousing(newName+"T0House")),
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),
  T0Chopper(new constructSystem::DiskChopper(newName+"TZero")),

  VPipeFinal(new constructSystem::VacuumPipe(newName+"PipeFinal")),
  FocusFinal(new beamlineSystem::GuideLine(newName+"FFinal")),
  
  GuideA(new beamlineSystem::GuideLine(newName+"GA")),

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
  PinA(new constructSystem::PinHole(newName+"Pin")),

  BeamStop(new RentrantBS(newName+"BeamStop"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.cell("shortOdinAxis");
  OR.addObject(odinAxis);
  
  OR.addObject(BladeChopper);
  OR.addObject(GuideA);
  OR.addObject(T0Chopper);
  OR.addObject(T0House);
  OR.addObject(VPipeA);
  OR.addObject(VacBoxA);
  OR.addObject(VacBoxB);
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

shortODIN::~shortODIN()
  /*!
    Destructor
  */
{}

void
shortODIN::setBeamAxis(const attachSystem::FixedGroup& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Primary beam object
    \param reverseZ :: Reverse Z direction
  */
{
  ELog::RegMethod RegA("shortODIN","setBeamAxis");

  odinAxis->createUnitVector(GItem);
  odinAxis->setLinkSignedCopy(0,GItem.getKey("Main"),1);
  odinAxis->setLinkSignedCopy(1,GItem.getKey("Main"),2);

  odinAxis->setLinkSignedCopy(2,GItem.getKey("Beam"),1);
  odinAxis->setLinkSignedCopy(3,GItem.getKey("Beam"),2);

  if (reverseZ)
    odinAxis->reverseZ();
  return;
}

void
shortODIN::buildChopperBlock(Simulation& System,
			 const Bunker& bunkerObj,
			 const attachSystem::FixedComp& prevFC,
			 const constructSystem::VacuumBox& prevVacBox,
			 constructSystem::VacuumBox& VacBox,
			 beamlineSystem::GuideLine& GL,
			 constructSystem::DiskChopper& Disk,
			 constructSystem::ChopperHousing& House,
			 constructSystem::VacuumPipe& Pipe)
  /*!
    Build a chopper block [about to move to some higher level]
    \param System :: Simulation 
    \param bunkerObj :: Object
    \param prevFC :: FixedComponent for like point [uses side 2]
    \param GL :: Guide Line 
  */
{
  ELog::RegMethod RegA("SHORTDREAM","buildChopperBlock");
  
  // Box for BandA Disk
  VacBox.addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBox.createAll(System,prevFC,2);

  // Double disk T0 chopper
  Disk.addInsertCell(VacBox.getCell("Void",0));
  Disk.setCentreFlag(3);  // Z direction
  Disk.createAll(System,VacBox,0);

  // Double disk chopper housing
  House.addInsertCell(VacBox.getCells("Void"));
  House.addInsertCell(VacBox.getCells("Box"));  // soon to become lid
  House.addInsertCell(bunkerObj.getCell("MainVoid"));
  House.createAll(System,Disk.getKey("Main"),0);
  House.insertComponent(System,"Void",Disk);

  Pipe.addInsertCell(bunkerObj.getCell("MainVoid"));
  Pipe.setFront(prevVacBox,2);
  Pipe.setBack(VacBox,1);
  Pipe.createAll(System,prevVacBox,2);
  
  GL.addInsertCell(Pipe.getCells("Void"));
  GL.addInsertCell(prevVacBox.getCells("Void"));
  GL.addInsertCell(VacBox.getCells("Void"));
  GL.createAll(System,prevFC,2,prevFC,2);
  return;
} 

  
void 
shortODIN::build(Simulation& System,const attachSystem::FixedGroup& GItem,
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
  ELog::RegMethod RegA("shortODIN","build");

  ELog::EM<<"\nBuilding shortODIN on : "<<GItem.getKeyName()<<ELog::endDiag;
  setBeamAxis(GItem,1);
  
  // First section out of monolyth
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,GItem.getKey("Beam"),2);
  // PIPE

  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->setFront(GItem.getKey("Beam"),2);
  VPipeA->setBack(*VacBoxA,1);
  VPipeA->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  FocusB->createAll(System,GItem.getKey("Beam"),2,
		    GItem.getKey("Beam"),2);

  BladeChopper->addInsertCell(VacBoxA->getCell("Void",0));
  BladeChopper->setCentreFlag(3);  // Z direction
  BladeChopper->createAll(System,FocusB->getKey("Guide0"),2);

  // Double disk chopper housing
  //  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  //  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid

  buildChopperBlock(System,bunkerObj,
		    BladeChopper->getKey("Beam"),*VacBoxA,
		    *VacBoxB,*FocusC,
		    *T0Chopper,*T0House,
		    *VPipeB);


  // CONNECT TO  WALL
  VPipeFinal->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeFinal->setFront(*VacBoxB,2);
  VPipeFinal->setBack(bunkerObj,1);
  VPipeFinal->createAll(System,*VacBoxB,2);

  FocusFinal->addInsertCell(VPipeFinal->getCell("Void"));
  FocusFinal->addInsertCell(VacBoxB->getCells("Void"));
  FocusFinal->setBack(bunkerObj.getLinkString(1));
  FocusFinal->createAll(System,T0Chopper->getKey("Beam"),2,
			T0Chopper->getKey("Beam"),2);

  // Make bunker insert
  const attachSystem::FixedComp& GFC(FocusFinal->getKey("Guide0"));
  BInsert->createAll(System,GFC,2,bunkerObj);   // changed from -1
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  


  // Guide in the bunker insert
  GuideC->addInsertCell(BInsert->getCell("Void"));
  GuideC->setBack(bunkerObj.getLinkString(-2));
  GuideC->createAll(System,*BInsert,-1,*BInsert,-1);
  
  GuideD->addInsertCell(voidCell);
  GuideD->createAll(System,*BInsert,2,GuideC->getKey("Guide0"),2);

  //
  // First chopper pit out of bunker
  // Guide guide String
  HeadRule GuideCut=
    attachSystem::unionLink(GuideD->getKey("Shield"),{2,3,4,5,6});
  PitA->addInsertCell(voidCell);
  PitA->createAll(System,GuideD->getKey("Guide0"),2);

  ELog::EM<<"PitA == "<<PitA->getCentre()
	  <<" :: "<<PitA->getCentre().abs()<<ELog::endDebug;
  
  GuidePitAFront->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitAFront->setBack(PitA->getKey("Inner").getLinkString(1));
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
  GuidePitABack->setBack(PitA->getKey("Inner").getLinkString(2));
  GuidePitABack->createAll(System,GuideE->getKey("Guide0"),-1,
			    GuideE->getKey("Guide0"),-1);

  ELog::EM<<"GuideE exit point == "<<
    GuideE->getKey("Guide0").getLinkPt(2).abs()<<ELog::endDebug;

  // SECOND CHOPPER PIT:
  // First chopper pit out of bunker
  // Guide guide String
  GuideCut=attachSystem::unionLink(GuideE->getKey("Shield"),{2,3,4,5,6});
  PitB->addInsertCell(voidCell);
  PitB->createAll(System,GuideE->getKey("Guide0"),2);

  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->createAll(System,*PitB,0);

  ELog::EM<<"PitB == "<<PitB->getCentre()
	  <<" :: "<<PitB->getCentre().abs()<<ELog::endDebug;

  GuidePitBFront->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBFront->setBack(PitB->getKey("Inner").getLinkString(1));
  GuidePitBFront->createAll(System,GuideE->getKey("Guide0"),2,
			    GuideE->getKey("Guide0"),2);

  GuideF->addInsertCell(voidCell);
  GuideF->addInsertCell(PitB->getCells("MidLayer"));
  GuideF->addInsertCell(PitB->getCell("Outer"));
  GuideF->createAll(System,PitB->getKey("Mid"),2,PitB->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitBBack->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBBack->addInsertCell(PitB->getCells("Collet"));
  GuidePitBBack->setBack(PitB->getKey("Inner").getLinkString(2));
  GuidePitBBack->createAll(System,GuideF->getKey("Guide0"),-1,
			    GuideF->getKey("Guide0"),-1);

  //
  // THIRD CHOPPER PIT:
  //
  const attachSystem::FixedComp& GOuterC=GuideF->getKey("Shield");
  GuideCut.reset();
  for(long int i=2;i<7;i++)
    GuideCut.addUnion(GOuterC.getLinkString(i));
  PitC->addInsertCell(voidCell);
  PitC->createAll(System,GuideF->getKey("Guide0"),2);

  ELog::EM<<"PitC == "<<PitC->getCentre()
	  <<" :: "<<PitC->getCentre().abs()<<ELog::endDebug;

  GuidePitCFront->addInsertCell(PitC->getCell("MidLayer"));
  GuidePitCFront->setBack(PitC->getKey("Inner").getLinkString(1));
  GuidePitCFront->createAll(System,GuideF->getKey("Guide0"),2,
			    GuideF->getKey("Guide0"),2);


  GuideG->addInsertCell(voidCell);
  GuideG->addInsertCell(PitC->getCell("MidLayer"));
  GuideG->addInsertCell(PitC->getCell("Outer"));
  GuideG->createAll(System,PitC->getKey("Mid"),2,PitC->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitCBack->addInsertCell(PitC->getCell("MidLayer"));
  GuidePitCBack->addInsertCell(PitC->getCells("Collet"));
  GuidePitCBack->setBack(PitC->getKey("Inner"),2);
  GuidePitCBack->createAll(System,GuideG->getKey("Guide0"),-1,
			    GuideG->getKey("Guide0"),-1);
  

  GuideCut=attachSystem::unionLink(GuideG->getKey("Shield"),{3,4,5,6});
  Cave->addInsertCell(voidCell);  
  Cave->createAll(System,GuideG->getKey("Guide0"),2);

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

