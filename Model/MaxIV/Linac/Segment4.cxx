/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment4.cxx
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
#include "StriplineBPM.h"
#include "LQuadF.h"
#include "LSexupole.h"
#include "CorrectorMag.h"
#include "YagUnit.h"
#include "YagScreen.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment4.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment4::Segment4(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bpmA(new tdcSystem::StriplineBPM(keyName+"BPMA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  SexuA(new tdcSystem::LSexupole(keyName+"SexuA")),
  QuadB(new tdcSystem::LQuadF(keyName+"QuadB")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bpmA);
  OR.addObject(pipeB);
  OR.addObject(QuadA);
  OR.addObject(SexuA);
  OR.addObject(QuadB);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(shieldA);
  OR.addObject(bellowA);
  OR.addObject(pipeC);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);

  setFirstItems(pipeA);
}

Segment4::~Segment4()
  /*!
    Destructor
   */
{}

void
Segment4::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment4","buildObjects");

  int outerCell;

  if (isActive("front"))
    {
      ELog::EM<<"Front == "<<ELog::endDiag;
      pipeA->copyCutSurf("front",*this,"front");
    }
  pipeA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bpmA);

  pipeB->createAll(System,*bpmA,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadA);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",SexuA);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",QuadB);
  pipeTerminate(System,*buildZone,pipeB);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  shieldA->createAll(System, *yagScreen, 0);
  shieldA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*yagUnit,"back",*bellowA);

  pipeC->createAll(System,*bellowA,"back");
  correctorMagnetPair(System,*buildZone,pipeC,cMagHA,cMagVA);
  pipeTerminate(System,*buildZone,pipeC);

  return;
}

void
Segment4::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*pipeC,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment4::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment4","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
