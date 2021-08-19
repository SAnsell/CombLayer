/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: species/SPECIES.cxx
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
#include "PointMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "CopiedComp.h"

#include "VacuumPipe.h"
#include "R1Ring.h"
#include "R1FrontEnd.h"
#include "speciesFrontEnd.h"
#include "speciesOpticsHut.h"
#include "speciesOpticsLine.h"
#include "WallLead.h"

#include "R1Beamline.h"
#include "SPECIES.h"

namespace xraySystem
{

SPECIES::SPECIES(const std::string& KN) :
  R1Beamline("Species",KN),
  frontBeam(new speciesFrontEnd(newName+"FrontBeam")),
  wallLead(new WallLead(newName+"WallLead")),
  opticsHut(new speciesOpticsHut(newName+"OpticsHut")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsBeam(new speciesOpticsLine(newName+"OpticsBeam"))
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

SPECIES::~SPECIES()
  /*!
    Destructor
   */
{}

void 
SPECIES::build(Simulation& System,
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
  ELog::RegMethod RControl("SPECIES","build");

  const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-2);
  const size_t SIndex=(PIndex+1) % r1Ring->nConcave();
  const size_t OIndex=(sideIndex+1) % r1Ring->getNCells("OuterSegment");

  frontBeam->setStopPoint(stopPoint);
  frontBeam->setCutSurf("Floor",r1Ring->getSurf("Floor"));
  frontBeam->setCutSurf("Roof",-r1Ring->getSurf("Roof"));
  frontBeam->setCutSurf("REWall",r1Ring->getSurf("BeamInner",SIndex));
  frontBeam->addInsertCell(r1Ring->getCell("Void",9));
  frontBeam->addInsertCell(r1Ring->getCell("Void",0));
  frontBeam->addInsertMagnetCell(r1Ring->getCell("Void",9));
  frontBeam->addInsertMagnetCell(r1Ring->getCell("Void",0));
  frontBeam->addInsertCell(r1Ring->getCell("VoidTriangle",PIndex));

  frontBeam->createAll(System,FCOrigin,sideIndex);

  
  wallLead->addInsertCell(r1Ring->getCell("FrontWall",SIndex));
  wallLead->setFront(-r1Ring->getSurf("BeamInner",SIndex));
  wallLead->setBack(r1Ring->getSurf("BeamOuter",SIndex));
  wallLead->createAll(System,FCOrigin,sideIndex);
  
  if (!stopPoint.empty())
    ELog::EM<<"Stop Point == "<<stopPoint<<ELog::endDiag;

  if (stopPoint=="frontEnd" ||  stopPoint=="Dipole" ||
      stopPoint=="Heat" || stopPoint=="Quadrupole") return;

  opticsHut->setCutSurf("Floor",r1Ring->getSurf("Floor"));
  opticsHut->setCutSurf("RingWall",-r1Ring->getSurf("BeamOuter",SIndex));
  opticsHut->addInsertCell(r1Ring->getCell("OuterSegment",OIndex));
  opticsHut->createAll(System,*wallLead,2);

  joinPipe->addAllInsertCell(frontBeam->getCell("MasterVoid"));
  joinPipe->addInsertCell("Main",wallLead->getCell("Void"));
  joinPipe->addAllInsertCell(opticsHut->getCell("InletHole"));
  joinPipe->createAll(System,*frontBeam,2);

  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->setCutSurf("front",*opticsHut,
			 opticsHut->getSideIndex("innerFront"));
  opticsBeam->setCutSurf("back",*opticsHut,
			 opticsHut->getSideIndex("innerBack"));
  opticsBeam->setCutSurf("floor",r1Ring->getSurfRule("Floor"));
  opticsBeam->setCutSurf("roof",r1Ring->getSurfRule("#Roof"));
  opticsBeam->setPreInsert(joinPipe);
  
  opticsBeam->createAll(System,*joinPipe,"back");
  opticsBeam->buildExtras(System,*opticsHut);
  

  
  return;
}


}   // NAMESPACE xraySystem

