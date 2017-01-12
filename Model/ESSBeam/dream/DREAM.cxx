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
#include "FrontBackCut.h"
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
  dreamAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
 
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),
  SDisk(new constructSystem::DiskChopper(newName+"SBlade")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),
  
  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  BandADisk(new constructSystem::DiskChopper(newName+"BandADisk")),  
  
  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")), 
  T0DiskA(new constructSystem::DiskChopper(newName+"T0DiskA")),
  FocusT0Mid(new beamlineSystem::GuideLine(newName+"FT0Mid")),
  T0DiskB(new constructSystem::DiskChopper(newName+"T0DiskB")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  
  ChopperD(new constructSystem::ChopperUnit(newName+"ChopperD")),

  BandBDisk(new constructSystem::DiskChopper(newName+"BandBDisk")),  
  ChopperE(new constructSystem::ChopperUnit(newName+"ChopperE")),

  T1DiskA(new constructSystem::DiskChopper(newName+"T1DiskA")),
  FocusT1Mid(new beamlineSystem::GuideLine(newName+"FT1Mid")),
  T1DiskB(new constructSystem::DiskChopper(newName+"T1DiskB")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),
  
  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),

  Cave(new DreamHut(newName+"Cave")),

  VPipeCaveA(new constructSystem::VacuumPipe(newName+"PipeCaveA")),
  FocusCaveA(new beamlineSystem::GuideLine(newName+"FCaveA"))

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
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(ChopperA);
  OR.addObject(DDisk);  
  OR.addObject(SDisk);  
  OR.addObject(VPipeC);
  OR.addObject(FocusC);
  OR.addObject(ChopperB);
  OR.addObject(BandADisk);
  
  OR.addObject(ChopperC);
  OR.addObject(T0DiskA);
  OR.addObject(FocusT0Mid);
  OR.addObject(T0DiskB);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(ChopperD);

  OR.addObject(BandBDisk);
  OR.addObject(ChopperE);
  
  OR.addObject(T1DiskA);
  OR.addObject(FocusT1Mid);
  OR.addObject(T1DiskB);

  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(BInsert);
  OR.addObject(FocusWall);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
  
  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);

  OR.addObject(Cave);
  
  OR.addObject(VPipeCaveA);
  OR.addObject(FocusCaveA);

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

  // BEAM needs to be shifted/rotated:
  dreamAxis->linkShift(3);
  dreamAxis->linkShift(4);
  dreamAxis->linkAngleRotate(3);
  dreamAxis->linkAngleRotate(4);

  if (reverseZ)
    dreamAxis->reverseZ();
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
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  setBeamAxis(GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);

  if (stopPoint==1) return;                      // STOP At monolith edge

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  // NEW TEST SECTION:
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,FocusA->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Beam"),0);

  // Double disk chopper
  SDisk->addInsertCell(ChopperA->getCell("Void"));
  SDisk->setCentreFlag(3);  // Z direction
  SDisk->setOffsetFlag(1);  // Centre offset control
  SDisk->createAll(System,ChopperA->getKey("Beam"),0);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // NEW TEST SECTION:
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,*VPipeC,2);
  // Double disk chopper
  BandADisk->addInsertCell(ChopperB->getCell("Void"));
  BandADisk->setCentreFlag(3);  // Z direction
  BandADisk->setOffsetFlag(1);  // Centre offset control
  BandADisk->createAll(System,ChopperB->getKey("Beam"),0);

  // NEW TEST SECTION:
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,ChopperB->getKey("Beam"),2);

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
  

  //  FocusC->addInsertCell(VPipeC->getCells("Void"));
  // FocusC->createAll(System,*VPipeC,-1,*VPipeC,-1);

  // NEW TEST SECTION:
  ChopperD->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperD->createAll(System,FocusD->getKey("Guide0"),2);

  // Double disk chopper
  BandBDisk->addInsertCell(ChopperD->getCell("Void"));
  BandBDisk->setCentreFlag(3);  // Z direction
  BandBDisk->setOffsetFlag(1);  // Centre offset control
  BandBDisk->createAll(System,ChopperD->getKey("Beam"),0);


  // Second T0 Chopper::
  ChopperE->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperE->createAll(System,ChopperD->getKey("Beam"),2);

  // First disk of a T0 chopper
  T1DiskA->addInsertCell(ChopperE->getCell("Void",0));
  T1DiskA->setCentreFlag(3);  // Z direction
  T1DiskA->setOffsetFlag(0);  // Centre offset control
  T1DiskA->createAll(System,ChopperE->getKey("Beam"),0);

  FocusT1Mid->addInsertCell(ChopperE->getCell("Void"));
  FocusT1Mid->createAll(System,ChopperE->getKey("Beam"),0,
                        ChopperE->getKey("Beam"),0);

  // Second disk of a T0 chopper
  T1DiskB->addInsertCell(ChopperE->getCell("Void",0));
  T1DiskB->setCentreFlag(3);  // Z direction
  T1DiskB->setOffsetFlag(0);  // Centre offset control
  T1DiskB->createAll(System,ChopperE->getKey("Beam"),0);

  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,ChopperE->getKey("Beam"),2);
  
  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);
  // move guide
  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,*VPipeE,2);
  // VPipeH->setBack(bunkerObj,1);
  
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  BInsert->createAll(System,FocusF->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);


  if (stopPoint==3) return;                      // STOP At bunker edge
  // Section to 17m
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->createAll(System,*BInsert,2);

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);
      
  FocusOutA->addInsertCell(VPipeOutA->getCell("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  // Section to 34m
  ShieldB->addInsertCell(voidCell);
  ShieldB->createAll(System,*ShieldA,2);

  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->setFront(*ShieldB,-1);
  VPipeOutB->setBack(*ShieldB,-2);
  VPipeOutB->createAll(System,*ShieldB,2);

  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,7,*VPipeOutB,7);

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldB,2);
  Cave->insertComponent(System,"FrontWall",*ShieldB);

  VPipeCaveA->addInsertCell(Cave->getCell("FrontWall"));
  VPipeCaveA->addInsertCell(Cave->getCell("Void"));
  VPipeCaveA->setFront(*VPipeOutB,2);
  VPipeCaveA->createAll(System,*VPipeOutB,2);
  FocusCaveA->addInsertCell(VPipeCaveA->getCell("Void"));
  FocusCaveA->createAll(System,*VPipeCaveA,7,*VPipeCaveA,7);
                        
  return;
}


}   // NAMESPACE essSystem

