/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BIFROST.cxx
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
#include <array>

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
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "Aperture.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "SingleChopper.h"
#include "ChopperPit.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"
#include "HoleShape.h"
#include "BifrostHut.h"

#include "BIFROST.h"

namespace essSystem
{

BIFROST::BIFROST(const std::string& keyName) :
  attachSystem::CopiedComp("bifrost",keyName),
  nGuideSection(8),nSndSection(7),nEllSection(4),stopPoint(0),
  bifrostAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),
  AppA(new constructSystem::Aperture(newName+"AppA")),

  ChopperA(new constructSystem::SingleChopper(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),
  
  ChopperB(new constructSystem::SingleChopper(newName+"ChopperB")),
  FOCDiskB(new constructSystem::DiskChopper(newName+"FOC1Blade")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperC(new constructSystem::SingleChopper(newName+"ChopperC")),
  FOCDiskC(new constructSystem::DiskChopper(newName+"FOC2Blade")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),
  
  AppB(new constructSystem::Aperture(newName+"AppB")),

  //  BInsert(new BunkerInsert(newName+"BInsert")),
  BInsert(new CompBInsert(newName+"CInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),


  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),

  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"FOutC")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  OutACutFront(new constructSystem::HoleShape(newName+"OutACutFront")),
  OutACutBack(new constructSystem::HoleShape(newName+"OutACutBack")),

  ChopperOutA(new constructSystem::SingleChopper(newName+"ChopperOutA")),
  FOCDiskOutA(new constructSystem::DiskChopper(newName+"FOCOutABlade")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),

  Cave(new BifrostHut(newName+"Cave")),
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
      const std::string strNum(StrFunc::makeString(i));
      RecPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeR"+strNum));
      RecFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutR"+strNum));
      OR.addObject(RecPipe[i]);
      OR.addObject(RecFocus[i]);
    }

  for(size_t i=0;i<nSndSection;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      SndPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeS"+strNum));
      SndFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutS"+strNum));
      OR.addObject(SndPipe[i]);
      OR.addObject(SndFocus[i]);
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      EllPipe[i]=std::shared_ptr<constructSystem::VacuumPipe>
        (new constructSystem::VacuumPipe(newName+"PipeE"+strNum));
      EllFocus[i]=std::shared_ptr<beamlineSystem::GuideLine>
        (new beamlineSystem::GuideLine(newName+"FOutE"+strNum));
      OR.addObject(EllPipe[i]);
      OR.addObject(EllFocus[i]);
    }

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
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
  FocusA->createAll(System,*bifrostAxis,-3,*bifrostAxis,-3); 
  
  if (stopPoint==1) return;                      // STOP At monolith

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);
  AppA->addInsertCell(bunkerObj.getCell("MainVoid"));
  AppA->createAll(System,FocusB->getKey("Guide0"),2);

  // Fist chopper
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,*AppA,2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDisk); 
  
  // Elliptic 4m section
  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // 10.5m FOC chopper
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,FocusC->getKey("Guide0"),2);
  // Single disk FOC
  FOCDiskB->addInsertCell(ChopperB->getCell("Void"));
  FOCDiskB->setOffsetFlag(1);  // Z direction
  FOCDiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*FOCDiskB);
  
  // Rectangle 6m section
  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);
  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // Rectangle 4m section
  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,FocusD->getKey("Guide0"),2);
  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // 20.5m FOC chopper-2
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusE->getKey("Guide0"),2);
  // Single disk FOC-2
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->setOffsetFlag(1);  // Z direction
  FOCDiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*FOCDiskC);
  
  // Rectangle 4m section
  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,ChopperC->getKey("Beam"),2);
  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

  AppB->addInsertCell(bunkerObj.getCell("MainVoid"));
  AppB->createAll(System,FocusF->getKey("Guide0"),2);

  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->addInsertCell(74123);
  BInsert->createAll(System,*AppB,2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  
  //  VPipeWall->addInsertCell(BInsert->getCell("Void"));
  //  VPipeWall->createAll(System,*BInsert,-1);

  // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCells("Item"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);

  if (stopPoint==3) return;                      // STOP Out of bunker
  

  // First put pit into the main void
  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,FocusWall->getKey("Shield"),2);
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(*BInsert,2);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->setBack(OutPitA->getKey("Mid"),1);
  ShieldA->createAll(System,FocusWall->getKey("Shield"),2);

  // Elliptic 6m section
  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  // Elliptic 6m section
  VPipeOutB->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutB->createAll(System,FocusOutA->getKey("Guide0"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);

  // Elliptic 6m section
  VPipeOutC->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutC->createAll(System,FocusOutB->getKey("Guide0"),2);

  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  const attachSystem::FixedComp* LinkPtr= &FocusOutC->getKey("Guide0");
  for(size_t i=0;i<nGuideSection;i++)
    {
      // Elliptic 6m section
      RecPipe[i]->addInsertCell(ShieldA->getCell("Void"));
      RecPipe[i]->createAll(System,*LinkPtr,2);
      
      RecFocus[i]->addInsertCell(RecPipe[i]->getCells("Void"));
      RecFocus[i]->createAll(System,*RecPipe[i],0,*RecPipe[i],0);

      LinkPtr= &RecFocus[i]->getKey("Guide0");
    }

  OutACutFront->addInsertCell(OutPitA->getCells("MidLayerFront"));
  OutACutFront->setFaces(OutPitA->getKey("Mid").getFullRule(-1),
                         OutPitA->getKey("Inner").getFullRule(1));
  OutACutFront->createAll(System,OutPitA->getKey("Inner"),-1);


  OutACutBack->addInsertCell(OutPitA->getCells("MidLayerBack"));
  OutACutBack->addInsertCell(OutPitA->getCells("Collet"));
  OutACutBack->setFaces(OutPitA->getKey("Inner").getFullRule(2),
                        OutPitA->getKey("Mid").getFullRule(-2));
  OutACutBack->createAll(System,OutPitA->getKey("Inner"),2);
  

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
  ShieldB->setFront(OutPitA->getKey("Mid"),2);
  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldB->createAll(System,*LinkPtr,2);

  LinkPtr= &ChopperOutA->getKey("Beam");
  for(size_t i=0;i<nSndSection;i++)
    {
      // Rectangle 6m section
      SndPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      SndPipe[i]->createAll(System,*LinkPtr,2);
      
      SndFocus[i]->addInsertCell(SndPipe[i]->getCells("Void"));
      SndFocus[i]->createAll(System,*SndPipe[i],0,*SndPipe[i],0);

      LinkPtr= &SndFocus[i]->getKey("Guide0");
    }

  for(size_t i=0;i<nEllSection;i++)
    {
      // Elliptic 6m sections
      EllPipe[i]->addInsertCell(ShieldB->getCell("Void"));
      EllPipe[i]->createAll(System,*LinkPtr,2);
      
      EllFocus[i]->addInsertCell(EllPipe[i]->getCells("Void"));
      EllFocus[i]->createAll(System,*EllPipe[i],0,*EllPipe[i],0);

      LinkPtr= &EllFocus[i]->getKey("Guide0");
    }

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldB,2);

  ShieldB->addInsertCell(Cave->getCells("FrontWall"));
  ShieldB->insertObjects(System);

  CaveCut->addInsertCell(Cave->getCells("IronFront"));
  CaveCut->setFaces(Cave->getKey("Mid").getFullRule(-1),
                    Cave->getKey("Inner").getFullRule(1));
  CaveCut->createAll(System,*ShieldB,2);

  // Elliptic 6m section
  VPipeCave->addInsertCell(Cave->getCell("Void"));
  VPipeCave->createAll(System,*CaveCut,2);

  
  return; 
}


}   // NAMESPACE essSystem

