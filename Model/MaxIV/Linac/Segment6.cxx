/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment6.cxx
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "CeramicGap.h"
#include "Scrapper.h"
#include "EBeamStop.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment6.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment6::Segment6(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  scrapper(new tdcSystem::Scrapper(keyName+"Scrapper")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  shieldB(new tdcSystem::LocalShielding(keyName+"ShieldB")),
  ceramicA(new tdcSystem::CeramicGap(keyName+"CeramicA")),
  beamStop(new tdcSystem::EBeamStop(keyName+"EBeam")),
  ceramicB(new tdcSystem::CeramicGap(keyName+"CeramicB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(pipeB);
  OR.addObject(pipeC);
  OR.addObject(scrapper);
  OR.addObject(shieldA);
  OR.addObject(pipeD);
  OR.addObject(shieldB);
  OR.addObject(ceramicA);
  OR.addObject(beamStop);
  OR.addObject(ceramicB);

  setFirstItems(pipeA);
}

Segment6::~Segment6()
  /*!
    Destructor
   */
{}

void
Segment6::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment6","buildObjects");

  int outerCell;

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");
  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*pipeB);
  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*pipeC);
  constructSystem::constructUnit
    (System,*buildZone,*pipeC,"back",*scrapper);

  shieldA->setCutSurf("Inner", scrapper->getFullRule("Outer") *
		      scrapper->getFullRule("OuterTop") *
		      scrapper->getFullRule("OuterBottom"));
  shieldA->createAll(System, *scrapper, 0);
  shieldA->insertInCell(System,outerCell+3);

  pipeD->setFront(*scrapper, "back");
  pipeD->createAll(System,*scrapper,"back");
  pipeMagUnit(System,*buildZone,pipeD,"#front","outerPipe",shieldB);
  pipeTerminate(System,*buildZone,pipeD);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeD,"back",*ceramicA);

  beamStop->setCutSurf("Front", ceramicA->getExclude() +
		       pipeD->getExclude("FlangeB") +
		       pipeD->getExclude("Main"));

  constructSystem::constructUnit
    (System,*buildZone,*ceramicA,"back",*beamStop);

  constructSystem::constructUnit
    (System,*buildZone,*beamStop,"back",*ceramicB);

  if (beamStop->isShieldActive()) // optimise \todo
    {
      beamStop->insertAllInCell(System,outerCell);
      beamStop->insertAllInCell(System,outerCell-1);

      beamStop->insertAllInCell(System,outerCell+2);

      //      attachSystem::addToInsertControl(System,*beamStop,*ceramicB);
      beamStop->insertComponent(System,"ShieldSideBackHole",*ceramicB);
    }


  return;
}

void
Segment6::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*ceramicB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment6::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("Segment6","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  if (beamStop->isShieldActive())
      CellMap::setCells("ShieldSideBackHole",beamStop->getCells("ShieldSideBackHole"));

  return;
}


}   // NAMESPACE tdcSystem
