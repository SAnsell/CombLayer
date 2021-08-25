/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment9.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LQuadF.h"
#include "StriplineBPM.h"
#include "CorrectorMag.h"
#include "CeramicGap.h"
#include "IonPumpTube.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment9.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment9::Segment9(const std::string& Key) :
  TDCsegment(Key,2),

  ceramicBellowA(new tdcSystem::CeramicGap(keyName+"CeramicBellowA")),
  pumpA(new xraySystem::IonPumpTube(keyName+"PumpA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),

  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  bpm(new tdcSystem::StriplineBPM(keyName+"BPM")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),

  bellowC(new constructSystem::Bellows(keyName+"BellowC"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ceramicBellowA);
  OR.addObject(pumpA);
  OR.addObject(pipeA);
  OR.addObject(cMagVA);
  OR.addObject(cMagHA);
  OR.addObject(bellowB);
  OR.addObject(bpm);
  OR.addObject(pipeB);
  OR.addObject(QuadA);
  OR.addObject(bellowC);

  setFirstItems(ceramicBellowA);
}

Segment9::~Segment9()
  /*!
    Destructor
   */
{}

void
Segment9::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment9","buildObjects");

  int outerCell;

  if (isActive("front"))
    ceramicBellowA->copyCutSurf("front",*this,"front");

  ceramicBellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*ceramicBellowA,2);
  ceramicBellowA->insertInCell(System,outerCell);


  // FAKE INSERT REQUIRED
  constructSystem::constructUnit
    (System,*buildZone,*ceramicBellowA,"back",*pumpA);

  
  pipeA->createAll(System,*pumpA,"back");
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",cMagVA);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",cMagHA);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,*bellowB,"back",*bpm);

  pipeB->createAll(System,*bpm,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadA);
  pipeTerminate(System,*buildZone,pipeB);

  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*bellowC);

  return;
}

void
Segment9::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*ceramicBellowA,1);
  setLinkCopy(1,*bellowC,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment9::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment9","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
