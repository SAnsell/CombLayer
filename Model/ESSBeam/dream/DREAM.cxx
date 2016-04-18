/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DREAM.cxx
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
#include "DreamHut.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "DREAM.h"

namespace essSystem
{

DREAM::DREAM(const std::string& keyName) :
  attachSystem::CopiedComp("dream",keyName),
  stopPoint(0),
  dreamAxis(new attachSystem::FixedComp(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
 
  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),
  
  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  SDisk(new constructSystem::DiskChopper(newName+"SBlade")),
  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")), 
  T0DiskA(new constructSystem::DiskChopper(newName+"T0DiskA")),
  FocusT0Mid(new beamlineSystem::GuideLine(newName+"FT0Mid")),
  T0DiskB(new constructSystem::DiskChopper(newName+"T0DiskB")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  
  ChopperD(new constructSystem::ChopperUnit(newName+"ChopperD")),
  BandADisk(new constructSystem::DiskChopper(newName+"BandADisk")),  

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperE(new constructSystem::ChopperUnit(newName+"ChopperE")),
  BandBDisk(new constructSystem::DiskChopper(newName+"BandBDisk")),  

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),


  ChopperG(new constructSystem::ChopperUnit(newName+"ChopperG")), 
  T1DiskA(new constructSystem::DiskChopper(newName+"T1DiskA")),
  FocusT1Mid(new beamlineSystem::GuideLine(newName+"FT1Mid")),
  T1DiskB(new constructSystem::DiskChopper(newName+"T1DiskB")),

  VPipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  FocusG(new beamlineSystem::GuideLine(newName+"FG")),



  

  
  DDiskHouse(new constructSystem::ChopperHousing(newName+"DBladeHouse")),

  SDiskHouse(new constructSystem::ChopperHousing(newName+"SBladeHouse")),



  VacBoxB(new constructSystem::VacuumBox(newName+"VacB",1)),
  T0DiskBHouse(new constructSystem::ChopperHousing(newName+"T0DiskBHouse")),

  T0DiskAHouse(new constructSystem::ChopperHousing(newName+"T0DiskAHouse")),
  
  VacBoxC(new constructSystem::VacuumBox(newName+"VacC",1)),


  BandAHouse(new constructSystem::ChopperHousing(newName+"BandAHouse")),
  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),

  VacBoxD(new constructSystem::VacuumBox(newName+"VacD",1)),
  BandBHouse(new constructSystem::ChopperHousing(newName+"BandBHouse")),

  VacBoxE(new constructSystem::VacuumBox(newName+"VacE",1)),
  T0DiskC(new constructSystem::DiskChopper(newName+"T0DiskC")),  
  T0HouseC(new constructSystem::ChopperHousing(newName+"T0DiskCHouse")),


  VacBoxF(new constructSystem::VacuumBox(newName+"VacF",1)),
  T0DiskD(new constructSystem::DiskChopper(newName+"T0DiskD")),  
  T0HouseD(new constructSystem::ChopperHousing(newName+"T0DiskDHouse")),

  VPipeFinal(new constructSystem::VacuumPipe(newName+"PipeFinal")),
  FocusFinal(new beamlineSystem::GuideLine(newName+"FFinal")),
  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),

  Cave(new DreamHut(newName+"Cave"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("DREAM","DREAM");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(dreamAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeA);
  OR.addObject(FocusB);
  OR.addObject(ChopperA);
  OR.addObject(DDisk);  
  OR.addObject(ChopperB);
  OR.addObject(SDisk);  
  OR.addObject(VPipeB);
  OR.addObject(FocusC);
  OR.addObject(ChopperC);
  OR.addObject(T0DiskA);
  OR.addObject(FocusT0Mid);
  OR.addObject(T0DiskB);
  OR.addObject(VPipeD);
  OR.addObject(FocusD);
  OR.addObject(ChopperD);
  OR.addObject(BandADisk);

  OR.addObject(VPipeE);
  OR.addObject(FocusE);
  OR.addObject(ChopperE);
  OR.addObject(BandBDisk);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(T1DiskA);
  OR.addObject(FocusT1Mid);
  OR.addObject(T1DiskB);

  OR.addObject(VPipeG);
  OR.addObject(FocusG);

  OR.addObject(Cave);
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
    \param reverseZ :: Reverse axis
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

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);

  setBeamAxis(GItem,1);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);

  if (stopPoint==1) return;                      // STOP At monolith edge

  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->createAll(System,FocusA->getKey("Guide0"),2,
		    FocusA->getKey("Guide0"),2);

  // NEW TEST SECTION:
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,FocusA->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Beam"),0);

  // NEW TEST SECTION:
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,ChopperA->getKey("Main"),2);

  // Double disk chopper
  SDisk->addInsertCell(ChopperB->getCell("Void"));
  SDisk->setCentreFlag(3);  // Z direction
  SDisk->setOffsetFlag(1);  // Centre offset control
  SDisk->createAll(System,ChopperB->getKey("Beam"),0);

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,ChopperB->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeB->getCells("Void"));
  FocusC->createAll(System,*VPipeB,-1,*VPipeB,-1);
  
    // NEW TEST SECTION:
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusC->getKey("Guide0"),2);

  // First disk of a T0 chopper
  T0DiskA->addInsertCell(ChopperC->getCell("Void",0));
  T0DiskA->setCentreFlag(3);  // Z direction
  T0DiskA->setOffsetFlag(0);  // Centre offset control
  T0DiskA->createAll(System,ChopperC->getKey("Beam"),0);

  FocusT0Mid->addInsertCell(ChopperC->getCell("Void"));
  FocusT0Mid->createAll(System,ChopperC->getKey("Beam"),0,
                        ChopperC->getKey("Beam"),0);

  // Second disk of a T0 chopper
  T0DiskB->addInsertCell(ChopperC->getCell("Void",0));
  T0DiskB->setCentreFlag(3);  // Z direction
  T0DiskB->setOffsetFlag(0);  // Centre offset control
  T0DiskB->createAll(System,ChopperC->getKey("Beam"),0);
  
  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperC->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // NEW TEST SECTION:
  ChopperD->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperD->createAll(System,FocusD->getKey("Guide0"),2);

  // Double disk chopper
  BandADisk->addInsertCell(ChopperD->getCell("Void"));
  BandADisk->setCentreFlag(3);  // Z direction
  BandADisk->setOffsetFlag(1);  // Centre offset control
  BandADisk->createAll(System,ChopperD->getKey("Beam"),0);

  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,ChopperD->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // NEW TEST SECTION:
  ChopperE->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperE->createAll(System,FocusE->getKey("Guide0"),2);

  // Double disk chopper
  BandBDisk->addInsertCell(ChopperE->getCell("Void"));
  BandBDisk->setCentreFlag(3);  // Z direction
  BandBDisk->setOffsetFlag(1);  // Centre offset control
  BandBDisk->createAll(System,ChopperE->getKey("Beam"),0);

  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,ChopperE->getKey("Beam"),2);

  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

      // NEW TEST SECTION:
  ChopperG->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperG->createAll(System,FocusF->getKey("Guide0"),2);

  // First disk of a T0 chopper
  T1DiskA->addInsertCell(ChopperG->getCell("Void",0));
  T1DiskA->setCentreFlag(3);  // Z direction
  T1DiskA->setOffsetFlag(0);  // Centre offset control
  T1DiskA->createAll(System,ChopperG->getKey("Beam"),0);

  FocusT1Mid->addInsertCell(ChopperG->getCell("Void"));
  FocusT1Mid->createAll(System,ChopperG->getKey("Beam"),0,
                        ChopperG->getKey("Beam"),0);

  // Second disk of a T0 chopper
  T1DiskB->addInsertCell(ChopperG->getCell("Void",0));
  T1DiskB->setCentreFlag(3);  // Z direction
  T1DiskB->setOffsetFlag(0);  // Centre offset control
  T1DiskB->createAll(System,ChopperG->getKey("Beam"),0);

  // Pipe to bunker wall
  VPipeG->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeG->setBack(bunkerObj,1);
  VPipeG->createAll(System,ChopperG->getKey("Beam"),2);

  FocusG->addInsertCell(VPipeG->getCells("Void"));
  FocusG->addEndCut(bunkerObj.getSignedLinkString(1));
  FocusG->createAll(System,*VPipeG,0,*VPipeF,0);



  return;

  
  // First section out of monolith
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,FocusA->getKey("Guide0"),2);

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

    // Double disk chopper housing
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Void"));
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  T0DiskAHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0DiskAHouse->createAll(System,T0DiskA->getKey("Main"),0);
  T0DiskAHouse->insertComponent(System,"Void",*T0DiskA);

  return;

  buildChopperBlock(System,bunkerObj,
		    T0DiskA->getKey("Beam"),*VacBoxA,
		    *VacBoxB,*FocusC,
		    *T0DiskB,*T0DiskBHouse,
		    *VPipeB);
  
  // GOING TO POSITION 2:
  // buildChopperBlock(System,bunkerObj,
  //       	    T0DiskB->getKey("Beam"),*VacBoxB,
  //       	    *VacBoxC,*FocusD,
  //       	    *BandADisk,*BandAHouse,
  //       	    *VPipeC);
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

  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  const attachSystem::FixedComp& GFC(FocusFinal->getKey("Guide0"));
  BInsert->createAll(System,GFC,-1,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,-1,
			 FocusFinal->getKey("Guide0"),2);

    if (stopPoint==3) return;                      // STOP At bunker edge
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

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,FocusOutB->getKey("Guide0"),2);
  Cave->insertComponent(System,"FrontWall",*ShieldB);
  Cave->insertComponent(System,"Void",*ShieldB);
  return;
}


}   // NAMESPACE essSystem

