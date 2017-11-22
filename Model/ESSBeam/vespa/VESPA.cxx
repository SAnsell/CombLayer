/********************************************************************* 
  CombLayer : MCNP(X) Input builder

 * File:   ESSBeam/vespa/VESPA.cxx
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
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "SingleChopper.h"
#include "TwinBase.h"
#include "TwinChopper.h"
#include "TwinChopperFlat.h"
#include "DetectorTank.h"
#include "LineShield.h"
#include "TriangleShield.h"
#include "Jaws.h"
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
  startPoint(0),stopPoint(0),
  vespaAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),
  
  ChopperA(new constructSystem::SingleChopper(newName+"ChopperA")),
  WFMDiskA(new constructSystem::DiskChopper(newName+"WFMBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperB(new constructSystem::SingleChopper(newName+"ChopperB")), 
  WFMDiskB(new constructSystem::DiskChopper(newName+"WFMBladeB")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  ChopperC(new constructSystem::SingleChopper(newName+"ChopperC")),
  WFMDiskC(new constructSystem::DiskChopper(newName+"WFMBladeC")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperD(new constructSystem::SingleChopper(newName+"ChopperD")),
  FOCDiskA(new constructSystem::DiskChopper(newName+"FOCBladeA")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  BInsert(new BunkerInsert(newName+"BInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),  
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  OutPitT0(new constructSystem::ChopperPit(newName+"OutPitT0")),
  ChopperT0(new constructSystem::SingleChopper(newName+"ChopperT0")),
  T0Disk(new constructSystem::DiskChopper(newName+"T0Disk")),
  T0ExitPort(new constructSystem::HoleShape(newName+"T0ExitPort")),
  
  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  ShieldA(new constructSystem::TriangleShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),
  ChopperOutA(new constructSystem::SingleChopper(newName+"ChopperOutA")),

  OutPitB(new constructSystem::ChopperPit(newName+"OutPitB")),
  PitBPortA(new constructSystem::HoleShape(newName+"PitBPortA")),
  PitBPortB(new constructSystem::HoleShape(newName+"PitBPortB")),
  
  FOCDiskB(new constructSystem::DiskChopper(newName+"FOCBladeB")),
  ShieldB(new constructSystem::TriangleShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),
  
  ChopperOutB(new constructSystem::SingleChopper(newName+"ChopperOutB")),
  FOCDiskOutB(new constructSystem::DiskChopper(newName+"FOCBladeOutB")),
  
  ShieldC(new constructSystem::LineShield(newName+"ShieldC")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"FOutC")),
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
  OR.cell(newName+"Axis");
  OR.addObject(vespaAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeA);
  OR.addObject(FocusB);

  OR.addObject(ChopperA);
  OR.addObject(WFMDiskA);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(ChopperB);
  OR.addObject(WFMDiskB);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(ChopperC);
  OR.addObject(WFMDiskC);

  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(ChopperD);
  OR.addObject(FOCDiskA);

  OR.addObject(VPipeF);
  OR.addObject(FocusF);
  
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
      typedef std::shared_ptr<beamlineSystem::GuideLine> GTYPE;
      
      ShieldArray.push_back
        (STYPE(new constructSystem::TriangleShield(newName+"ShieldArray"+
						   StrFunc::makeString(i))));
      VPipeArray.push_back
        (VTYPE(new constructSystem::VacuumPipe(newName+"VPipeArray"+
                                               StrFunc::makeString(i))));
      FocusArray.push_back
        (GTYPE(new beamlineSystem::GuideLine(newName+"FocusArray"+
                                             StrFunc::makeString(i))));
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
  
  VPipeA->addInsertCell(bunkerVoid);
  VPipeA->createAll(System,FA,startIndex);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->createAll(System,*VPipeA,0,*VPipeA,0);

  // First Chopper
  ChopperA->addInsertCell(bunkerVoid);
  ChopperA->createAll(System,FocusB->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskA->addInsertCell(ChopperA->getCell("Void"));
  WFMDiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*WFMDiskA);
  
  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);


  // First Chopper
  ChopperB->addInsertCell(bunkerVoid);
  ChopperB->createAll(System,FocusC->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskB->addInsertCell(ChopperB->getCell("Void"));
  WFMDiskB->createAll(System,ChopperB->getKey("Main"),0);
  ChopperB->insertAxle(System,*WFMDiskB);
  
  VPipeD->addInsertCell(bunkerVoid);
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // First Chopper
  ChopperC->addInsertCell(bunkerVoid);
  ChopperC->createAll(System,FocusD->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskC->addInsertCell(ChopperC->getCell("Void"));
  WFMDiskC->createAll(System,ChopperC->getKey("Main"),0);
  ChopperC->insertAxle(System,*WFMDiskC);
  
  VPipeE->addInsertCell(bunkerVoid);
  VPipeE->createAll(System,ChopperC->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // 10m Chopper
  ChopperD->addInsertCell(bunkerVoid);
  ChopperD->createAll(System,FocusE->getKey("Guide0"),2);

  // Double disk chopper
  FOCDiskA->addInsertCell(ChopperD->getCell("Void"));
  FOCDiskA->createAll(System,ChopperD->getKey("Main"),0);
  ChopperD->insertAxle(System,*FOCDiskA);
  
  VPipeF->addInsertCell(bunkerVoid);
  VPipeF->createAll(System,ChopperD->getKey("Beam"),2);

  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

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
    \param startPoint :: link point 
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
  T0ExitPort->setFaces(OutPitT0->getKey("Inner").getFullRule(2),
                       OutPitT0->getKey("Mid").getFullRule(-2));
  T0ExitPort->createAll(System,OutPitT0->getKey("Inner"),2);

  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,FocusWall->getKey("Shield"),2);
  
  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitT0->getCells("Outer"));
  ShieldA->addInsertCell(OutPitT0->getCells("MidLayer"));
  ShieldA->setFront(OutPitT0->getKey("Mid"),2);
  ShieldA->addInsertCell(OutPitA->getCells("Outer"));
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->setBack(OutPitA->getKey("Mid"),1);
  ShieldA->createAll(System,FocusWall->getKey("Shield"),2);

  // Elliptic 6m section
  VPipeOutA->addInsertCell(ShieldA->getCells("Void"));
  VPipeOutA->setBack(OutPitA->getKey("Inner"),1);
  VPipeOutA->addInsertCell(OutPitA->getCells("MidLayer"));
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  // First Chopper
  ChopperOutA->addInsertCell(OutPitA->getCells("Void"));
  ChopperOutA->createAll(System,FocusOutA->getKey("Guide0"),2);

  // Double disk chopper
  FOCDiskB->addInsertCell(ChopperOutA->getCell("Void"));
  FOCDiskB->createAll(System,ChopperOutA->getKey("Main"),0);
  ChopperOutA->insertAxle(System,*FOCDiskB); 
 
  ShieldB->addInsertCell(OutPitA->getCells("Outer"));
  ShieldB->addInsertCell(voidCell);
  ShieldB->setFront(OutPitA->getKey("Mid"),2);
  ShieldB->createAll(System,ChopperOutA->getKey("Beam"),2);

  VPipeOutB->addInsertCell(ShieldB->getCells("Void"));
  VPipeOutB->addInsertCell(OutPitA->getCells("Collet"));
  VPipeOutB->addInsertCell(OutPitA->getCells("MidLayer"));
  VPipeOutB->setFront(OutPitA->getKey("Inner"),2);
  VPipeOutB->createAll(System,ChopperOutA->getKey("Beam"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,-1,*VPipeOutB,-1);

  // Mid section array:
  ShieldArray[0]->addInsertCell(voidCell);
  ShieldArray[0]->setFront(*ShieldB,2);
  ShieldArray[0]->createAll(System,*ShieldB,2);
  VPipeArray[0]->addInsertCell(ShieldArray[0]->getCells("Void"));
  VPipeArray[0]->createAll(System,*ShieldB,2);
  FocusArray[0]->addInsertCell(VPipeArray[0]->getCell("Void"));
  FocusArray[0]->createAll(System,*VPipeArray[0],7,*VPipeArray[0],7);

  OutPitB->addInsertCell(voidCell);
  OutPitB->createAll(System,OutPitA->getKey("Inner"),2);
  
  PitBPortA->addInsertCell(OutPitB->getCells("MidLayerFront"));
  PitBPortA->setFaces(OutPitB->getKey("Inner").getFullRule(1),
                       OutPitB->getKey("Mid").getFullRule(-1));
  PitBPortA->createAll(System,OutPitB->getKey("Inner"),2);

  PitBPortB->addInsertCell(OutPitB->getCells("MidLayerBack"));
  PitBPortB->addInsertCell(OutPitB->getCells("Collet"));
  PitBPortB->setFaces(OutPitB->getKey("Inner").getFullRule(2),
                      OutPitB->getKey("Mid").getFullRule(-2));
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
      VPipeArray[i]->addInsertCell(ShieldArray[i]->getCells("Void"));
      VPipeArray[i]->createAll(System,*ShieldArray[i-1],2);
      FocusArray[i]->addInsertCell(VPipeArray[i]->getCell("Void"));
      FocusArray[i]->createAll(System,*VPipeArray[i],7,*VPipeArray[i],7);
    }

    // First Chopper
  ChopperOutB->addInsertCell(OutPitB->getCells("Void"));
  ChopperOutB->createAll(System,FocusArray[lastIndex]->getKey("Guide0"),2);

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
  VInnerExit->setFaces(VInner->getKey("Inner").getFullRule(2),
                       VInner->getKey("Outer").getFullRule(-2));
  VInnerExit->createAll(System,VInner->getKey("Inner"),2);

  ShieldC->addInsertCell(Cave->getCells("FrontWall"));
  ShieldC->insertObjects(System);

  VPipeOutC->addInsertCell(Cave->getCells("FrontWall"));
  VPipeOutC->addInsertCell(Cave->getCells("Void"));
  VPipeOutC->addInsertCell(VInner->getCells("Void"));
  VPipeOutC->addInsertCell(ShieldC->getCells("Void"));
  VPipeOutC->createAll(System,connectFC,connectIndex);

  FocusOutC->addInsertCell(VPipeOutC->getCell("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  
  VJaws->setInsertCell(VInner->getCell("Void"));
  VJaws->createAll(System,*ShieldC,2);
  
  Cryo->setInsertCell(Cave->getCell("Void"));
  Cryo->createAll(System,*VJaws,2);

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
  
  for(size_t i=0;i<nDet;i++)
    {
      typedef std::shared_ptr<constructSystem::CrystalMount> XTYPE;
      typedef std::shared_ptr<constructSystem::TubeDetBox> DTYPE;

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
      FStart= &(FocusF->getKey("Guide0"));
      startIndex= 2;
    }
  if (stopPoint==2 || stopPoint==1) return;

  if (startPoint<2)
    {
      VPipeWall->addInsertCell(voidCell);
      VPipeWall->createAll(System,*FStart,startIndex);
      
      FocusWall->addInsertCell(VPipeWall->getCell("Void"));
      FocusWall->createAll(System,*VPipeWall,0,*VPipeWall,0);
      FStart= &(FocusWall->getKey("Guide0"));
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
  FocusA->createAll(System,*vespaAxis,-3,*vespaAxis,-3);
  
  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  // IN WALL
  // Make bunker insert
  BInsert->createAll(System,FocusF->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);
  OutPitT0->addFrontWall(bunkerObj,2);
  
  if (stopPoint==3) return;                      // STOP At bunker exit
  buildOutGuide(System,FocusWall->getKey("Guide0"),2,voidCell);


  if (stopPoint==4) return;                      // STOP At hutch
  buildHut(System,ChopperOutB->getKey("Beam"),2,voidCell);
  buildDetectorArray(System,*Sample,0,Cave->getCell("Void"));
  return;
}

}   // NAMESPACE essSystem

