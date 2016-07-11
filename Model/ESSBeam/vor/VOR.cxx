/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VOR.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "ChopperUnit.h"
#include "DHut.h"
#include "DetectorTank.h"
#include "CylSample.h"

#include "VOR.h"

namespace essSystem
{

VOR::VOR(const std::string& keyName) :
  attachSystem::CopiedComp("vor",keyName),
  vorAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),

  BInsert(new BunkerInsert(newName+"BInsert")),

  FocusBExtra(new beamlineSystem::GuideLine(newName+"FBextra")),
  PitA(new constructSystem::ChopperPit(newName+"PitA")),
  GuidePitAFront(new beamlineSystem::GuideLine(newName+"GPitAFront")),
  GuidePitABack(new beamlineSystem::GuideLine(newName+"GPitABack")),


  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  GuidePitBFront(new beamlineSystem::GuideLine(newName+"GPitBFront")),
  GuidePitBBack(new beamlineSystem::GuideLine(newName+"GPitBBack")),
  ChopperB(new constructSystem::DiskChopper(newName+"ChopperB")),

  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  Cave(new DHut(newName+"Cave")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  Tank(new DetectorTank(newName+"Tank")),
  Sample(new instrumentSystem::CylSample(newName+"Sample"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("VOR","VOR");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell("vorAxis");
  OR.addObject(vorAxis);

  OR.addObject(FocusA);
  
  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);


  OR.addObject(ChopperA);
  OR.addObject(DDisk);

  OR.addObject(BInsert);

  OR.addObject(FocusBExtra);
  OR.addObject(PitA);
  OR.addObject(GuidePitAFront);
  OR.addObject(GuidePitABack);


  // Second chopper
  OR.addObject(PitB);
  OR.addObject(GuidePitBFront);
  OR.addObject(GuidePitBBack);
  OR.addObject(ChopperB);

  OR.addObject(FocusD);
  OR.addObject(FocusE);

  OR.addObject(Cave);
  OR.addObject(FocusF);

  OR.addObject(Tank);
  OR.addObject(Sample);

}

VOR::~VOR()
  /*!
    Destructor
  */
{}

void
VOR::setBeamAxis(const FuncDataBase& Control,
		 const GuideItem& GItem,
		 const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Database for variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis (if required)
   */
{
  ELog::RegMethod RegA("VOR","setBeamAxis");

  vorAxis->populate(Control);
  vorAxis->createUnitVector(GItem);
  vorAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  vorAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  vorAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  vorAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    vorAxis->reverseZ();
  return;
}
  
void 
VOR::build(Simulation& System,
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
  ELog::RegMethod RegA("VOR","build");

  ELog::EM<<"\nBuilding VOR on : "<<GItem.getKeyName()
	  <<" Bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"Stop point == "<<stopPoint<<ELog::endDiag;
  
  setBeamAxis(Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addFrontCut(GItem.getKey("Beam"),-1);
  FocusA->addEndCut(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);
  if (stopPoint==1) return;

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);



  // First (green chopper)
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,FocusC->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Beam"),0);





  return;
  // First straight section
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,FocusA->getKey("Guide0"),2);
  FocusA->addInsertCell(VacBoxA->getCells("Void"));
  FocusA->insertObjects(System);
  
  // Double disk chopper
  DDisk->addInsertCell(VacBoxA->getCell("Void",0));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->createAll(System,FocusA->getKey("Guide0"),2);

  // Double disk chopper housing
  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  DDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  DDiskHouse->createAll(System,DDisk->getKey("Main"),0);
  DDiskHouse->insertComponent(System,"Void",*DDisk);

    return;

  //  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  //FocusB->insertObjects(System);

  // PIPE
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VacBoxA,2);
  VPipeB->setBack(bunkerObj,1);
  VPipeB->createAll(System,*VacBoxA,2);

  FocusB->addInsertCell(VPipeB->getCell("Void"));
  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  if (stopPoint==2)
    FocusB->addEndCut(bunkerObj.getSignedLinkString(1));
  else
    FocusB->addEndCut(bunkerObj.getSignedLinkString(-2));
  FocusB->createAll(System,DDisk->getKey("Beam"),2,
		    DDisk->getKey("Beam"),2);
  if (stopPoint==2) return;
    
  // Make bunker insert
  BInsert->createAll(System,FocusB->getKey("Guide0"),-1,bunkerObj);
  attachSystem::addToInsertLineCtrl(System,bunkerObj,"frontWall",
				    *BInsert,*BInsert);

  //  FocusB->addInsertCell(BInsert->getCell("Void"));
  BInsert->insertComponent(System,"Void",*FocusB);

  if (stopPoint==3) return;
  
  // Continuation of guide FocusB [Out of void]
  FocusBExtra->addInsertCell(voidCell);
  FocusBExtra->createAll(System,*BInsert,2,FocusB->getKey("Guide0"),2);

  // First chopper pit out of bunker
  // Guide guide String
  HeadRule GuideCut=
    attachSystem::unionLink(FocusBExtra->getKey("Shield"),{2,3,4,5,6});
  PitA->addInsertCell(voidCell);
  PitA->createAll(System,FocusBExtra->getKey("Guide0"),2);
  PitA->insertComponent(System,"Outer",GuideCut);
  
  GuidePitAFront->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitAFront->addEndCut(PitA->getKey("Inner").getSignedLinkString(1));
  GuidePitAFront->createAll(System,FocusBExtra->getKey("Guide0"),2,
			    FocusBExtra->getKey("Guide0"),2);

  //  ChopperA->addInsertCell(PitA->getCell("Void"));
  //  ChopperA->setCentreFlag(3);  // -Z direction
  //  ChopperA->createAll(System,*PitA,0);

  
  FocusC->addInsertCell(voidCell);
  FocusC->addInsertCell(PitA->getCells("MidLayer"));
  FocusC->addInsertCell(PitA->getCell("Outer"));
  FocusC->createAll(System,PitA->getKey("Mid"),2,PitA->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitABack->addInsertCell(PitA->getCells("MidLayer"));
  GuidePitABack->addInsertCell(PitA->getCells("Collet"));
  GuidePitABack->addEndCut(PitA->getKey("Inner").getSignedLinkString(2));
  GuidePitABack->createAll(System,FocusC->getKey("Guide0"),-1,
			   FocusC->getKey("Guide0"),-1);


  // SECOND CHOPPER PIT OUT OF BUNKER
  HeadRule GuideCutB=
    attachSystem::unionLink(FocusC->getKey("Shield"),{2,3,4,5,6});
  PitB->addInsertCell(voidCell);
  PitB->createAll(System,FocusC->getKey("Guide0"),2);
  PitB->insertComponent(System,"Outer",GuideCutB);

  GuidePitBFront->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBFront->addEndCut(PitB->getKey("Inner").getSignedLinkString(1));
  GuidePitBFront->createAll(System,FocusC->getKey("Guide0"),2,
			    FocusC->getKey("Guide0"),2);

  ChopperB->addInsertCell(PitB->getCell("Void"));
  ChopperB->setCentreFlag(3);  // Z direction
  ChopperB->createAll(System,*PitB,0);

  // EXIT GUIDE:
  FocusD->addInsertCell(voidCell);
  FocusD->addInsertCell(PitB->getCells("MidLayer"));
  FocusD->addInsertCell(PitB->getCell("Outer"));
  FocusD->createAll(System,PitB->getKey("Mid"),2,PitB->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitBBack->addInsertCell(PitB->getCells("MidLayer"));
  GuidePitBBack->addInsertCell(PitB->getCells("Collet"));
  GuidePitBBack->addEndCut(PitB->getKey("Inner").getSignedLinkString(2));
  GuidePitBBack->createAll(System,FocusD->getKey("Guide0"),-1,
			   FocusD->getKey("Guide0"),-1);


  // EXIT GUIDE:
  FocusE->addInsertCell(voidCell);
  FocusE->createAll(System,FocusD->getKey("Guide0"),2,
		    FocusD->getKey("Guide0"),2);


  Cave->addInsertCell(voidCell);
  Cave->createAll(System,FocusE->getKey("Guide0"),2);

  // EXIT GUIDE:
  //  FocusF->addInsertCell(Cave->getCell("Void"));
  FocusF->createAll(System,FocusE->getKey("Guide0"),2,
		    FocusE->getKey("Guide0"),2);

  Cave->insertComponent(System,"Void",*FocusF);
  Cave->insertComponent(System,"Steel",*FocusF);
  Cave->insertComponent(System,"Concrete",*FocusF);

  Tank->addInsertCell(Cave->getCell("Void"));
  Tank->createAll(System,FocusF->getKey("Guide0"),2);
    
  Sample->addInsertCell(Tank->getCell("SampleVoid"));
  Sample->createAll(System,*Tank,0);
  
  return;
}


}   // NAMESPACE essSystem

