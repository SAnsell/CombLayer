/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment49.cxx
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"

#include "CylGateValve.h"
#include "VacuumPipe.h"

#include "TDCsegment.h"
#include "Segment49.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment49::Segment49(const std::string& Key) :
  TDCsegment(Key,2),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  gateB(new xraySystem::CylGateValve(keyName+"GateB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gateA);
  OR.addObject(pipeA);
  OR.addObject(pipeB);
  OR.addObject(gateB);

  setFirstItems(gateA);
}

Segment49::~Segment49()
  /*!
    Destructor
   */
{}

void
Segment49::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment49","buildObjects");

  int outerCell;

  if (isActive("front"))
    gateA->copyCutSurf("front",*this,"front");

  gateA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*gateA,2);
  gateA->insertInCell(System,outerCell);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*gateA,"back",*pipeA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*pipeB);

  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*gateB);

  return;
}

void
Segment49::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment49","createLinks");

  setLinkSignedCopy(0,*gateA,1);
  setLinkSignedCopy(1,*gateB,2);

  joinItems.push_back(FixedComp::getFullRule("back"));

  return;
}

void
Segment49::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment49","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
