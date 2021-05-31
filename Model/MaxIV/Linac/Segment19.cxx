/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment19.cxx
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

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "GaugeTube.h"

#include "TDCsegment.h"
#include "Segment19.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment19::Segment19(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  gauge(new xraySystem::GaugeTube(keyName+"Gauge")),
  gateA(new constructSystem::GateValveCube(keyName+"GateA")),
  ionPump(new xraySystem::GaugeTube(keyName+"IonPump")),
  gateB(new xraySystem::CylGateValve(keyName+"GateB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(gauge);
  OR.addObject(gateA);
  OR.addObject(ionPump);
  OR.addObject(gateB);
  OR.addObject(bellowB);

  setFirstItems(bellowA);
}

Segment19::~Segment19()
  /*!
    Destructor
   */
{}

void
Segment19::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment19","buildObjects");

  int outerCell;

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  // Gauge
  constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*gauge);
  
  constructSystem::constructUnit
    (System,*buildZone,*gauge,"back",*gateA);

  // Ion pump
  constructSystem::constructUnit
    (System,*buildZone,*gateA,"back",*ionPump);

  constructSystem::constructUnit
    (System,*buildZone,*ionPump,"back",*gateB);

  constructSystem::constructUnit
    (System,*buildZone,*gateB,"back",*bellowB);

  return;
}

void
Segment19::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment19","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment19::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment19","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
