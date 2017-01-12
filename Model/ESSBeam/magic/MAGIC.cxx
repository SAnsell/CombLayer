/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MAGIC.cxx
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
#include "GuideItem.h"
#include "Aperture.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "ChopperUnit.h"
#include "ChopperPit.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"
#include "MultiChannel.h"
#include "HoleShape.h"

#include "MAGIC.h"

namespace essSystem
{

MAGIC::MAGIC(const std::string& keyName) :
  attachSystem::CopiedComp("magic",keyName),stopPoint(0),
  magicAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::GuideLine(newName+"BC")),
  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  PSCDisk(new constructSystem::DiskChopper(newName+"PSCBlade")),
  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),
  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),
  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"OutFA")),
  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"OutFB")),

  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"OutFC")),

  ShieldD(new constructSystem::LineShield(newName+"ShieldD")),
  VPipeOutD(new constructSystem::VacuumPipe(newName+"PipeOutD")),
  FocusOutD(new beamlineSystem::GuideLine(newName+"OutFD")),
  
  ShieldE(new constructSystem::LineShield(newName+"ShieldE")),
  VPipeOutE(new constructSystem::VacuumPipe(newName+"PipeOutE")),
  FocusOutE(new beamlineSystem::GuideLine(newName+"OutFE")),
  
  ShieldF(new constructSystem::LineShield(newName+"ShieldF")),
  VPipeOutF(new constructSystem::VacuumPipe(newName+"PipeOutF")),
  FocusOutF(new beamlineSystem::GuideLine(newName+"OutFF")),

  PolarizerPit(new constructSystem::ChopperPit(newName+"PolarizerPit")),
  MCGuideA(new beamlineSystem::GuideLine(newName+"MCGuideA")),
  MCInsertA(new constructSystem::MultiChannel(newName+"MCA")),

  MCGuideB(new beamlineSystem::GuideLine(newName+"MCGuideB")),
  MCInsertB(new constructSystem::MultiChannel(newName+"MCB")),

  ShieldG(new constructSystem::LineShield(newName+"ShieldG")),
  VPipeOutG(new constructSystem::VacuumPipe(newName+"PipeOutG")),
  FocusOutG(new beamlineSystem::GuideLine(newName+"OutFG")),

  AppA(new constructSystem::Aperture(newName+"AppA"))

 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("MAGIC","MAGIC");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(magicAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);

  OR.addObject(VPipeC);
  OR.addObject(BendC);

  OR.addObject(ChopperA);
  OR.addObject(PSCDisk);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);
  OR.addObject(VPipeE);
  OR.addObject(FocusE);
  OR.addObject(VPipeF);
  OR.addObject(FocusF);
  OR.addObject(BInsert);
  OR.addObject(FocusWall);
  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);  
  OR.addObject(FocusOutB);

  OR.addObject(ShieldC);
  OR.addObject(VPipeOutC);  
  OR.addObject(FocusOutC);

    OR.addObject(ShieldD);
  OR.addObject(VPipeOutD);  
  OR.addObject(FocusOutD);

  OR.addObject(ShieldE);
  OR.addObject(VPipeOutE);  
  OR.addObject(FocusOutE);

  OR.addObject(ShieldF);
  OR.addObject(VPipeOutF);  
  OR.addObject(FocusOutF);

  OR.addObject(PolarizerPit);
  OR.addObject(MCGuideA);  
  OR.addObject(MCInsertA);

  OR.addObject(ShieldG);
  OR.addObject(VPipeOutG);  
  OR.addObject(FocusOutG);
  
  OR.addObject(AppA);

}

MAGIC::~MAGIC()
  /*!
    Destructor
  */
{}

void
MAGIC::setBeamAxis(const FuncDataBase& Control,
		   const GuideItem& GItem,
		   const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Database of variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("MAGIC","setBeamAxis");

  magicAxis->populate(Control);
  magicAxis->createUnitVector(GItem,0);
  magicAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  magicAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  magicAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  magicAxis->setLinkCopy(3,GItem.getKey("Beam"),1);
  
  magicAxis->linkShift(3);
  magicAxis->linkShift(4);
  magicAxis->linkAngleRotate(3);
  magicAxis->linkAngleRotate(4);

  if (reverseZ)
    magicAxis->reverseZ();
  return;
}

  
void 
MAGIC::build(Simulation& System,
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
  ELog::RegMethod RegA("MAGIC","build");

  ELog::EM<<"\nBuilding MAGIC on : "<<GItem.getKeyName()<<ELog::endDiag;

  
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
  setBeamAxis(Control,GItem,0);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*magicAxis,-3,*magicAxis,-3);
  
  if (stopPoint==1) return;                      // STOP At monolith

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,*VPipeB,2);
  
  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,0,*VPipeC,0);

  // PulseShapping Chopper [2-blades]
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperA->createAll(System,BendC->getKey("Guide0"),2);

  // Double disk chopper
  PSCDisk->addInsertCell(ChopperA->getCell("Void"));
  PSCDisk->setCentreFlag(3);  // Z direction
  PSCDisk->setOffsetFlag(1);  // Z direction
  PSCDisk->createAll(System,ChopperA->getKey("BuildBeam"),0);

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperA->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,FocusD->getKey("Guide0"),2);
  
  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,FocusE->getKey("Guide0"),2);
  
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

  
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(*BInsert,2);
  ShieldA->createAll(System,FocusWall->getKey("Shield"),2);
  
  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);
  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  ShieldB->addInsertCell(voidCell);
  ShieldB->createAll(System,*ShieldA,2);
  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,FocusOutA->getKey("Guide0"),2);
  FocusOutB->addInsertCell(VPipeOutB->getCells("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);

  
  ShieldC->addInsertCell(voidCell);
  ShieldC->createAll(System,*ShieldB,2);
  VPipeOutC->addInsertCell(ShieldC->getCell("Void"));
  VPipeOutC->createAll(System,FocusOutB->getKey("Guide0"),2);
  FocusOutC->addInsertCell(VPipeOutC->getCells("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

    
  ShieldD->addInsertCell(voidCell);
  ShieldD->createAll(System,*ShieldC,2);
  VPipeOutD->addInsertCell(ShieldD->getCell("Void"));
  VPipeOutD->createAll(System,FocusOutC->getKey("Guide0"),2);
  FocusOutD->addInsertCell(VPipeOutD->getCells("Void"));
  FocusOutD->createAll(System,*VPipeOutD,0,*VPipeOutD,0);

      
  ShieldE->addInsertCell(voidCell);
  ShieldE->createAll(System,*ShieldD,2);
  VPipeOutE->addInsertCell(ShieldE->getCell("Void"));
  VPipeOutE->createAll(System,FocusOutD->getKey("Guide0"),2);
  FocusOutE->addInsertCell(VPipeOutE->getCells("Void"));
  FocusOutE->createAll(System,*VPipeOutE,0,*VPipeOutE,0);


  PolarizerPit->addInsertCell(voidCell);
  PolarizerPit->createAll(System,*ShieldE,2);

  MCGuideA->addInsertCell(PolarizerPit->getCells("Void"));
  MCGuideA->createAll(System,*PolarizerPit,0,*PolarizerPit,0);

  // NOTE: Guide numbers links point round guide not +/- x, z
  MCInsertA->addInsertCell(MCGuideA->getCells("Guide0Void"));
  MCInsertA->setLeftRight(MCGuideA->getKey("Guide0"),4,
                          MCGuideA->getKey("Guide0"),6);
  MCInsertA->setFaces(MCGuideA->getKey("Guide0"),3,5);
  MCInsertA->createAll(System,MCGuideA->getKey("Guide0"),0);
  ELog::EM<<"Origin == "<<MCGuideA->getKey("Guide0").getSignedLinkPt(0)
	  <<ELog::endDiag;

  
  ShieldF->addInsertCell(voidCell);
  ShieldF->addInsertCell(PolarizerPit->getCells("Outer"));
  ShieldF->addInsertCell(PolarizerPit->getCells("MidLayer"));
  ShieldF->setBack(PolarizerPit->getKey("Mid"),1);
  ShieldF->createAll(System,*ShieldE,2);

  VPipeOutF->addInsertCell(ShieldE->getCell("Void"));
  VPipeOutF->addInsertCell(ShieldF->getCell("Void"));
  VPipeOutF->createAll(System,FocusOutE->getKey("Guide0"),2);
  FocusOutF->addInsertCell(VPipeOutF->getCells("Void"));
  FocusOutF->createAll(System,*VPipeOutF,0,*VPipeOutF,0);


  ShieldG->addInsertCell(voidCell);
  ShieldG->addInsertCell(PolarizerPit->getCells("Outer"));
  ShieldG->addInsertCell(PolarizerPit->getCells("MidLayer"));
  ShieldG->setFront(PolarizerPit->getKey("Mid"),2);
  ShieldG->createAll(System,*ShieldF,2);



  //  AppA->addInsertCell(bunkerObj.getCell("MainVoid"));
  //  AppA->createAll(System,*VPipeB,2);

  
  return; 
}


}   // NAMESPACE essSystem

