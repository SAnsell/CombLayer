/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment23.cxx
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

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "StriplineBPM.h"
#include "VacuumPipe.h"
#include "LQuadF.h"
#include "LQuadH.h"
#include "LObjectSupport.h"
#include "CorrectorMag.h"
#include "YagUnit.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"

#include "Line.h"
#include "ContainedGroup.h"
#include "YagScreen.h"

#include "CylGateValve.h"

#include "TDCsegment.h"
#include "Segment23.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment23::Segment23(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  quad(new tdcSystem::LQuadH(keyName+"Quad")),
  bpm(new tdcSystem::StriplineBPM(keyName+"BPM")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  gate(new xraySystem::CylGateValve(keyName+"Gate")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(pipeA);
  OR.addObject(quad);
  OR.addObject(bpm);
  OR.addObject(bellowB);
  OR.addObject(pipeB);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);
  OR.addObject(yagScreen);
  OR.addObject(yagUnit);
  OR.addObject(pipeC);
  OR.addObject(gate);
  OR.addObject(bellowC);

  setFirstItems(bellowA);
}

Segment23::~Segment23()
  /*!
    Destructor
   */
{}

void
Segment23::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment23","buildObjects");

  int outerCell;

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  pipeA->createAll(System,*bellowA, "back");

  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",quad);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bpm);

  constructSystem::constructUnit
    (System,*buildZone,*bpm,"back",*bellowB);

  pipeB->createAll(System,*bellowB, "back");

  correctorMagnetPair(System,*buildZone,pipeB,cMagHA,cMagVA);
  pipeTerminate(System,*buildZone,pipeB);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  constructSystem::constructUnit
    (System,*buildZone,*yagUnit,"back",*pipeC);

  constructSystem::constructUnit
    (System,*buildZone,*pipeC,"back",*gate);

  constructSystem::constructUnit
    (System,*buildZone,*gate,"back",*bellowC);

  return;
}

void
Segment23::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment23","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowC,2);
  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment23::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment23","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
