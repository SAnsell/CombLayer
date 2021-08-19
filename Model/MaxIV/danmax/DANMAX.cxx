/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/DANMAX.cxx
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
#include "FixedOffsetUnit.h"
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

#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "WallLead.h"
#include "R3FrontEnd.h"
#include "danmaxFrontEnd.h"
#include "danmaxOpticsHutch.h"
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
  opticsHut(new danmaxOpticsHutch(newName+"OpticsHut")),
  opticsBeam(new danmaxOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  connectUnit(new danmaxConnectLine(newName+"ConnectUnit")),
  joinPipeC(new constructSystem::VacuumPipe(newName+"JoinPipeC")),
  exptHut(new xraySystem::ExperimentalHutch(newName+"ExptHut")),
  pShield(new xraySystem::PipeShield(newName+"PShield")),
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
  OR.addObject(connectUnit);
  OR.addObject(joinPipeC);
  OR.addObject(exptHut);
  OR.addObject(pShield);
  
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
  opticsBeam->setInnerMat(opticsHut->getCellMat(System,"Void"));
  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  opticsBeam->setPreInsert(joinPipe);
  opticsBeam->createAll(System,*joinPipe,2);

 
  joinPipeB->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell("Main",opticsHut->getCell("ExitHole"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);


  exptHut->setCutSurf("Floor",r3Ring->getSurf("Floor"));
  exptHut->setCutSurf("InnerSideWall",r3Ring->getSurf("FlatInner",PIndex));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut->createAll(System,*r3Ring,r3Ring->getSideIndex(exitLink));

  connectUnit->registerJoinPipe(joinPipeC);
  connectUnit->setInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  connectUnit->setFront(*opticsHut,2);
  connectUnit->setBack(*exptHut,1);
  connectUnit->createAll(System,*joinPipeB,"back");


  joinPipeB->insertAllInCell(System,connectUnit->getCell("FirstVoid"));
  
  joinPipeC->insertAllInCell(System,exptHut->getCell("Void"));
  joinPipeC->insertInCell("Main",System,exptHut->getCell("EntranceHole"));

  // pipe shield goes around joinPipeC:
  pShield->addAllInsertCell(exptHut->getCell("Void"));
  pShield->setCutSurf("inner",*joinPipeC,"outerPipe");
  //  pShield->setCutSurf("front",*opticsHut,"innerBack");
  pShield->createAll(System,*exptHut,"innerFront");

  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->createAll(System,*joinPipeC,2);

  return;
}


}   // NAMESPACE xraySystem

