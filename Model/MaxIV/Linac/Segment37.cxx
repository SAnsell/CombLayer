/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment37.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "EBeamStop.h"

#include "TDCsegment.h"
#include "Segment37.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment37::Segment37(const std::string& Key) :
  TDCsegment(Key,2),
  ceramicA(new tdcSystem::CeramicGap(keyName+"CeramicA")),
  beamStop(new tdcSystem::EBeamStop(keyName+"BeamStop")),
  ceramicB(new tdcSystem::CeramicGap(keyName+"CeramicB")),
  pipe(new constructSystem::VacuumPipe(keyName+"Pipe"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ceramicA);
  OR.addObject(beamStop);
  OR.addObject(ceramicB);
  OR.addObject(pipe);

  setFirstItems(ceramicA);
}

Segment37::~Segment37()
  /*!
    Destructor
   */
{}

void
Segment37::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment37","buildObjects");

  int outerCell;

  if (isActive("front"))
    ceramicA->copyCutSurf("front",*this,"front");
  ceramicA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*ceramicA,2);
  ceramicA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*ceramicA,"back",*beamStop);

  constructSystem::constructUnit
    (System,*buildZone,*beamStop,"back",*ceramicB);

  constructSystem::constructUnit
    (System,*buildZone,*ceramicB,"back",*pipe);

  return;
}

void
Segment37::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*ceramicA,1);
  setLinkCopy(1,*pipe,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment37::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment37","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
