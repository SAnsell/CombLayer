/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment1.cxx
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
#include "StriplineBPM.h"
#include "LQuadF.h"
#include "CorrectorMag.h"
#include "IonPumpTube.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment1.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment1::Segment1(const std::string& Key) :
  TDCsegment(Key,2),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  cMagHB(new xraySystem::CorrectorMag(keyName+"CMagHB")),
  cMagVB(new xraySystem::CorrectorMag(keyName+"CMagVB")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  bpm(new tdcSystem::StriplineBPM(keyName+"BPM")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  cMagHC(new xraySystem::CorrectorMag(keyName+"CMagHC")),
  cMagVC(new xraySystem::CorrectorMag(keyName+"CMagVC")),
  pumpA(new xraySystem::IonPumpTube(keyName+"PumpA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(pipeB);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);
  OR.addObject(pipeC);
  OR.addObject(pipeD);
  OR.addObject(cMagHB);
  OR.addObject(cMagVB);
  OR.addObject(QuadA);
  OR.addObject(bpm);

  OR.addObject(pipeE);
  OR.addObject(cMagHC);
  OR.addObject(cMagVC);
  OR.addObject(pumpA);
  
  setFirstItems(pipeA);
}

Segment1::~Segment1()
  /*!
    Destructor
   */
{}

void
Segment1::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment1","buildObjects");

  int outerCell;
  
  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bellowA);

  //
  // build pipe + corrector magnets together:
  // THIS becomes a function:
  //
  pipeB->createAll(System,*bellowA,"back");
  correctorMagnetPair(System,*buildZone,pipeB,cMagHA,cMagVA);

  outerCell=buildZone->createUnit(System,*pipeB,"back");
  pipeB->insertAllInCell(System,outerCell);


  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*pipeC);

  pipeD->createAll(System,*pipeC,"back");
  correctorMagnetPair(System,*buildZone,pipeD,cMagHB,cMagVB);

  pipeMagUnit(System,*buildZone,pipeD,"#front","outerPipe",QuadA);
  pipeTerminate(System,*buildZone,pipeD);

  constructSystem::constructUnit
    (System,*buildZone,*pipeD,"back",*bpm);

  pipeE->createAll(System,*bpm,"back");
  correctorMagnetPair(System,*buildZone,pipeE,cMagHC,cMagVC);
  pipeTerminate(System,*buildZone,pipeE);

  constructSystem::constructUnit
    (System,*buildZone,*pipeE,"back",*pumpA);

  

  return;
}

void
Segment1::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*pumpA,2);

    //  const constructSystem::portItem& VPB=pumpA->getPort(1);

  // setLinkCopy(1,VPB,VPB.getSideIndex("OuterPlate"));

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment1::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment1","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
