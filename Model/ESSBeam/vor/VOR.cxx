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
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "ChopperUnit.h"
#include "LineShield.h"
#include "HoleShape.h"
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

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  
  BInsert(new BunkerInsert(newName+"BInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  ChopperOutA(new constructSystem::ChopperUnit(newName+"ChopperOutA")),
  FOCDisk(new constructSystem::DiskChopper(newName+"FOCDisk")),
  FOCExitPort(new constructSystem::HoleShape(newName+"FOCExitPort")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  OutPitB(new constructSystem::ChopperPit(newName+"OutPitB")),
  ChopperOutB(new constructSystem::ChopperUnit(newName+"ChopperOutB")),
  FOCDiskB(new constructSystem::DiskChopper(newName+"FOCDiskB")),
  FOCEntryPortB(new constructSystem::HoleShape(newName+"FOCEntryPortB")),
  FOCExitPortB(new constructSystem::HoleShape(newName+"FOCExitPortB")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"FOutC")),
  
  Cave(new DHut(newName+"Cave")),
  CavePort(new constructSystem::HoleShape(newName+"CavePort")),
  
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

  OR.addObject(VPipeD);
  OR.addObject(FocusD);


  OR.addObject(ChopperA);
  OR.addObject(DDisk);

  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);

  OR.addObject(OutPitA);
  OR.addObject(ChopperOutA);
  OR.addObject(FOCDisk);
  OR.addObject(FOCExitPort);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);

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

  // BEAM needs to be shifted/rotated:
  vorAxis->linkShift(3);
  vorAxis->linkShift(4);
  vorAxis->linkAngleRotate(3);
  vorAxis->linkAngleRotate(4);

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
  
  setBeamAxis(Control,GItem,0);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*vorAxis,-3,*vorAxis,-3);
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

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperA->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  if (stopPoint==2) return;

  // Make bunker insert
  BInsert->createAll(System,FocusD->getKey("Guide0"),-1,bunkerObj);
  attachSystem::addToInsertLineCtrl(System,bunkerObj,"frontWall",
				    *BInsert,*BInsert);

  VPipeWall->addInsertCell(BInsert->getCell("Void"));
  VPipeWall->createAll(System,*BInsert,-1);

    // using 7 : mid point
  FocusWall->addInsertCell(VPipeWall->getCells("Void"));
  FocusWall->createAll(System,*VPipeWall,0,*VPipeWall,0);

  if (stopPoint==3) return;

  OutPitA->addInsertCell(voidCell);
  OutPitA->addFrontWall(bunkerObj,2);
  OutPitA->createAll(System,FocusWall->getKey("Guide0"),2);

  // First Chopper
  ChopperOutA->addInsertCell(OutPitA->getCells("Void"));
  ChopperOutA->createAll(System,FocusWall->getKey("Guide0"),2);

  // Double disk chopper
  FOCDisk->addInsertCell(ChopperOutA->getCell("Void"));
  FOCDisk->setCentreFlag(3);  // Z direction
  FOCDisk->setOffsetFlag(1);  // Z direction
  FOCDisk->createAll(System,ChopperOutA->getKey("Beam"),0);
  
  FOCExitPort->addInsertCell(OutPitA->getCells("MidLayerBack"));
  FOCExitPort->addInsertCell(OutPitA->getCells("Collet"));
  FOCExitPort->setFaces(OutPitA->getKey("Inner").getSignedFullRule(2),
                       OutPitA->getKey("Mid").getSignedFullRule(-2));
  FOCExitPort->createAll(System,OutPitA->getKey("Inner"),2);


    // First put pit into the main void
  OutPitB->addInsertCell(voidCell);
  OutPitB->createAll(System,OutPitA->getKey("Inner"),0);

  FOCEntryPortB->addInsertCell(OutPitB->getCells("MidLayerFront"));
  FOCEntryPortB->setFaces(OutPitB->getKey("Inner").getSignedFullRule(1),
			  OutPitB->getKey("Mid").getSignedFullRule(-1));
  FOCEntryPortB->createAll(System,OutPitB->getKey("Inner"),1);

  
  FOCExitPortB->addInsertCell(OutPitB->getCells("MidLayerBack"));
  FOCExitPortB->addInsertCell(OutPitB->getCells("Collet"));
  FOCExitPortB->setFaces(OutPitB->getKey("Inner").getSignedFullRule(2),
                       OutPitB->getKey("Mid").getSignedFullRule(-2));
  FOCExitPortB->createAll(System,OutPitB->getKey("Inner"),2);

  // shielding between PitA and P it B
  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->setFront(OutPitA->getKey("Mid"),2);
  ShieldA->addInsertCell(OutPitB->getCells("Outer"));
  ShieldA->addInsertCell(OutPitB->getCells("MidLayer"));
  ShieldA->setBack(OutPitB->getKey("Mid"),1);
  ShieldA->createAll(System,FocusWall->getKey("Shield"),2);

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,*ShieldA,-1);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

    // First Chopper
  ChopperOutB->addInsertCell(OutPitB->getCells("Void"));
  ChopperOutB->createAll(System,FocusOutA->getKey("Guide0"),2);
  
  Cave->addInsertCell(voidCell);
  Cave->createAll(System,OutPitB->getKey("Inner"),0);



  ShieldB->addInsertCell(voidCell);
  ShieldB->addInsertCell(OutPitB->getCells("Outer"));
  ShieldB->addInsertCell(OutPitB->getCells("MidLayer"));
  ShieldB->addInsertCell(Cave->getCells("Concrete"));
  ShieldB->setFront(OutPitB->getKey("Mid"),2);
  ShieldB->setBack(Cave->getKey("Mid"),1);
  ShieldB->createAll(System,OutPitB->getKey("Inner"),0);

  VPipeOutC->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutC->createAll(System,*ShieldB,-1);

  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  CavePort->addInsertCell(Cave->getCells("Steel"));
  CavePort->setCutFaceFlag(1);
  CavePort->setFaces(Cave->getKey("Inner").getSignedFullRule(1),
		     Cave->getKey("Mid").getSignedFullRule(-1));
  CavePort->createAll(System,FocusOutC->getKey("Guide0"),2);

  Tank->addInsertCell(Cave->getCells("Void"));
  Tank->createAll(System,FocusOutC->getKey("Guide0"),2);
  return;
}


}   // NAMESPACE essSystem

