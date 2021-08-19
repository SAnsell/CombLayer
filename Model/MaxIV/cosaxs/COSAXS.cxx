/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: cosaxs/COSAXS.cxx
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
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
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
#include "BlockZone.h"

#include "VacuumPipe.h"

#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "JawFlange.h"
#include "R3FrontEnd.h"
#include "cosaxsFrontEnd.h"
#include "cosaxsOpticsLine.h"
#include "cosaxsExptLine.h"
#include "PipeShield.h"
#include "WallLead.h"
#include "R3Ring.h"
#include "R3Beamline.h"
#include "COSAXS.h"

namespace xraySystem
{

COSAXS::COSAXS(const std::string& KN) :
  R3Beamline("Balder",KN),
  frontBeam(new cosaxsFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new cosaxsOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  exptHut(new ExperimentalHutch(newName+"ExptHut")),
  exptBeam(new cosaxsExptLine(newName+"ExptLine"))
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
  OR.addObject(exptHut);
  OR.addObject(exptBeam);
}

COSAXS::~COSAXS()
  /*!
    Destructor
   */
{}

  
void 
COSAXS::build(Simulation& System,
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
  ELog::RegMethod RControl("COSAXS","build");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1);
  const size_t SIndex=(PIndex+1) % NS;
  const size_t prevIndex=(NS+PIndex-1) % NS;

  const std::string exitLink="ExitCentre"+std::to_string(PIndex);
  
  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("REWall",-r3Ring->getSurf("BeamInner",PIndex));
  //  frontBeam->deactivateFM3();  ACTIVE on COSAXS
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  wallLead->addInsertCell(r3Ring->getCell("FrontWall",PIndex));
  wallLead->setFront(r3Ring->getSurf("BeamInner",PIndex));
  wallLead->setBack(-r3Ring->getSurf("BeamOuter",PIndex));    
  wallLead->createAll(System,FCOrigin,sideIndex);
  
  if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

  buildOpticsHutch(System,opticsHut,PIndex,exitLink);

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
  joinPipeB->addAllInsertCell(r3Ring->getCell("OuterSegment", PIndex));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);


  screenA->setCutSurf("inner",joinPipeB->getSurfRule("OuterRadius"));
  screenA->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  screenA->createAll(System,*opticsBeam,2);

  exptHut->setCutSurf("frontWall",*opticsHut,"back");
  exptHut->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
  exptHut->createAll(System,*opticsHut,"exitHole");

  if (stopPoint=="exptHut")
    {
      joinPipeB->insertAllInCell(System,exptHut->getCell("Void"));
      return;
    }
  
  exptBeam->setStopPoint(stopPoint);
  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->setCutSurf("front",*exptHut,
			 exptHut->getSideIndex("innerFront"));
  exptBeam->setCutSurf("back",*exptHut,
			 exptHut->getSideIndex("innerBack"));
  exptBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptBeam->setPreInsert(joinPipeB);

  exptBeam->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptBeam->createAll(System,*joinPipeB,2);

  // Intersection of the exptHut back wall with tube and exptBeam:
  const std::string tubeName(exptBeam->getKeyName()+"DetTube");

  const attachSystem::CellMap* tube =
    System.getObjectThrow<attachSystem::CellMap>(tubeName,"CellMap");


  HeadRule wallCut=
    exptHut->getSurfRule("innerBack")*
    exptHut->getSurfRule("#outerBack")*
    exptHut->getSurfRule("exitHole");
  wallCut.makeComplement();
  tube->insertComponent(System,"tubeVoid",9,wallCut);

  return;

}


}   // NAMESPACE xraySystem

