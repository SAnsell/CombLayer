/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKI.cxx
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
#include "insertObject.h"
#include "insertPlate.h"

#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "SingleChopper.h"
#include "ChopperPit.h"
#include "LineShield.h"
#include "Aperture.h"
#include "HoleShape.h"
#include "PipeCollimator.h"

#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "Hut.h"
#include "RotaryCollimator.h"
#include "PinHole.h"
#include "RentrantBS.h"
#include "LokiHut.h"
#include "VacTank.h"

#include "LOKI.h"

namespace essSystem
{

LOKI::LOKI(const std::string& keyN) :
  attachSystem::CopiedComp("loki",keyN),startPoint(0),stopPoint(0),
  lokiAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  BendA(new beamlineSystem::GuideLine(newName+"BA")),

  ShutterA(new insertSystem::insertPlate(newName+"BlockShutter")),  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeBLink(new constructSystem::VacuumPipe(newName+"PipeBLink")),
  BendBLink(new beamlineSystem::GuideLine(newName+"BBLink")),

  ChopperA(new constructSystem::SingleChopper(newName+"ChopperA")),
  DDiskA(new constructSystem::DiskChopper(newName+"DBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  BInsert(new CompBInsert(newName+"CInsert")),
  VPipeWall(new constructSystem::VacuumPipe(newName+"PipeWall")),  
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  OutPitA(new constructSystem::ChopperPit(newName+"OutPitA")),
  PitACut(new constructSystem::HoleShape(newName+"PitACut")),
  ChopperOutA(new constructSystem::SingleChopper(newName+"ChopperOutA")),
  DDiskOutA(new constructSystem::DiskChopper(newName+"DBladeOutA")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),
  AppA(new constructSystem::Aperture(newName+"AppA")),
  CollA(new constructSystem::PipeCollimator(newName+"CollA")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),
  CollB(new constructSystem::PipeCollimator(newName+"CollB")),
  AppB(new constructSystem::Aperture(newName+"AppB")),

  Cave(new LokiHut(newName+"Cave")),
  VPipeOutC(new constructSystem::VacuumPipe(newName+"PipeOutC")),
  FocusOutC(new beamlineSystem::GuideLine(newName+"FOutC")),
  VTank(new VacTank(newName+"VTank"))


  /*!
    Constructor
    \param keyN :: keyName
  */
{
  ELog::RegMethod RegA("LOKI","LOKI");
  registerObjects();
}



LOKI::~LOKI()
  /*!
    Destructor
  */
{}

void
LOKI::registerObjects()
  /*!
    Register all the objects with the object register
  */
{
  ELog::RegMethod RegA("LOKI","registerObjects");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  // This necessary:
  OR.cell(newName+"Axis");
  OR.addObject(lokiAxis);

  OR.addObject(BendA);

  OR.addObject(ShutterA);
  OR.addObject(VPipeB);
  OR.addObject(BendB);
  
  OR.addObject(VPipeBLink);
  OR.addObject(BendBLink);

  OR.addObject(ChopperA);
  OR.addObject(DDiskA);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);
  
  OR.addObject(BInsert);
  OR.addObject(VPipeWall);
  OR.addObject(FocusWall);

  OR.addObject(OutPitA);
  OR.addObject(PitACut);
  OR.addObject(ChopperOutA);
  OR.addObject(DDiskOutA);

  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);
  OR.addObject(AppA);
  OR.addObject(CollA);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);
  OR.addObject(CollB);	
  OR.addObject(AppB);
       
  OR.addObject(Cave);
  OR.addObject(VPipeOutC);
  OR.addObject(FocusOutC);
  OR.addObject(VTank);

  return;
}

void
LOKI::buildBunkerUnits(Simulation& System,
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
  ELog::RegMethod RegA("LOKI","buildBunkerUnits");
  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());  

  VPipeB->addInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);
  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);

  // Shield around gamma shield
  ShutterA->setNoInsert();
  ShutterA->addInsertCell(bunkerVoid);
  ShutterA->setAxisControl(3,ZVert);
  ShutterA->createAll(System,BendB->getKey("Guide0"),-1);
  ShutterA->insertComponent(System,"Main",*VPipeB);

  // Link as gamma shield must move up and down
  VPipeBLink->addInsertCell(bunkerVoid);
  VPipeBLink->createAll(System,BendB->getKey("Guide0"),2);

  BendBLink->addInsertCell(VPipeBLink->getCells("Void"));
  BendBLink->createAll(System,*VPipeBLink,0,*VPipeBLink,0);

  // First [6m]
  ChopperA->addInsertCell(bunkerVoid);
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);  
  ChopperA->createAll(System,BendBLink->getKey("Guide0"),2);

  // Double disk chopper
  DDiskA->addInsertCell(ChopperA->getCell("Void"));
  DDiskA->setOffsetFlag(1);  // centre disk based on thickness
  DDiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*DDiskA);

  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  return;

}

void
LOKI::buildOutGuide(Simulation& System,
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
  ELog::RegMethod RegA("LOKI","buildOutGuide");

  //Chopper pit merged with bunker wall
  OutPitA->addInsertCell(voidCell);
  OutPitA->createAll(System,FW,startIndex);
  
  //Cut throught chopper pit for guide and pipe that are following it 
  PitACut->addInsertCell(OutPitA->getCells("MidLayerBack"));
  PitACut->addInsertCell(OutPitA->getCells("Collet"));
  PitACut->setFaces(OutPitA->getKey("Inner").getFullRule(2),
                    OutPitA->getKey("Mid").getFullRule(-2));
  PitACut->createAll(System,OutPitA->getKey("Inner"),2);
  
  //Chopper unit
  ChopperOutA->addInsertCell(OutPitA->getCell("Void"));
  ChopperOutA->createAll(System,FocusWall->getKey("Guide0"),2);

  // Double disk chopper
  DDiskOutA->addInsertCell(ChopperOutA->getCell("Void"));
  DDiskOutA->setOffsetFlag(1);  // Normalized thickness to centre
  DDiskOutA->createAll(System,ChopperOutA->getKey("Main"),0);  
  ChopperOutA->insertAxle(System,*DDiskOutA);

 //Beamline Shileding
  ShieldA->addInsertCell(voidCell);
  ShieldA->addInsertCell(OutPitA->getCells("MidLayer"));
  ShieldA->addInsertCell(OutPitA->getCell("Outer"));
  ShieldA->setFront(OutPitA->getKey("Mid"),2);
  ShieldA->createAll(System,OutPitA->getKey("Mid"),2);

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->addInsertCell(OutPitA->getCell("Void"));
  VPipeOutA->addInsertCell(PitACut->getCell("Void")); 
  VPipeOutA->createAll(System,ChopperOutA->getKey("Beam"),2);

  FocusOutA->addInsertCell(VPipeOutA->getCell("Void"));
  FocusOutA->createAll(System,*VPipeOutA,0,*VPipeOutA,0);

  //Aperture after first collimator drum
  AppA->addInsertCell(ShieldA->getCell("Void"));
  AppA->createAll(System,FocusOutA->getKey("Guide0"),2);

  attachSystem::addToInsertLineCtrl(System,*ShieldA,*AppA,*AppA);
  attachSystem::addToInsertForced(System,*AppA,*VPipeOutA);

  //Collimator block in first shielding
  CollA->setInnerExclude(VPipeOutA->getFullRule(9));
  CollA->setOuter(ShieldA->getXSectionIn());
  CollA->addInsertCell(ShieldA->getCell("Void"));
  CollA->addInsertCell(VPipeOutA->getCell("OutVoid"));
  //  CollA->createAll(System,*VPipeOutA,-1);


//Beamline shielding
  ShieldB->addInsertCell(voidCell);
  ShieldB->createAll(System,*ShieldA,2);

  //Vacuum pipe and guide
  VPipeOutB->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->createAll(System,*AppA,2);
  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,0,*VPipeOutB,0);

  CollB->setInnerExclude(VPipeOutB->getFullRule(9));
  CollB->setOuter(ShieldB->getXSectionIn());
  CollB->addInsertCell(ShieldB->getCell("Void"));
  CollB->addInsertCell(VPipeOutB->getCell("OutVoid"));
  CollB->createAll(System,*VPipeOutB,-1);
  
  // Aperture after second collimator drum
  AppB->addInsertCell(ShieldB->getCell("Void"));
  AppB->createAll(System,FocusOutB->getKey("Guide0"),2);

  return;
}
  
void
LOKI::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("LOKI","buildIsolated");


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
      FStart= &FocusC->getKey("Guide0");
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
      OutPitA->addFrontWall(*VPipeWall,2);
      startIndex=2;
    }
  if (stopPoint==3) return;

  if (startPoint<3)
    {

      //      buildOutGuide(System,*FStart,startIndex,voidCell);      
      //      FStart=&(ChopperOutB->getKey("Beam"));
      startIndex=2;
    }

  if (stopPoint==4) return;      

  if (startPoint<4)
    {
      //      buildHut(System,*FStart,startIndex,voidCell);
      //      buildDetectorArray(System,*Sample,0,Cave->getCell("Void"));
    }
  
  return;
}
  
void 
LOKI::build(Simulation& System,
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
  ELog::RegMethod RegA("LOKI","build");
  ELog::EM<<"\nBuilding LOKI on : "<<GItem.getKeyName()<<ELog::endDiag;

  FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(Control);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);

  essBeamSystem::setBeamAxis(*lokiAxis,System.getDataBase(),GItem,1);

  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*lokiAxis,-3,*lokiAxis,-3); // beam front reversed

  if (stopPoint==1) return;                // STOP At monolith edge

  buildBunkerUnits(System,BendA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                // STOP At Bunker Wall
  
  // WALL
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->setFront(bunkerObj,-1);
  BInsert->setBack(bunkerObj,-2);
  BInsert->createAll(System,FocusC->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  FocusWall->addInsertCell(BInsert->getCells("Item"));
  FocusWall->setFront(*BInsert,-1);
  FocusWall->setBack(*BInsert,-2);
  FocusWall->createAll(System,*BInsert,7,*BInsert,7); 

  if (stopPoint==3) return;                // STOP At Outer-Bunker Wall
  
  OutPitA->addFrontWall(bunkerObj,2);
  buildOutGuide(System,FocusWall->getKey("Guide0"),2,voidCell);
  
  Cave->addInsertCell(voidCell);
  Cave->createAll(System,*ShieldB,2);

  ShieldB->addInsertCell(Cave->getCells("FrontWall"));
  ShieldB->insertObjects(System);

  VPipeOutC->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutC->addInsertCell(Cave->getCells("FrontWall"));
  VPipeOutC->addInsertCell(Cave->getCells("Void"));
  VPipeOutC->createAll(System,*AppB,2);
  FocusOutC->addInsertCell(VPipeOutC->getCell("Void"));
  FocusOutC->createAll(System,*VPipeOutC,0,*VPipeOutC,0);

  VTank->addInsertCell(Cave->getCell("Void"));
  VTank->createAll(System,FocusOutC->getKey("Guide0"),2);
 
  return;
}


}   // NAMESPACE essSystem

