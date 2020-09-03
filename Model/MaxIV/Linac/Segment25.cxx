/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment25.cxx
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
#include "TriPipe.h"
#include "DipoleDIBMag.h"
#include "SixPortTube.h"
#include "MultiPipe.h"
#include "YagUnit.h"
#include "YagScreen.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment25.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment25::Segment25(const std::string& Key) :
  TDCsegment(Key,6),
  IZTop(new attachSystem::InnerZone(*this,cellIndex)),
  IZMid(new attachSystem::InnerZone(*this,cellIndex)),
  IZLower(new attachSystem::InnerZone(*this,cellIndex)),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  triPipeA(new tdcSystem::TriPipe(keyName+"TriPipeA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  sixPortA(new tdcSystem::SixPortTube(keyName+"SixPortA")),
  multiPipe(new tdcSystem::MultiPipe(keyName+"MultiPipe")),
  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),
  bellowCA(new constructSystem::Bellows(keyName+"BellowCA")),

  pipeAA(new constructSystem::VacuumPipe(keyName+"PipeAA")),
  pipeBA(new constructSystem::VacuumPipe(keyName+"PipeBA")),
  pipeCA(new constructSystem::VacuumPipe(keyName+"PipeCA")),

  bellowAB(new constructSystem::Bellows(keyName+"BellowAB")),
  bellowBB(new constructSystem::Bellows(keyName+"BellowBB")),
  bellowCB(new constructSystem::Bellows(keyName+"BellowCB")),

  yagUnitA(new tdcSystem::YagUnit(keyName+"YagUnitA")),
  yagUnitB(new tdcSystem::YagUnit(keyName+"YagUnitB")),
  yagScreenA(new tdcSystem::YagScreen(keyName+"YagScreenA")),
  yagScreenB(new tdcSystem::YagScreen(keyName+"YagScreenB")),

  pipeAB(new constructSystem::VacuumPipe(keyName+"PipeAB")),
  pipeBB(new constructSystem::VacuumPipe(keyName+"PipeBB")),
  pipeCB(new constructSystem::VacuumPipe(keyName+"PipeCB"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(triPipeA);
  OR.addObject(dipoleA);
  OR.addObject(pipeB);
  OR.addObject(sixPortA);
  OR.addObject(multiPipe);
  OR.addObject(bellowAA);
  OR.addObject(bellowBA);
  OR.addObject(bellowCA);

  OR.addObject(pipeAA);
  OR.addObject(pipeBA);
  OR.addObject(pipeCA);

  OR.addObject(bellowAB);
  OR.addObject(bellowBB);
  OR.addObject(bellowCB);

  OR.addObject(yagUnitA);
  OR.addObject(yagScreenA);
  OR.addObject(yagUnitB);
  OR.addObject(yagScreenB);

  OR.addObject(pipeAB);
  OR.addObject(pipeBB);
  OR.addObject(pipeCB);

  setFirstItems(bellowA);
}

Segment25::~Segment25()
  /*!
    Destructor
   */
{}

void
Segment25::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment25","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  bellowA->createAll(System,*this,0);

  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  triPipeA->setFront(*bellowA,2);
  triPipeA->createAll(System,*bellowA,"back");

  // insert-units : Origin : excludeSurf
  pipeMagGroup(System,*buildZone,triPipeA,
	       {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,triPipeA,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*triPipeA,"back",*pipeB);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeB,"back",*sixPortA);

  const int outerCellMulti=
    constructSystem::constructUnit
    (System,*buildZone,masterCell,*sixPortA,"back",*multiPipe);

  // BELLOWS:
  bellowAA->createAll(System,*multiPipe,2);
  bellowBA->addInsertCell(outerCellMulti);
  bellowBA->createAll(System,*multiPipe,3);

  bellowCA->addInsertCell(outerCellMulti);
  bellowCA->createAll(System,*multiPipe,4);

  const int outerCellBellow=
    buildZone->createOuterVoidUnit(System,masterCell,*bellowAA,2);
  bellowAA->insertInCell(System,outerCellBellow);
  bellowBA->insertInCell(System,outerCellBellow);

  CellMap::addCell("MultiCell",outerCellMulti);
  CellMap::addCell("BellowCell",outerCellBellow);
  buildZone->removeLastMaster(System);

  return;
}

void
Segment25::constructVoid(Simulation& System,
			 const attachSystem::FixedComp& FC) const
  /*!
    Creates the space for the InnerZone
  */
{
  ELog::RegMethod RegA("Segment25","constructVoid");

  const attachSystem::CellMap* CPtr=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  if (CPtr)
    {
      const HeadRule volHR=IZTop->getVolumeExclude();

      CPtr->insertComponent(System,"LongVoid",IZTop->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZMid->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZLower->getVolumeExclude());
    }
  return;
}

void
Segment25::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowAA,2);
  setLinkSignedCopy(2,*bellowBA,2);
  setLinkSignedCopy(3,*bellowCA,2);

  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"backMid");
  FixedComp::nameSideIndex(3,"backLower");

  joinItems.push_back(FixedComp::getFullRule("backFlat"));
  joinItems.push_back(FixedComp::getFullRule("backMid"));
  joinItems.push_back(FixedComp::getFullRule("backLower"));

  return;
}

void
Segment25::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment25","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  constructVoid(System,FC);
  return;
}


}   // NAMESPACE tdcSystem
