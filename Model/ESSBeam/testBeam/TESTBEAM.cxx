/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TESTBEAM.cxx
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
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "SingleChopper.h"
#include "Motor.h"
#include "TwinBase.h"
#include "TwinChopper.h"
#include "Cryostat.h"

#include "TESTBEAM.h"

namespace essSystem
{

TESTBEAM::TESTBEAM(const std::string& keyName) :
  attachSystem::CopiedComp("testBeam",keyName),
  startPoint(0),stopPoint(0),
  testAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  
  TwinA(new constructSystem::TwinChopper(newName+"TwinA")),
  ADisk(new constructSystem::DiskChopper(newName+"BladeA")),
  BDisk(new constructSystem::DiskChopper(newName+"BladeB")),

  ChopperT0(new constructSystem::SingleChopper(newName+"ChopperT0")), 
  T0Disk(new constructSystem::DiskChopper(newName+"T0Disk")),
  T0Motor(new constructSystem::Motor(newName+"T0Motor")),
  CryoA(new constructSystem::Cryostat(newName+"CryoA"))
  /*!
    Constructor
    \param keyName :: keyname to process
  */
{
  ELog::RegMethod RegA("TESTBEAM","TESTBEAM");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(testAxis);

  OR.addObject(FocusA);
  OR.addObject(TwinA);
  
  OR.addObject(ADisk);
  OR.addObject(BDisk);

  OR.addObject(ChopperT0);  
  OR.addObject(T0Disk);
  OR.addObject(T0Motor);

  OR.addObject(CryoA);

}
  
TESTBEAM::~TESTBEAM()
  /*!
    Destructor
  */
{}


void
TESTBEAM::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("TESTBEAM","buildBunkerUnits");

  ChopperT0->addInsertCell(bunkerVoid);
  ChopperT0->createAll(System,FA,startIndex);

  T0Disk->addInsertCell(ChopperT0->getCell("Void"));
  T0Disk->createAll(System,ChopperT0->getKey("Main"),0,
                    ChopperT0->getKey("BuildBeam"),0);

  //  T0Motor->addInsertCell(bunkerVoid);
  //  T0Motor->createAll(System,ChopperT0->getKey("Main"),1);
  return;
  CryoA->addInsertCell(bunkerVoid);
  CryoA->createAll(System,FA,startIndex);

    
  TwinA->addInsertCell(bunkerVoid);
  TwinA->createAll(System,FA,startIndex);

  ADisk->addInsertCell(TwinA->getCell("Void"));
  ADisk->createAll(System,TwinA->getKey("MotorTop"),0,
                   TwinA->getKey("Beam"),-1);
  TwinA->insertAxle(System,*ADisk,attachSystem::CellMap());
  //  DiskA->createAll(System,
  return;
}
  
void
TESTBEAM::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("TESTBEAM","buildIsolated");


  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  startPoint=Control.EvalDefVar<int>(newName+"StartPoint",0);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"BUILD ISOLATED Start/Stop:"
          <<startPoint<<" "<<stopPoint<<ELog::endDiag;
  const attachSystem::FixedComp* FStart(&(World::masterOrigin()));
  long int startIndex(0);
  
  if (startPoint<1)
    {
      buildBunkerUnits(System,*FStart,startIndex,voidCell);
      // Set the start point fo rb
      //      FStart= &(FocusF->getKey("Guide0"));
      startIndex= 2;
    }
  
  return;
}

  
void 
TESTBEAM::build(Simulation& System,
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
  ELog::RegMethod RegA("TESTBEAM","build");

  ELog::EM<<"\nBuilding TESTBEAM on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;

  essBeamSystem::setBeamAxis(*testAxis,Control,GItem,1);
    
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*testAxis,-3,*testAxis,-3);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  
  return;
}


}   // NAMESPACE essSystem

