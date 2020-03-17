/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/MAXPEEM.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "CopiedComp.h"
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

#include "R1Ring.h"
#include "R1FrontEnd.h"
#include "maxpeemFrontEnd.h"
#include "maxpeemOpticsHut.h"
#include "maxpeemOpticsBeamline.h"
#include "ExperimentalHutch.h"
#include "GateValveCube.h"
#include "FlangeMount.h"
#include "WallLead.h"


#include "R1Beamline.h"
#include "MAXPEEM.h"

namespace xraySystem
{

MAXPEEM::MAXPEEM(const std::string& KN) :
  R1Beamline("Maxpeem",KN),
  frontBeam(new maxpeemFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  opticsHut(new maxpeemOpticsHut(newName+"OpticsHut")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsBeam(new maxpeemOpticsBeamline(newName+"OpticsBeam"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(frontBeam);
  OR.addObject(wallLead);
  OR.addObject(opticsHut);
  OR.addObject(joinPipe);
}

MAXPEEM::~MAXPEEM()
  /*!
    Destructor
   */
{}

void 
MAXPEEM::build(Simulation& System,
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
  ELog::RegMethod RControl("MAXPEEM","build");

  const size_t PIndex=static_cast<size_t>(sideIndex-2);
  const size_t SIndex=(PIndex+1) % r1Ring->nConcave();
  const size_t OIndex=(sideIndex+1) % r1Ring->getNCells("OuterSegment");

  frontBeam->addInsertCell(r1Ring->getCell("Void"));
  frontBeam->addInsertCell(r1Ring->getCell("VoidTriangle",PIndex));

  frontBeam->setBack(r1Ring->getSurf("BeamInner",SIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);
  
  wallLead->addInsertCell(r1Ring->getCell("FrontWall",SIndex));
  wallLead->setFront(-r1Ring->getSurf("BeamInner",SIndex));
  wallLead->setBack(r1Ring->getSurf("BeamOuter",SIndex));
  wallLead->createAll(System,FCOrigin,sideIndex);

  opticsHut->setCutSurf("Floor",r1Ring->getSurf("Floor"));
  opticsHut->setCutSurf("RingWall",-r1Ring->getSurf("BeamOuter",SIndex));
  opticsHut->addInsertCell(r1Ring->getCell("OuterSegment",OIndex));
  opticsHut->createAll(System,*wallLead,2);

  joinPipe->addInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell(wallLead->getCell("Void"));
  joinPipe->addInsertCell(opticsHut->getCell("InletHole"));
  joinPipe->createAll(System,*frontBeam,2);


  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r1Ring->getSurf("Floor"));
  opticsBeam->createAll(System,*joinPipe,2);

  joinPipe->insertInCell(System,opticsBeam->getCell("OuterVoid",0));

  std::vector<int> cells(opticsHut->getCells("Back"));
  cells.emplace_back(opticsHut->getCell("Extension"));
  opticsBeam->buildOutGoingPipes(System,opticsBeam->getCell("LeftVoid"),
  				 opticsBeam->getCell("RightVoid"),
  				 cells);
  
  return;
}


}   // NAMESPACE xraySystem

