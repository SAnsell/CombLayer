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
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "ChopperPit.h"
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
#include "HoleShape.h"
#include "PipeCollimator.h"
#include "Aperture.h"
#include "LineShield.h"
#include "LokiHut.h"
#include "VacTank.h"

#include "LOKI.h"

namespace essSystem
{

LOKI::LOKI(const std::string& keyN) :
  attachSystem::CopiedComp("loki",keyN),stopPoint(0),
  lokiAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  BendA(new beamlineSystem::GuideLine(newName+"BA")),
  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  ShutterA(new constructSystem::insertPlate(newName+"BlockShutter")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDiskA(new constructSystem::DiskChopper(newName+"DBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperAExtra(new constructSystem::ChopperUnit(newName+"ChopperAExtra")),

  SDiskAEFirst(new constructSystem::DiskChopper(newName+"SBladeAEFirst")),
  SDiskAESecond(new constructSystem::DiskChopper(newName+"SBladeAESecond")),

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

  //BInsert(new BunkerInsert(newName+"BInsert")),
  BInsert(new CompBInsert(newName+"CInsert")),
  FocusFBunker(new beamlineSystem::GuideLine(newName+"FFBunker")),

  PitD(new constructSystem::ChopperPit(newName+"PitD")),
  ChopperD(new constructSystem::ChopperUnit(newName+"ChopperD")),

  // SDiskDFirst(new constructSystem::DiskChopper(newName+"SBladeDFirst")),
  // SDiskDSecond(new constructSystem::DiskChopper(newName+"SBladeDSecond")),

  DDiskD(new constructSystem::DiskChopper(newName+"DBladeD")),
  CutD(new constructSystem::HoleShape(newName+"CutD")),

  CutG(new constructSystem::HoleShape(newName+"CutG")),
  ShieldG(new constructSystem::LineShield(newName+"ShieldG")),
  VPipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  FocusG(new beamlineSystem::GuideLine(newName+"FG")),
  CollG(new constructSystem::PipeCollimator(newName+"CollG")),
  AppA(new constructSystem::Aperture(newName+"AppA")),

  ShieldH(new constructSystem::LineShield(newName+"ShieldH")),
  VPipeH(new constructSystem::VacuumPipe(newName+"PipeH")),
  FocusH(new beamlineSystem::GuideLine(newName+"FH")),
  CollH(new constructSystem::PipeCollimator(newName+"CollH")),

  AppB(new constructSystem::Aperture(newName+"AppB")),

  
  Cave(new LokiHut(newName+"Cave")),
  FocusK(new beamlineSystem::GuideLine(newName+"FK")),
  Tank(new VacTank(newName+"Tank"))

  /*!
    Constructor
    \param keyN :: keyName
  */
{
  ELog::RegMethod RegA("LOKI","LOKI");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  // This necessary:
  OR.cell(newName+"Axis");
  OR.addObject(lokiAxis);

  OR.addObject(BendA);

  OR.addObject(ShutterA);
  OR.addObject(VPipeB);
  OR.addObject(BendB);

  OR.addObject(ChopperA);
  OR.addObject(DDiskA);
  
  OR.addObject(ChopperAExtra);
  OR.addObject(SDiskAEFirst);
  OR.addObject(SDiskAESecond);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

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

  OR.addObject(BInsert);
  OR.addObject(FocusFExtra);

  OR.addObject(PitD);
  OR.addObject(ChopperD);
  OR.addObject(DDiskD);

  // OR.addObject(SDiskDFirst);
  // OR.addObject(SDiskDSecond);

  OR.addObject(CutD);

  OR.addObject(CutG);
  OR.addObject(ShieldG);
  OR.addObject(VPipeG);
  OR.addObject(FocusG);
  OR.addObject(CollG);
  OR.addObject(AppA);

  OR.addObject(ShieldH);
  OR.addObject(VPipeH);
  OR.addObject(FocusH);
  OR.addObject(CollH);
  OR.addObject(AppB);

  OR.addObject(FocusK);
  OR.addObject(Cave);
  OR.addObject(Tank);
}

LOKI::LOKI(const LOKI& A) : 
  attachSystem::CopiedComp(A),
  stopPoint(A.stopPoint),lokiAxis(A.lokiAxis),BendA(A.BendA),
  VPipeB(A.VPipeB),ShutterA(A.ShutterA),BendB(A.BendB),
  ChopperA(A.ChopperA),
  DDiskA(A.DDiskA),VPipeC(A.VPipeC),FocusC(A.FocusC),
  VPipeD(A.VPipeD),BendD(A.BendD),ChopperB(A.ChopperB),
  DDiskB(A.DDiskB),VPipeE(A.VPipeE),FocusE(A.FocusE),
  ChopperC(A.ChopperC),DDiskC(A.DDiskC),VPipeF(A.VPipeF),
  FocusF(A.FocusF),GridA(A.GridA)
  /*!
    Copy constructor
    \param A :: LOKI to copy
  */
{}

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
      ShutterA=A.ShutterA;
      VPipeB=A.VPipeB;
      BendB=A.BendB;
      ChopperA=A.ChopperA;
      DDiskA=A.DDiskA;
      VPipeC=A.VPipeC;
      FocusC=A.FocusC;
      ChopperAExtra=A.ChopperAExtra;
      VPipeD=A.VPipeD;
      BendD=A.BendD;
      ChopperB=A.ChopperB;
      DDiskB=A.DDiskB;
      VPipeE=A.VPipeE;
      FocusE=A.FocusE;
      ChopperC=A.ChopperC;
      DDiskC=A.DDiskC;
      DDiskD=A.DDiskD;
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

  BendA->addFrontCut(GItem.getKey("Beam"),-1);
  BendA->addEndCut(GItem.getKey("Beam"),-2);
  BendA->createAll(System,*lokiAxis,-3,*lokiAxis,-3); // beam front reversed


  if (stopPoint==1) return;                // STOP At monolith edge


  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,BendA->getKey("Guide0"),2);
 
  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);


  ShutterA->setNoInsert();
  ShutterA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ShutterA->setAxisControl(3,ZVert);
  ShutterA->createAll(System,BendB->getKey("Guide0"),-1);
  ShutterA->insertComponent(System,"Main",*VPipeB);
  
  // First chopper unit
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperA->createAll(System,BendB->getKey("Guide0"),2); 

  // Double disk chopper
  DDiskA->addInsertCell(ChopperA->getCell("Void"));
  DDiskA->setCentreFlag(3);  // Z direction
  DDiskA->setOffsetFlag(1);  // X direction
  DDiskA->createAll(System,ChopperA->getKey("BuildBeam"),0);

  // Vacuum pipe and guide between first and second chopper units
  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // // Second chopper unit
  // ChopperAExtra->addInsertCell(bunkerObj.getCell("MainVoid"));
  // ChopperAExtra->createAll(System,FocusC->getKey("Guide0"),2);

  // // Two single Disks!
  // SDiskAEFirst->addInsertCell(ChopperAExtra->getCell("Void"));
  // SDiskAEFirst->setCentreFlag(3);  // Z direction
  // SDiskAEFirst->setOffsetFlag(1);  // X direction
  // SDiskAEFirst->createAll(System,ChopperAExtra->getKey("Beam"),0);

  // SDiskAESecond->addInsertCell(ChopperAExtra->getCell("Void"));
  // SDiskAESecond->setCentreFlag(-3);  // Z direction
  // SDiskAESecond->setOffsetFlag(1);  // X direction
  // SDiskAESecond->createAll(System,ChopperAExtra->getKey("Beam"),0);

  // //Vacuum pipe and guide between second and third chopper units
  // VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  // VPipeD->createAll(System,ChopperAExtra->getKey("Beam"),2);
  // BendD->addInsertCell(VPipeD->getCells("Void"));
  // BendD->createAll(System,*VPipeD,0,*VPipeD,0);

  // // Third chopper unit
  // ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  // ChopperB->createAll(System,BendD->getKey("Guide0"),2);

  // // Double disk chopper
  // DDiskB->addInsertCell(ChopperB->getCell("Void"));
  // DDiskB->setCentreFlag(3);  // Z direction
  // DDiskB->setOffsetFlag(1);  // Z direction
  // DDiskB->createAll(System,ChopperB->getKey("Beam"),0);

  // //Vacuum pipe and guide between third and fourth choppers 
  // VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  // VPipeE->createAll(System,DDiskB->getKey("Beam"),2);
  // FocusE->addInsertCell(VPipeE->getCells("Void"));
  // FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // // Fourth chopper
  // ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  // ChopperC->createAll(System,FocusE->getKey("Guide0"),2);

  // // Double disk chopper
  // DDiskC->addInsertCell(ChopperC->getCell("Void"));
  // DDiskC->setCentreFlag(3);  // Z direction
  // DDiskC->setOffsetFlag(1);  // Z direction
  // DDiskC->createAll(System,ChopperC->getKey("Beam"),0);

  // //Vacuum pipe and guide between fourth chopper and bunker wall
  // VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  // VPipeF->createAll(System,DDiskC->getKey("Beam"),2);
  // FocusF->addInsertCell(VPipeF->getCells("Void"));
  // FocusF->createAll(System,*VPipeF,0,*VPipeF,0);


  // Bunker insert and guide inside



  BInsert->addInsertCell(bunkerObj.getCell("MainVoid"));
  BInsert->addInsertCell(74123);

  BInsert->addInsertCell(PitD->getCells("Void"));
  BInsert->addInsertCell(PitD->getCells("MidLayer"));
  BInsert->addInsertCell(PitD->getCells("Outer"));



  //BInsert->createAll(System,*FocusC->getKey("Guide0"),-1,bunkerObj);
  BInsert->createAll(System,FocusC->getKey("Guide0"),2,bunkerObj);
  //BInsert->createAll(System,*FocusC,-2,bunkerObj); //should be focusF for the full model - not full, model with many choppers
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // BInsert->createAll(System,FocusC->getKey("Guide0"),-1,bunkerObj); //should be focusF for the full model
  // attachSystem::addToInsertLineCtrl(System,bunkerObj,"frontWall",
  //           *BInsert,*BInsert);

  FocusFBunker->addInsertCell(BInsert->getCells("Item"));
  FocusFBunker->createAll(System,*BInsert,7,*BInsert,7); // 0,*BInsert,0);

  FocusFBunker->addInsertCell(PitD->getCells("Void"));
  FocusFBunker->addInsertCell(PitD->getCells("MidLayer"));
  FocusFBunker->addInsertCell(PitD->getCells("Outer"));


  ELog::EM<<"Axis == "<<FocusFBunker->getKey("Guide0").getSignedLinkAxis(2)
	  <<ELog::endDiag;
  
  // BInsert->insertComponent(System,"Void",*FocusFBunker);


//Chopper pit(=shielding)
  PitD->addInsertCell(voidCell);
  PitD->addFrontWall(bunkerObj,2);
  PitD->createAll(System,FocusFBunker->getKey("Guide0"),2);
  //attachSystem::addToInsertControl(System,*PitD,"Outer", *BInsert, *BInsert);

  if (stopPoint==3) return;                      // STOP Out of bunker
     



//Cut through pitD to accomodate bunker insert that sticks into the void of pipeD

  //Cut throught chopper pit for guide and pipe that are following it 
  CutD->addInsertCell(PitD->getCells("MidLayerBack"));
  CutD->addInsertCell(PitD->getCells("Collet"));
  CutD->setFaces(PitD->getKey("Inner").getSignedFullRule(2),
                    PitD->getKey("Mid").getSignedFullRule(-2));
  CutD->createAll(System,PitD->getKey("Inner"),2);


  //BInsert->addInsertCell(ShieldG->getCell("Void"));
  BInsert->addInsertCell(CutD->getCell("Void")); //to make it belong to the both 
  BInsert->addInsertCell(CutD->getCells("MidLayer"));
  //VPipeG->createAll(System,ChopperD->getKey("Beam"),2);





 //Vacuum pipe and guide outside of the bunker wall, inside chopper pit D
  VPipeFExtra->addInsertCell(PitD->getCells("Void"));
  VPipeFExtra->addInsertCell(PitD->getCells("MidLayer"));
  VPipeFExtra->addInsertCell(PitD->getCells("Outer"));
// VPipeFExtra->addEndCut(PitD->getKey("Inner").getSignedLinkString(1));
  VPipeFExtra->createAll(System,FocusFBunker->getKey("Guide0"),2);

  FocusFExtra->addInsertCell(VPipeFExtra->getCells("Void"));
  FocusFExtra->createAll(System,*VPipeFExtra,0,*VPipeFExtra,0);

  //Chopper unit
  ChopperD->addInsertCell(PitD->getCell("Void"));
  ChopperD->createAll(System,FocusFExtra->getKey("Guide0"),2);

  // Double disk chopper
  DDiskD->addInsertCell(ChopperD->getCell("Void"));
  DDiskD->setCentreFlag(3);  // Z direction
  DDiskD->setOffsetFlag(1);  // X direction
  DDiskD->createAll(System,ChopperD->getKey("BuildBeam"),0);  

     

 //Beamline Shileding
  ShieldG->addInsertCell(voidCell);
  ShieldG->addInsertCell(PitD->getCells("MidLayer"));
  ShieldG->addInsertCell(PitD->getCell("Outer"));
  ShieldG->setFront(PitD->getKey("Mid"),2);
  ShieldG->createAll(System,PitD->getKey("Mid"),2);

//Cut throught chopper pit for guide and pipe that are following it 
  CutG->addInsertCell(PitD->getCells("MidLayerBack"));
  CutG->addInsertCell(PitD->getCells("Collet"));
  CutG->setFaces(PitD->getKey("Inner").getSignedFullRule(2),
                    PitD->getKey("Mid").getSignedFullRule(-2));
  CutG->createAll(System,PitD->getKey("Inner"),2);


  VPipeG->addInsertCell(ShieldG->getCell("Void"));
  VPipeG->addInsertCell(CutG->getCell("Void")); //to make it belong to the both 
  VPipeG->addInsertCell(CutG->getCells("MidLayer"));
  VPipeG->createAll(System,ChopperD->getKey("Beam"),2);

  FocusG->addInsertCell(VPipeG->getCell("Void"));
  FocusG->createAll(System,*VPipeG,0,*VPipeG,0);

//Collimator block
  CollG->setInnerExclude(VPipeG->getSignedFullRule(9));
  CollG->setOuter(ShieldG->getXSectionIn());
  CollG->addInsertCell(ShieldG->getCell("Void"));
  CollG->createAll(System,*VPipeG,-1);
 return; 
//Aperture after first collimator drum
//  AppA->addInsertCell(ShieldG->getCell("Void"));
  System.populateCells();
  System.validateObjSurfMap();
  AppA->createAll(System,FocusG->getKey("Guide0"),2);
  attachSystem::addToInsertLineCtrl(System,*ShieldG,*AppA,*AppA);

//Beamline shielding
  ShieldH->addInsertCell(voidCell);
  ShieldH->createAll(System,*ShieldG,2);



//Vacuum pipe and guide
  VPipeH->addInsertCell(ShieldH->getCell("Void"));
  VPipeH->createAll(System,*AppA,2);
  FocusH->addInsertCell(VPipeH->getCell("Void"));
  FocusH->createAll(System,*VPipeH,0,*VPipeH,0);

//Second Collimator drum
  // CollH->setInnerExclude(FocusH->getXSectionOut());
  // CollH->setOuter(VPipeH->getSignedFullRule(-3));
  // CollH->addInsertCell(VPipeH->getCell("Void"));
  // CollH->createAll(System,*VPipeH,-1);

  CollH->setInnerExclude(VPipeH->getSignedFullRule(9));
  CollH->setOuter(ShieldH->getXSectionIn());
  CollH->addInsertCell(ShieldH->getCell("Void"));
  CollH->createAll(System,*VPipeH,-1);

//Aperture after second collimator drum
  AppB->addInsertCell(ShieldH->getCell("Void"));
  AppB->createAll(System,FocusH->getKey("Guide0"),2);



  Cave->addInsertCell(voidCell);
  Cave->createAll(System,FocusH->getKey("Guide0"),2);

  //ShieldH->addInsertCell(Cave->getCells("frontWall"));
  //ShieldG->addInsertCell(Cave->getCell("Outer"));
  //ShieldH->setBack(Cave->getKey("Mid"),2);

  // EXIT GUIDE:
  //  FocusF->addInsertCell(Cave->getCell("Void"));
  FocusK->createAll(System,FocusH->getKey("Guide0"),2,
        FocusH->getKey("Guide0"),2);

  Cave->insertComponent(System,"Void",*FocusK);
  //Cave->insertComponent(System,"Steel",*FocusK);
  //Cave->insertComponent(System,"Concrete",*FocusK);

  return;

  Tank->addInsertCell(Cave->getCell("Void"));
  Tank->createAll(System,FocusK->getKey("Guide0"),2);



  // GridA->addInsertCell(bunkerObj.getCell("MainVoid"));
  // GridA->createAll(System,FocusF->getKey("Guide0"),2);

  // if (stopPoint==2) return;      // STOP At bunker edge

  // CollA->addInsertCell(bunkerObj.getCell("MainVoid"));
  // CollA->createAll(System,GridA->getKey("Beam"),2);
  // attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
		// 		   CollA->getKey("Main"),*CollA);  

  // ELog::EM<<"CollA == "<<CollA->getKey("Beam").getSignedLinkPt(0)
	 //  <<ELog::endDiag;
  //  FocusCA0->addInsertCell(CollA->getCell("Void0"));
  //  FocusCA0->createAll(System,CollA->getKey("Beam"),0,
  //                      CollA->getKey("Beam"),0);

  // // For monment in main void : 
  // CollB->addInsertCell(voidCell);
  // CollB->createAll(System,CollA->getKey("Beam"),2);
  // attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
		// 		   CollB->getKey("Main"),*CollB);  

  // CollC->addInsertCell(voidCell);
  // CollC->createAll(System,CollB->getKey("Beam"),2);


  // CBoxB->setFrontSurf(CollA->getKey("Main"),2);
  // CBoxB->setBackSurf(CollB->getKey("Main"),1);
  // CBoxB->createAll(System,CollA->getKey("Main"),2);
  // attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
		// 		   *CBoxB,*CBoxB);  


  // GridB->addInsertCell(CBoxB->getCell("Main"));
  // GridB->createAll(System,CollA->getKey("Beam"),2);

  // GridC->addInsertCell(voidCell);
  // GridC->createAll(System,CollB->getKey("Beam"),2);

  // GridD->addInsertCell(voidCell);
  // GridD->createAll(System,CollC->getKey("Beam"),2);
  

  
  return;
}


}   // NAMESPACE essSystem

