/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment45.cxx
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
#include "InnerZone.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "YagUnitBig.h"
#include "YagScreen.h"
#include "CeramicSep.h"

#include "TDCsegment.h"
#include "Segment45.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment45::Segment45(const std::string& Key) :
  TDCsegment(Key,2),
  ceramic(new tdcSystem::CeramicSep(keyName+"Ceramic")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  yagUnit(new tdcSystem::YagUnitBig(keyName+"YagUnit")),
  yagScreen(new tdcSystem::YagScreen(keyName+"YagScreen")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ceramic);
  OR.addObject(pipeA);
  OR.addObject(yagUnit);
  OR.addObject(yagScreen);
  OR.addObject(pipeB);

  setFirstItems(ceramic);
}

Segment45::~Segment45()
  /*!
    Destructor
   */
{}

void
Segment45::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment45","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  ceramic->createAll(System,*this,0);
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System,*ceramic,-1);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*ceramic,2);
  ceramic->insertInCell(System,outerCell);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*ceramic,"back",*pipeA);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*yagUnit);

  yagScreen->setBeamAxis(*yagUnit,1);
  yagScreen->createAll(System,*yagUnit,4);
  yagScreen->insertInCell("Outer",System,outerCell);
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Plate"));
  yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
  yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*yagUnit,"back",*pipeB);

  buildZone->removeLastMaster(System);

  return;
}

void
Segment45::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment45","createLinks");

  setLinkSignedCopy(0,*ceramic,1);
  setLinkSignedCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment45::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment45","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
