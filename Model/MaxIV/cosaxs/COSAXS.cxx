/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: cosaxs/COSAXS.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
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
#include "World.h"
#include "AttachSupport.h"
#include "InnerZone.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LeadPipe.h"

#include "balderOpticsHutch.h"
#include "ExperimentalHutch.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "R3FrontEnd.h"
#include "cosaxsFrontEnd.h"
#include "cosaxsOpticsLine.h"
#include "cosaxsExptLine.h"
#include "ConnectZone.h"
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
  opticsHut(new balderOpticsHutch(newName+"OpticsHut")),
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
  opticsBeam->createAll(System,*joinPipe,2);
  
  joinPipe->insertInCell(System,opticsBeam->getCell("OuterVoid",0));

  joinPipeB->addInsertCell(opticsBeam->getCell("LastVoid"));
  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
  joinPipeB->addInsertCell(r3Ring->getCell("OuterSegment", PIndex));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);

  
  screenA->setCutSurf("inner",joinPipeB->getSurfRule("OuterRadius"));
  screenA->addAllInsertCell(opticsBeam->getCell("LastVoid"));
  screenA->createAll(System,*opticsBeam,2);
  
  
  exptHut->setCutSurf("frontWall",opticsHut->getSurf("outerWall"));
  exptHut->setCutSurf("Floor",r3Ring->getSurf("Floor"));
  exptHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptHut->createAll(System,*r3Ring,r3Ring->getSideIndex(exitLink));

  if (stopPoint=="exptHut")
    {
      joinPipeB->insertInCell(System,exptHut->getCell("Void"));
      return;
    }

  exptBeam->setStopPoint(stopPoint);
  exptBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  exptBeam->setCutSurf("front",opticsHut->getSurf("outerWall"));
  //  exptBeam->setCutSurf("back",exptHut->getSurf("innerBack"));
 
  exptBeam->addInsertCell(exptHut->getCell("Void"));
  exptBeam->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));
  exptBeam->createAll(System,*joinPipeB,2);

  // Intersection of the exptHut back wall with tube and exptBeam:
  const std::string tubeName(exptBeam->getKeyName()+"Tube");
  const std::string seg3name(tubeName+"Segment3");

  const attachSystem::CellMap* tube =
    System.getObjectThrow<attachSystem::CellMap>(tubeName,"Tube CellMap");
  const attachSystem::SurfMap* seg3Surf =
    System.getObjectThrow<attachSystem::SurfMap>(seg3name,"Surf of Segment3");

  HeadRule wallCut;
  wallCut.addUnion(exptHut->getSurf("innerBack"));
  wallCut.addUnion(exptHut->getSurf("outerBack"));

  tube->insertComponent(System,"OuterVoid",5,wallCut);
  exptBeam->insertComponent(System,"SurroundVoid",wallCut);

  const int cylN=seg3Surf->getSurf("OuterCyl");
  exptHut->insertComponent(System,"InnerBackWall",HeadRule(cylN));
  exptHut->insertComponent(System,"LeadBackWall",HeadRule(cylN));
  exptHut->insertComponent(System,"OuterBackWall",HeadRule(cylN));

  joinPipeB->insertInCell(System,exptBeam->getCell("OuterVoid",0));

  return;
}


}   // NAMESPACE xraySystem

