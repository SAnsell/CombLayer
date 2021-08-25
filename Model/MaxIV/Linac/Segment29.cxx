/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment29.cxx
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
#include "FixedUnit.h"
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
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumPipe.h"
#include "YagUnit.h"
#include "YagScreen.h"
#include "VoidUnit.h"
#include "NBeamStop.h"
#include "LBeamStop.h"
#include "BeamWing.h"
#include "BeamBox.h"
#include "LowBeamBox.h"

#include "TDCsegment.h"
#include "Segment29.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment29::Segment29(const std::string& Key) :
  TDCsegment(Key,6),

  IZTop(new attachSystem::BlockZone(keyName+"IZTop")),
  IZMid(new attachSystem::BlockZone(keyName+"IZMid")),

  pipeAA(new constructSystem::VacuumPipe(keyName+"PipeAA")),
  pipeBA(new constructSystem::VacuumPipe(keyName+"PipeBA")),

  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),

  yagUnitA(new tdcSystem::YagUnit(keyName+"YagUnitA")),
  yagUnitB(new tdcSystem::YagUnit(keyName+"YagUnitB")),

  yagScreenA(new tdcSystem::YagScreen(keyName+"YagScreenA")),
  yagScreenB(new tdcSystem::YagScreen(keyName+"YagScreenB")),

  endVoid(new constructSystem::VoidUnit(keyName+"EndVoid")),
  
  beamStopA(new tdcSystem::NBeamStop(keyName+"BeamStopA")),
  beamStopB(new tdcSystem::NBeamStop(keyName+"BeamStopB")),

  beamWingA(new tdcSystem::BeamWing(keyName+"BeamWingA")),
  beamWingB(new tdcSystem::BeamWing(keyName+"BeamWingB")),
  beamBoxA(new tdcSystem::BeamBox(keyName+"BeamBoxA")),
  beamBoxB(new tdcSystem::LowBeamBox(keyName+"BeamBoxB"))
  
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeAA);
  OR.addObject(pipeBA);

  OR.addObject(bellowAA);
  OR.addObject(bellowBA);

  OR.addObject(yagUnitA);
  OR.addObject(yagUnitB);

  OR.addObject(yagScreenA);
  OR.addObject(yagScreenB);

  OR.addObject(beamStopA);
  OR.addObject(beamStopB);

  OR.addObject(beamWingA);
  OR.addObject(beamWingB);
  OR.addObject(beamBoxA);
  OR.addObject(beamBoxB);
  
  setFirstItems(pipeAA);
  setFirstItems(pipeBA);
}

Segment29::~Segment29()
  /*!
    Destructor
   */
{}


void
Segment29::createSplitInnerZone()
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment29","createSplitInnerZone");

  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();

  // create surfaces
  attachSystem::FixedUnit FA("FA");
  FA.createPairVector(*pipeAA,-1,*pipeBA,-1);

  if (!prevSegPtr || !prevSegPtr->hasSurf("TopDivider"))
    {
      // create surfaces
      ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
      SurfMap::addSurf("TopDivider",SMap.realSurf(buildIndex+5005));
    }
  else
    {
      SurfMap::addSurf("TopDivider",prevSegPtr->getSurf("TopDivider"));
    }

  const Geometry::Vec3D ZEffective(FA.getZ());
  HSurroundA.removeMatchedPlanes(ZEffective,0.9);   // remove base
  HSurroundB.removeMatchedPlanes(-ZEffective,0.9);

  HSurroundA.addIntersection(SurfMap::getSurf("TopDivider"));
  HSurroundB.addIntersection(-SurfMap::getSurf("TopDivider"));

  IZTop->setFront(pipeAA->getFullRule(-1));
  IZMid->setFront(pipeBA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZMid->setSurround(HSurroundB);

  return;
}

void
Segment29::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment29","buildObjects");

  int outerCellA,outerCellB;

  if (isActive("front"))
    pipeAA->copyCutSurf("front",*this,"front");
  
  if (firstItemVec.size()>=2)
    {
      if (prevSegPtr && prevSegPtr->hasLinkSurf("backMid"))
	pipeBA->setFront(*prevSegPtr,"backMid");
    }
  
  pipeAA->createAll(System,*this,0);
  pipeBA->createAll(System,*this,0);

  createSplitInnerZone();
  
  outerCellA=IZTop->createUnit(System,*pipeAA,2);
  outerCellB=IZMid->createUnit(System,*pipeBA,2);

  pipeAA->insertAllInCell(System,outerCellA);
  pipeBA->insertAllInCell(System,outerCellB);

  constructSystem::constructUnit
    (System,*IZTop,*pipeAA,"back",*bellowAA);
  constructSystem::constructUnit
    (System,*IZMid,*pipeBA,"back",*bellowBA);

  outerCellA = constructSystem::constructUnit
    (System,*IZTop,*bellowAA,"back",*yagUnitA);

  yagScreenA->setBeamAxis(*yagUnitA,1);
  yagScreenA->createAll(System,*yagUnitA,-3);
  yagScreenA->insertInCell("Outer",System,outerCellA);
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("PlateA"));
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("Void"));
  yagScreenA->insertInCell("Payload",System,yagUnitA->getCell("Void"));

  outerCellB = constructSystem::constructUnit
    (System,*IZMid,*bellowBA,"back",*yagUnitB);

  yagScreenB->setBeamAxis(*yagUnitA,1);
  yagScreenB->createAll(System,*yagUnitB,-3);
  yagScreenB->insertInCell("Outer",System,outerCellB);
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("PlateA"));
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("Void"));
  yagScreenB->insertInCell("Payload",System,yagUnitB->getCell("Void"));

  outerCellA = constructSystem::constructUnit
    (System,*IZTop,*yagUnitA,"back",*endVoid);
  
  outerCellB =IZMid->createUnit(System,*endVoid,"back");
  
  // Create Final object:
  const HeadRule& frontHR=IZTop->getBack();
  const HeadRule& backHR=ExternalCut::getRule("BackWallFront");
  const HeadRule& surHR=buildZone->getSurround();

  makeCell("EndVoid",System,cellIndex++,0,0.0,
	   frontHR*backHR.complement()*surHR);
  const int outerVoid=CellMap::getCell("EndVoid");
  
  
  // inital cell if needed
  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {
      HeadRule volume=buildZone->getFront();
      volume*=IZTop->getFront().complement();
      volume*=IZTop->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZMid->getFront().complement();
      volume*=IZMid->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      buildZone->copyCells(*this,"FrontSpace");
    }
  // beamboxes
  beamBoxA->addInsertCell(outerVoid);
  beamBoxA->createAll(System,*yagUnitA,"back");

  beamBoxB->setCutSurf("top",*beamBoxA,"base");
  beamBoxB->setCutSurf("base",getRule("Floor"));

  beamBoxB->addInsertCell(outerVoid);
  beamBoxB->createAll(System,*yagUnitA,"back");
  
  beamStopA->addInsertCell(beamBoxA->getCells("Void"));
  beamStopA->createAll(System,*yagUnitA,"back");

  beamStopB->setCutSurf("front",frontHR);
  beamStopB->setCutSurf("base",ExternalCut::getRule("Floor"));
  beamStopB->addInsertCell(beamBoxB->getCells("Void"));
  beamStopB->createAll(System,*yagUnitB,"back");

  return;
}

void
Segment29::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeAA,1);
  setLinkCopy(1,*endVoid,2);
  setLinkCopy(2,*pipeBA,1);
  setLinkCopy(3,*endVoid,2);


  FixedComp::nameSideIndex(0,"frontFlat");
  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"frontMid");
  FixedComp::nameSideIndex(3,"backMid");

  //    setLinkCopy(1,*triPipeA,2);
  joinItems.push_back(FixedComp::getFullRule("backFlat"));

  buildZone->setBack(ExternalCut::getRule("BackWallFront"));

  buildZone->copyCells(*this,"FrontSpace");
  buildZone->copyAllCells(*IZTop);
  buildZone->copyAllCells(*IZMid);
  return;
}

void
Segment29::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment29","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);

	  
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
