/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment2.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"
#include "LQuad.h"
#include "BPM.h"
#include "CylGateValve.h"
#include "EArrivalMon.h"
#include "YagUnit.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment2.h"

namespace tdcSystem
{

// Note currently uncopied:
  
L2SPFsegment2::L2SPFsegment2(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  QuadA(new tdcSystem::LQuad(keyName+"QuadA")),
  bpmA(new tdcSystem::BPM(keyName+"BPMA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  QuadB(new tdcSystem::LQuad(keyName+"QuadB")),
  gateTube(new xraySystem::CylGateValve(keyName+"GateTube")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  beamArrivalMon(new tdcSystem::EArrivalMon(keyName+"BeamArrivalMon")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),  
  bpmB(new tdcSystem::BPM(keyName+"BPMB")),  
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  QuadC(new tdcSystem::LQuad(keyName+"QuadC")),
  QuadD(new tdcSystem::LQuad(keyName+"QuadD")),
  QuadE(new tdcSystem::LQuad(keyName+"QuadE")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit"))
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
}
  
L2SPFsegment2::~L2SPFsegment2()
  /*!
    Destructor
   */
{}

void
L2SPFsegment2::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment2","buildObjects");

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  pipeA->createAll(System,*this,0);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",QuadA);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bpmA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bpmA,"back",*bellowA);

  pipeB->createAll(System,*bellowA,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadB);
  pipeTerminate(System,*buildZone,pipeB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*gateTube);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*gateTube,"back",*pipeC);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeC,"back",*beamArrivalMon);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*beamArrivalMon,"back",*pipeD);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeD,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bellowB,"back",*bpmB);

  pipeE->createAll(System,*bpmB,"back");
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadC);
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadD);
  pipeMagUnit(System,*buildZone,pipeE,"#front","outerPipe",QuadE);
  pipeTerminate(System,*buildZone,pipeE);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeE,"back",*yagUnit);

  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment2::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*yagUnit,2);

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment2::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment2","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem

