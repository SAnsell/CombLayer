/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/FREIA.cxx
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
#include "ChopperPit.h"
#include "ChopperUnit.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"
#include "HoleShape.h"
#include "JawSet.h"

#include "FREIA.h"

namespace essSystem
{

FREIA::FREIA(const std::string& keyName) :
  attachSystem::CopiedComp("freia",keyName),
  stopPoint(0),
  freiaAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  BendA(new beamlineSystem::GuideLine(newName+"BA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::GuideLine(newName+"BC")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),

  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  WFMDisk(new constructSystem::DiskChopper(newName+"WFMBlade")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::GuideLine(newName+"BD")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")),
  FOCDiskC(new constructSystem::DiskChopper(newName+"FOC1Blade")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  BendE(new beamlineSystem::GuideLine(newName+"BE")),
  
  ChopperD(new constructSystem::ChopperUnit(newName+"ChopperD")),
  WBC2Disk(new constructSystem::DiskChopper(newName+"WBC2Blade")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  BendF(new beamlineSystem::GuideLine(newName+"BF")),

  ChopperE(new constructSystem::ChopperUnit(newName+"ChopperE")),
  FOC2Disk(new constructSystem::DiskChopper(newName+"FOC2Blade")),

  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACut(new constructSystem::HoleShape(newName+"OutACut")),
  ChopperOutA(new constructSystem::ChopperUnit(newName+"ChopperOutA")),
  WBC3Disk(new constructSystem::DiskChopper(newName+"WBC3Blade")),

  ChopperOutB(new constructSystem::ChopperUnit(newName+"ChopperOutB")),
  FOC3Disk(new constructSystem::DiskChopper(newName+"FOC3Blade")),

  JawPit(new constructSystem::ChopperPit(newName+"JawPit")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"OutFA")),

  CaveJaw(new constructSystem::JawSet(newName+"CaveJaws")),
  OutBCutFront(new constructSystem::HoleShape(newName+"OutBCutFront")),
  OutBCutBack(new constructSystem::HoleShape(newName+"OutBCutBack"))
  
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("FREIA","FREIA");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(freiaAxis);

  OR.addObject(BendA);

  OR.addObject(VPipeB);
  OR.addObject(BendB);

  OR.addObject(VPipeC);
  OR.addObject(BendC);

  OR.addObject(ChopperA);
  OR.addObject(DDisk);  

  OR.addObject(ChopperB);
  OR.addObject(WFMDisk);  

  OR.addObject(VPipeD);
  OR.addObject(BendD);

  OR.addObject(ChopperC);
  OR.addObject(FOCDiskC);  

  OR.addObject(VPipeE);
  OR.addObject(BendE);

  OR.addObject(ChopperD);
  OR.addObject(WBC2Disk);  

  OR.addObject(VPipeF);
  OR.addObject(BendF);

  OR.addObject(ChopperE);
  OR.addObject(FOC2Disk);  

  OR.addObject(BInsert);
  OR.addObject(FocusWall);  

  OR.addObject(OutPitA);
  OR.addObject(OutACut);
  OR.addObject(ChopperOutA);
  OR.addObject(WBC3Disk);  

  OR.addObject(ChopperOutB);
  OR.addObject(FOC3Disk);  

  OR.addObject(JawPit);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(CaveJaw);
  OR.addObject(OutBCutFront);
  OR.addObject(OutBCutBack);
      
}

FREIA::~FREIA()
  /*!
    Destructor
  */
{}

void
FREIA::setBeamAxis(const FuncDataBase& Control,
                   const GuideItem& GItem,
                   const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("FREIA","setBeamAxis");

  freiaAxis->populate(Control);
  freiaAxis->createUnitVector(GItem);
  freiaAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  freiaAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  freiaAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  freiaAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  // BEAM needs to be shifted/rotated:
  freiaAxis->linkShift(3);
  freiaAxis->linkShift(4);
  freiaAxis->linkAngleRotate(3);
  freiaAxis->linkAngleRotate(4);

  if (reverseZ)
    freiaAxis->reverseZ();
  return;
}

  
void 
FREIA::build(Simulation& System,
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
  ELog::RegMethod RegA("FREIA","build");

  ELog::EM<<"\nBuilding FREIA on : "<<GItem.getKeyName()<<ELog::endDiag;
  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<ELog::endDiag;
  setBeamAxis(Control,GItem,0);
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*freiaAxis,-3,*freiaAxis,-3);
  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge
  
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,BendA->getKey("Guide0"),2);

  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);
  
  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,BendB->getKey("Guide0"),2);

  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,0,*VPipeC,0);
  
  // First (green chopper)
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperA->createAll(System,BendC->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("BuildBeam"),0);


    // First (green chopper)
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->getKey("Main").setAxisControl(3,ZVert);
  ChopperB->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperB->createAll(System,ChopperA->getKey("Beam"),2);
  
  // Double disk chopper
  WFMDisk->addInsertCell(ChopperB->getCell("Void"));
  WFMDisk->setCentreFlag(3);  // Z direction
  WFMDisk->setOffsetFlag(1);  // Z direction
  WFMDisk->createAll(System,ChopperB->getKey("BuildBeam"),0);

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*VPipeD,0,*VPipeD,0);

  // 8.5m FOC chopper
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->getKey("Main").setAxisControl(3,ZVert);
  ChopperC->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperC->createAll(System,BendD->getKey("Guide0"),2);
  // Double disk chopper
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->setCentreFlag(3);  // Z direction
  FOCDiskC->setOffsetFlag(1);  // Z direction
  FOCDiskC->createAll(System,ChopperC->getKey("BuildBeam"),0);


  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,ChopperC->getKey("Beam"),2);

  BendE->addInsertCell(VPipeE->getCells("Void"));
  BendE->createAll(System,*VPipeE,0,*VPipeE,0);

    // 8.5m FOC chopper
  ChopperD->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperD->getKey("Main").setAxisControl(3,ZVert);
  ChopperD->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperD->createAll(System,BendE->getKey("Guide0"),2);
  // Double disk chopper
  WBC2Disk->addInsertCell(ChopperD->getCell("Void"));
  WBC2Disk->setCentreFlag(3);  // Z direction
  WBC2Disk->setOffsetFlag(1);  // Z direction
  WBC2Disk->createAll(System,ChopperD->getKey("BuildBeam"),0);

  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,ChopperD->getKey("Beam"),2);

  BendF->addInsertCell(VPipeF->getCells("Void"));
  BendF->createAll(System,*VPipeF,0,*VPipeF,0);

  // 11.1m FOC chopper
  ChopperE->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperE->getKey("Main").setAxisControl(3,ZVert);
  ChopperE->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperE->createAll(System,BendF->getKey("Guide0"),2);

  // Double disk chopper
  FOC2Disk->addInsertCell(ChopperE->getCell("Void"));
  FOC2Disk->setCentreFlag(3);  // Z direction
  FOC2Disk->setOffsetFlag(1);  // Z direction
  FOC2Disk->createAll(System,ChopperE->getKey("BuildBeam"),0);  
  
  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  BInsert->createAll(System,ChopperE->getKey("Beam"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

    // using 7 : mid point 
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);

  if (stopPoint==3) return;                      // STOP Out of bunker
  
  OutPitA->addInsertCell(voidCell);
  OutPitA->addFrontWall(bunkerObj,2);
  OutPitA->setAxisControl(3,ZVert);
  OutPitA->createAll(System,FocusWall->getKey("Guide0"),2);

  OutACut->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutACut->addInsertCell(OutPitA->getCells("Collet"));
  OutACut->setFaces(OutPitA->getKey("Inner").getSignedFullRule(2),
                    OutPitA->getKey("Mid").getSignedFullRule(-2));
  OutACut->createAll(System,FocusWall->getKey("Guide0"),2);

  // 15m WBC chopper
  ChopperOutA->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutA->getKey("Main").setAxisControl(3,ZVert);
  ChopperOutA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperOutA->createAll(System,FocusWall->getKey("Guide0"),2);
  // Double disk chopper
  WBC3Disk->addInsertCell(ChopperOutA->getCell("Void"));
  WBC3Disk->setCentreFlag(3);  // Z direction
  WBC3Disk->setOffsetFlag(1);  // Z direction
  WBC3Disk->createAll(System,ChopperOutA->getKey("BuildBeam"),0);

  ChopperOutB->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutB->getKey("Main").setAxisControl(3,ZVert);
  ChopperOutB->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperOutB->createAll(System,ChopperOutA->getKey("Beam"),2);

  // Double disk chopper
  FOC3Disk->addInsertCell(ChopperOutB->getCell("Void"));
  FOC3Disk->setCentreFlag(3);  // Z direction
  FOC3Disk->setOffsetFlag(1);  // Z direction
  FOC3Disk->createAll(System,ChopperOutB->getKey("BuildBeam"),0);
  
  JawPit->addInsertCell(voidCell);
  JawPit->setAxisControl(-3,ZVert);
  JawPit->createAll(System,ChopperOutA->getKey("Beam"),2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->addInsertCell(JawPit->getCells("Outer"));
  ShieldA->addInsertCell(JawPit->getCells("MidLayer"));
  ShieldA->setFront(OutPitA->getKey("Mid"),2);
  ShieldA->setBack(JawPit->getKey("Mid"),1);
  ShieldA->createAll(System,OutPitA->getKey("Inner"),0);
  //  ShieldA->insertComponent(System,"Void",*VPipeOutA)

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,ChopperOutA->getKey("Beam"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  CaveJaw->setInsertCell(JawPit->getCell("Void"));
  CaveJaw->createAll(System,JawPit->getKey("Inner"),0);

  OutBCutBack->addInsertCell(JawPit->getCells("MidLayerBack"));
  OutBCutBack->addInsertCell(JawPit->getCells("Collet"));
  OutBCutBack->setFaces(JawPit->getKey("Inner").getSignedFullRule(2),
                        JawPit->getKey("Mid").getSignedFullRule(-2));
  OutBCutBack->createAll(System,JawPit->getKey("Inner"),2);

  OutBCutFront->addInsertCell(JawPit->getCells("MidLayerFront"));
  OutBCutFront->setFaces(JawPit->getKey("Mid").getSignedFullRule(-1),
                         JawPit->getKey("Inner").getSignedFullRule(1));
  OutBCutFront->createAll(System,JawPit->getKey("Inner"),-1);
  
  return;
}


}   // NAMESPACE essSystem

