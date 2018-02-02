/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKI.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
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
#include "Hut.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"
#include "PinHole.h"
#include "RentrantBS.h"
#include "LokiHut.h"
#include "VacTank.h"

#include "longLOKI.h"


namespace essSystem
{

longLOKI::longLOKI() : 
  lokiAxis(new attachSystem::FixedOffset("longLokiAxis",4)),
  BendA(new beamlineSystem::GuideLine("longLokiBA")),
  VacBoxA(new constructSystem::VacuumBox("longLokiVacA")),
  GuideA(new beamlineSystem::GuideLine("longLokiGA")),
  DDisk(new constructSystem::DiskChopper("longLokiDBlade")),
  DDiskHouse(new constructSystem::ChopperHousing("longLokiDBladeHouse")),
  GuideInner(new beamlineSystem::GuideLine("longLokiGInner")),
  SDisk(new constructSystem::DiskChopper("longLokiSBlade")),
  SDiskHouse(new constructSystem::ChopperHousing("longLokiSBladeHouse")),
  GuideB(new beamlineSystem::GuideLine("longLokiGB")),
  BendB(new beamlineSystem::GuideLine("longLokiBB")),
  GuideC(new beamlineSystem::GuideLine("longLokiGC")),
  VacBoxB(new constructSystem::VacuumBox("longLokiVacB")),
  SingleDisk(new constructSystem::DiskChopper("longLoki10mBlade")),
  SingleDiskHouse(new constructSystem::ChopperHousing("longLoki10mBladeHouse")),
  GuideD(new beamlineSystem::GuideLine("longLokiGD")),
  VacBoxC(new constructSystem::VacuumBox("longLokiVacC")),
  D12mDisk(new constructSystem::DiskChopper("longLokiDDisk")),
  D12mDiskHouse(new constructSystem::ChopperHousing("longLokiDDiskHouse")),
  Guide12mInter(new beamlineSystem::GuideLine("longLokiG12mI")),
  S12mDisk(new constructSystem::DiskChopper("longLokiSDisk")),
  S12mDiskHouse(new constructSystem::ChopperHousing("longLokiSDiskHouse")),
  GuideE(new beamlineSystem::GuideLine("longLokiGE")),
  GridA(new constructSystem::RotaryCollimator("longLokiGridA")),
  CollA(new constructSystem::RotaryCollimator("longLokiCollA")),
  GridB(new constructSystem::RotaryCollimator("longLokiGridB")),
  CollB(new constructSystem::RotaryCollimator("longLokiCollB")),
  GridC(new constructSystem::RotaryCollimator("longLokiGridC")),
  CollC(new constructSystem::RotaryCollimator("longLokiCollC")),
  GridD(new constructSystem::RotaryCollimator("longLokiGridD")),
  GuideCollA(new beamlineSystem::GuideLine("longLokiGuideCA")),
  GuideCollB(new beamlineSystem::GuideLine("longLokiGuideCB")),
  GuideCollC(new beamlineSystem::GuideLine("longLokiGuideCC")),
  Cave(new LokiHut("longLokiCave")),
  CaveGuide(new beamlineSystem::GuideLine("longLokiCaveGuide")),
  VTank(new VacTank("longLokiVTank")),
  VPipeB(new constructSystem::VacuumPipe("longLokiPipeB")),
  VPipeC(new constructSystem::VacuumPipe("longLokiPipeC")),
  VPipeD(new constructSystem::VacuumPipe("longLokiPipeD"))
  /*!
    Constructor
 */
{
  ELog::RegMethod RegA("longLOKI","longLOKI");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell("longLokiAxis");
  OR.addObject(lokiAxis);

  OR.addObject(BendA);
  OR.addObject(VacBoxA);
  OR.addObject(GuideA);
  OR.addObject(DDisk);
  OR.addObject(DDiskHouse);
  OR.addObject(GuideInner);
  OR.addObject(SDisk);
  OR.addObject(SDiskHouse);
  OR.addObject(GuideB);
  OR.addObject(BendB);
  OR.addObject(GuideC);
  OR.addObject(VacBoxB);
  OR.addObject(SingleDisk);
  OR.addObject(SingleDiskHouse);
  OR.addObject(GuideD);
  OR.addObject(VacBoxC);
  OR.addObject(D12mDisk);
  OR.addObject(D12mDiskHouse);
  OR.addObject(Guide12mInter);
  OR.addObject(S12mDisk);
  OR.addObject(S12mDiskHouse);
  OR.addObject(GuideE);

  OR.addObject(GridA);
  OR.addObject(CollA);
  OR.addObject(GridB);
  OR.addObject(CollB);
  OR.addObject(GridC);
  OR.addObject(CollC);
  OR.addObject(GridD);

  OR.addObject(GuideCollA);
  OR.addObject(GuideCollB);
  OR.addObject(GuideCollC);

  OR.addObject(Cave);
  OR.addObject(CaveGuide);
  OR.addObject(VTank);

  OR.addObject(VPipeB);
  OR.addObject(VPipeC);
  OR.addObject(VPipeD);
}



longLOKI::~longLOKI()
  /*!
    Destructor
  */
{}

void
longLOKI::setBeamAxis(const FuncDataBase& Control,
		  const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Data base of info on variables
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("longLOKI","setBeamAxis");

  lokiAxis->populate(Control);
  lokiAxis->createUnitVector(GItem);
  lokiAxis->setLinkSignedCopy(0,GItem.getKey("Main"),1);
  lokiAxis->setLinkSignedCopy(1,GItem.getKey("Main"),2);

  lokiAxis->setLinkSignedCopy(2,GItem.getKey("Beam"),1);
  lokiAxis->setLinkSignedCopy(3,GItem.getKey("Beam"),2);
  // BEAM needs to be rotated:
  lokiAxis->linkAngleRotate(3);
  lokiAxis->linkAngleRotate(4);
  
    if (reverseZ)
      lokiAxis->reverseZ();
  return;
}
  
void 
longLOKI::build(Simulation& System,
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
  ELog::RegMethod RegA("longLOKI","build");
  ELog::EM<<"\nBuilding longLOKI on : "<<GItem.getKeyName()<<ELog::endDiag;

  setBeamAxis(System.getDataBase(),GItem,0);
  ELog::EM<<"SET LOKI AXIS"<<ELog::endDiag;
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->addInsertCell(bunkerObj.getCell("MainVoid"));
  BendA->createAll(System,*lokiAxis,-3,*lokiAxis,-3); // beam front reversed
  
  // First straight section
  VacBoxA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxA->createAll(System,BendA->getKey("Guide0"),2);

  // First straight section
  GuideA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideA->addInsertCell(VacBoxA->getCells("Void"));
  GuideA->createAll(System,BendA->getKey("Guide0"),2,
		    BendA->getKey("Guide0"),2);
  // Double disk chopper
  DDisk->addInsertCell(VacBoxA->getCells("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->createAll(System,GuideA->getKey("Guide0"),2);

  // Double disk chopper housing
  DDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  DDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  DDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  DDiskHouse->createAll(System,DDisk->getKey("Main"),0);
  DDiskHouse->insertComponent(System,"Void",*DDisk);

  // First straight section
  GuideInner->addInsertCell(VacBoxA->getCells("Void"));
  GuideInner->createAll(System,DDisk->getKey("Beam"),2,
			DDisk->getKey("Beam"),2);

  // Single disk chopper
  SDisk->addInsertCell(VacBoxA->getCells("Void"));
  SDisk->setCentreFlag(3);  // Z direction
  SDisk->createAll(System,GuideInner->getKey("Guide0"),2);


  // Single disk chopper housing
  SDiskHouse->addInsertCell(VacBoxA->getCells("Void"));
  SDiskHouse->addInsertCell(VacBoxA->getCells("Box"));  // soon to become lid
  SDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  SDiskHouse->createAll(System,SDisk->getKey("Main"),0);
  SDiskHouse->insertComponent(System,"Void",*SDisk);

  
  GuideB->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideB->addInsertCell(VacBoxA->getCells("Void"));
  GuideB->createAll(System,SDisk->getKey("Beam"),2,
		    SDisk->getKey("Beam"),2);


  BendB->addInsertCell(bunkerObj.getCell("MainVoid"));
  BendB->createAll(System,GuideB->getKey("Guide0"),2,
		   GuideB->getKey("Guide0"),2);

  // Vac box for 10m chopper
  VacBoxB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxB->createAll(System,BendB->getKey("Guide0"),2);

  BendB->addInsertCell(VacBoxB->getCells("Void"));
  BendB->insertObjects(System);
  
  // Straigh section to single 10m chopper
  GuideC->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideC->addInsertCell(VacBoxB->getCells("Void"));
  GuideC->createAll(System,BendB->getKey("Guide0"),2,
		    BendB->getKey("Guide0"),2);

  // Single disk chopper
  SingleDisk->addInsertCell(VacBoxB->getCells("Void"));
  SingleDisk->setCentreFlag(3);  // Z direction
  SingleDisk->createAll(System,GuideC->getKey("Guide0"),2);

    // Single disk chopper housing
  SingleDiskHouse->addInsertCell(VacBoxB->getCells("Void"));
  SingleDiskHouse->addInsertCell(VacBoxB->getCells("Box"));  
  SingleDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  SingleDiskHouse->createAll(System,SingleDisk->getKey("Main"),0);
  SingleDiskHouse->insertComponent(System,"Void",*SingleDisk);

  // Straight section after single 10m chopper
  GuideD->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideD->addInsertCell(VacBoxB->getCells("Void"));
  GuideD->createAll(System,SingleDisk->getKey("Beam"),2,
		    SingleDisk->getKey("Beam"),2);

  // Vac box for 12m chopper
  VacBoxC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBoxC->createAll(System,GuideD->getKey("Guide0"),2);
  GuideD->addInsertCell(VacBoxC->getCells("Void"));
  GuideD->insertObjects(System);

  // Doulbe disk chopper
  D12mDisk->addInsertCell(VacBoxC->getCells("Void"));
  D12mDisk->setCentreFlag(3);  // Z direction
  D12mDisk->createAll(System,GuideD->getKey("Guide0"),2);

  // D12m disk chopper housing
  D12mDiskHouse->addInsertCell(VacBoxC->getCells("Void"));
  D12mDiskHouse->addInsertCell(VacBoxC->getCells("Box"));  
  D12mDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  D12mDiskHouse->createAll(System,D12mDisk->getKey("Main"),0);
  D12mDiskHouse->insertComponent(System,"Void",*D12mDisk);

    // Straight section after single 10m chopper
  Guide12mInter->addInsertCell(VacBoxC->getCells("Void"));
  Guide12mInter->createAll(System,D12mDisk->getKey("Beam"),2,
			   D12mDisk->getKey("Beam"),2);

  // Single disk chopper
  S12mDisk->addInsertCell(VacBoxC->getCells("Void"));
  S12mDisk->setCentreFlag(3);  // Z direction
  S12mDisk->createAll(System,Guide12mInter->getKey("Guide0"),2);

  // S12m disk chopper housing
  S12mDiskHouse->addInsertCell(VacBoxC->getCells("Void"));
  S12mDiskHouse->addInsertCell(VacBoxC->getCells("Box"));  
  S12mDiskHouse->addInsertCell(bunkerObj.getCell("MainVoid"));
  S12mDiskHouse->createAll(System,S12mDisk->getKey("Main"),0);
  S12mDiskHouse->insertComponent(System,"Void",*S12mDisk);

  // Straight section leaving chopper pit 3.
  GuideE->addInsertCell(VacBoxC->getCells("Void"));
  GuideE->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideE->createAll(System,S12mDisk->getKey("Beam"),2,
		    S12mDisk->getKey("Beam"),2);

  // First grid cutter
  GridA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GridA->createAll(System,GuideE->getKey("Guide0"),2);

  // First collimator [In WALL]

  const attachSystem::FixedComp& GFC(GridA->getKey("Beam"));

  CollA->addInsertCell(bunkerObj.getCell("MainVoid"));
  CollA->addInsertCell(voidCell);
  CollA->createAll(System,GFC,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"MainWall",*CollA);  
  
  // Second grid cutter
  //GridB->addInsertCell(bunkerObj.getCells(BSector));
  GridB->createAll(System,CollA->getKey("Beam"),2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"MainWall",*GridB);
  
  // Second collimator
  CollB->addInsertCell(voidCell);
  CollB->createAll(System,GridB->getKey("Beam"),2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"MainWall",*CollB);
  // Second grid cutter
  GridC->addInsertCell(voidCell);
  GridC->createAll(System,CollB->getKey("Beam"),2);

  // Last collimator block
  CollC->addInsertCell(voidCell);
  CollC->createAll(System,GridC->getKey("Beam"),2);


  // GUIDE for Collimator blocks:
  GuideCollA->addInsertCell(CollA->getCell("Main"));
  GuideCollA->setBack(CollA->getKey("Beam"),-2);
  GuideCollA->createAll(System,CollA->getKey("Beam"),-1,
			CollA->getKey("Beam"),-1);

  // GUIDE for Collimator blocks [B]:
  GuideCollB->addInsertCell(CollB->getCell("Main"));
  GuideCollB->setBack(CollB->getKey("Beam"),-2);
  GuideCollB->createAll(System,CollB->getKey("Beam"),-1,
			CollB->getKey("Beam"),-1);

  // GUIDE for Collimator blocks [C]:
  GuideCollC->addInsertCell(CollC->getCell("Main"));
  GuideCollC->setBack(CollC->getKey("Beam"),-2);
  GuideCollC->createAll(System,CollC->getKey("Beam"),-1,
			CollC->getKey("Beam"),-1);


  Cave->addInsertCell(voidCell);
  Cave->createAll(System,GuideCollC->getKey("Guide0"),2);

  CollC->addInsertCell(Cave->getCells("FrontWall"));
  CollC->addInsertCell(Cave->getCell("Void"));
  CollC->insertObjects(System);

    // Final definin apperature
  GridD->addInsertCell(Cave->getCell("Void"));
  GridD->createAll(System,CollC->getKey("Beam"),2);

      // Final definive apperature
  CaveGuide->addInsertCell(Cave->getCell("Void"));
  CaveGuide->createAll(System,GridD->getKey("Beam"),2,
		       GridD->getKey("Beam"),2);
  
  // Vacuum tank
  VTank->addInsertCell(Cave->getCell("Void"));
  VTank->createAll(System,CaveGuide->getKey("Guide0"),2);


  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VacBoxA,2);
  VPipeB->createAll(System,*VacBoxA,2);
  VPipeB->insertComponent(System,"Void",*GuideB);
  VPipeB->insertComponent(System,"Void",*BendB);  

    // Vacuum Pipes
  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->setFront(*VacBoxB,1);
  VPipeC->setBack(*VPipeB,2);
  VPipeC->createAll(System,*VacBoxB,1);
  VPipeC->insertComponent(System,"Void",*BendB);  

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->setFront(*VacBoxB,2);
  VPipeD->setBack(*VacBoxC,1);
  VPipeD->createAll(System,*VacBoxB,1);
  VPipeD->insertComponent(System,"Void",*GuideD);  

  return;
}


}   // NAMESPACE essSystem

