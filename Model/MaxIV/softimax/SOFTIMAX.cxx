/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File: softimax/SOFTIMAX.cxx
  *
  * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "NameStack.h"
#include "FileReport.h"
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
#include "JawFlange.h"
#include "R3FrontEnd.h"
#include "OpticsHutch.h"
#include "softimaxFrontEnd.h"
#include "softimaxOpticsLine.h"
#include "WallLead.h"
#include "R3Ring.h"
#include "R3Beamline.h"
#include "SOFTIMAX.h"

namespace xraySystem
{

SOFTIMAX::SOFTIMAX(const std::string& KN) :
  R3Beamline("Balder",KN),
  frontBeam(new softimaxFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsBeam(new softimaxOpticsLine(newName+"OpticsLine"))
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
  OR.addObject(opticsBeam);
  
}

SOFTIMAX::~SOFTIMAX()
/*!
  Destructor
*/
{}
  
void
SOFTIMAX::build(Simulation& System,
		const attachSystem::FixedComp& FCOrigin,
		const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RControl("SOFTIMAX","build");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1);
  const size_t SIndex=(PIndex+1) % NS;
  const size_t prevIndex=(NS+PIndex-1) % NS;

  const std::string exitLink="ExitCentre"+std::to_string(PIndex);

  frontBeam->deactivateFM3();
  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("REWall",-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

  frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
  frontBeam->createAll(System,FCOrigin,sideIndex);

  wallLead->addInsertCell(r3Ring->getCell("FrontWall",PIndex));
  wallLead->setFront(r3Ring->getSurf("BeamInner",PIndex));
  wallLead->setBack(-r3Ring->getSurf("BeamOuter",PIndex));
  wallLead->createAll(System,FCOrigin,sideIndex);

  if (stopPoint=="frontEnd" || stopPoint=="Dipole"
      || stopPoint=="FM1" || stopPoint=="FM2")
    return;

  buildOpticsHutch(System,opticsHut,PIndex,exitLink);

  // Inner space

  if (stopPoint=="opticsHut") return;

  joinPipe->addAllInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell("Main",wallLead->getCell("Void"));
  joinPipe->createAll(System,*frontBeam,2);

  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));

  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r3Ring->getSurf("Floor"));
  opticsBeam->setPreInsert(joinPipe);
  opticsBeam->createAll(System,*joinPipe,2);

  opticsBeam->buildExtras(System,*opticsHut);

  return;
  std::vector<int> cells(opticsHut->getCells("BackWall"));
  cells.emplace_back(opticsHut->getCell("Extension"));
  
  return;
  opticsBeam->buildOutGoingPipes(System,opticsBeam->getCell("LeftVoid"),
				 opticsBeam->getCell("RightVoid"),
				 cells);

  return;
}


}   // NAMESPACE xraySystem
