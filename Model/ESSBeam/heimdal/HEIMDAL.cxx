/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/heimdal/HEIMDAL.cxx
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
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "SingleChopper.h"

#include "HEIMDAL.h"

namespace essSystem
{

HEIMDAL::HEIMDAL(const std::string& keyName) :
  attachSystem::CopiedComp("heimdal",keyName),
  startPoint(0),stopPoint(0),
  heimdalAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  FocusTA(new beamlineSystem::PlateUnit(newName+"FTA")),
  FocusCA(new beamlineSystem::PlateUnit(newName+"FCA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),

  FocusTB(new beamlineSystem::PlateUnit(newName+"FTB")),
  FocusCB(new beamlineSystem::PlateUnit(newName+"FCB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusTC(new beamlineSystem::PlateUnit(newName+"FTC")),
  FocusCC(new beamlineSystem::PlateUnit(newName+"FCC")),

  TChopA(new essConstruct::SingleChopper(newName+"TChopA")),
  ADiskOne(new essConstruct::DiskChopper(newName+"ADiskOne")),
  ADiskTwo(new essConstruct::DiskChopper(newName+"ADiskTwo")),

  VPipeTD(new constructSystem::VacuumPipe(newName+"PipeTD")),
  FocusTD(new beamlineSystem::PlateUnit(newName+"FTD")),
  
  VPipeCD(new constructSystem::VacuumPipe(newName+"PipeCD")),
  FocusCD(new beamlineSystem::PlateUnit(newName+"FCD")),
  BendCD(new beamlineSystem::BenderUnit(newName+"BCD")),

  TChopB(new essConstruct::SingleChopper(newName+"TChopB")),
  BDisk(new essConstruct::DiskChopper(newName+"BDisk")),

  VPipeTE(new constructSystem::VacuumPipe(newName+"PipeTE")),
  FocusTE(new beamlineSystem::PlateUnit(newName+"FTE")),

  ChopperT0(new essConstruct::SingleChopper(newName+"ChopperT0")), 
  T0Disk(new essConstruct::DiskChopper(newName+"T0Disk")),

  VPipeTF(new constructSystem::VacuumPipe(newName+"PipeTF")),
  FocusTF(new beamlineSystem::PlateUnit(newName+"FTF"))
  
 /*!
    Constructor
    \param keyName :: keyname for the beamline
 */
{
  ELog::RegMethod RegA("HEIMDAL","HEIMDAL");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(heimdalAxis);

  OR.addObject(FocusTA);
  OR.addObject(FocusCA);
  OR.addObject(VPipeB);

  OR.addObject(FocusCB);
  OR.addObject(FocusTB);

  OR.addObject(VPipeC);
  OR.addObject(FocusTC);
  OR.addObject(FocusCC);

  OR.addObject(TChopA);
  OR.addObject(ADiskOne);
  OR.addObject(ADiskTwo);

  OR.addObject(VPipeTD);
  OR.addObject(FocusTD);

  OR.addObject(VPipeTE);
  OR.addObject(FocusTE);

  OR.addObject(ChopperT0);  
  OR.addObject(T0Disk);

  OR.addObject(VPipeTF);
  OR.addObject(FocusTF);

}

  
HEIMDAL::~HEIMDAL()
  /*!
    Destructor
  */
{}


void
HEIMDAL::buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& FTA,
                        const long int thermalIndex,
                        const attachSystem::FixedComp& FCA,
                        const long int coldIndex,
                        const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FTA :: FixedComp for thermal guide
    \param thermalIndex :: FixedComp link point for thermal
    \param FCA :: Fixed component for cold guide
    \param coldIndex :: FixedComp link point for cold
    \param bunkerVoid :: cell to place objects in
   */
{
  ELog::RegMethod RegA("HEIMDAL","buildBunkerUnits");

  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());

  VPipeB->addAllInsertCell(bunkerVoid);
  VPipeB->createAll(System,FTA,thermalIndex);

  // Offset from VPipeB center
  FocusTB->addInsertCell(VPipeB->getCells("Void"));
  FocusTB->createAll(System,FTA,thermalIndex);
  
  FocusCB->addInsertCell(VPipeB->getCells("Void"));
  FocusCB->createAll(System,FCA,coldIndex);
  
  ELog::EM<<"Cold = "<<FocusCB->getLinkAxis(2)
	  <<ELog::endDiag;
  ELog::EM<<"Thermal = "<<FocusTB->getLinkAxis(2)
	  <<ELog::endDiag;

  ELog::EM<<"EARLY RETURN"<<ELog::endDiag;
  return;
  
  VPipeC->addAllInsertCell(bunkerVoid);
  VPipeC->createAll(System,*VPipeB,2);

  FocusTC->addInsertCell(VPipeC->getCells("Void"));
  FocusTC->createAll(System,*FocusTB,2);

  FocusCC->addInsertCell(VPipeC->getCells("Void"));
  FocusCC->createAll(System,*FocusCB,2);

  TChopA->addInsertCell(bunkerVoid);
  TChopA->getKey("Main").setAxisControl(3,ZVert);
  TChopA->createAll(System,*FocusTC,2);

  // Double disk chopper
  ADiskOne->addInsertCell(TChopA->getCell("Void"));
  ADiskOne->createAll(System,TChopA->getKey("Main"),0);
  TChopA->insertAxle(System,*ADiskOne);
  
  ADiskTwo->addInsertCell(TChopA->getCell("Void"));
  ADiskTwo->createAll(System,TChopA->getKey("Main"),0);
  TChopA->insertAxle(System,*ADiskOne);
  TChopA->insertAxle(System,*ADiskTwo);
  
  VPipeTD->addAllInsertCell(bunkerVoid);
  VPipeTD->createAll(System,TChopA->getKey("Beam"),2);

  FocusTD->addInsertCell(VPipeTD->getCells("Void"));
  FocusTD->createAll(System,*VPipeTD,0);

  VPipeCD->addAllInsertCell(bunkerVoid);
  VPipeCD->createAll(System,*FocusCC,2);

  // First part of cold guide
  FocusCD->addInsertCell(VPipeCD->getCells("Void"));
  FocusCD->createAll(System,*FocusCC,2);

  
  TChopB->addInsertCell(bunkerVoid);
  TChopB->getKey("Main").setAxisControl(3,ZVert);
  TChopB->createAll(System,*FocusTD,2);
  
  // Double disk chopper
  BDisk->addInsertCell(TChopB->getCell("Void"));
  BDisk->createAll(System,TChopB->getKey("Main"),0);
  TChopB->insertAxle(System,*BDisk);
  
  VPipeTE->addAllInsertCell(bunkerVoid);
  VPipeTE->createAll(System,TChopB->getKey("Beam"),2);

  FocusTE->addInsertCell(VPipeTE->getCells("Void"));
  FocusTE->createAll(System,*VPipeTE,0);

  ChopperT0->addInsertCell(bunkerVoid);
  ChopperT0->createAll(System,*VPipeTE,2);

  T0Disk->addInsertCell(ChopperT0->getCell("Void"));
  T0Disk->createAll(System,ChopperT0->getKey("Main"),0,
                    ChopperT0->getKey("BuildBeam"),0);
  ChopperT0->insertAxle(System,*T0Disk);

  VPipeTF->addAllInsertCell(bunkerVoid);
  VPipeTF->createAll(System,ChopperT0->getKey("Beam"),2);

  FocusTF->addInsertCell(VPipeTF->getCells("Void"));
  FocusTF->createAll(System,*VPipeTF,0);

  return;
}

  
void
HEIMDAL::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("HEIMDAL","buildIsolated");

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  startPoint=Control.EvalDefVar<int>(newName+"StartPoint",0);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);

  ELog::EM<<"BUILD ISOLATED Start/Stop:"
          <<startPoint<<" "<<stopPoint<<ELog::endDiag;
  const attachSystem::FixedComp* FTA(&(World::masterOrigin()));
  const attachSystem::FixedComp* FCA(&(World::masterOrigin()));

  long int FCindex(0);
  long int FTindex(0);
  
  
  if (startPoint<1)
    {
      buildBunkerUnits(System,*FTA,FTindex,*FCA,FCindex,voidCell);


      //      FTA= &(FocusF->getKey("Guide0"));
      //      FTA= &(FocusF->getKey("Guide0"));
      FCindex= 2;
      FTindex= 2;
    }
  if (stopPoint==2 || stopPoint==1) return;

  if (startPoint<2)
    {
    }
  if (stopPoint==3) return;

  if (startPoint<3)
    {
    }

  if (stopPoint==4) return;      

  if (startPoint<4)
    {
    }
  
  return;
}

  
void 
HEIMDAL::build(Simulation& System,
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
  ELog::RegMethod RegA("HEIMDAL","build");

  ELog::EM<<"\nBuilding HEIMDAL on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;

  essBeamSystem::setBeamAxis(*heimdalAxis,Control,GItem,1);
    
  FocusTA->addInsertCell(GItem.getCells("Void"));
  FocusTA->setFront(GItem.getKey("Beam"),-1);
  FocusTA->setBack(GItem.getKey("Beam"),-2);
  FocusTA->createAll(System,*heimdalAxis,-3);

  FocusCA->addInsertCell(GItem.getCells("Void"));
  FocusCA->setFront(GItem.getKey("Beam"),-1);
  FocusCA->setBack(GItem.getKey("Beam"),-2);
  FocusCA->createAll(System,*heimdalAxis,-3);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,*FocusTA,2,*FocusCA,2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  // // IN WALL
  // // Make bunker insert
  // BInsert->createAll(System,FocusF->getKey("Guide0"),2,bunkerObj);
  // attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // // using 7 : mid point
  // FocusWall->addInsertCell(BInsert->getCell("Void"));
  // FocusWall->createAll(System,*BInsert,7,*BInsert,7);
  
  if (stopPoint==3) return;                      // STOP At bunker exit
  //  buildOutGuide(System,FocusWall->getKey("Guide0"),2,voidCell);

  if (stopPoint==4) return;                      // STOP At hutch
  //  buildHut(System,ChopperOutB->getKey("Beam"),2,voidCell);

  ELog::EM<<"FocusCA Axis == "<<FocusCA->getLinkPt(0)
	  <<" : "<<FocusCA->getY()<<ELog::endDiag;;
  ELog::EM<<"FocusTA Axis == "<<FocusTA->getLinkPt(0)
	  <<" : "<<FocusTA->getY()<<ELog::endDiag;;

  return;
}


}   // NAMESPACE essSystem

