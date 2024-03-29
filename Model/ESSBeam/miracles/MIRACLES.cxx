/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/miracles/MIRACLES.cxx
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
#include <array>

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
#include "Aperture.h"
#include "TwinBase.h"
#include "TwinChopper.h"
#include "GuideUnit.h"
#include "BenderUnit.h"
#include "PlateUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "RectanglePipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "SingleChopper.h"
#include "LineShield.h"
#include "BeamShutter.h"

#include "MIRACLES.h"

namespace essSystem
{

MIRACLES::MIRACLES(const std::string& keyName) :
  attachSystem::CopiedComp("miracles",keyName),
  nGuideSection(8),nSndSection(7),nEllSection(4),stopPoint(0),
  miraclesAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),
  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::PlateUnit(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::PlateUnit(newName+"FC")),

  AppA(new constructSystem::Aperture(newName+"AppA")),
  
  TwinB(new essConstruct::TwinChopper(newName+"TwinB")),
  BDiskTop(new essConstruct::DiskChopper(newName+"BBladeTop")),
  BDiskLow(new essConstruct::DiskChopper(newName+"BBladeLow")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::PlateUnit(newName+"FD")),
  
  TwinC(new essConstruct::TwinChopper(newName+"TwinC")),
  CDiskTop(new essConstruct::DiskChopper(newName+"CBladeTop")),
  CDiskLow(new essConstruct::DiskChopper(newName+"CBladeLow")),

  VPipeE(new constructSystem::RectanglePipe(newName+"PipeE")),
  FocusE(new beamlineSystem::PlateUnit(newName+"FE")),

  ChopE(new essConstruct::SingleChopper(newName+"ChopE")),
  EDisk(new essConstruct::DiskChopper(newName+"EBlade")),

  ShutterA(new constructSystem::BeamShutter(newName+"ShutterA")),

  VPipeF(new constructSystem::RectanglePipe(newName+"PipeF")),
  FocusF(new beamlineSystem::PlateUnit(newName+"FF")),

  VPipeG(new constructSystem::RectanglePipe(newName+"PipeG")),
  BendG(new beamlineSystem::BenderUnit(newName+"BG")),

  BInsert(new BunkerInsert(newName+"BInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  FocusWall(new beamlineSystem::BenderUnit(newName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  BendOutA(new beamlineSystem::BenderUnit(newName+"BOutA")),

  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  BendOutB(new beamlineSystem::BenderUnit(newName+"BOutB"))
/*!
    Constructor
 */
{
  ELog::RegMethod RegA("MIRACLES","MIRACLES");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(miraclesAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(VPipeC);
  OR.addObject(FocusC);
  OR.addObject(AppA);

  OR.addObject(TwinB);
  OR.addObject(BDiskTop);
  OR.addObject(BDiskLow);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(TwinC);
  OR.addObject(CDiskTop);
  OR.addObject(CDiskLow);

  OR.addObject(VPipeE);
  OR.addObject(FocusE); 
 
  OR.addObject(ChopE);
  OR.addObject(EDisk);

  OR.addObject(ShutterA);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(VPipeG);
  OR.addObject(BendG);

  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(BendOutA);

  OR.addObject(VPipeOutB);
  OR.addObject(BendOutB);

}

MIRACLES::~MIRACLES()
  /*!
    Destructor
  */
{}


void
MIRACLES::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("MIRACLES","buildBunkerUnits");

  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
  
  VPipeB->addAllInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0);
  
  VPipeC->addAllInsertCell(bunkerVoid);
  VPipeC->createAll(System,*FocusB,2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0);

  AppA->addInsertCell(VPipeC->getCells("Void"));
  AppA->createAll(System,*FocusC,2);

  TwinB->addInsertCell(bunkerVoid);
  TwinB->createAll(System,*AppA,2);

  BDiskLow->addInsertCell(TwinB->getCell("Void"));
  BDiskLow->createAll(System,TwinB->getKey("MotorBase"),0,
                      TwinB->getKey("Beam"),-1);

  BDiskTop->addInsertCell(TwinB->getCell("Void"));
  BDiskTop->createAll(System,TwinB->getKey("MotorTop"),0,
                      TwinB->getKey("Beam"),-1);

  TwinB->insertAxle(System,*BDiskLow,*BDiskTop);

  VPipeD->addAllInsertCell(bunkerVoid);
  VPipeD->createAll(System,TwinB->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0);

  TwinC->addInsertCell(bunkerVoid);
  TwinC->createAll(System,*FocusD,2);

  CDiskLow->addInsertCell(TwinC->getCell("Void"));
  CDiskLow->createAll(System,TwinC->getKey("MotorBase"),0,
                      TwinC->getKey("Beam"),-1);

  CDiskTop->addInsertCell(TwinC->getCell("Void"));
  CDiskTop->createAll(System,TwinC->getKey("MotorTop"),0,
                      TwinC->getKey("Beam"),-1);
  TwinC->insertAxle(System,*CDiskLow,*CDiskTop);

  VPipeE->addAllInsertCell(bunkerVoid);
  VPipeE->createAll(System,TwinC->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0);

  ChopE->addInsertCell(bunkerVoid);
  ChopE->getKey("Main").setAxisControl(3,ZVert);  
  ChopE->createAll(System,*FocusE,2);

  EDisk->addInsertCell(ChopE->getCell("Void"));
  EDisk->createAll(System,ChopE->getKey("Main"),0);
  ChopE->insertAxle(System,*EDisk);
  
  ShutterA->addInsertCell(bunkerVoid);
  ShutterA->createAll(System,ChopE->getKey("Beam"),2);

  VPipeF->addAllInsertCell(bunkerVoid);
  VPipeF->createAll(System,ShutterA->getKey("Beam"),2);

  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0);

  VPipeG->addAllInsertCell(bunkerVoid);
  VPipeG->createAll(System,*FocusF,2);

  BendG->addInsertCell(VPipeG->getCells("Void"));
  BendG->createAll(System,*VPipeG,0);
  
  return;
}

void
MIRACLES::buildOutGuide(Simulation& System,
			const attachSystem::FixedComp& FA,
			const long int startIndex,
			const int voidCell)
  /*!
    Build the section out of the bunker wall
    \param System :: Simulation ot use
    \param FA :: Fixed point for guide
    \param startIndex :: Fixed point link point 
    \param voidCell :: void cell for constuction 
   */
{
  ELog::RegMethod RegA("MIRACLES","buildOutGuide");

  ShieldA->addInsertCell(voidCell);
  ShieldA->createAll(System,FA,startIndex);

  // Bender 10m
  VPipeOutA->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FA,startIndex);

  BendOutA->addInsertCell(VPipeOutA->getCells("Void"));
  BendOutA->createAll(System,*VPipeOutA,0);

  // Bender 10m
  VPipeOutB->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutB->createAll(System,*BendOutA,2);

  BendOutB->addInsertCell(VPipeOutB->getCells("Void"));
  BendOutB->createAll(System,*VPipeOutB,0);
  
  
  
  return;
}
  
void
MIRACLES::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
  */
{
  ELog::RegMethod RegA("MIRACLES","buildIsolated");

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());

  const int startPoint=Control.EvalDefVar<int>(newName+"StartPoint",0);
  const int stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"BUILD ISOLATED Start/Stop:"
          <<startPoint<<" "<<stopPoint<<ELog::endDiag;
  const attachSystem::FixedComp* FStart(&(World::masterOrigin()));
  long int startIndex(0);
  
  if (startPoint<1)
    {
      buildBunkerUnits(System,*FStart,startIndex,voidCell);
      // Set the start point fo rb
      //      FStart= &(Focus->getKey("Guide0"));
      //      startIndex= 2;
    }
  if (stopPoint==2 || stopPoint==1) return;

  if (startPoint<2)
    {
      VPipeWall->addAllInsertCell(voidCell);
      VPipeWall->createAll(System,*FStart,startIndex);
      
      FocusWall->addInsertCell(VPipeWall->getCell("Void"));
      FocusWall->createAll(System,*VPipeWall,0);
      FStart= FocusWall.get();
    }


  return;
}


  
void 
MIRACLES::build(Simulation& System,
               const GuideItem& GItem,
               const Bunker& bunkerObj,
               const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param bunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
  */
{
  // For output stream
  ELog::RegMethod RegA("MIRACLES","build");

  ELog::EM<<"\nBuilding MIRACLES on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*miraclesAxis,Control,GItem,1);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*miraclesAxis,-3); 

  
  if (stopPoint==1) return;                      // STOP At monolith


  buildBunkerUnits(System,*FocusA,2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  // IN WALL
  // Make bunker insert
  BInsert->setCutSurf("front",bunkerObj,-1);
  BInsert->setCutSurf("back",bunkerObj,-2);
  BInsert->createAll(System,*FocusF,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7);

  ShieldA->setFront(bunkerObj,2);
  buildOutGuide(System,*FocusWall,2,voidCell);

  return;

  
  return; 
}


}   // NAMESPACE essSystem

