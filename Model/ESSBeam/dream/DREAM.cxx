/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DREAM.cxx
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
#include "DHut.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "DREAM.h"

namespace essSystem
{

DREAM::DREAM() :
  dreamAxis(new attachSystem::FixedComp("dreamAxis",4)),
  FocusA(new beamlineSystem::GuideLine("dreamFA")),
  VacBoxA(new constructSystem::VacuumBox("dreamVacA")),
  VPipeA(new constructSystem::VacuumPipe("dreamPipeA")),
  FocusB(new beamlineSystem::GuideLine("dreamFB")),
  DDisk(new constructSystem::DiskChopper("dreamDBlade")),
  DDiskHouse(new constructSystem::ChopperHousing("dreamDBladeHouse")),
  SDisk(new constructSystem::DiskChopper("dreamSBlade")),
  SDiskHouse(new constructSystem::ChopperHousing("dreamSBladeHouse")),
  T0DiskA(new constructSystem::DiskChopper("dreamT0DiskA")),

  T0DiskAHouse(new constructSystem::ChopperHousing("dreamT0DiskAHouse")),
  VacBoxB(new constructSystem::VacuumBox("dreamVacB",1)),
  VPipeB(new constructSystem::VacuumPipe("dreamPipeB")),
  T0DiskB(new constructSystem::DiskChopper("dreamT0DiskB")),  
  T0DiskBHouse(new constructSystem::ChopperHousing("dreamT0DiskBHouse")),
  FocusC(new beamlineSystem::GuideLine("dreamFC")),

  VacBoxC(new constructSystem::VacuumBox("dreamVacC",1)),
  BandADisk(new constructSystem::DiskChopper("dreamBandADisk")),  
  BandAHouse(new constructSystem::ChopperHousing("dreamBandAHouse")),
  VPipeC(new constructSystem::VacuumPipe("dreamPipeC")),
  FocusD(new beamlineSystem::GuideLine("dreamFD")),

  VacBoxD(new constructSystem::VacuumBox("dreamVacD",1)),
  BandBDisk(new constructSystem::DiskChopper("dreamBandBDisk")),  
  BandBHouse(new constructSystem::ChopperHousing("dreamBandBHouse")),
  VPipeD(new constructSystem::VacuumPipe("dreamPipeD")),
  FocusE(new beamlineSystem::GuideLine("dreamFE")),

  VacBoxE(new constructSystem::VacuumBox("dreamVacE",1)),
  T0DiskC(new constructSystem::DiskChopper("dreamT0DiskC")),  
  T0HouseC(new constructSystem::ChopperHousing("dreamT0DiskCHouse")),
  VPipeE(new constructSystem::VacuumPipe("dreamPipeE")),
  FocusF(new beamlineSystem::GuideLine("dreamFF")),

  VacBoxF(new constructSystem::VacuumBox("dreamVacF",1)),
  T0DiskD(new constructSystem::DiskChopper("dreamT0DiskD")),  
  T0HouseD(new constructSystem::ChopperHousing("dreamT0DiskDHouse")),
  VPipeF(new constructSystem::VacuumPipe("dreamPipeF")),
  FocusG(new beamlineSystem::GuideLine("dreamFG")),

  VPipeFinal(new constructSystem::VacuumPipe("dreamPipeFinal")),
  FocusFinal(new beamlineSystem::GuideLine("dreamFFinal")),
  BInsert(new BunkerInsert("dreamBInsert")),
  FocusWall(new beamlineSystem::GuideLine("dreamFWall")),

  ShieldA(new constructSystem::LineShield("dreamShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe("dreamPipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine("dreamFOutA")),

  ShieldB(new constructSystem::LineShield("dreamShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe("dreamPipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine("dreamFOutB"))
/*!
    Constructor
 */
{
  ELog::RegMethod RegA("DREAM","DREAM");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary:
  OR.cell("dreamAxis");
  OR.addObject(dreamAxis);

  OR.addObject(FocusA);
  OR.addObject(VacBoxA);
  OR.addObject(VPipeA);
  
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(DDisk);
  OR.addObject(DDiskHouse);  
  OR.addObject(SDisk);  
  OR.addObject(SDiskHouse);

  OR.addObject(T0DiskA);
  OR.addObject(T0DiskB);  
  OR.addObject(T0DiskAHouse);
  OR.addObject(T0DiskBHouse);
  OR.addObject(FocusC);

  OR.addObject(BandADisk);
  OR.addObject(BandAHouse);
  OR.addObject(VPipeC);
  OR.addObject(VacBoxC);
  OR.addObject(FocusD);

  OR.addObject(BandBDisk);
  OR.addObject(BandBHouse);
  OR.addObject(VPipeD);
  OR.addObject(VacBoxD);
  OR.addObject(FocusE);

  OR.addObject(T0DiskC);
  OR.addObject(T0HouseC);
  OR.addObject(VPipeE);
  OR.addObject(VacBoxE);
  OR.addObject(FocusF);

  OR.addObject(T0DiskD);
  OR.addObject(T0HouseD);
  OR.addObject(VPipeF);
  OR.addObject(VacBoxF);
  OR.addObject(FocusG);

  OR.addObject(VPipeFinal);
  OR.addObject(FocusFinal);
  OR.addObject(BInsert);
  OR.addObject(FocusWall);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
}

DREAM::~DREAM()
  /*!
    Destructor
  */
{}

void
DREAM::setBeamAxis(const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("DREAM","setBeamAxis");

  dreamAxis->createUnitVector(GItem);
  dreamAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  dreamAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  dreamAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  dreamAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    dreamAxis->reverseZ();
  return;
}

void
DREAM::buildChopperBlock(Simulation& System,
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
    \param prevVacBox :: 
    \param GL :: Guide Line 
    \parma Pipe :: Pip surrounding block
  */
{
  ELog::RegMethod RegA("DREAM","buildChopperBlock");
  
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

  setBeamAxis(GItem,1);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);

  // First section out of monolyth
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,FocusA->getKey("Guide0"),2);
  // PIPE

  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->setFront(GItem.getKey("Beam"),2);
  VPipeA->setBack(*VacBoxA,1);
  VPipeA->setDivider(GItem.getKey("Beam"),2);
  VPipeA->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  FocusB->createAll(System,FocusA->getKey("Guide0"),2,
		    FocusA->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(VacBoxA->getCell("Void",0));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->createAll(System,FocusB->getKey("Guide0"),2);
  // Double disk chopper housing
  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  DDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  DDiskHouse->createAll(System,DDisk->getKey("Main"),0);
  DDiskHouse->insertComponent(System,"Void",*DDisk);

  // Double disk chopper
  SDisk->addInsertCell(VacBoxA->getCell("Void",0));
  SDisk->setCentreFlag(-3);  // Z direction
  SDisk->createAll(System,DDisk->getKey("Beam"),2);

  // Double disk chopper housing
  SDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  SDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  SDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  SDiskHouse->createAll(System,SDisk->getKey("Main"),0);
  
  SDiskHouse->insertComponent(System,"Void",*SDisk);

  // Double disk T0 chopper
  T0DiskA->addInsertCell(VacBoxA->getCell("Void",0));
  T0DiskA->setCentreFlag(3);  // Z direction
  T0DiskA->createAll(System,SDisk->getKey("Beam"),2);

    // Double disk chopper housing
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Void"));
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  T0DiskAHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0DiskAHouse->createAll(System,T0DiskA->getKey("Main"),0);
  T0DiskAHouse->insertComponent(System,"Void",*T0DiskA);


  buildChopperBlock(System,bunkerObj,
		    T0DiskA->getKey("Beam"),*VacBoxA,
		    *VacBoxB,*FocusC,
		    *T0DiskB,*T0DiskBHouse,
		    *VPipeB);
  
  // GOING TO POSITION 2:
  buildChopperBlock(System,bunkerObj,
		    T0DiskB->getKey("Beam"),*VacBoxB,
		    *VacBoxC,*FocusD,
		    *BandADisk,*BandAHouse,
		    *VPipeC);
  // GOING TO 1300 m
  buildChopperBlock(System,bunkerObj,
		    BandADisk->getKey("Beam"),*VacBoxC,
		    *VacBoxD,*FocusE,
		    *BandBDisk,*BandBHouse,
		    *VPipeD);

  // T0 after Position 3
  buildChopperBlock(System,bunkerObj,
		    BandBDisk->getKey("Beam"),*VacBoxD,
		    *VacBoxE,*FocusF,
		    *T0DiskC,*T0HouseC,
		    *VPipeE);

  // T0 after Position 3 PartB
  buildChopperBlock(System,bunkerObj,
		    T0DiskC->getKey("Beam"),*VacBoxE,
		    *VacBoxF,*FocusG,
		    *T0DiskD,*T0HouseD,
		    *VPipeF);


  // CONNECT TO  WALL
  VPipeFinal->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeFinal->setFront(*VacBoxF,2);
  VPipeFinal->setBack(bunkerObj,1);
  VPipeFinal->createAll(System,*VacBoxF,2);

  FocusFinal->addInsertCell(VPipeFinal->getCell("Void"));
  FocusFinal->addInsertCell(VacBoxF->getCells("Void"));
  FocusFinal->addEndCut(bunkerObj.getSignedLinkString(1));
  FocusFinal->createAll(System,T0DiskD->getKey("Beam"),2,
			T0DiskD->getKey("Beam"),2);

  // IN WALL
  // Make bunker insert
  const attachSystem::FixedComp& GFC(FocusFinal->getKey("Guide0"));
  BInsert->createAll(System,GFC,-1,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,-1,
			 FocusFinal->getKey("Guide0"),2);

  // Section to 17m
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setDivider(bunkerObj,2);
  ShieldA->createAll(System,*BInsert,2);

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->setFront(bunkerObj,2);
  VPipeOutA->setDivider(bunkerObj,2);
  VPipeOutA->setBack(*ShieldA,-2);
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCell("Void"));
  FocusOutA->createAll(System,FocusWall->getKey("Guide0"),2,
		       FocusWall->getKey("Guide0"),2);

  // Section to 34  
  ShieldB->addInsertCell(voidCell);
  ShieldB->createAll(System,*ShieldA,2);

  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->setFront(*ShieldB,-1);
  VPipeOutB->setBack(*ShieldB,-2);
  VPipeOutB->createAll(System,*ShieldB,1);

  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,FocusOutA->getKey("Guide0"),2,
		       FocusOutA->getKey("Guide0"),2);
  
  return;
}


}   // NAMESPACE essSystem

