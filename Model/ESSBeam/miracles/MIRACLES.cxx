/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MIRACLES.cxx
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
#include "GuideItem.h"
#include "Aperture.h"
#include "TwinChopper.h"
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
#include "HoleShape.h"

#include "MIRACLES.h"

namespace essSystem
{

MIRACLES::MIRACLES(const std::string& keyName) :
  attachSystem::CopiedComp("miracles",keyName),
  nGuideSection(8),nSndSection(7),nEllSection(4),stopPoint(0),
  miraclesAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  AppA(new constructSystem::Aperture(newName+"AppA")),
  
  TwinB(new constructSystem::TwinChopper(newName+"TwinB")),
  BDiskTop(new constructSystem::DiskChopper(newName+"BBladeTop")),
  BDiskLow(new constructSystem::DiskChopper(newName+"BBladeLow")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  TwinC(new constructSystem::TwinChopper(newName+"TwinC")),
  CDiskTop(new constructSystem::DiskChopper(newName+"CBladeTop")),
  CDiskLow(new constructSystem::DiskChopper(newName+"CBladeLow")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopE(new constructSystem::ChopperUnit(newName+"ChopE")),
  EDisk(new constructSystem::DiskChopper(newName+"EBlade")),

  //  BInsert(new BunkerInsert(newName+"BInsert")),
  BInsert(new CompBInsert(newName+"CInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("MIRACLES","MIRACLES");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
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

  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);  
}

MIRACLES::~MIRACLES()
  /*!
    Destructor
  */
{}

void
MIRACLES::setBeamAxis(const FuncDataBase& Control,
                     const GuideItem& GItem,
                     const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("MIRACLES","setBeamAxis");

  miraclesAxis->populate(Control);
  miraclesAxis->createUnitVector(GItem);
  miraclesAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  miraclesAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  miraclesAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  miraclesAxis->setLinkCopy(3,GItem.getKey("Beam"),1);
  
  miraclesAxis->linkShift(3);
  miraclesAxis->linkShift(4);
  miraclesAxis->linkAngleRotate(3);
  miraclesAxis->linkAngleRotate(4);

  if (reverseZ)
    miraclesAxis->reverseZ();
  return;
}

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
  
  VPipeB->addInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  AppA->addInsertCell(VPipeC->getCells("Void"));
  AppA->createAll(System,FocusC->getKey("Guide0"),2);

  TwinB->addInsertCell(bunkerVoid);
  TwinB->createAll(System,*AppA,2);

  BDiskLow->addInsertCell(TwinB->getCell("Void"));
  BDiskLow->createAll(System,TwinB->getKey("Motor"),6,
                      TwinB->getKey("BuildBeam"),-1);

  BDiskTop->addInsertCell(TwinB->getCell("Void"));
  BDiskTop->createAll(System,TwinB->getKey("Motor"),3,
                      TwinB->getKey("BuildBeam"),-1);

  VPipeD->addInsertCell(bunkerVoid);
  VPipeD->createAll(System,TwinB->getKey("BuildBeam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  TwinC->addInsertCell(bunkerVoid);
  TwinC->createAll(System,FocusD->getKey("Guide0"),2);

  CDiskLow->addInsertCell(TwinC->getCell("Void"));
  CDiskLow->createAll(System,TwinC->getKey("Motor"),6,
                      TwinC->getKey("BuildBeam"),-1);

  CDiskTop->addInsertCell(TwinC->getCell("Void"));
  CDiskTop->createAll(System,TwinC->getKey("Motor"),3,
                      TwinC->getKey("BuildBeam"),-1);

  VPipeE->addInsertCell(bunkerVoid);
  VPipeE->createAll(System,TwinC->getKey("BuildBeam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

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
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  setBeamAxis(Control,GItem,1);
  ELog::EM<<"Beam axis == "<<miraclesAxis->getSignedLinkPt(3)<<ELog::endDiag;
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*miraclesAxis,-3,*miraclesAxis,-3); 

  
  if (stopPoint==1) return;                      // STOP At monolith


  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));


  return;

  
  return; 
}


}   // NAMESPACE essSystem

