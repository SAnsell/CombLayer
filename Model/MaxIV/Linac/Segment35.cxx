/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment35.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "LQuadF.h"
#include "CorrectorMag.h"
#include "YagUnitBig.h"
#include "YagScreen.h"
#include "Bellows.h"
#include "ButtonBPM.h"
#include "CrossWayBlank.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment35.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment35::Segment35(const std::string& Key) :
  TDCsegment(Key,2),

  yagUnit(new tdcSystem::YagUnitBig(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  quadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  bpm(new tdcSystem::ButtonBPM(keyName+"BPM")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  quadB(new tdcSystem::LQuadF(keyName+"QuadB")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA")),
  mirrorChamber(new tdcSystem::CrossWayBlank(keyName+"MirrorChamber")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  bellow(new constructSystem::Bellows(keyName+"Bellow"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(pipeA);
  OR.addObject(quadA);
  OR.addObject(bpm);
  OR.addObject(pipeB);
  OR.addObject(quadB);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);
  OR.addObject(mirrorChamber);
  OR.addObject(pipeC);
  OR.addObject(bellow);

  setFirstItems(yagUnit);
}

Segment35::~Segment35()
  /*!
    Destructor
   */
{}

void
Segment35::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment35","buildObjects");

  int outerCell;

  if (isActive("front"))
    yagUnit->copyCutSurf("front",*this,"front");
  yagUnit->createAll(System,*this,0);

  outerCell=buildZone->createUnit(System,*yagUnit,"back");
  yagUnit->insertInCell(System,outerCell);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,4);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Plate"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  pipeA->createAll(System,*yagUnit, "back");
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",quadA);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bpm);

  pipeB->createAll(System,*bpm,"back");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",quadB);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagHA);
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagVA);
  pipeTerminate(System,*buildZone,pipeB);

  constructSystem::constructUnit
    (System,*buildZone,*pipeB,"back",*mirrorChamber);

  constructSystem::constructUnit
    (System,*buildZone,*mirrorChamber,"back",*pipeC);

  constructSystem::constructUnit
    (System,*buildZone,*pipeC,"back",*bellow);
  
  return;
}

void
Segment35::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*yagUnit,1);
  setLinkCopy(1,*bellow,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment35::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment35","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
