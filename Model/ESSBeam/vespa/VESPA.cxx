/********************************************************************* 
  CombLayer : MCNP(X) Input builder

 * File:   ESSBeam/vespa/VESPA.cxx
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
#include "World.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "SingleChopper.h"
#include "TwinBase.h"
#include "TwinChopperFlat.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "LineShield.h"
#include "TriangleShield.h"
#include "HoleShape.h"
#include "JawSet.h"
#include "VespaHut.h"
#include "VespaInner.h"
#include "CylSample.h"
#include "CrystalMount.h"
#include "TubeDetBox.h"
#include "Cryostat.h"

#include "VESPA.h"

namespace essSystem
{

VESPA::VESPA(const std::string& keyName) :
  attachSystem::CopiedComp("vespa",keyName),
  cryoFlag(1),startPoint(0),stopPoint(0),
  vespaAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),
  
  // Guide into the monolith
  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),

  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusB(new beamlineSystem::PlateUnit(newName+"FB")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusC(new beamlineSystem::PlateUnit(newName+"FC")),

  TwinChopperA(new essConstruct::TwinChopperFlat(newName+"TwinChopperA")),
  PSCDiskTopA(new essConstruct::DiskChopper(newName+"PSCTopBladeA")),
  PSCDiskBottomA(new essConstruct::DiskChopper(newName+"PSCBottomBladeA")),

  // Joining Pipe AB
  JPipeAB(new constructSystem::VacuumPipe(newName+"JoinPipeAB")),
  FocusD(new beamlineSystem::PlateUnit(newName+"FD")),

  TwinChopperB(new essConstruct::TwinChopperFlat(newName+"TwinChopperB")),
  PSCDiskTopB(new essConstruct::DiskChopper(newName+"PSCTopBladeB")),
  PSCDiskBottomB(new essConstruct::DiskChopper(newName+"PSCBottomBladeB")),

  // Joining Pipe AB
  JPipeBC(new constructSystem::VacuumPipe(newName+"JoinPipeBC")),
  FocusE(new beamlineSystem::PlateUnit(newName+"FE")),

  TwinChopperC(new essConstruct::TwinChopperFlat(newName+"TwinChopperC")),
  PSCDiskTopC(new essConstruct::DiskChopper(newName+"PSCTopBladeC")),
  PSCDiskBottomC(new essConstruct::DiskChopper(newName+"PSCBottomBladeC")),

  // Joining Pipe C to outer
  JPipeCOut(new constructSystem::VacuumPipe(newName+"JoinPipeCOut")),
  FocusF(new beamlineSystem::PlateUnit(newName+"FF")),

  VPipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  FocusG(new beamlineSystem::PlateUnit(newName+"FG")),

  // FOC
  ChopperFOC(new essConstruct::SingleChopper(newName+"ChopperFOC")),
  FOCDisk(new essConstruct::DiskChopper(newName+"FOCBlade")),

  VPipeH(new constructSystem::VacuumPipe(newName+"PipeH")),
  FocusH(new beamlineSystem::PlateUnit(newName+"FH")),

  
  BInsert(new BunkerInsert(newName+"BInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),  
  FocusWall(new beamlineSystem::PlateUnit(newName+"FWall")),

  OutPitT0(new constructSystem::ChopperPit(newName+"OutPitT0")),
  ChopperT0(new essConstruct::SingleChopper(newName+"ChopperT0")),
  T0Disk(new essConstruct::DiskChopper(newName+"T0Disk")),
  T0ExitPort(new constructSystem::HoleShape(newName+"T0ExitPort")),
  
  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  ShieldA(new constructSystem::TriangleShield(newName+"ShieldA")),
  
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::PlateUnit(newName+"FOutA")),
  ChopperOutA(new essConstruct::SingleChopper(newName+"ChopperOutA")),

  OutPitB(new constructSystem::ChopperPit(newName+"OutPitB")),
  PitBPortA(new constructSystem::HoleShape(newName+"PitBPortA")),
  PitBPortB(new constructSystem::HoleShape(newName+"PitBPortB")),
  
  FOCDiskB(new essConstruct::DiskChopper(newName+"FOCBladeB")),
  ShieldB(new constructSystem::TriangleShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::PlateUnit(newName+"FOutB")),
  
  ChopperOutB(new essConstruct::SingleChopper(newName+"ChopperOutB")),
  FOCDiskOutB(new essConstruct::DiskChopper(newName+"FOCBladeOutB")),
  
  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::PlateUnit(newName+"FOutC")),
  Cave(new VespaHut(newName+"Cave")),
  VInner(new VespaInner(newName+"Inner")),
  VInnerExit(new constructSystem::HoleShape(newName+"InnerExit")),
  
  VJaws(new constructSystem::JawSet(newName+"VJaws")),
  Sample(new instrumentSystem::CylSample(newName+"Sample")),
  Cryo(new constructSystem::Cryostat(newName+"Cryo"))
 /*!
    Constructor
    \param keyName :: Head name for the instrument
 */
{
  ELog::RegMethod RegA("VESPA","VESPA");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.addObject(vespaAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeA);
  OR.addObject(FocusB);

  OR.addObject(TwinChopperA);
  OR.addObject(PSCDiskTopA);
  OR.addObject(PSCDiskBottomA);

  OR.addObject(JPipeAB);
  OR.addObject(FocusD);
  
  OR.addObject(TwinChopperB);
  OR.addObject(PSCDiskTopB);
  OR.addObject(PSCDiskBottomB);

  OR.addObject(JPipeBC);
  OR.addObject(FocusE);

  OR.addObject(TwinChopperC);
  OR.addObject(PSCDiskTopC);
  OR.addObject(PSCDiskBottomC);

  
  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);
  
  OR.addObject(OutPitT0);
  OR.addObject(ChopperT0);
  OR.addObject(T0Disk);
  OR.addObject(T0ExitPort);

  OR.addObject(OutPitA);
  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(ChopperOutA);
  OR.addObject(OutPitB);
  OR.addObject(PitBPortA);
  OR.addObject(PitBPortB);
  
  OR.addObject(FOCDiskB);
  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);

  for(size_t i=0;i<4;i++)
    {
      typedef std::shared_ptr<constructSystem::TriangleShield> STYPE;
      typedef std::shared_ptr<constructSystem::VacuumPipe> VTYPE;
      typedef std::shared_ptr<beamlineSystem::PlateUnit> GTYPE;
      
      ShieldArray.push_back
        (STYPE(new constructSystem::TriangleShield(newName+"ShieldArray"+
						   std::to_string(i))));
      VPipeArray.push_back
        (VTYPE(new constructSystem::VacuumPipe(newName+"VPipeArray"+
                                               std::to_string(i))));
      FocusArray.push_back
        (GTYPE(new beamlineSystem::PlateUnit(newName+"FocusArray"+
                                             std::to_string(i))));
      OR.addObject(ShieldArray.back());
      OR.addObject(VPipeArray.back());
      OR.addObject(FocusArray.back());
    }
  
  OR.addObject(ChopperOutB);
  OR.addObject(FOCDiskOutB);

  OR.addObject(ShieldC);
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);
  
  OR.addObject(Cave);
  OR.addObject(VInner);
  OR.addObject(VInnerExit);
  OR.addObject(VJaws);
  OR.addObject(Sample);
  OR.addObject(Cryo);
}

  
VESPA::~VESPA()
  /*!
    Destructor
  */
{}

void
VESPA::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("VESPA","buildBunkerUnits");
  
  VPipeA->addAllInsertCell(bunkerVoid);
  VPipeA->createAll(System,FA,startIndex);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->createAll(System,*VPipeA,0);

  VPipeB->addAllInsertCell(bunkerVoid);
  VPipeB->createAll(System,*FocusB,2);

  FocusC->addInsertCell(VPipeB->getCells("Void"));
  FocusC->createAll(System,*VPipeB,0);

  //PSC-A
  TwinChopperA->addInsertCell(bunkerVoid);
  TwinChopperA->createAll(System,*FocusC,2);
  PSCDiskTopA->addInsertCell(TwinChopperA->getCell("Void"));
  PSCDiskTopA->createAll(System,TwinChopperA->getKey("MotorTop"),0);
  PSCDiskBottomA->addInsertCell(TwinChopperA->getCell("Void"));
  PSCDiskBottomA->createAll(System,TwinChopperA->getKey("MotorBase"),0);
  TwinChopperA->insertAxle(System,*PSCDiskBottomA,*PSCDiskTopA);

    //PSC-B
  TwinChopperB->addInsertCell(bunkerVoid);
  TwinChopperB->createAll(System,*FocusC,2);
  PSCDiskTopB->addInsertCell(TwinChopperB->getCell("Void"));
  PSCDiskTopB->createAll(System,TwinChopperB->getKey("MotorTop"),0);
  PSCDiskBottomB->addInsertCell(TwinChopperB->getCell("Void"));
  PSCDiskBottomB->createAll(System,TwinChopperB->getKey("MotorBase"),0);
  TwinChopperB->insertAxle(System,*PSCDiskBottomB,*PSCDiskTopB);

  //PSC-C
  TwinChopperC->addInsertCell(bunkerVoid);
  TwinChopperC->createAll(System,*FocusC,2);
  PSCDiskTopC->addInsertCell(TwinChopperC->getCell("Void"));
  PSCDiskTopC->createAll(System,TwinChopperC->getKey("MotorTop"),0);
  PSCDiskBottomC->addInsertCell(TwinChopperC->getCell("Void"));
  PSCDiskBottomC->createAll(System,TwinChopperC->getKey("MotorBase"),0);
  TwinChopperC->insertAxle(System,*PSCDiskBottomC,*PSCDiskTopC);


  // JPipeAB
  JPipeAB->addAllInsertCell(bunkerVoid);
  JPipeAB->setFront(TwinChopperA->getKey("Main"),2);
  JPipeAB->setBack(TwinChopperB->getKey("Main"),1);  
  JPipeAB->createAll(System, TwinChopperA->getKey("Main"),2);
  FocusD->addInsertCell(JPipeAB->getCells("Void"));
  FocusD->createAll(System,*JPipeAB,7);

  // JPipeBC
  JPipeBC->addAllInsertCell(bunkerVoid);
  JPipeBC->setFront(TwinChopperB->getKey("Main"),2);
  JPipeBC->setBack(TwinChopperC->getKey("Main"),1);
  JPipeBC->createAll(System,TwinChopperB->getKey("Main"),2);
  FocusE->addInsertCell(JPipeBC->getCells("Void"));
  FocusE->createAll(System,*JPipeBC,7);

    // JPipeCOut
  JPipeCOut->addAllInsertCell(bunkerVoid);
  JPipeCOut->setFront(TwinChopperC->getKey("Main"),2);
  JPipeCOut->createAll(System,TwinChopperC->getKey("Main"),2);
  FocusF->addInsertCell(JPipeCOut->getCells("Void"));
  FocusF->createAll(System,*JPipeCOut,7);

  VPipeG->addAllInsertCell(bunkerVoid);
  VPipeG->createAll(System,*FocusF,2);
  FocusG->addInsertCell(VPipeG->getCells("Void"));
  FocusG->createAll(System,*VPipeG,0);

  // FO-Chopper
  ChopperFOC->addInsertCell(bunkerVoid);
  ChopperFOC->createAll(System,*FocusG,2);
  FOCDisk->addInsertCell(ChopperFOC->getCell("Void"));
  FOCDisk->createAll(System,ChopperFOC->getKey("Main"),0);
  ChopperFOC->insertAxle(System,*FOCDisk);

  VPipeH->addAllInsertCell(bunkerVoid);
  VPipeH->createAll(System,ChopperFOC->getKey("Beam"),2);
  FocusH->addInsertCell(VPipeH->getCells("Void"));
  FocusH->createAll(System,*VPipeH,0);

  return;
}

void
VESPA::buildOutGuide(Simulation& System,
                     const attachSystem::FixedComp& FW,
                     const long int startIndex,
                     const int voidCell)
  /*!
    Build all the components that are outside of the wall
    \param System :: Simulation 
    \param FW :: Focus wall fixed axis
    \param startIndex :: link point 
    \param voidCell :: void cell nubmer
   */
{
  ELog::RegMethod RegA("VESPA","buildOutGuide");
  //
  // OUTSIDE:
  //  added before:
  //    OutPitT0->addFrontWall(bunkerObj,2);
  //
	
  OutPitT0->addInsertCell(voidCell);
  OutPitT0->createAll(System,FW,startIndex);

  // First Chopper
  ChopperT0->addInsertCell(OutPitT0->getCells("Void"));
  ChopperT0->createAll(System,FW,startIndex);

  // Double disk chopper
  T0Disk->addInsertCell(ChopperT0->getCell("Void"));
  T0Disk->setOffsetFlag(1);  // Z direction
  T0Disk->createAll(System,ChopperT0->getKey("Main"),0);
  ChopperT0->insertAxle(System,*T0Disk);
  
  T0ExitPort->addInsertCell(OutPitT0->getCells("MidLayerBack"));
  T0ExitPort->addInsertCell(OutPitT0->getCells("Collet"));
  T0ExitPort->setCutSurf("front",OutPitT0->getKey("Inner"),2);
  T0ExitPort->setCutSurf("back",OutPitT0->getKey("Mid"),-2);
  T0ExitPort->createAll(System,OutPitT0->getKey("Inner"),2);

  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,*FocusWall,2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitT0->getCells("Outer"));
  ShieldA->addInsertCell(OutPitT0->getCells("MidLayer"));

  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer")); 

  ShieldA->setFront(OutPitT0->getKey("Mid"),2);
  ShieldA->setBack(OutPitA->getKey("Mid"),1);

  ShieldA->createAll(System,*FocusWall,2);

  // Elliptic 6m section
  VPipeOutA->addAllInsertCell(ShieldA->getCells("Void"));
  VPipeOutA->setFront(OutPitT0->getKey("Mid"),2);
  VPipeOutA->setBack(OutPitA->getKey("Inner"),1);
  VPipeOutA->addAllInsertCell(OutPitA->getCells("MidLayer"));
  VPipeOutA->createAll(System,*FocusWall,2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0);

  // First Chopper
  ChopperOutA->addInsertCell(OutPitA->getCells("Void"));
  ChopperOutA->createAll(System,*FocusOutA,2);

  // Double disk chopper
  FOCDiskB->addInsertCell(ChopperOutA->getCell("Void"));
  FOCDiskB->createAll(System,ChopperOutA->getKey("Main"),0);
  ChopperOutA->insertAxle(System,*FOCDiskB); 

  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->addInsertCell(voidCell);
  ShieldB->setFront(OutPitA->getKey("Mid"),2);
  ShieldB->createAll(System,ChopperOutA->getKey("Beam"),2);

  VPipeOutB->addAllInsertCell(ShieldB->getCells("Void"));
  VPipeOutB->addAllInsertCell(OutPitA->getCells("Collet"));
  VPipeOutB->addAllInsertCell(OutPitA->getCells("MidLayer"));
  VPipeOutB->setFront(OutPitA->getKey("Inner"),2);
  VPipeOutB->createAll(System,ChopperOutA->getKey("Beam"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,-1);


  // Mid section array:
  ShieldArray[0]->addInsertCell(voidCell);
  ShieldArray[0]->setFront(*ShieldB,2);
  ShieldArray[0]->createAll(System,*ShieldB,2);
  VPipeArray[0]->addAllInsertCell(ShieldArray[0]->getCells("Void"));
  VPipeArray[0]->createAll(System,*ShieldB,2);
  FocusArray[0]->addInsertCell(VPipeArray[0]->getCell("Void"));
  FocusArray[0]->createAll(System,*VPipeArray[0],7);

  OutPitB->addInsertCell(voidCell);
  OutPitB->createAll(System,OutPitA->getKey("Inner"),2);
  
  PitBPortA->addInsertCell(OutPitB->getCells("MidLayerFront"));
  PitBPortA->setCutSurf("front",OutPitB->getKey("Inner"),1);
  PitBPortA->setCutSurf("back",OutPitB->getKey("Mid"),-1);
  PitBPortA->createAll(System,OutPitB->getKey("Inner"),2);

  PitBPortB->addInsertCell(OutPitB->getCells("MidLayerBack"));
  PitBPortB->addInsertCell(OutPitB->getCells("Collet"));
  PitBPortB->setCutSurf("front",OutPitB->getKey("Inner"),2);
  PitBPortB->setCutSurf("back",OutPitB->getKey("Mid"),-2);
  PitBPortB->createAll(System,OutPitB->getKey("Inner"),2);
  
  const size_t lastIndex(ShieldArray.size()-1);
  for(size_t i=1;i<ShieldArray.size();i++)
    {	  
      ShieldArray[i]->addInsertCell(voidCell);
      ShieldArray[i]->setFront(*ShieldArray[i-1],2);
      if (i+1==ShieldArray.size())
        {
          ShieldArray[i]->addInsertCell(OutPitB->getCells("Outer"));
          ShieldArray[i]->addInsertCell(OutPitB->getCells("MidLayer"));
          ShieldArray[i]->setBack(OutPitB->getKey("Mid"),1);
        }
      ShieldArray[i]->createAll(System,*ShieldArray[i-1],2);
      VPipeArray[i]->addAllInsertCell(ShieldArray[i]->getCells("Void"));
      VPipeArray[i]->createAll(System,*ShieldArray[i-1],2);
      FocusArray[i]->addInsertCell(VPipeArray[i]->getCell("Void"));
      FocusArray[i]->createAll(System,*VPipeArray[i],7);
    }

    // Outer Chopper
  ChopperOutB->addInsertCell(OutPitB->getCells("Void"));
  ChopperOutB->createAll(System,*FocusArray[lastIndex],2);

  // Double disk chopper FOC
  FOCDiskOutB->addInsertCell(ChopperOutB->getCell("Void"));
  FOCDiskOutB->createAll(System,ChopperOutB->getKey("Main"),0);
  ChopperOutB->insertAxle(System,*FOCDiskOutB);
  
  return;
}

void
VESPA::buildHut(Simulation& System,
		const attachSystem::FixedComp& connectFC,
		const long int connectIndex,
                const int voidCell)
  /*!
    Builds the hut connected to the FixedPoint given
    \param System :: Simulation to build with
    \param connectFC :: Connection point
    \param connectIndex :: Connection index
    \param voidCell :: Main void cell for this model
   */
{
  ELog::RegMethod RegA("VESPA","buildHut");

  // check if Previously built :
  if (OutPitB->hasItem("Outer"))
    {
      ShieldC->addInsertCell(OutPitB->getCells("Outer"));
      ShieldC->setFront(OutPitB->getKey("Mid"),2);
    }
  ShieldC->addInsertCell(voidCell);
  ShieldC->createAll(System,connectFC,connectIndex);

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldC,2);

  VInner->setInsertCell(Cave->getCell("Void"));
  VInner->setFront(Cave->getKey("Inner"),-1);
  VInner->createAll(System,Cave->getKey("Inner"),-1);

  VInnerExit->addInsertCell(VInner->getCells("FeLayer"));
  VInnerExit->addInsertCell(VInner->getCells("ConcLayer"));
  VInnerExit->setCutSurf("front",VInner->getKey("Inner"),2);
  VInnerExit->setCutSurf("back",VInner->getKey("Outer"),-2);
  VInnerExit->createAll(System,VInner->getKey("Inner"),2);

  ShieldC->addInsertCell(Cave->getCells("FrontWall"));
  ShieldC->insertObjects(System);

  VPipeOutC->addAllInsertCell(Cave->getCells("FrontWall"));
  VPipeOutC->addAllInsertCell(Cave->getCells("Void"));
  VPipeOutC->addAllInsertCell(VInner->getCells("Void"));
  VPipeOutC->addAllInsertCell(ShieldC->getCells("Void"));
  VPipeOutC->createAll(System,connectFC,connectIndex);

  FocusOutC->addInsertCell(VPipeOutC->getCell("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0);
  
  VJaws->setInsertCell(VInner->getCell("Void"));
  VJaws->createAll(System,*ShieldC,2);

  if (cryoFlag)
    {
      Cryo->setInsertCell(Cave->getCell("Void"));
      Cryo->createAll(System,*VJaws,2);
    }

  Sample->setInsertCell(Cryo->getCell("SampleVoid"));
  Sample->createAll(System,*Cryo,0);

  return;
}

void
VESPA::buildDetectorArray(Simulation& System,
                          const attachSystem::FixedComp& sampleFC,
                          const long int sampleIndex,
                          const int voidCell)
  /*!
    Builds the detector array in the cave (relative to the sample)
    \param System :: Simulation to build with
    \param sampleFC :: Sample (centre) fixed object
    \param sampleIndex :: Index for the sample
    \param voidCell :: Cell everything is in
  */
{
  ELog::RegMethod RegA("VESPA","buildDetectorArray");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
    
  const FuncDataBase& Control=System.getDataBase();
  const size_t nDet=Control.EvalVar<size_t>(newName+"NDet");

  std::set<size_t> deactivateDet;
  if (cryoFlag)
    deactivateDet=std::set<size_t>{6,22};

  typedef std::shared_ptr<constructSystem::CrystalMount> XTYPE;
  typedef std::shared_ptr<constructSystem::TubeDetBox> DTYPE;
  for(size_t i=0;i<nDet;i++)
    {
      if (deactivateDet.find(i)==deactivateDet.end())
	{
	  XTYPE xsPtr(new constructSystem::CrystalMount(newName+"XStal",i));
	  DTYPE dsPtr(new constructSystem::TubeDetBox(newName+"DBox",i));
	  OR.addObject(xsPtr);
	  OR.addObject(dsPtr);
	  
	  xsPtr->addInsertCell(voidCell);
	  xsPtr->createAll(System,sampleFC,sampleIndex);
	  
	  dsPtr->addInsertCell(voidCell);
	  dsPtr->createAll(System,*xsPtr,8);
	  
	  XStalArray.push_back(xsPtr);
	  ADetArray.push_back(dsPtr);
	}
    }
  return;

}
    
void
VESPA::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("VESPA","buildIsolated");


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
      FStart= FocusH.get();
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
      startIndex=2;
      OutPitT0->addFrontWall(*VPipeWall,2);
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
      buildHut(System,*FStart,startIndex,voidCell);
      buildDetectorArray(System,*Sample,0,Cave->getCell("Void"));
    }
  
  return;
}

  
void 
VESPA::build(Simulation& System,
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
  ELog::RegMethod RegA("VESPA","build");

  ELog::EM<<"\nBuilding VESPA on : "<<GItem.getKeyName()
	  <<" Bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;

  essBeamSystem::setBeamAxis(*vespaAxis,Control,GItem,0);
    
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*vespaAxis,-3);
  
  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,*FocusA,2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  // IN WALL
  // Make bunker insert
  BInsert->setCutSurf("front",bunkerObj,-1);
  BInsert->setCutSurf("back",bunkerObj,-2);
  BInsert->createAll(System,*FocusH,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7);
  OutPitT0->addFrontWall(bunkerObj,2);

  if (stopPoint==3) return;                      // STOP At bunker exit
  buildOutGuide(System,*FocusWall,2,voidCell);

  if (stopPoint==4) return;                      // STOP At hutch
  buildHut(System,ChopperOutB->getKey("Beam"),2,voidCell);
  buildDetectorArray(System,*Sample,0,Cave->getCell("Void"));
  return;
}

}   // NAMESPACE essSystem

