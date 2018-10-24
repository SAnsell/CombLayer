/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/BALDER.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "ExternalCut.h"
#include "World.h"
#include "AttachSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LeadPipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "CrossPipe.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "FlangeMount.h"
#include "BeamMount.h"
#include "FrontEndCave.h"
#include "WallLead.h"
#include "balderFrontEnd.h"
#include "OpticsBeamline.h"
#include "ConnectZone.h"
#include "PipeShield.h"
#include "ExptBeamline.h"
#include "BALDER.h"

namespace xraySystem
{

BALDER::BALDER(const std::string& KN) :
  attachSystem::CopiedComp("Balder",KN),
  ringCaveA(new FrontEndCave(newName+"RingCaveA")),
  ringCaveB(new FrontEndCave(newName+"RingCaveB")),
  frontBeam(new balderFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new OpticsBeamline(newName+"OpticsLine")),
  joinPipeB(new constructSystem::LeadPipe(newName+"JoinPipeB")),
  pShield(new xraySystem::PipeShield(newName+"PShield")),
  nShield(new xraySystem::PipeShield(newName+"NShield")),
  outerShield(new xraySystem::PipeShield(newName+"OuterShield")),
  connectZone(new ConnectZone(newName+"Connect")),
  joinPipeC(new constructSystem::LeadPipe(newName+"JoinPipeC")),
  exptHut(new ExperimentalHutch(newName+"Expt")),
  exptBeam(new ExptBeamline(newName+"ExptLine"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ringCaveA);
  OR.addObject(ringCaveB);
  OR.addObject(frontBeam);
  OR.addObject(wallLead);
  OR.addObject(joinPipe);
  
  OR.addObject(opticsHut);
  OR.addObject(opticsBeam);
  OR.addObject(joinPipeB);
  OR.addObject(pShield);
  OR.addObject(nShield);
  OR.addObject(outerShield);
  OR.addObject(joinPipeC);
  OR.addObject(exptHut);
  OR.addObject(exptBeam);
}

BALDER::~BALDER()
  /*!
    Destructor
   */
{}

void 
BALDER::build(Simulation& System,
	      const attachSystem::FixedComp& FCOrigin,
	      const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
  */
{
  // For output stream
  ELog::RegMethod RControl("BALDER","build");

  const int voidCell(74123);
 
  ringCaveA->addInsertCell(voidCell);
  ringCaveA->createAll(System,FCOrigin,sideIndex);

  ringCaveB->addInsertCell(voidCell);
  ringCaveB->setCutSurf("front",*ringCaveA,"connectPt");
  ringCaveB->createAll(System,*ringCaveA,
		       ringCaveA->getSideIndex("connectPt"));

  frontBeam->setFront(ringCaveA->getSurf("BeamFront"));
  frontBeam->setBack(ringCaveA->getSurf("BeamInner"));
  
  const HeadRule caveVoid=ringCaveA->getCellHR(System,"Void");
  frontBeam->addInsertCell(ringCaveA->getCell("Void"));
  frontBeam->createAll(System,*ringCaveA,-1);

  wallLead->addInsertCell(ringCaveA->getCell("FrontWall"));
  wallLead->setFront(-ringCaveA->getSurf("BeamInner"));
  wallLead->setBack(-ringCaveA->getSurf("BeamOuter"));
  wallLead->createAll(System,FCOrigin,sideIndex);

  if (stopPoint=="frontEnd") return;

  opticsHut->addInsertCell(voidCell);
  opticsHut->setCutSurf("ringWall",*ringCaveB,"outerWall");
  opticsHut->createAll(System,*ringCaveA,2);

  // Ugly HACK to get the two objects to merge
  const std::string OH=opticsHut->SurfMap::getSurfString("ringFlat");
  ringCaveB->insertComponent
    (System,"OuterWall",*opticsHut,opticsHut->getSideIndex("frontCut"));
  ringCaveB->insertComponent
    (System,"FloorA",*opticsHut,opticsHut->getSideIndex("floorCut"));
  ringCaveB->insertComponent
    (System,"RoofA",*opticsHut,opticsHut->getSideIndex("roofCut"));

  joinPipe->addInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell(wallLead->getCell("Void"));
  joinPipe->addInsertCell(opticsHut->getCell("Inlet"));
  joinPipe->addInsertCell(opticsHut->getCell("BeamVoid"));
  
  joinPipe->createAll(System,*frontBeam,2);
  joinPipe->insertObjects(System);


  opticsBeam->setCell("MasterVoid",opticsHut->getCell("BeamVoid"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("innerBack",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  
  opticsBeam->setCutSurf("beam",opticsHut->getSurf("BeamTube"));  
  opticsBeam->createAll(System,*joinPipe,2);

  joinPipe->insertInCell(System,opticsBeam->getCell("OuterVoid"));

  return;
  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);

  // pipe shield goes around joinPipeB:
  pShield->addInsertCell(joinPipeB->getCell("OuterSpace"));
  pShield->setCutSurf("inner",*joinPipeB,"outerPipe");
  pShield->setCutSurf("front",*opticsHut,"innerBack");
  pShield->createAll(System,*opticsHut,opticsHut->getSideIndex("exitHole"));

  // pipe shield goes around joinPipeB:
  nShield->addInsertCell(joinPipeB->getCell("OuterSpace"));
  nShield->setCutSurf("inner",*joinPipeB,"outerPipe");
  nShield->createAll(System,*opticsHut,opticsHut->getSideIndex("exitHole"));

  System.removeCell(opticsHut->getCell("Void"));

  exptHut->addInsertCell(voidCell);
  exptHut->createAll(System,*ringCaveA,2);

  //  connectZone->registerJoinPipe(joinPipeC);
  connectZone->addInsertCell(voidCell);
  connectZone->setFront(*opticsHut,2);
  connectZone->setBack(*exptHut,1);
  connectZone->createAll(System,*joinPipeB,2);

  // horrid way to create a SECOND register space [MAKE INTERNAL]
  joinPipeB->insertInCell(System,connectZone->getCell("firstVoid"));

  // pipe shield goes around joinPipeB:

  outerShield->addInsertCell(connectZone->getCell("firstVoid"));
  outerShield->setCutSurf("inner",*joinPipeB,"outerPipe");
  outerShield->setCutSurf("front",*opticsHut,"back");
  outerShield->createAll(System,*opticsHut,
			 opticsHut->getSideIndex("-exitHole"));

  joinPipeC->insertInCell(System,exptHut->getCell("Void"));
  joinPipeC->insertInCell(System,exptHut->getCell("EnteranceHole"));

  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->createAll(System,*joinPipeC,2);

  return;
}


}   // NAMESPACE xraySystem

