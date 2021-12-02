/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment43.cxx
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

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumPipe.h"
#include "StriplineBPM.h"
#include "YagUnitBig.h"
#include "YagScreen.h"
#include "CylGateValve.h"
#include "ButtonBPM.h"
#include "CorrectorMag.h"
#include "LObjectSupport.h"

#include "TDCsegment.h"
#include "Segment43.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment43::Segment43(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  bpmA(new tdcSystem::StriplineBPM(keyName+"BPMA")),
  yagUnit(new tdcSystem::YagUnitBig(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  gate(new xraySystem::CylGateValve(keyName+"Gate")),
  pipe(new constructSystem::VacuumPipe(keyName+"Pipe")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  bpmB(new tdcSystem::ButtonBPM(keyName+"BPMB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(bpmA);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(gate);
  OR.addObject(pipe);
  OR.addObject(cMagHA);
  OR.addObject(bpmB);
  OR.addObject(bellowB);

  setFirstItems(bellowA);
}

Segment43::~Segment43()
  /*!
    Destructor
   */
{}

void
Segment43::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment43","buildObjects");

  int outerCell;

  bellowA->createAll(System,*this,0);

  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*bpmA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*bpmA,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,4);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Plate"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  constructSystem::constructUnit
    (System,*buildZone,*yagUnit,"back",*gate);

  pipe->createAll(System,*gate,"back");
  pipeMagUnit(System,*buildZone,pipe,"#front","outerPipe",cMagHA);
  pipeTerminate(System,*buildZone,pipe);

  constructSystem::constructUnit
    (System,*buildZone,*pipe,"back",*bpmB);

  constructSystem::constructUnit
    (System,*buildZone,*bpmB,"back",*bellowB);

  return;
}

void
Segment43::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment43","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowB,2);

  joinItems.push_back(FixedComp::getFullRule("back"));

  return;
}

void
Segment43::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment43","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
