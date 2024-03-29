/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/freia/FREIA.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "SingleChopper.h"
#include "LineShield.h"
#include "HoleShape.h"
#include "JawSet.h"

#include "FREIA.h"

namespace essSystem
{

FREIA::FREIA(const std::string& keyName) :
  attachSystem::CopiedComp("freia",keyName),
  startPoint(0),stopPoint(0),
  freiaAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  BendA(new beamlineSystem::BenderUnit(newName+"BA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::BenderUnit(newName+"BB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::BenderUnit(newName+"BC")),

  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  DDisk(new essConstruct::DiskChopper(newName+"DBlade")),

  ChopperB(new essConstruct::SingleChopper(newName+"ChopperB")),
  WFMDisk(new essConstruct::DiskChopper(newName+"WFMBlade")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::BenderUnit(newName+"BD")),

  ChopperC(new essConstruct::SingleChopper(newName+"ChopperC")),
  FOCDiskC(new essConstruct::DiskChopper(newName+"FOC1Blade")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  BendE(new beamlineSystem::BenderUnit(newName+"BE")),
  
  ChopperD(new essConstruct::SingleChopper(newName+"ChopperD")),
  WBC2Disk(new essConstruct::DiskChopper(newName+"WBC2Blade")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  BendF(new beamlineSystem::BenderUnit(newName+"BF")),

  ChopperE(new essConstruct::SingleChopper(newName+"ChopperE")),
  FOC2Disk(new essConstruct::DiskChopper(newName+"FOC2Blade")),

  BInsert(new BunkerInsert(newName+"BInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),  
  FocusWall(new beamlineSystem::PlateUnit(newName+"FWall")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACut(new constructSystem::HoleShape(newName+"OutACut")),
  ChopperOutA(new essConstruct::SingleChopper(newName+"ChopperOutA")),
  WBC3Disk(new essConstruct::DiskChopper(newName+"WBC3Blade")),

  ChopperOutB(new essConstruct::SingleChopper(newName+"ChopperOutB")),
  FOC3Disk(new essConstruct::DiskChopper(newName+"FOC3Blade")),

  JawPit(new constructSystem::ChopperPit(newName+"JawPit")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::PlateUnit(newName+"OutFA")),

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
FREIA::buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& FA,
                        const long int startIndex,
                        const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FA :: Fixed component to start build from [Mono guide]
    \param startIndex :: Fixed component link point
    \param bunkerVoid :: cell to place objects in
   */
{
  ELog::RegMethod RegA("FREIA","buildBunkerUnits");
  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());  

  VPipeB->addAllInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);

  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0);
  
  VPipeC->addAllInsertCell(bunkerVoid);
  VPipeC->createAll(System,*BendB,2);

  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,0);
  
  // First (green chopper)
  ChopperA->addInsertCell(bunkerVoid);
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperA->createAll(System,*BendC,2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDisk);


    // First (green chopper)
  ChopperB->addInsertCell(bunkerVoid);
  ChopperB->getKey("Main").setAxisControl(3,ZVert);
  ChopperB->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperB->createAll(System,ChopperA->getKey("Beam"),2);
  
  // Double disk chopper
  WFMDisk->addInsertCell(ChopperB->getCell("Void"));
  WFMDisk->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*WFMDisk);
  
  VPipeD->addAllInsertCell(bunkerVoid);
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*VPipeD,0);

  // 8.5m FOC chopper
  ChopperC->addInsertCell(bunkerVoid);
  ChopperC->getKey("Main").setAxisControl(3,ZVert);
  ChopperC->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperC->createAll(System,*BendD,2);
  // Double disk chopper
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*FOCDiskC);
  
  VPipeE->addAllInsertCell(bunkerVoid);
  VPipeE->createAll(System,ChopperC->getKey("Beam"),2);

  BendE->addInsertCell(VPipeE->getCells("Void"));
  BendE->createAll(System,*VPipeE,0);

    // 8.5m FOC chopper
  ChopperD->addInsertCell(bunkerVoid);
  ChopperD->getKey("Main").setAxisControl(3,ZVert);
  ChopperD->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperD->createAll(System,*BendE,2);
  // Double disk chopper
  WBC2Disk->addInsertCell(ChopperD->getCell("Void"));
  WBC2Disk->createAll(System,ChopperD->getKey("Main"),0);
  ChopperD->insertAxle(System,*WBC2Disk);
  
  VPipeF->addAllInsertCell(bunkerVoid);
  VPipeF->createAll(System,ChopperD->getKey("Beam"),2);

  BendF->addInsertCell(VPipeF->getCells("Void"));
  BendF->createAll(System,*VPipeF,0);

  // 11.1m FOC chopper
  ChopperE->addInsertCell(bunkerVoid);
  ChopperE->getKey("Main").setAxisControl(3,ZVert);
  ChopperE->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperE->createAll(System,*BendF,2);

  // Double disk chopper
  FOC2Disk->addInsertCell(ChopperE->getCell("Void"));
  FOC2Disk->createAll(System,ChopperE->getKey("Main"),0);
  ChopperE->insertAxle(System,*FOC2Disk);

  return;
}


void
FREIA::buildOutGuide(Simulation& System,
                     const attachSystem::FixedComp&,
                     const long int,
                     const int voidCell)
  /*!
    Build all the components that are outside of the wall
    \param System :: Simulation 
    \param FW :: Focus wall fixed axis
    \param startPoint :: link point 
    \param voidCell :: void cell nubmer
   */
{
  ELog::RegMethod RegA("FREIA","buildOutGuide");

  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
    // OUTSIDE:
  //  added before:
  //    OutPitT0->addFrontWall(bunkerObj,2);
  //

  OutPitA->addInsertCell(voidCell);
  OutPitA->setAxisControl(3,ZVert);
  OutPitA->createAll(System,*FocusWall,2);

  OutACut->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutACut->addInsertCell(OutPitA->getCells("Collet"));
  OutACut->setCutSurf("front",*OutPitA,"innerBack");
  OutACut->setCutSurf("back",*OutPitA,"#midBack");
  OutACut->createAll(System,*FocusWall,2);

  // 15m WBC chopper
  ChopperOutA->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutA->getKey("Main").setAxisControl(3,ZVert);
  ChopperOutA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperOutA->createAll(System,*FocusWall,2);
  // Double disk chopper
  WBC3Disk->addInsertCell(ChopperOutA->getCell("Void"));
  WBC3Disk->setOffsetFlag(1);  // Centre disk
  WBC3Disk->createAll(System,ChopperOutA->getKey("Main"),0);
  ChopperOutA->insertAxle(System,*WBC3Disk);
  
  ChopperOutB->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutB->getKey("Main").setAxisControl(3,ZVert);
  ChopperOutB->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperOutB->createAll(System,ChopperOutA->getKey("Beam"),2);

  // Double disk chopper
  FOC3Disk->addInsertCell(ChopperOutB->getCell("Void"));
  FOC3Disk->setOffsetFlag(1);  // Centre in chopper void
  FOC3Disk->createAll(System,ChopperOutB->getKey("Main"),0);
  ChopperOutB->insertAxle(System,*FOC3Disk);
    
  JawPit->addInsertCell(voidCell);
  JawPit->setAxisControl(-3,ZVert);
  JawPit->createAll(System,ChopperOutA->getKey("Beam"),2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->addInsertCell(JawPit->getCells("Outer"));
  ShieldA->addInsertCell(JawPit->getCells("MidLayer"));
  ShieldA->setFront(*OutPitA,"midBack");
  ShieldA->setBack(*JawPit,"midFront");
  ShieldA->createAll(System,*OutPitA,0);
  //  ShieldA->insertComponent(System,"Void",*VPipeOutA)

  VPipeOutA->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,ChopperOutA->getKey("Beam"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0);

  return;
}

void
FREIA::buildHut(Simulation& System)

  /*!
    Builds the hut connected to the FixedPoint given
    \param System :: Simulation to build with
   */
{
  ELog::RegMethod RegA("FREIA","buildHut");

  CaveJaw->setInsertCell(JawPit->getCell("Void"));
  CaveJaw->createAll(System,*JawPit,0);

  OutBCutBack->addInsertCell(JawPit->getCells("MidLayerBack"));
  OutBCutBack->setCutSurf("front",*JawPit,"innerBack");
  OutBCutBack->setCutSurf("back",*JawPit,"#midBack");
  OutBCutBack->addInsertCell(JawPit->getCells("Collet"));
  OutBCutBack->createAll(System,*JawPit,"innerBack");

  OutBCutFront->addInsertCell(JawPit->getCells("MidLayerFront"));
  OutBCutFront->setCutSurf("front",*JawPit,"#midFront");
  OutBCutFront->setCutSurf("back",*JawPit,"innerFront");
  OutBCutFront->createAll(System,*JawPit,"#innerFront");

  return;
}

  
void
FREIA::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("FREIA","buildIsolated");


  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  startPoint=Control.EvalDefVar<int>(newName+"StartPoint",0);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"BUILD ISOLATED Start/Stop:"
          <<startPoint<<" "<<stopPoint<<ELog::endDiag;
  const attachSystem::FixedComp* FStart(&(World::masterOrigin()));
  long int startIndex(0);
  
  if (!startPoint)
    {
      buildBunkerUnits(System,*FStart,startIndex,voidCell);
      FStart= &ChopperE->getKey("Beam");
      startIndex= 2;
    }
  if (stopPoint==2 || stopPoint==1) return;

  if (startPoint<2)
    {
      VPipeWall->addAllInsertCell(voidCell);
      VPipeWall->createAll(System,*FStart,startIndex);
      
      FocusWall->addInsertCell(VPipeWall->getCell("Void"));
      FocusWall->createAll(System,*VPipeWall,0);
      FStart= FocusWall.get();
      OutPitA->addFrontWall(*VPipeWall,2);
      startIndex=2;
    }
  if (stopPoint==3) return;

  if (startPoint<3)
    {
      buildOutGuide(System,*FStart,startIndex,voidCell);      
      FStart=&(ChopperOutB->getKey("Beam"));
      startIndex=2;
    }

  if (stopPoint==4) return;      

  if (startPoint<4)
    {
      buildHut(System);
      //      buildDetectorArray(System,*Sample,0,Cave->getCell("Void"));
    }
  
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
  
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());  
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*freiaAxis,Control,GItem,1);
  
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*freiaAxis,-3);

  if (stopPoint==1) return;                 // STOP At monolith edge
  buildBunkerUnits(System,*BendA,2,
                   bunkerObj.getCell("MainVoid"));
  
  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  BInsert->setCutSurf("front",bunkerObj,-1);
  BInsert->setCutSurf("back",bunkerObj,-2);
  BInsert->createAll(System,ChopperE->getKey("Beam"),2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

    // using 7 : mid point 
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7);

  if (stopPoint==3) return;                      // STOP Out of bunker

  OutPitA->addFrontWall(bunkerObj,2);
  buildOutGuide(System,*FocusWall,2,voidCell);      

  if (stopPoint==4) return;                      // STOP Out of bunker


  buildHut(System);
  
  
  return;
}


}   // NAMESPACE essSystem

