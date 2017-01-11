/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKI.cxx
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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "ChopperUnit.h"

#include "FrontBackCut.h"
#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "Hut.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"
#include "PinHole.h"
#include "RentrantBS.h"
#include "LokiHut.h"
#include "VacTank.h"

#include "LOKI.h"

namespace essSystem
{

LOKI::LOKI(const std::string& keyN) :
  attachSystem::CopiedComp("loki",keyN),stopPoint(0),
  lokiAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  BendA(new beamlineSystem::GuideLine(newName+"BA")),

  ShutterA(new constructSystem::insertPlate(newName+"BlockShutter")),  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeBLink(new constructSystem::VacuumPipe(newName+"PipeBLink")),
  BendBLink(new beamlineSystem::GuideLine(newName+"BBLink")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDiskA(new constructSystem::DiskChopper(newName+"DBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  BInsert(new CompBInsert(newName+"CInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),
  
  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::GuideLine(newName+"BD")),

  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  DDiskB(new constructSystem::DiskChopper(newName+"DBladeB")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")),
  DDiskC(new constructSystem::DiskChopper(newName+"DBladeC")),
  
  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  VPipeFExtra(new constructSystem::VacuumPipe(newName+"PipeFExtra")),
  FocusFExtra(new beamlineSystem::GuideLine(newName+"FExtra")),

  GridA(new constructSystem::RotaryCollimator(newName+"GridA")),
  CollA(new constructSystem::RotaryCollimator(newName+"CollA")),


  VPipeCollA(new constructSystem::VacuumPipe(newName+"PipeCollA")),
  VPipeCollAX(new constructSystem::VacuumPipe(newName+"PipeCollAX")),
  VPipeCollB(new constructSystem::VacuumPipe(newName+"PipeCollB")),
  VPipeCollC(new constructSystem::VacuumPipe(newName+"PipeCollC")),

  FocusCollA(new beamlineSystem::GuideLine(newName+"FCollA")),
  FocusCollB(new beamlineSystem::GuideLine(newName+"FCollB")),
  FocusCollC(new beamlineSystem::GuideLine(newName+"FCollC")),

  CBoxB(new constructSystem::insertPlate(newName+"CBoxB")),
  GridB(new constructSystem::RotaryCollimator(newName+"GridB")),
  CollB(new constructSystem::RotaryCollimator(newName+"CollB")),

  GridC(new constructSystem::RotaryCollimator(newName+"GridC")),
  CollC(new constructSystem::RotaryCollimator(newName+"CollC")),

  GridD(new constructSystem::RotaryCollimator(newName+"GridD")),

  Cave(new LokiHut(newName+"Cave")),
  CaveGuide(new beamlineSystem::GuideLine(newName+"CaveGuide")),
  VTank(new VacTank(newName+"VTank"))
  /*!
    Constructor
    \param keyN :: keyName
  */
{
  ELog::RegMethod RegA("LOKI","LOKI");
  registerObjects();
}

LOKI::LOKI(const LOKI& A) : 
  attachSystem::CopiedComp(A),
  stopPoint(A.stopPoint),
  lokiAxis(new attachSystem::FixedOffset(*A.lokiAxis)),
  BendA(new beamlineSystem::GuideLine(*A.BendA)),
  VPipeB(new constructSystem::VacuumPipe(*A.VPipeB)),
  BendB(new beamlineSystem::GuideLine(*A.BendB)),
  ChopperA(new constructSystem::ChopperUnit(*A.ChopperA)),
  DDiskA(new constructSystem::DiskChopper(*A.DDiskA)),
  VPipeC(new constructSystem::VacuumPipe(*A.VPipeC)),
  FocusC(new beamlineSystem::GuideLine(*A.FocusC)),
  VPipeD(new constructSystem::VacuumPipe(*A.VPipeD)),
  BendD(new beamlineSystem::GuideLine(*A.BendD)),
  ChopperB(new constructSystem::ChopperUnit(*A.ChopperB)),
  DDiskB(new constructSystem::DiskChopper(*A.DDiskB)),
  VPipeE(new constructSystem::VacuumPipe(*A.VPipeE)),
  FocusE(new beamlineSystem::GuideLine(*A.FocusE)),
  ChopperC(new constructSystem::ChopperUnit(*A.ChopperC)),
  DDiskC(new constructSystem::DiskChopper(*A.DDiskC)),
  
  VPipeF(new constructSystem::VacuumPipe(*A.VPipeF)),
  FocusF(new beamlineSystem::GuideLine(*A.FocusF)),

  GridA(new constructSystem::RotaryCollimator(*A.GridA)),
  CollA(new constructSystem::RotaryCollimator(*A.CollA)),

  VPipeCollA(new constructSystem::VacuumPipe(*A.VPipeCollA)),
  VPipeCollAX(new constructSystem::VacuumPipe(*A.VPipeCollAX)),
  VPipeCollB(new constructSystem::VacuumPipe(*A.VPipeCollB)),
  VPipeCollC(new constructSystem::VacuumPipe(*A.VPipeCollC)),

  FocusCollA(new beamlineSystem::GuideLine(*A.FocusCollA)),
  FocusCollB(new beamlineSystem::GuideLine(*A.FocusCollB)),
  FocusCollC(new beamlineSystem::GuideLine(*A.FocusCollC)),

  //  CBoxB(new constructSystem::insertPlate(*CBoxB)),
  GridB(new constructSystem::RotaryCollimator(*A.GridB)),
  CollB(new constructSystem::RotaryCollimator(*A.CollB)),

  GridC(new constructSystem::RotaryCollimator(*A.GridC)),
  CollC(new constructSystem::RotaryCollimator(*A.CollC)),

  GridD(new constructSystem::RotaryCollimator(*A.GridD)),

  Cave(new LokiHut(*A.Cave)),
  CaveGuide(new beamlineSystem::GuideLine(*A.CaveGuide)),
  VTank(new VacTank(*A.VTank))
  /*!
    Copy constructor
    \param A :: LOKI to copy
  */
{
  registerObjects();
}

LOKI&
LOKI::operator=(const LOKI& A)
  /*!
    Assignment operator
    \param A :: LOKI to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::CopiedComp::operator=(A);
      stopPoint=A.stopPoint;
      lokiAxis=A.lokiAxis;
      BendA=A.BendA;
      VPipeB=A.VPipeB;
      BendB=A.BendB;
      ChopperA=A.ChopperA;
      DDiskA=A.DDiskA;
      VPipeC=A.VPipeC;
      FocusC=A.FocusC;
      VPipeD=A.VPipeD;
      BendD=A.BendD;
      ChopperB=A.ChopperB;
      DDiskB=A.DDiskB;
      VPipeE=A.VPipeE;
      FocusE=A.FocusE;
      ChopperC=A.ChopperC;
      DDiskC=A.DDiskC;
      VPipeF=A.VPipeF;
      FocusF=A.FocusF;
      GridA=A.GridA;
    }
  return *this;
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


  // NOT USED:
  OR.addObject(VPipeD);
  OR.addObject(BendD);

  OR.addObject(ChopperB);
  OR.addObject(DDiskB);
  
  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(ChopperC);
  OR.addObject(DDiskC);
  
  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(GridA);
  OR.addObject(CollA);
  
  OR.addObject(VPipeCollA);
  OR.addObject(VPipeCollAX);
  OR.addObject(VPipeCollB);
  OR.addObject(VPipeCollC);

  OR.addObject(FocusCollA);
  OR.addObject(FocusCollB);
  OR.addObject(FocusCollC);
  
  OR.addObject(CBoxB);
  OR.addObject(GridB);
  OR.addObject(CollB);
  OR.addObject(GridC);
  OR.addObject(CollC);
  OR.addObject(GridD);

  OR.addObject(Cave);
  OR.addObject(CaveGuide);
  OR.addObject(VTank);

  return;
}

void
LOKI::setBeamAxis(const FuncDataBase& Control,
		  const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Data base of info on variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse the z-direction 
   */
{
  ELog::RegMethod RegA("LOKI","setBeamAxis");

  lokiAxis->populate(Control);
  lokiAxis->createUnitVector(GItem);
  lokiAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  lokiAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  lokiAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  lokiAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  // BEAM needs to be shifted/rotated:
  lokiAxis->linkShift(3);
  lokiAxis->linkShift(4);
  lokiAxis->linkAngleRotate(3);
  lokiAxis->linkAngleRotate(4);

  if (reverseZ)
    lokiAxis->reverseZ();
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
  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
  
  FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(Control);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);

  setBeamAxis(System.getDataBase(),GItem,0);

  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->setFront(GItem.getKey("Beam"),-1);
  BendA->setBack(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*lokiAxis,-3,*lokiAxis,-3); // beam front reversed


  if (stopPoint==1) return;                // STOP At monolith edge

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,BendA->getKey("Guide0"),2);
  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);

  // Shield around gamma shield
  ShutterA->setNoInsert();
  ShutterA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ShutterA->setAxisControl(3,ZVert);
  ShutterA->createAll(System,BendB->getKey("Guide0"),-1);
  ShutterA->insertComponent(System,"Main",*VPipeB);

  // Link as gamma shield must move up and down
  VPipeBLink->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeBLink->createAll(System,BendB->getKey("Guide0"),2);

  BendBLink->addInsertCell(VPipeBLink->getCells("Void"));
  BendBLink->createAll(System,*VPipeBLink,0,*VPipeBLink,0);

  // First [6m]
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);  
  ChopperA->createAll(System,BendBLink->getKey("Guide0"),2);

  // Double disk chopper
  DDiskA->addInsertCell(ChopperA->getCell("Void"));
  DDiskA->setCentreFlag(3);  // Z direction
  DDiskA->setOffsetFlag(1);  // X direction
  DDiskA->createAll(System,ChopperA->getKey("Beam"),0);


  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // WALL
  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->setFront(bunkerObj,-1);
  BInsert->setBack(bunkerObj,-2);
  BInsert->createAll(System,FocusC->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  
  ELog::EM<<"BInsert == "<<bunkerObj.getSignedLinkString(-1)<<ELog::endDiag;
  ELog::EM<<"BInsert == "<<BInsert->getSignedLinkString(-1)<<ELog::endDiag;
  ELog::EM<<"BInsert == "<<BInsert->getSignedLinkString(-2)<<ELog::endDiag;


  FocusWall->addInsertCell(BInsert->getCells("Item"));
  FocusWall->setFront(*BInsert,-1);
  FocusWall->setBack(*BInsert,-2);
  FocusWall->createAll(System,*BInsert,7,*BInsert,7); 

  return;
  
  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->setFront(*VPipeC,2,true);
  VPipeD->createAll(System,FocusC->getKey("Guide0"),2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*VPipeD,0,*VPipeD,0);

  // First [9.7m]
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,BendD->getKey("Guide0"),2);

  // Double disk chopper
  DDiskB->addInsertCell(ChopperB->getCell("Void"));
  DDiskB->setCentreFlag(3);  // Z direction
  DDiskB->setOffsetFlag(1);  // Z direction
  DDiskB->createAll(System,ChopperB->getKey("Beam"),0);

  
  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,DDiskB->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);
  //  FocusE->createAll(System,DDiskB->getKey("Beam"),2,
  //                    DDiskB->getKey("Beam"),2);


  // Third [11.0m]
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusE->getKey("Guide0"),2);

  // Single disk chopper
  DDiskC->addInsertCell(ChopperC->getCell("Void"));
  DDiskC->setCentreFlag(3);  // Z direction
  DDiskC->setOffsetFlag(1);  // Z direction
  DDiskC->createAll(System,ChopperC->getKey("Beam"),0);

    
  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,DDiskC->getKey("Beam"),2);

  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

  GridA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GridA->createAll(System,FocusF->getKey("Guide0"),2);

  if (stopPoint==2) return;      // STOP At bunker edge

  CollA->addInsertCell(bunkerObj.getCell("MainVoid"));
  CollA->createAll(System,GridA->getKey("Beam"),2);
  attachSystem::addToInsertLineCtrl(System,bunkerObj,"frontWall",
                                    CollA->getKey("Main"),*CollA);  


  ELog::EM<<"CollA == "<<CollA->getKey("Beam").getSignedLinkPt(3)
	  <<ELog::endDiag;

  VPipeCollA->addInsertCell(CollA->getCell("Void0"));  // multiple units
  VPipeCollA->createAll(System,CollA->getKey("Hole"),-1);

  return;
  FocusCollA->addInsertCell(VPipeCollA->getCells("Void"));
  FocusCollA->createAll(System,*VPipeCollA,0,*VPipeCollA,0);

  return;
  //  FocusCA0->addInsertCell(CollA->getCell("Void0"));
  //  FocusCA0->createAll(System,CollA->getKey("Beam"),0,
  //                      CollA->getKey("Beam"),0);

  // For monment in main void : 
  CollB->addInsertCell(voidCell);
  CollB->createAll(System,CollA->getKey("Beam"),2);
  attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
				   CollB->getKey("Main"),*CollB);  

  CollC->addInsertCell(voidCell);
  CollC->createAll(System,CollB->getKey("Beam"),2);


  CBoxB->setFront(CollA->getKey("Main"),2);
  CBoxB->setBack(CollB->getKey("Main"),1);
  CBoxB->createAll(System,CollA->getKey("Main"),2);
  attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
				   *CBoxB,*CBoxB);  


  GridB->addInsertCell(CBoxB->getCell("Main"));
  GridB->createAll(System,CollA->getKey("Beam"),2);

  GridC->addInsertCell(voidCell);
  GridC->createAll(System,CollB->getKey("Beam"),2);
 
  
  Cave->addInsertCell(voidCell);
  Cave->createAll(System,CollC->getKey("Beam"),2);


  CollC->addInsertCell(Cave->getCells("FrontWall"));
  CollC->addInsertCell(Cave->getCell("Void"));
  CollC->insertObjects(System);

  GridD->addInsertCell(Cave->getCell("Void"));
  GridD->createAll(System,CollC->getKey("Beam"),2);

  // Vacuum tank
  VTank->addInsertCell(Cave->getCell("Void"));
  //  VTank->createAll(System,CaveGuide->getKey("Guide0"),2);
  VTank->createAll(System,GridD->getKey("Beam"),2);

  return;
}


}   // NAMESPACE essSystem

