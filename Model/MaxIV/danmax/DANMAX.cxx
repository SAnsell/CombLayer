/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: danmax/DANMAX.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and Konstantin Batkov
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
#include "NameStack.h"
#include "RegMethod.h"
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
#include "AttachSupport.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"

#include "GeneralPipe.h"
#include "VacuumPipe.h"

#include "forkHoles.h"
#include "XRayHutchBase.h"
#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "OpticsStepHutch.h"
#include "ProximityShielding.h"
#include "WallLead.h"
#include "R3FrontEnd.h"
#include "R3FrontEndToyamaDanMAX.h"
#include "danmaxFrontEnd.h"
#include "danmaxOpticsLine.h"
#include "danmaxConnectLine.h"
#include "PipeShield.h"
#include "balderExptLine.h"

#include "R3Ring.h"
#include "R3Beamline.h"
#include "DANMAX.h"

namespace xraySystem
{

DANMAX::DANMAX(const std::string& KN) :
  R3Beamline("Balder",KN),
  frontBeam(new danmaxFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsStepHutch(newName+"OpticsHut")),
  opticsBeam(new danmaxOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  guillotineOHToEH2(new xraySystem::PipeShield(newName+"GuillotineOHToEH2")),
  connectUnit(new danmaxConnectLine(newName+"ConnectUnit")),
  joinPipeC(new constructSystem::VacuumPipe(newName+"JoinPipeC")),
  exptHut1(new xraySystem::ExperimentalHutch(newName+"ExptHut1")),
  exptHut2(new xraySystem::ExperimentalHutch(newName+"ExptHut2")),
  guillotine(new xraySystem::PipeShield(newName+"Guillotine")),
  exptBeam(new balderExptLine(newName+"ExptLine"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ELog::RegMethod RegA("DANMAX","DANMAX(constructor)");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(frontBeam);
  OR.addObject(wallLead);
  OR.addObject(joinPipe);
  OR.addObject(opticsHut);
  OR.addObject(opticsBeam);
  OR.addObject(joinPipeB);
  OR.addObject(guillotineOHToEH2);
  OR.addObject(connectUnit);
  OR.addObject(joinPipeC);
  OR.addObject(exptHut1);
  OR.addObject(exptHut2);
  OR.addObject(guillotine);

}

DANMAX::~DANMAX()
  /*!
    Destructor
   */
{}

void
DANMAX::build(Simulation& System,
	      const attachSystem::FixedComp& FCOrigin,
	      const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: R3Ring position
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RControl("DANMAX","build");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1)-NS;
  const size_t SIndex=(PIndex+1) % NS;
  const size_t prevIndex=(2*NS+PIndex-1) % NS;

  const std::string exitLink="ExitCentre"+std::to_string(PIndex);

  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("REWall",-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->deactivateFM3();
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  wallLead->addInsertCell(r3Ring->getCell("RatchetEndWall",PIndex));
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

  if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

  buildOpticsHutch(System,opticsHut,PIndex,exitLink);

  // Inner space

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
  opticsBeam->setCutSurf("BackPlateFloorShine",opticsHut->getFullRule("#BackPlateFloorShine"));
  opticsBeam->setPreInsert(joinPipe);
  opticsBeam->setFrontEnd(frontBeam);
  opticsBeam->createAll(System,*joinPipe,2);

  if (stopPoint=="opticsHutch") return;

  joinPipeB->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell("Main",opticsHut->getCell("ExitHole"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);

  guillotineOHToEH2->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  guillotineOHToEH2->setCutSurf("inner",*joinPipeB,"outerPipe");
  guillotineOHToEH2->createAll(System,*opticsHut,"innerBack");

  exptHut2->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptHut2->setCutSurf("frontWall",*opticsHut,"back");
  exptHut2->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut2->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut2->createAll(System,*opticsHut,"back");

  // two ways to insert BackPlateOuter into expt. hutch2:
  const std::set<std::string> cells = {
    "Void", "FloorShineRingWallVoid", "InnerSkinRingWall", "LeadRingWall",
    "OuterSkinRingWall", "OuterRightVoid"};
  for (const auto& c : cells)
    opticsHut->insertInCell("BackPlateOuter", System, exptHut2->getCell(c));
  // the same but ~x10 slower (~2.5 ms vs 250 us) since needs to check all cells:
  //  attachSystem::addToInsertSurfCtrl(System, *exptHut2, opticsHut->getCC("BackPlateOuter"));

  connectUnit->setExptHut2(exptHut2);
  connectUnit->registerJoinPipe(joinPipeC);
  connectUnit->setInsertCell(
    {
      exptHut2->getCell("Void"),
      exptHut2->getCell("FloorShineRingWallVoid"),
    }
  );
  connectUnit->setFront(*opticsHut, "backPlateOuter");
  connectUnit->setBack(*exptHut2,"innerBack");
  connectUnit->createAll(System,*joinPipeB,"back");

  joinPipeB->insertAllInCell(System,connectUnit->getCell("FirstVoid"));

  if (stopPoint=="exptHut2") return;

  exptHut1->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptHut1->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut1->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut1->createAll(System,*exptHut2,"back");

  joinPipeC->insertAllInCell(System,exptHut1->getCell("Void"));
  joinPipeC->insertInCell("Main",System,exptHut1->getCell("EntranceHole"));

  // TODO: The following guillotine probably does not exist, therefore the code is
  // commented.
  // guillotine->addAllInsertCell(
  //   {
  //     exptHut2->getCell("Void"),
  //     connectUnit->getConnectShieldCell("BackOuterVoid"),
  //     connectUnit->getConnectShieldCell("BackOuterSkin"),
  //     connectUnit->getConnectShieldCell("BackWall"),
  //     connectUnit->getConnectShieldCell("BackInnerSkin"),
  //     connectUnit->getBuildZone().getLastCell("Unit")
  //   }
  // );
  // guillotine->setCutSurf("inner",*joinPipeC,"outerPipe");
  // guillotine->createAll(System,*exptHut1,"front");

  exptBeam->addInsertCell(exptHut1->getCell("Void"));
  exptBeam->createAll(System,*joinPipeC,2);

  return;
}


}   // NAMESPACE xraySystem
