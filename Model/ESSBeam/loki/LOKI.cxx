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
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "insertPlate.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "ChopperUnit.h"

#include "BunkerInsert.h"
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
  
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDiskA(new constructSystem::DiskChopper(newName+"DBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::GuideLine(newName+"BD")),

  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  SDiskB(new constructSystem::DiskChopper(newName+"SBladeB")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")),
  SDiskC(new constructSystem::DiskChopper(newName+"SBladeC")),
  
  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  GridA(new constructSystem::RotaryCollimator(newName+"GridA")),
  CollA(new constructSystem::RotaryCollimator(newName+"CollA")),

  CBoxB(new constructSystem::insertPlate(newName+"CBoxB")),
  GridB(new constructSystem::RotaryCollimator(newName+"GridB")),
  CollB(new constructSystem::RotaryCollimator(newName+"CollB")),

  GridC(new constructSystem::RotaryCollimator(newName+"GridC")),
  CollC(new constructSystem::RotaryCollimator(newName+"CollC")),

  GridD(new constructSystem::RotaryCollimator(newName+"GridD"))

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

  OR.addObject(ChopperA);
  OR.addObject(DDiskA);

  OR.addObject(VPipeB);
  OR.addObject(BendB);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(VPipeD);
  OR.addObject(BendD);

  OR.addObject(ChopperB);
  OR.addObject(SDiskB);
  
  OR.addObject(VPipeE);
  OR.addObject(FocusE);

  OR.addObject(ChopperC);
  OR.addObject(SDiskC);
  
  OR.addObject(VPipeF);
  OR.addObject(FocusF);

  OR.addObject(GridA);
  OR.addObject(CollA);
  OR.addObject(CBoxB);
  OR.addObject(GridB);
  OR.addObject(CollB);
  OR.addObject(GridC);
  OR.addObject(CollC);
  OR.addObject(GridD);

}

LOKI::LOKI(const LOKI& A) : 
  attachSystem::CopiedComp(A),
  stopPoint(A.stopPoint),lokiAxis(A.lokiAxis),BendA(A.BendA),
  VPipeB(A.VPipeB),BendB(A.BendB),ChopperA(A.ChopperA),
  DDiskA(A.DDiskA),VPipeC(A.VPipeC),FocusC(A.FocusC),
  VPipeD(A.VPipeD),BendD(A.BendD),ChopperB(A.ChopperB),
  SDiskB(A.SDiskB),VPipeE(A.VPipeE),FocusE(A.FocusE),
  ChopperC(A.ChopperC),SDiskC(A.SDiskC),VPipeF(A.VPipeF),
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
      VPipeB=A.VPipeB;
      BendB=A.BendB;
      ChopperA=A.ChopperA;
      DDiskA=A.DDiskA;
      VPipeC=A.VPipeC;
      FocusC=A.FocusC;
      VPipeD=A.VPipeD;
      BendD=A.BendD;
      ChopperB=A.ChopperB;
      SDiskB=A.SDiskB;
      VPipeE=A.VPipeE;
      FocusE=A.FocusE;
      ChopperC=A.ChopperC;
      SDiskC=A.SDiskC;
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
  // BEAM needs to be rotated:
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
  
  // First [6m]
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,BendB->getKey("Guide0"),2);

  // Double disk chopper
  DDiskA->addInsertCell(ChopperA->getCell("Void"));
  DDiskA->setCentreFlag(3);  // Z direction
  DDiskA->setOffsetFlag(1);  // X direction
  DDiskA->createAll(System,ChopperA->getKey("Beam"),0);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->setFront(*VPipeC,2,true);
  VPipeD->createAll(System,FocusC->getKey("Guide0"),2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*VPipeD,0,*VPipeD,0);

  // First [9.7m]
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,BendD->getKey("Guide0"),2);

  // Double disk chopper
  SDiskB->addInsertCell(ChopperB->getCell("Void"));
  SDiskB->setCentreFlag(3);  // Z direction
  SDiskB->setOffsetFlag(1);  // Z direction
  SDiskB->createAll(System,ChopperB->getKey("Beam"),0);

  
  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,SDiskB->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);
  //  FocusE->createAll(System,SDiskB->getKey("Beam"),2,
  //                    SDiskB->getKey("Beam"),2);


  // Third [11.0m]
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusE->getKey("Guide0"),2);

  // Single disk chopper
  SDiskC->addInsertCell(ChopperC->getCell("Void"));
  SDiskC->setCentreFlag(3);  // Z direction
  SDiskC->setOffsetFlag(1);  // Z direction
  SDiskC->createAll(System,ChopperC->getKey("Beam"),0);

    
  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,SDiskC->getKey("Beam"),2);

  FocusF->addInsertCell(VPipeF->getCells("Void"));
  FocusF->createAll(System,*VPipeF,0,*VPipeF,0);

  GridA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GridA->createAll(System,FocusF->getKey("Guide0"),2);

  if (stopPoint==2) return;      // STOP At bunker edge

  CollA->addInsertCell(bunkerObj.getCell("MainVoid"));
  CollA->createAll(System,GridA->getKey("Beam"),2);
  attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
				   CollA->getKey("Main"),*CollA);  

  // For monment in main void : 
  CollB->addInsertCell(voidCell);
  CollB->createAll(System,CollA->getKey("Beam"),2);
  attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
				   CollB->getKey("Main"),*CollB);  

  CollC->addInsertCell(voidCell);
  CollC->createAll(System,CollB->getKey("Beam"),2);


  CBoxB->setFrontSurf(CollA->getKey("Main"),2);
  CBoxB->setBackSurf(CollB->getKey("Main"),1);
  CBoxB->createAll(System,CollA->getKey("Main"),2);
  attachSystem::addToInsertControl(System,bunkerObj,"frontWall",
				   *CBoxB,*CBoxB);  


  GridB->addInsertCell(CBoxB->getCell("Main"));
  GridB->createAll(System,CollA->getKey("Beam"),2);

  GridC->addInsertCell(voidCell);
  GridC->createAll(System,CollB->getKey("Beam"),2);

  GridD->addInsertCell(voidCell);
  GridD->createAll(System,CollC->getKey("Beam"),2);
  

  
  return;
}


}   // NAMESPACE essSystem

