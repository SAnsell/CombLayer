/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LOKI.cxx
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
#include "LOKI.h"

namespace essSystem
{

LOKI::LOKI() :
  lokiAxis(new attachSystem::FixedComp("lokiAxis",2)),
  BendA(new beamlineSystem::GuideLine("lokiBA")),
  VacBoxA(new constructSystem::VacuumBox("lokiVacA")),
  GuideA(new beamlineSystem::GuideLine("lokiGA")),
  DDisk(new constructSystem::DiskChopper("lokiDBlade")),
  GuideInner(new beamlineSystem::GuideLine("lokiGInner")),
  SDisk(new constructSystem::DiskChopper("lokiSBlade")),
  GuideB(new beamlineSystem::GuideLine("lokiGB")),
  BendB(new beamlineSystem::GuideLine("lokiBB")),
  GuideC(new beamlineSystem::GuideLine("lokiGC")),
  VacBoxB(new constructSystem::VacuumBox("lokiVacB")),
  SingleDisk(new constructSystem::DiskChopper("loki10mBlade")),
  GuideD(new beamlineSystem::GuideLine("lokiGD")),
  VacBoxC(new constructSystem::VacuumBox("lokiVacC")),
  D12mDisk(new constructSystem::DiskChopper("lokiDDisk")),
  Guide12mInter(new beamlineSystem::GuideLine("lokiG12mI")),
  S12mDisk(new constructSystem::DiskChopper("lokiSDisk")),
  GuideE(new beamlineSystem::GuideLine("lokiGE")),
  GridA(new constructSystem::RotaryCollimator("lokiGridA")),
  CollA(new constructSystem::RotaryCollimator("lokiCollA")),
  GridB(new constructSystem::RotaryCollimator("lokiGridB")),
  CollB(new constructSystem::RotaryCollimator("lokiCollB")),
  GridC(new constructSystem::RotaryCollimator("lokiGridC")),
  CollC(new constructSystem::RotaryCollimator("lokiCollC")),
  GridD(new constructSystem::RotaryCollimator("lokiGridD")),
  GuideCollA(new beamlineSystem::GuideLine("lokiGuideCA")),
  GuideCollB(new beamlineSystem::GuideLine("lokiGuideCB")),
  GuideCollC(new beamlineSystem::GuideLine("lokiGuideCC")),
  Cave(new LokiHut("lokiCave")),
  CaveGuide(new beamlineSystem::GuideLine("lokiCaveGuide")),
  VTank(new VacTank("lokiVTank"))
  /*!
    Constructor
 */
{
  ELog::RegMethod RegA("LOKI","LOKI");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell("lokiAxis");
  OR.addObject(lokiAxis);

  OR.addObject(BendA);
  OR.addObject(VacBoxA);
  OR.addObject(GuideA);
  OR.addObject(DDisk);
  OR.addObject(GuideInner);
  OR.addObject(SDisk);
  OR.addObject(GuideB);
  OR.addObject(BendB);
  OR.addObject(GuideC);
  OR.addObject(VacBoxB);
  OR.addObject(SingleDisk);
  OR.addObject(GuideD);
  OR.addObject(VacBoxC);
  OR.addObject(D12mDisk);
  OR.addObject(Guide12mInter);
  OR.addObject(S12mDisk);
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
}



LOKI::~LOKI()
  /*!
    Destructor
  */
{}

void
LOKI::setBeamAxis(const GuideItem& GItem)
  /*!
    Set the primary direction object
   */
{
  ELog::RegMethod RegA("LOKI","setBeamAxis");

  lokiAxis->createUnitVector(GItem);
  lokiAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  lokiAxis->setLinkCopy(1,GItem.getKey("Main"),1);

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
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("LOKI","build");
  ELog::EM<<"Building LOKI on : "<<GItem.getKeyName()<<ELog::endDiag;

  setBeamAxis(GItem);
  
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->addInsertCell(bunkerObj.getCell("MainVoid"));
  //  BendA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  BendA->createAll(System,GItem.getKey("Beam"),-1,
		   GItem.getKey("Beam"),-1);
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

  // First straight section
  GuideInner->addInsertCell(VacBoxA->getCells("Void"));
  GuideInner->createAll(System,DDisk->getKey("Beam"),2,
			DDisk->getKey("Beam"),2);

  // Single disk chopper
  SDisk->addInsertCell(VacBoxA->getCells("Void"));
  SDisk->setCentreFlag(3);  // Z direction
  SDisk->createAll(System,GuideInner->getKey("Guide0"),2);


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

    // Straight section after single 10m chopper
  Guide12mInter->addInsertCell(VacBoxC->getCells("Void"));
  Guide12mInter->createAll(System,D12mDisk->getKey("Beam"),2,
			   D12mDisk->getKey("Beam"),2);

  // Single disk chopper
  S12mDisk->addInsertCell(VacBoxC->getCells("Void"));
  S12mDisk->setCentreFlag(3);  // Z direction
  S12mDisk->createAll(System,Guide12mInter->getKey("Guide0"),2);

  // Straight section leaving chopper pit 3.
  GuideE->addInsertCell(VacBoxC->getCells("Void"));
  GuideE->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideE->createAll(System,S12mDisk->getKey("Beam"),2,
		    S12mDisk->getKey("Beam"),2);

  // First grid cutter
  GridA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GridA->createAll(System,GuideE->getKey("Guide0"),2);

  // First collimator
  CollA->setInsertCell(bunkerObj.getCells("MainWall8"));
  CollA->addInsertCell(bunkerObj.getCell("MainVoid"));
  CollA->addInsertCell(voidCell);
  CollA->createAll(System,GridA->getKey("Beam"),2);
  // Second grid cutter
  GridB->addInsertCell(voidCell);
  GridB->createAll(System,CollA->getKey("Beam"),2);

  // First collimator
  CollB->addInsertCell(voidCell);
  CollB->createAll(System,GridB->getKey("Beam"),2);
  // Second grid cutter
  GridC->addInsertCell(voidCell);
  GridC->createAll(System,CollB->getKey("Beam"),2);

  // Last collimator block
  CollC->addInsertCell(voidCell);
  CollC->createAll(System,GridC->getKey("Beam"),2);


  // GUIDE for Collimator blocks:
  GuideCollA->addInsertCell(CollA->getCell("Main"));
  GuideCollA->addEndCut(CollA->getKey("Beam").getSignedLinkString(-2));
  GuideCollA->createAll(System,CollA->getKey("Beam"),-1,
			CollA->getKey("Beam"),-1);

  // GUIDE for Collimator blocks [B]:
  GuideCollB->addInsertCell(CollB->getCell("Main"));
  GuideCollB->addEndCut(CollB->getKey("Beam").getSignedLinkString(-2));
  GuideCollB->createAll(System,CollB->getKey("Beam"),-1,
			CollB->getKey("Beam"),-1);

  // GUIDE for Collimator blocks [C]:
  GuideCollC->addInsertCell(CollC->getCell("Main"));
  GuideCollC->addEndCut(CollC->getKey("Beam").getSignedLinkString(-2));
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


  return;
}


}   // NAMESPACE essSystem

