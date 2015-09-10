/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DREAM.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "LayerComp.h"
#include "CellMap.h"
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
#include "DHut.h"
#include "DetectorTank.h"
#include "CylSample.h"

#include "DREAM.h"

namespace essSystem
{

DREAM::DREAM() :
  dreamAxis(new attachSystem::FixedComp("dreamAxis",4)),
  FocusA(new beamlineSystem::GuideLine("dreamFA")),
  VacBoxA(new constructSystem::VacuumBox("dreamVacA")),
  VPipeA(new constructSystem::VacuumPipe("dreamPipeA")),
  FocusB(new beamlineSystem::GuideLine("dreamFB")),
  DDisk(new constructSystem::DiskChopper("dreamDBlade")),
  DDiskHouse(new constructSystem::ChopperHousing("dreamDBladeHouse")),
  SDisk(new constructSystem::DiskChopper("dreamSBlade")),
  SDiskHouse(new constructSystem::ChopperHousing("dreamSBladeHouse")),
  T0DiskA(new constructSystem::DiskChopper("dreamT0DiskA")),

  T0DiskAHouse(new constructSystem::ChopperHousing("dreamT0DiskAHouse")),
  VacBoxB(new constructSystem::VacuumBox("dreamVacB")),
  VPipeB(new constructSystem::VacuumPipe("dreamPipeB")),
  T0DiskB(new constructSystem::DiskChopper("dreamT0DiskB")),  
  T0DiskBHouse(new constructSystem::ChopperHousing("dreamT0DiskBHouse")),

  FocusC(new beamlineSystem::GuideLine("dreamFC"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("DREAM","DREAM");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary:
  OR.cell("dreamAxis");
  OR.addObject(dreamAxis);

  OR.addObject(FocusA);
  OR.addObject(VacBoxA);
  OR.addObject(VPipeA);
  
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(DDisk);
  OR.addObject(DDiskHouse);  
  OR.addObject(SDisk);  
  OR.addObject(SDiskHouse);

  OR.addObject(T0DiskA);
  OR.addObject(T0DiskB);  
  OR.addObject(T0DiskAHouse);
  OR.addObject(T0DiskBHouse);
  OR.addObject(FocusC);

}

DREAM::~DREAM()
  /*!
    Destructor
  */
{}

void
DREAM::setBeamAxis(const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("DREAM","setBeamAxis");

  dreamAxis->createUnitVector(GItem);
  dreamAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  dreamAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  dreamAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  dreamAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    dreamAxis->reverseZ();
  return;
}
  
void 
DREAM::build(Simulation& System,
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
  ELog::RegMethod RegA("DREAM","build");

  ELog::EM<<"\nBuilding DREAM on : "<<GItem.getKeyName()<<ELog::endDiag;

  setBeamAxis(GItem,0);
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  FocusA->createAll(System,GItem.getKey("Beam"),-1,
		    GItem.getKey("Beam"),-1);
  
  // First section out of monolyth
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,FocusA->getKey("Guide0"),2);
  // PIPE

  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->setFront(GItem.getKey("Beam"),2);
  VPipeA->setBack(*VacBoxA,1);
  VPipeA->createAll(System,GItem.getKey("Beam"),2);

  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->addInsertCell(VacBoxA->getCells("Void"));
  FocusB->createAll(System,FocusA->getKey("Guide0"),2,
		    FocusA->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(VacBoxA->getCell("Void",0));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->createAll(System,FocusB->getKey("Guide0"),2);
  // Double disk chopper housing
  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  DDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  DDiskHouse->createAll(System,DDisk->getKey("Main"),0);
  DDiskHouse->insertComponent(System,"Void",*DDisk);

  // Double disk chopper
  SDisk->addInsertCell(VacBoxA->getCell("Void",0));
  SDisk->setCentreFlag(-3);  // Z direction
  SDisk->createAll(System,DDisk->getKey("Beam"),2);

  // Double disk chopper housing
  SDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  SDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  SDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  SDiskHouse->createAll(System,SDisk->getKey("Main"),0);
  
  SDiskHouse->insertComponent(System,"Void",*SDisk);

  // Double disk T0 chopper
  T0DiskA->addInsertCell(VacBoxA->getCell("Void",0));
  T0DiskA->setCentreFlag(3);  // Z direction
  T0DiskA->createAll(System,SDisk->getKey("Beam"),2);

    // Double disk chopper housing
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Void"));
  T0DiskAHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  T0DiskAHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0DiskAHouse->createAll(System,T0DiskA->getKey("Main"),0);
  T0DiskAHouse->insertComponent(System,"Void",*T0DiskA);

  // Secon T0 system
  VacBoxB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxB->createAll(System,T0DiskA->getKey("Beam"),2);


  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VacBoxA,2);
  VPipeB->setBack(*VacBoxB,1);
  VPipeB->createAll(System,*VacBoxA,2);

  // PIPE

  // offset disk
  T0DiskB->addInsertCell(VacBoxB->getCell("Void",0));
  T0DiskB->setCentreFlag(3);  // Z direction
  T0DiskB->createAll(System,SDisk->getKey("Beam"),2);

  // T0 Part B chopper housing
  T0DiskBHouse->addInsertCell(VacBoxB->getCells("Void"));
  T0DiskBHouse->addInsertCell(VacBoxB->getCells("Box"));  // soon to become lid
  T0DiskBHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0DiskBHouse->createAll(System,T0DiskB->getKey("Main"),0);
  T0DiskBHouse->insertComponent(System,"Void",*T0DiskB);
  

  FocusC->addInsertCell(VPipeB->getCells("Void"));
  FocusC->addInsertCell(VacBoxA->getCells("Void"));
  FocusC->addInsertCell(VacBoxB->getCells("Void"));
  FocusC->createAll(System,T0DiskA->getKey("Beam"),2,
		    T0DiskA->getKey("Beam"),2);

  return;
}


}   // NAMESPACE essSystem

