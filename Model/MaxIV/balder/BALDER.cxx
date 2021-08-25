/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/BALDER.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "LeadPipe.h"

#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "WallLead.h"
#include "R3FrontEnd.h"
#include "balderFrontEnd.h"
#include "balderOpticsLine.h"
#include "balderConnectZone.h"
#include "PipeShield.h"
#include "balderExptLine.h"

#include "R3Ring.h"
#include "R3Beamline.h"
#include "BALDER.h"

namespace xraySystem
{

BALDER::BALDER(const std::string& KN) :
  R3Beamline("Balder",KN),
  frontBeam(new balderFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new balderOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::LeadPipe(newName+"JoinPipeB")),
  pShield(new xraySystem::PipeShield(newName+"PShield")),
  nShield(new xraySystem::PipeShield(newName+"NShield")),
  outerShield(new xraySystem::PipeShield(newName+"OuterShield")),
  connectZone(new balderConnectZone(newName+"Connect")),
  joinPipeC(new constructSystem::LeadPipe(newName+"JoinPipeC")),
  exptHut(new ExperimentalHutch(newName+"ExptHut")),
  exptBeam(new balderExptLine(newName+"ExptLine"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

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
    \param FCOrigin :: R3Ring position
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RControl("BALDER","build");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1);
  const size_t SIndex=(PIndex+1) % NS;
  const size_t prevIndex=(NS+PIndex-1) % NS;
  
  const std::string exitLink="ExitCentre"+std::to_string(PIndex);

  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("REWall",-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->deactivateFM3();
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  wallLead->addInsertCell(r3Ring->getCell("FrontWall",PIndex));
  wallLead->setFront(r3Ring->getSurf("BeamInner",PIndex));
  wallLead->setBack(-r3Ring->getSurf("BeamOuter",PIndex));    
  wallLead->createAll(System,FCOrigin,sideIndex);
  
  if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

  buildOpticsHutch(System,opticsHut,PIndex,exitLink);

  // Inner space

  if (stopPoint=="opticsHut") return;

  joinPipe->addAllInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell("Main",wallLead->getCell("Void"));
  joinPipe->createAll(System,*frontBeam,2);

    // new
  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  opticsBeam->setPreInsert(joinPipe);
  opticsBeam->createAll(System,*joinPipe,2);

  if (stopPoint=="opticsBeam") return;

  joinPipeB->addInsertCell("FlangeA",opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell("Main",opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell("Main",opticsHut->getCell("ExitHole"));
  joinPipeB->setCutSurf("front",*opticsBeam,"back");
  joinPipeB->createAll(System,*opticsBeam,2);


  //  exptHut->addInsertCell(voidCell);

  exptHut->setCutSurf("floor",r3Ring->getSurf("Floor"));

  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut->createAll(System,*r3Ring,r3Ring->getSideIndex(exitLink));


  connectZone->registerJoinPipe(joinPipeC);
  connectZone->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  connectZone->setCutSurf("front",*opticsHut,2);
  connectZone->setCutSurf("back",*exptHut,1);
  connectZone->createAll(System,*joinPipeB,2);


    
  joinPipeC->insertAllInCell(System,exptHut->getCell("Void"));
  //  joinPipeC->insertInCell(System,exptHut->getCell("EntranceHole"));

  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->createAll(System,*joinPipeC,2);


  // pipe shield goes around joinPipeB:
  pShield->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  pShield->setCutSurf("inner",*joinPipeB,"outer");
  pShield->createAll(System,*opticsHut,"#exitHole0");

  // pipe shield goes around joinPipeB:
  ELog::EM<<"Cell = "<<connectZone->getCell("FirstVoid")<<ELog::endDiag;
  nShield->addAllInsertCell(connectZone->getCell("FirstVoid"));
  nShield->setCutSurf("inner",*joinPipeB,"outer");
  nShield->createAll(System,*opticsHut,"exitHole0");
  ELog::EM<<"NShield == "<<nShield->getLinkPt(0)<<ELog::endDiag;
  // outer pipe shield goes around joinPipeB:
  /*
  outerShield->addAllInsertCell(connectZone->getCell("OuterVoid"));
  outerShield->setCutSurf("inner",*joinPipeB,"outerPipe");
  outerShield->setCutSurf("front",*opticsHut,"back");
  outerShield->createAll(System,*opticsHut,
			 opticsHut->getSideIndex("-exitHole"));
  */
  return;
}


}   // NAMESPACE xraySystem

