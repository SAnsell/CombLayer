/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBuild/bifrost/BIFROST.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "FixedOffsetGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "Aperture.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "CompBInsert.h"
#include "SingleChopper.h"
#include "ChopperPit.h"
#include "LineShield.h"
#include "HoleShape.h"
#include "EssHut.h"

#include "BIFROST.h"

namespace essSystem
{

BIFROST::BIFROST(const std::string& keyName) :
  attachSystem::CopiedComp("bifrost",keyName),
  nGuideSection(8),nSndSection(7),nEllSection(4),stopPoint(0),
  bifrostAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),
  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::PlateUnit(newName+"FB")),
  AppA(new constructSystem::Aperture(newName+"AppA")),

  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  DDisk(new essConstruct::DiskChopper(newName+"DBlade")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::PlateUnit(newName+"FC")),
  
  ChopperB(new essConstruct::SingleChopper(newName+"ChopperB")),
  FOCDiskB(new essConstruct::DiskChopper(newName+"FOC1Blade")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::PlateUnit(newName+"FD")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::PlateUnit(newName+"FE")),

  ChopperC(new essConstruct::SingleChopper(newName+"ChopperC")),
  FOCDiskC(new essConstruct::DiskChopper(newName+"FOC2Blade")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::PlateUnit(newName+"FF")),
  
  AppB(new constructSystem::Aperture(newName+"AppB")),

  //  BInsert(new BunkerInsert(newName+"BInsert")),
  BInsert(new CompBInsert(newName+"CInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  FocusWall(new beamlineSystem::PlateUnit(newName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::PlateUnit(newName+"FOutA")),

  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::PlateUnit(newName+"FOutB")),

  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::PlateUnit(newName+"FOutC")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACutFront(new constructSystem::HoleShape(newName+"OutACutFront")),
  OutACutBack(new constructSystem::HoleShape(newName+"OutACutBack")),

  ChopperOutA(new essConstruct::SingleChopper(newName+"ChopperOutA")),
  FOCDiskOutA(new essConstruct::DiskChopper(newName+"FOCOutABlade")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),

  Cave(new EssHut(newName+"Cave")),
  CaveCut(new constructSystem::HoleShape(newName+"CaveCut")),

  VPipeCave(new constructSystem::VacuumPipe(newName+"PipeCave"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("BIFROST","BIFROST");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<nGuideSection;i++)
    {
      const std::string strNum(std::to_string(i));
      RecPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeR"+strNum));
      RecFocus[i]=std::shared_ptr<beamlineSystem::PlateUnit>
        (new beamlineSystem::PlateUnit(newName+"FOutR"+strNum));
      OR.addObject(RecPipe[i]);
      OR.addObject(RecFocus[i]);
    }

  for(size_t i=0;i<nSndSection;i++)
    {
      const std::string strNum(std::to_string(i));
      SndPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeS"+strNum));
      SndFocus[i]=std::shared_ptr<beamlineSystem::PlateUnit>
        (new beamlineSystem::PlateUnit(newName+"FOutS"+strNum));
      OR.addObject(SndPipe[i]);
      OR.addObject(SndFocus[i]);
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      const std::string strNum(std::to_string(i));
      EllPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeE"+strNum));
      EllFocus[i]=std::shared_ptr<beamlineSystem::PlateUnit>
        (new beamlineSystem::PlateUnit(newName+"FOutE"+strNum));
      OR.addObject(EllPipe[i]);
      OR.addObject(EllFocus[i]);
    }

  OR.addObject(bifrostAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(AppA);

  OR.addObject(ChopperA);
  OR.addObject(DDisk);  
  
  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(ChopperB);
  OR.addObject(FOCDiskB);  

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(ChopperC);
  OR.addObject(FOCDiskC);  
  
  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(AppB);

  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);  

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);
  
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);

  OR.addObject(OutPitA);
  OR.addObject(OutACutFront);
  OR.addObject(OutACutBack);

  OR.addObject(ChopperOutA);
  OR.addObject(FOCDiskOutA);  

  OR.addObject(Cave);
  OR.addObject(CaveCut);

  OR.addObject(VPipeCave);
}

BIFROST::~BIFROST()
  /*!
    Destructor
  */
{}
  
void 
BIFROST::build(Simulation& System,
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
  ELog::RegMethod RegA("BIFROST","build");

  ELog::EM<<"\nBuilding BIFROST on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*bifrostAxis,Control,GItem,1);
  ELog::EM<<"Beam axis == "<<bifrostAxis->getLinkPt(3)<<ELog::endDiag;
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*bifrostAxis,-3); 
  
  if (stopPoint==1) return;                      // STOP At monolith

  VPipeB->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0);
  AppA->addInsertCell(bunkerObj.getCell("MainVoid"));
  AppA->createAll(System,*FocusB,2);

  // Fist chopper
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,*AppA,2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDisk); 
  
  // Elliptic 4m section
  VPipeC->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0);

  // 10.5m FOC chopper
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,*FocusC,2);
  // Single disk FOC
  FOCDiskB->addInsertCell(ChopperB->getCell("Void"));
  FOCDiskB->setOffsetFlag(1);  // Z direction
  FOCDiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*FOCDiskB);
  
  // Rectangle 6m section
  VPipeD->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);
  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0);

  // Rectangle 4m section
  VPipeE->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,*FocusD,2);
  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0);

  // 20.5m FOC chopper-2
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,*FocusE,2);
  // Single disk FOC-2
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->setOffsetFlag(1);  // Z direction
  FOCDiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*FOCDiskC);
  
  // Rectangle 4m section
  VPipeF->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,ChopperC->getKey("Beam"),2);
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0);

  AppB->addInsertCell(bunkerObj.getCell("MainVoid"));
  AppB->createAll(System,*FocusF,2);

  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  BInsert->setCutSurf("front",bunkerObj,-1);
  BInsert->setCutSurf("back",bunkerObj,-2);
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->createAll(System,*AppB,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  
  //  VPipeWall->addAllInsertCell(BInsert->getCell("Void"));
  //  VPipeWall->createAll(System,*BInsert,-1);

  // using 7 : mid point
  FocusWall->setFront(*BInsert,-1);
  FocusWall->setBack(*BInsert,-2);
  FocusWall->addInsertCell(BInsert->getCells("Item"));
  FocusWall->createAll(System,*BInsert,7);

  if (stopPoint==3) return;                      // STOP Out of bunker
  

  // First put pit into the main void
  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,*FocusWall,2);
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(*BInsert,2);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->setBack(*OutPitA,"midFront");
  ShieldA->createAll(System,*FocusWall,2);

  // Elliptic 6m section
  VPipeOutA->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,*FocusWall,2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0);

  // Elliptic 6m section
  VPipeOutB->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutB->createAll(System,*FocusOutA,2);

  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0);

  // Elliptic 6m section
  VPipeOutC->addAllInsertCell(ShieldA->getCell("Void"));
  VPipeOutC->createAll(System,*FocusOutB,2);

  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0);

  const attachSystem::FixedComp* LinkPtr= FocusOutC.get();
  for(size_t i=0;i<nGuideSection;i++)
    {
      // Elliptic 6m section
      RecPipe[i]->addAllInsertCell(ShieldA->getCell("Void"));
      RecPipe[i]->createAll(System,*LinkPtr,2);
      
      RecFocus[i]->addInsertCell(RecPipe[i]->getCells("Void"));
      RecFocus[i]->createAll(System,*RecPipe[i],0);

      LinkPtr= RecFocus[i].get();
    }

  OutACutFront->addInsertCell(OutPitA->getCells("MidLayerFront"));
  OutACutFront->setCutSurf("front",*OutPitA,"#midFront");
  OutACutFront->setCutSurf("back",*OutPitA,"innerFront");
  OutACutFront->createAll(System,*OutPitA,"#innerFront");


  OutACutBack->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutACutBack->setCutSurf("front",*OutPitA,"innerBack");
  OutACutBack->setCutSurf("back",*OutPitA,"#midBack");
  OutACutBack->addInsertCell(OutPitA->getCells("Collet"));
  OutACutBack->createAll(System,*OutPitA,"innerBack");

  // FOC chopper out of bunker
  ChopperOutA->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutA->createAll(System,*LinkPtr,2);    // copied from last of
                                                // guide array
  // Single disk FOC-OutA
  FOCDiskOutA->addInsertCell(ChopperOutA->getCell("Void"));
  FOCDiskOutA->setOffsetFlag(1);  // Z direction
  FOCDiskOutA->createAll(System,ChopperOutA->getKey("Main"),0);
  ChopperOutA->insertAxle(System,*FOCDiskOutA);
  
  ShieldB->addInsertCell(voidCell);
  ShieldB->setFront(*OutPitA,"midBack");
  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldB->createAll(System,*LinkPtr,2);

  LinkPtr= &ChopperOutA->getKey("Beam");
  for(size_t i=0;i<nSndSection;i++)
    {
      // Rectangle 6m section
      SndPipe[i]->addAllInsertCell(ShieldB->getCell("Void"));
      SndPipe[i]->createAll(System,*LinkPtr,2);
      
      SndFocus[i]->addInsertCell(SndPipe[i]->getCells("Void"));
      SndFocus[i]->createAll(System,*SndPipe[i],0);

      LinkPtr= SndFocus[i].get();
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      // Elliptic 6m sections
      EllPipe[i]->addAllInsertCell(ShieldB->getCell("Void"));
      EllPipe[i]->createAll(System,*LinkPtr,2);
      
      EllFocus[i]->addInsertCell(EllPipe[i]->getCells("Void"));
      EllFocus[i]->createAll(System,*EllPipe[i],0);

      LinkPtr= EllFocus[i].get();
    }

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldB,2);

  ShieldB->addInsertCell(Cave->getCells("FrontWall"));
  ShieldB->insertObjects(System);

  CaveCut->addInsertCell(Cave->getCells("IronFront"));
  CaveCut->setCutSurf("front",*Cave,"#MidFront");
  CaveCut->setCutSurf("back",*Cave,"InnerFront");
  CaveCut->createAll(System,*ShieldB,2);

  // Elliptic 6m section
  VPipeCave->addAllInsertCell(Cave->getCell("Void"));
  VPipeCave->createAll(System,*CaveCut,2);
  
  return; 
}


}   // NAMESPACE essSystem

