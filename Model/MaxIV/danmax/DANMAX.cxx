/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/DANMAX.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "AttachSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LeadPipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "balderOpticsHutch.h"
#include "ExperimentalHutch.h"
#include "WallLead.h"
#include "R3FrontEnd.h"
#include "danmaxFrontEnd.h"
#include "danmaxOpticsLine.h"
#include "danmaxConnectLine.h"
#include "PipeShield.h"
#include "SqrShield.h"

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
  opticsHut(new balderOpticsHutch(newName+"OpticsHut")),
  opticsBeam(new danmaxOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  connectUnit(new danmaxConnectLine(newName+"ConnectUnit")),
  joinPipeC(new constructSystem::VacuumPipe(newName+"JoinPipeC")),
  exptHut(new xraySystem::ExperimentalHutch(newName+"ExptHut")),
  pShield(new xraySystem::PipeShield(newName+"PShield"))
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
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  wallLead->addInsertCell(r3Ring->getCell("FrontWall",PIndex));
  wallLead->setFront(r3Ring->getSurf("BeamInner",PIndex));
  wallLead->setBack(-r3Ring->getSurf("BeamOuter",PIndex));    
  wallLead->createAll(System,FCOrigin,sideIndex);

  if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

  opticsHut->setCutSurf("Floor",r3Ring->getSurf("Floor"));
  opticsHut->setCutSurf("RingWall",r3Ring->getSurf("BeamOuter",PIndex));

  opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));

  opticsHut->setCutSurf("SideWall",r3Ring->getSurf("FlatOuter",PIndex));
  opticsHut->setCutSurf("InnerSideWall",r3Ring->getSurf("FlatInner",PIndex));
  opticsHut->createAll(System,*r3Ring,r3Ring->getSideIndex(exitLink));

  // Ugly HACK to get the two objects to merge
  r3Ring->insertComponent
    (System,"OuterFlat",SIndex,
     *opticsHut,opticsHut->getSideIndex("frontCut"));

  // Inner space

  if (stopPoint=="opticsHut") return;

  joinPipe->addInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell(wallLead->getCell("Void"));
  joinPipe->addInsertCell(opticsHut->getCell("Inlet"));
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

  joinPipe->insertInCell(System,opticsBeam->getCell("OuterVoid",0));
  
  joinPipeB->addInsertCell(opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
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
  connectUnit->construct(System,*opticsHut,"back",*joinPipeB,"back");

  joinPipeB->insertInCell(System,connectUnit->getCell("OuterVoid",0));
  
  joinPipeC->insertInCell(System,exptHut->getCell("Void"));
  joinPipeC->insertInCell(System,exptHut->getCell("EntranceHole"));
  
  // pipe shield goes around joinPipeB:
  pShield->addAllInsertCell(exptHut->getCell("Void"));
  pShield->setCutSurf("inner",*joinPipeC,"outerPipe");
  //  pShield->setCutSurf("front",*opticsHut,"innerBack");
  pShield->createAll(System,*exptHut,"innerFront");

  return;
}


}   // NAMESPACE xraySystem

