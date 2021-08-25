/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment14.cxx
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "LObjectSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"

#include "DipoleDIBMag.h"
#include "CylGateValve.h"

#include "ContainedGroup.h"
#include "FlatPipe.h"

#include "TDCsegment.h"
#include "Segment14.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment14::Segment14(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dm1(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dm2(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(flatA);
  OR.addObject(dm1);
  OR.addObject(pipeB);
  OR.addObject(flatB);
  OR.addObject(dm2);
  OR.addObject(gateA);
  OR.addObject(bellowB);

  setFirstItems(bellowA);
}

Segment14::~Segment14()
  /*!
    Destructor
   */
{}



void
Segment14::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment14","buildObjects");

  int outerCell;
  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  flatA->setFront(*bellowA,"back");
  flatA->createAll(System,*bellowA,"back");
  pipeMagGroup(System,*buildZone,flatA,
     {"FlangeA","Pipe"},"Origin","outerPipe",dm1);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,*flatA,"back",*pipeB);

  flatB->setFront(*pipeB,"back");
  flatB->createAll(System,*pipeB,"back");
  pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dm2);
  pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,*flatB,"back",*gateA);

  constructSystem::constructUnit
    (System,*buildZone,*gateA,"back",*bellowB);

  return;
}

void
Segment14::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment14","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  CellMap::addCells("Unit",buildZone->getCells("Unit"));
  return;
}

void
Segment14::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment14","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
