/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment18.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "StriplineBPM.h"
#include "VacuumPipe.h"
#include "LQuadF.h"
#include "LQuadH.h"
#include "LObjectSupport.h"
#include "CorrectorMag.h"
#include "IonPumpTube.h"

#include "TDCsegment.h"
#include "Segment18.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment18::Segment18(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  ionPump(new xraySystem::IonPumpTube(keyName+"IonPump")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  bpm(new tdcSystem::StriplineBPM(keyName+"BPM")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  quad(new tdcSystem::LQuadH(keyName+"Quad")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(ionPump);
  OR.addObject(bellowB);
  OR.addObject(bpm);
  OR.addObject(pipeA);
  OR.addObject(quad);
  OR.addObject(pipeB);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);

  setFirstItems(bellowA);
}

Segment18::~Segment18()
  /*!
    Destructor
   */
{}

void
Segment18::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment18","buildObjects");
  int outerCell;

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*ionPump);

  constructSystem::constructUnit
    (System,*buildZone,*ionPump,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,*bellowB,"back",*bpm);

  pipeA->createAll(System,*bpm, "back");
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",quad);
  pipeTerminate(System,*buildZone,pipeA);

  pipeB->createAll(System,*pipeA, "back");
  correctorMagnetPair(System,*buildZone,pipeB,cMagHA,cMagVA);
  pipeTerminate(System,*buildZone,pipeB);

  return;
}

void
Segment18::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment18","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment18::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment18","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
