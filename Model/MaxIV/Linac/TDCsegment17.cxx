/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/TDCsegment17.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "VacuumPipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"

#include "TDCsegment.h"
#include "TDCsegment17.h"

namespace tdcSystem
{

// Note currently uncopied:

TDCsegment17::TDCsegment17(const std::string& Key) :
  TDCsegment(Key,2),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  ionPump(new constructSystem::BlankTube(keyName+"IonPump")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(ionPump);
  OR.addObject(pipeB);
}

TDCsegment17::~TDCsegment17()
  /*!
    Destructor
   */
{}

void
TDCsegment17::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("TDCsegment17","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  pipeA->createAll(System,*this,0);
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System,*pipeA,-1);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bellowA);

  // Ion pump
  ionPump->addAllInsertCell(masterCell->getName());
  ionPump->setPortRotation(3, Geometry::Vec3D(1,0,0));
  ionPump->createAll(System,*bellowA,"back");

  const constructSystem::portItem& ionPumpBackPort=ionPump->getPort(1);
  outerCell=
    buildZone->createOuterVoidUnit(System,
  				   masterCell,
  				   ionPumpBackPort,
  				   ionPumpBackPort.getSideIndex("OuterPlate"));
  ionPump->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,ionPumpBackPort,"OuterPlate",*pipeB);

  buildZone->removeLastMaster(System);

  return;
}

void
TDCsegment17::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("TDCsegment17","createLinks");

  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*pipeB,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  return;
}

void
TDCsegment17::createAll(Simulation& System,
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
  ELog::RegMethod RControl("TDCsegment17","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
