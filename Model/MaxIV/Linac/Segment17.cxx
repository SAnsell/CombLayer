/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment17.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "GeneralPipe.h"
#include "Bellows.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "IonPumpTube.h"

#include "TDCsegment.h"
#include "Segment17.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment17::Segment17(const std::string& Key) :
  TDCsegment(Key,2),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  ionPump(new xraySystem::IonPumpTube(keyName+"IonPump")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(ionPump);
  OR.addObject(pipeB);

  setFirstItems(pipeA);
}

Segment17::~Segment17()
  /*!
    Destructor
   */
{}

void
Segment17::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment17","buildObjects");
  int outerCell;

  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*ionPump);


  constructSystem::constructUnit
    (System,*buildZone,*ionPump,"back",*pipeB);

  return;
}

void
Segment17::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment17","createLinks");

  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment17::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment17","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
