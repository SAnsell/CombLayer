/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: tomowise/TOMOWISE.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#include <array>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "CopiedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"

#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "PipeShield.h"
#include "forkHoles.h"
#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "ProximityShielding.h"
#include "WallLead.h"
#include "R3Ring.h"
#include "R3FrontEnd.h"
#include "R3FrontEndToyama.h"
#include "tomowiseFrontEnd.h"
#include "tomowiseOpticsLine.h"
#include "tomowiseExptLine.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "MainBeamDump.h"
#include "LocalShielding.h"

#include "R3Beamline.h"
#include "TOMOWISE.h"

namespace xraySystem
{

TOMOWISE::TOMOWISE(const std::string& KN) :
  R3Beamline("Balder",KN),
  frontBeam(new tomowiseFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new tomowiseOpticsLine(newName+"OpticsLine")),
  exptHut(new ExperimentalHutch(newName+"ExptHut")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  collar(std::make_shared<tdcSystem::LocalShielding>(newName+"Collar")),
  beamStop(std::make_shared<tdcSystem::MainBeamDump>(newName+"BeamStop")),
  guillotine(new xraySystem::PipeShield(newName+"Guillotine")),
  exptBeam(new tomowiseExptLine(newName+"ExptLine")),
  joinPipeC(new constructSystem::VacuumPipe(newName+"JoinPipeC"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ELog::RegMethod RControl("TOMOWISE","TOMOWISE");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  OR.addObject(frontBeam);
  OR.addObject(wallLead);
  OR.addObject(joinPipe);

  OR.addObject(opticsHut);
  OR.addObject(opticsBeam);
  OR.addObject(joinPipeB);
  OR.addObject(collar);
  OR.addObject(beamStop);
  OR.addObject(exptHut);
  OR.addObject(exptBeam);
  OR.addObject(joinPipeC);
}

TOMOWISE::~TOMOWISE()
  /*!
    Destructor
   */
{}

void
TOMOWISE::build(Simulation& System,
	      const attachSystem::FixedComp& FCOrigin,
	      const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RControl("TOMOWISE","build");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1)-NS;
  const size_t SIndex=(PIndex+1) % NS;
  const size_t prevIndex=(2*NS+PIndex-1) % NS;

  const std::string exitLink="ExitCentre"+std::to_string(PIndex);

  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("REWall",-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  //  return;

  wallLead->addInsertCell(r3Ring->getCell("FrontWall",PIndex));
  wallLead->setFront(r3Ring->getSurf("BeamInner",PIndex));
  wallLead->setBack(-r3Ring->getSurf("BeamOuter",PIndex));
  wallLead->setCutSurf("Ring",r3Ring->getSurfRule("#FlatInner",PIndex));
  wallLead->createAll(System,FCOrigin,sideIndex);

  frontBeam->getProxiShieldBPipe()->insertInCell("Main", System, wallLead->getCell("Void"));
  frontBeam->getProxiShieldBPipe()->insertInCell("FlangeB", System, wallLead->getCell("Void"));
  frontBeam->getProxiShieldBPipe()->insertInCell("FlangeB", System, wallLead->getCell("SteelVoid"));

  frontBeam->getProxiShieldB()->insertInCell(System, wallLead->getCell("PreLeadVoid"));
  frontBeam->getProxiShieldB()->insertInCell(System, wallLead->getCell("Void"));
  frontBeam->getProxiShieldB()->insertInCell(System, wallLead->getCell("ExtraVoid"));

  // list of front-end stop points - no need to build geometry after them
  constexpr std::array<std::string_view, 9> frontStopPoints = {
        "frontEnd", "FM1", "FM2", "FM3", "TransPipe",
        "ChokeChamber", "U1Block", "Dipole", "HeatTable"
    };
   if (std::find(frontStopPoints.begin(), frontStopPoints.end(), stopPoint) != frontStopPoints.end())
        return;

  buildOpticsHutch(System,opticsHut,PIndex,exitLink);

  if (stopPoint=="opticsHut") return;

  joinPipe->addAllInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell("Main",wallLead->getCell("Void"));
  joinPipe->addInsertCell("FlangeA",wallLead->getCell("Void"));
  joinPipe->addInsertCell("FlangeA",wallLead->getCell("SteelVoid"));
  joinPipe->createAll(System,*frontBeam,2);



  opticsBeam->setInnerMat(opticsHut->getCellMat(System,"Void"));
  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  opticsBeam->setPreInsert(joinPipe);
  opticsBeam->createAll(System,*joinPipe,2);

  exptHut->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptHut->setCutSurf("frontWall",*opticsHut,"back");
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut->createAll(System,*opticsHut,"back");
  exptHut->splitChicane(System,1,2);

  joinPipeB->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell("Main",opticsHut->getCell("ExitHole"));
  //  joinPipeB->addAllInsertCell(r3Ring->getCell("OuterSegment", PIndex));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);

  beamStop->createAll(System,*exptHut,"#innerBack");

  collar->createAll(System, *beamStop, 0);

  // pipe shield go around joinPipeB:
  guillotine->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  guillotine->setCutSurf("inner",*joinPipeB,"outerPipe");
  guillotine->createAll(System,*opticsHut,"innerBack");

  if (stopPoint=="exptHut")
    {
      joinPipeB->insertAllInCell(System,exptHut->getCell("Void"));
      beamStop->insertAllInCell(System,exptHut->getCell("Void"));
      beamStop->insertAllInCell(System,collar->getCell("Wall"));
      collar->insertInCell(System,exptHut->getCell("Void"));
      return;
    }


  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->setCutSurf("front",*exptHut,
		       exptHut->getSideIndex("innerFront"));
  exptBeam->setCutSurf("back",*exptHut,
			 exptHut->getSideIndex("innerBack"));
  exptBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptBeam->setPreInsert(joinPipeB);
  exptBeam->createAll(System,*joinPipeB,2);



  // exptHutB->setCutSurf("floor",r3Ring->getSurf("Floor"));
  // exptHutB->setCutSurf("frontWall",*exptHut,"back");
  // exptHutB->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  // exptHutB->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  // exptHutB->createAll(System,*exptHut,"back");
  // special for portItem transfer
  // const PortChicane* PCPtr=exptHut->getPortItem(4);
  // if (PCPtr)
  //   PCPtr->insertInCell
  //     ("Main",System,r3Ring->getCell("OuterSegment",PIndex));

  // joinPipeC->addAllInsertCell(exptBeam->getCell("LastVoid"));
  // joinPipeC->addInsertCell("Main",exptHut->getCell("ExitHole"));
  // joinPipeC->setFront(*exptBeam,2);
  // joinPipeC->createAll(System,*exptBeam,2);

  return;
}


}   // NAMESPACE xraySystem
