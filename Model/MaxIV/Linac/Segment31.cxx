/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment31.cxx
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
#include "InnerZone.h"
#include "generalConstruct.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "VacuumPipe.h"
#include "portItem.h"
#include "BlankTube.h"
#include "CorrectorMag.h"
#include "LObjectSupport.h"
#include "CylGateValve.h"
#include "BPM.h"
#include "LQuadF.h"

#include "TDCsegment.h"
#include "Segment31.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment31::Segment31(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  ionPumpA(new constructSystem::BlankTube(keyName+"IonPumpA")),
  gate(new xraySystem::CylGateValve(keyName+"Gate")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  bpm(new tdcSystem::BPM(keyName+"BPM")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  quad(new tdcSystem::LQuadF(keyName+"Quad")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagH(new tdcSystem::CorrectorMag(keyName+"CMagH")),
  ionPumpB(new constructSystem::BlankTube(keyName+"IonPumpB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  bellowD(new constructSystem::Bellows(keyName+"BellowD"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(ionPumpA);
  OR.addObject(gate);
  OR.addObject(bellowB);
  OR.addObject(bpm);
  OR.addObject(pipeA);
  OR.addObject(quad);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(cMagH);
  OR.addObject(ionPumpB);
  OR.addObject(pipeC);
  OR.addObject(bellowD);

  setFirstItem(bellowA);
}

Segment31::~Segment31()
  /*!
    Destructor
   */
{}

void
Segment31::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment31","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  bellowA->createAll(System,*this,0);
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System,*bellowA,-1);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  const constructSystem::portItem& ionPumpABackPort =
    buildIonPump2Port(System,*buildZone,masterCell,*bellowA,"back",*ionPumpA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,ionPumpABackPort,"OuterPlate",*gate);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*gate,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bellowB,"back",*bpm);

  pipeA->createAll(System,*bpm, "back");
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",quad);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bellowC);

  pipeB->createAll(System,*bellowC,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagH);
  pipeTerminate(System,*buildZone,pipeB);

  const constructSystem::portItem& ionPumpBBackPort =
    buildIonPump2Port(System,*buildZone,masterCell,*pipeB,"back",*ionPumpB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,ionPumpBBackPort,"OuterPlate",*pipeC);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeC,"back",*bellowD);

  buildZone->removeLastMaster(System);

  return;
}

void
Segment31::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment31","createLinks");

  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowD,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  return;
}

void
Segment31::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment31","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
