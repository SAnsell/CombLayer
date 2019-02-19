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
#include "FrontBackCut.h"
#include "ExternalCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "InnerZone.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LeadPipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "R3FrontEndCave.h"
#include "R3FrontEnd.h"
#include "cosaxsFrontEnd.h"
#include "cosaxsOpticsLine.h"
#include "ConnectZone.h"
#include "WallLead.h"
#include "COSAXS.h"

namespace xraySystem
{

COSAXS::COSAXS(const std::string& KN) :
  attachSystem::CopiedComp("Balder",KN),
  ringCaveA(new R3FrontEndCave(newName+"RingCaveA")),
  ringCaveB(new R3FrontEndCave(newName+"RingCaveB")),
  frontBeam(new cosaxsFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new cosaxsOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB"))
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

  int voidCell(74123);
  frontBeam->setStopPoint(stopPoint);
  
  ringCaveA->addInsertCell(voidCell);
  ringCaveA->createAll(System,FCOrigin,sideIndex);

  ringCaveB->addInsertCell(voidCell);
  ringCaveB->setCutSurf("front",*ringCaveA,"connectPt");
  ringCaveB->createAll(System,*ringCaveA,
		       ringCaveA->getSideIndex("connectPt"));

  frontBeam->setFront(ringCaveA->getSurf("BeamFront"));
  frontBeam->setBack(ringCaveA->getSurf("BeamInner"));
  
  frontBeam->addInsertCell(ringCaveA->getCell("Void"));
  frontBeam->createAll(System,*ringCaveA,-1);

  wallLead->addInsertCell(ringCaveA->getCell("FrontWall"));
  wallLead->setFront(-ringCaveA->getSurf("BeamInner"));
  wallLead->setBack(-ringCaveA->getSurf("BeamOuter"));
  wallLead->createAll(System,FCOrigin,sideIndex);

  if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

  opticsHut->addInsertCell(voidCell);
  opticsHut->setCutSurf("ringWall",*ringCaveB,"outerWall");
  opticsHut->createAll(System,*ringCaveA,2);

    // UgLY HACK to get the two objects to merge
  const std::string OH=opticsHut->SurfMap::getSurfString("ringFlat");
  ringCaveB->insertComponent
    (System,"OuterWall",*opticsHut,opticsHut->getSideIndex("frontCut"));
  ringCaveB->insertComponent
    (System,"FloorA",*opticsHut,opticsHut->getSideIndex("floorCut"));
  ringCaveB->insertComponent
    (System,"RoofA",*opticsHut,opticsHut->getSideIndex("roofCut"));

  if (stopPoint=="opticsHut") return;

  joinPipe->addInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell(wallLead->getCell("Void"));
  joinPipe->addInsertCell(opticsHut->getCell("Inlet"));
  joinPipe->createAll(System,*frontBeam,2);


  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",opticsHut->getSurf("Floor"));
  opticsBeam->createAll(System,*joinPipe,2);

  joinPipe->insertInCell(System,opticsBeam->getCell("OuterVoid",0));
  
  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);



  return;
}


}   // NAMESPACE xraySystem

