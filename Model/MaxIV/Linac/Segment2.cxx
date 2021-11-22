/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment2.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
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
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LQuadF.h"
#include "StriplineBPM.h"
#include "CylGateValve.h"
#include "EArrivalMon.h"
#include "YagUnit.h"
#include "YagScreen.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment2.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment2::Segment2(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  bpmA(new tdcSystem::StriplineBPM(keyName+"BPMA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  QuadB(new tdcSystem::LQuadF(keyName+"QuadB")),
  gateTube(new xraySystem::CylGateValve(keyName+"Gate")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  beamArrivalMon(new tdcSystem::EArrivalMon(keyName+"BeamArrivalMon")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  bpmB(new tdcSystem::StriplineBPM(keyName+"BPMB")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  QuadC(new tdcSystem::LQuadF(keyName+"QuadC")),
  QuadD(new tdcSystem::LQuadF(keyName+"QuadD")),
  QuadE(new tdcSystem::LQuadF(keyName+"QuadE")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(QuadA);
  OR.addObject(bpmA);
  OR.addObject(bellowA);
  OR.addObject(pipeB);
  OR.addObject(QuadB);
  OR.addObject(gateTube);
  OR.addObject(pipeC);
  OR.addObject(beamArrivalMon);
  OR.addObject(pipeD);
  OR.addObject(bellowB);
  OR.addObject(bpmB);
  OR.addObject(pipeE);
  OR.addObject(QuadC);
  OR.addObject(QuadD);
  OR.addObject(QuadE);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(shieldA);

  setFirstItems(pipeA);
}

Segment2::~Segment2()
  /*!
    Destructor
   */
{}

void
Segment2::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment2","buildObjects");

  int outerCell;

  pipeA->createAll(System,*this,0);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",QuadA);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bpmA);

  constructSystem::constructUnit
    (System,*buildZone,*bpmA,"back",*bellowA);

  pipeB->createAll(System,*bellowA,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadB);
  pipeTerminate(System,*buildZone,pipeB);

  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*gateTube);

  constructSystem::constructUnit
    (System,*buildZone,*gateTube,"back",*pipeC);

  constructSystem::constructUnit
    (System,*buildZone,*pipeC,"back",*beamArrivalMon);

  constructSystem::constructUnit
    (System,*buildZone,*beamArrivalMon,"back",*pipeD);

  constructSystem::constructUnit
    (System,*buildZone,*pipeD,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,*bellowB,"back",*bpmB);

  pipeE->createAll(System,*bpmB,"back");
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadC);
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadD);
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadE);
  pipeTerminate(System,*buildZone,pipeE);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeE,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  shieldA->createAll(System, *yagScreen, 0);
  shieldA->insertInCell(System,outerCell);

  return;
}

void
Segment2::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*yagUnit,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment2::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment2","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
