/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment21.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "generalConstruct.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "BPM.h"
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

#include "TDCsegment.h"
#include "Segment21.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment21::Segment21(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  bpm(new tdcSystem::BPM(keyName+"BPM")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  quad(new tdcSystem::LQuadH(keyName+"Quad")),
  yagUnit(new tdcSystem::YagUnit(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagH(new tdcSystem::CorrectorMag(keyName+"CMagH")),
  cMagV(new tdcSystem::CorrectorMag(keyName+"CMagV")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(bpm);
  OR.addObject(pipeA);
  OR.addObject(quad);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(pipeB);
  OR.addObject(cMagH);
  OR.addObject(cMagV);
  OR.addObject(bellowB);

  setFirstItem(bellowA);
}

Segment21::~Segment21()
  /*!
    Destructor
   */
{}

void
Segment21::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment21","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  bellowA->createAll(System,*this,0);
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System,*bellowA,-1);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*bellowA,"back",*bpm);

  pipeA->createAll(System,*bpm, "back");

  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",quad);
  pipeTerminate(System,*buildZone,pipeA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,-3);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateA"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  pipeB->createAll(System,*yagUnit, "back");

  correctorMagnetPair(System,*buildZone,pipeB,cMagH,cMagV);
  pipeTerminate(System,*buildZone,pipeB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*bellowB);

  buildZone->removeLastMaster(System);

  return;
}

void
Segment21::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment21","createLinks");

  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment21::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment21","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
