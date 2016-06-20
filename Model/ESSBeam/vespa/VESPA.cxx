/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VESPA.cxx
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
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "ChopperUnit.h"
#include "DreamHut.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "VESPA.h"

namespace essSystem
{

VESPA::VESPA(const std::string& keyName) :
  attachSystem::CopiedComp("vespa",keyName),
  stopPoint(0),
  vespaAxis(new attachSystem::FixedComp(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  WFMDiskA(new constructSystem::DiskChopper(newName+"WFMBladeA")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),

  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  WFMDiskB(new constructSystem::DiskChopper(newName+"WFMBladeB")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")),
  WFMDiskC(new constructSystem::DiskChopper(newName+"WFMBladeC")),

  VPipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  FocusE(new beamlineSystem::GuideLine(newName+"FE")),

  ChopperD(new constructSystem::ChopperUnit(newName+"ChopperD")),
  FOCDiskA(new constructSystem::DiskChopper(newName+"FOCBladeA")),

  VPipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  FocusF(new beamlineSystem::GuideLine(newName+"FF")),

  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),

  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),

  PitB(new constructSystem::ChopperPit(newName+"PitB")),
  ChopperOutB(new constructSystem::ChopperUnit(newName+"ChopperOutB")),
  FOCDiskB(new constructSystem::DiskChopper(newName+"FOCBladeB")),

  ShieldB(new constructSystem::LineShield(newName+"ShieldB")),
  VPipeOutB(new constructSystem::VacuumPipe(newName+"PipeOutB")),
  FocusOutB(new beamlineSystem::GuideLine(newName+"FOutB")),
  
  PitC(new constructSystem::ChopperPit(newName+"PitC"))
  
 /*!
    Constructor
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
  OR.addObject(FocusWall);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(PitB);
  OR.addObject(ChopperOutB);
  OR.addObject(FOCDiskB);

  OR.addObject(ShieldB);
  OR.addObject(VPipeOutB);
  OR.addObject(FocusOutB);

  for(size_t i=0;i<4;i++)
    {
      typedef std::shared_ptr<constructSystem::LineShield> STYPE;
      typedef std::shared_ptr<constructSystem::VacuumPipe> VTYPE;
      typedef std::shared_ptr<beamlineSystem::GuideLine> GTYPE;
      
      ShieldArray.push_back
        (STYPE(new constructSystem::LineShield(newName+"ShieldArray"+
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
}

  
VESPA::~VESPA()
  /*!
    Destructor
  */
{}

void
VESPA::setBeamAxis(const GuideItem& GItem,
                   const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("VESPA","setBeamAxis");

  vespaAxis->createUnitVector(GItem);
  vespaAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  vespaAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  vespaAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  vespaAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    vespaAxis->reverseZ();
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
  // For output stream
  ELog::RegMethod RegA("VESPA","build");

  ELog::EM<<"\nBuilding VESPA on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<ELog::endDiag;
  
  setBeamAxis(GItem,1);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge
  
  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->createAll(System,*VPipeA,0,*VPipeA,0);

  // First Chopper
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,FocusB->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskA->addInsertCell(ChopperA->getCell("Void"));
  WFMDiskA->setCentreFlag(3);  // Z direction
  WFMDiskA->setOffsetFlag(1);  // Z direction
  WFMDiskA->createAll(System,ChopperA->getKey("Beam"),0);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,ChopperA->getKey("Beam"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // First Chopper
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,FocusC->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskB->addInsertCell(ChopperB->getCell("Void"));
  WFMDiskB->setCentreFlag(3);  // Z direction
  WFMDiskB->setOffsetFlag(1);  // Z direction
  WFMDiskB->createAll(System,ChopperB->getKey("Beam"),0);

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // First Chopper
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusD->getKey("Guide0"),2);

  // Double disk chopper
  WFMDiskC->addInsertCell(ChopperC->getCell("Void"));
  WFMDiskC->setCentreFlag(3);  // Z direction
  WFMDiskC->setOffsetFlag(1);  // Z direction
  WFMDiskC->createAll(System,ChopperC->getKey("Beam"),0);

  VPipeE->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->createAll(System,ChopperC->getKey("Beam"),2);

  FocusE->addInsertCell(VPipeE->getCells("Void"));
  FocusE->createAll(System,*VPipeE,0,*VPipeE,0);

  // 10m Chopper
  ChopperD->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperD->createAll(System,FocusE->getKey("Guide0"),2);

  // Double disk chopper
  FOCDiskA->addInsertCell(ChopperD->getCell("Void"));
  FOCDiskA->setCentreFlag(3);  // Z direction
  FOCDiskA->setOffsetFlag(1);  // Z direction
  FOCDiskA->createAll(System,ChopperD->getKey("Beam"),0);
  
  VPipeF->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeF->createAll(System,ChopperD->getKey("Beam"),2);

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

  if (stopPoint==3) return;                      // STOP At bunker exit

  //
  // OUTSIDE:
  //

  VPipeOutA->setFront(bunkerObj,2);
  VPipeOutA->setDivider(bunkerObj,2);
  VPipeOutA->createAll(System,FocusWall->getKey("Guide0"),2);
  
  FocusOutA->addInsertCell(VPipeOutA->getCell("Void"));
  FocusOutA->createAll(System,*VPipeOutA,7,*VPipeOutA,7);


  PitB->addInsertCell(voidCell);
  PitB->createAll(System,FocusOutA->getKey("Guide0"),2);

  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setBack(PitB->getKey("Mid"),1);
  ShieldA->setDivider(bunkerObj,2);
  ShieldA->createAll(System,*BInsert,2);
  ShieldA->insertComponent(System,"Void",*VPipeOutA);
  
  HeadRule GuideCut=
    attachSystem::unionLink(*ShieldA,{2,3,4,5,6});
  PitB->insertComponent(System,"Outer",GuideCut);
  PitB->insertComponent(System,"MidLayer",*VPipeOutA);
  PitB->insertComponent(System,"Void",*VPipeOutA);


  // First Chopper
  ChopperOutB->addInsertCell(PitB->getCells("Void"));
  ChopperOutB->createAll(System,FocusOutA->getKey("Guide0"),2);

  // Double disk chopper
  FOCDiskB->addInsertCell(ChopperOutB->getCell("Void"));
  FOCDiskB->setCentreFlag(3);  // Z direction
  FOCDiskB->setOffsetFlag(1);  // Z direction
  FOCDiskB->createAll(System,ChopperOutB->getKey("Beam"),0);
  
  ShieldB->addInsertCell(PitB->getCells("Outer"));
  ShieldB->addInsertCell(voidCell);
  ShieldB->setFront(PitB->getKey("Mid"),2);

  ShieldB->createAll(System,ChopperOutB->getKey("Beam"),2);

  VPipeOutB->addInsertCell(ShieldB->getCell("Void"));
  VPipeOutB->addInsertCell(PitB->getCells("Collet"));
  VPipeOutB->addInsertCell(PitB->getCells("MidLayer"));
  VPipeOutB->setFront(PitB->getKey("Inner"),2);
  VPipeOutB->createAll(System,ChopperOutB->getKey("Beam"),2);

  FocusOutB->addInsertCell(VPipeOutB->getCell("Void"));
  FocusOutB->createAll(System,*VPipeOutB,7,*VPipeOutB,7);

  // Mid section array:
  ShieldArray[0]->addInsertCell(voidCell);
  ShieldArray[0]->setFront(*ShieldB,2);
  ShieldArray[0]->createAll(System,*ShieldB,2);
  VPipeArray[0]->addInsertCell(ShieldArray[0]->getCell("Void"));
  VPipeArray[0]->createAll(System,*ShieldB,2);
  FocusArray[0]->addInsertCell(VPipeArray[0]->getCell("Void"));
  FocusArray[0]->createAll(System,*VPipeArray[0],7,*VPipeArray[0],7);

  for(size_t i=1;i<ShieldArray.size();i++)
    {
      ShieldArray[i]->addInsertCell(voidCell);
      ShieldArray[i]->setFront(*ShieldArray[i-1],2);
      ShieldArray[i]->createAll(System,*ShieldArray[i-1],2);
      VPipeArray[i]->addInsertCell(ShieldArray[i]->getCell("Void"));
      VPipeArray[i]->createAll(System,*ShieldArray[i-1],2);
      FocusArray[i]->addInsertCell(VPipeArray[i]->getCell("Void"));
      FocusArray[i]->createAll(System,*VPipeArray[i],7,*VPipeArray[i],7);
    }

  return;
}


}   // NAMESPACE essSystem

