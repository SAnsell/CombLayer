/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment26.cxx
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
#include "LocalShielding.h"

#include "TDCsegment.h"
#include "Segment26.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment26::Segment26(const std::string& Key) :
  TDCsegment(Key,6),

  IZTop(new attachSystem::BlockZone(keyName+"IZTop")),
  IZMid(new attachSystem::BlockZone(keyName+"IZMid")),
  IZLower(new attachSystem::BlockZone(keyName+"IZLower")),

  pipeAA(new constructSystem::VacuumPipe(keyName+"PipeAA")),
  pipeBA(new constructSystem::VacuumPipe(keyName+"PipeBA")),
  pipeCA(new constructSystem::VacuumPipe(keyName+"PipeCA")),

  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),

  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),
  bellowCA(new constructSystem::Bellows(keyName+"BellowCA")),

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

  OR.addObject(pipeAA);
  OR.addObject(pipeBA);
  OR.addObject(pipeCA);

  OR.addObject(shieldA);

  OR.addObject(bellowAA);
  OR.addObject(bellowBA);
  OR.addObject(bellowCA);

  OR.addObject(yagUnitA);
  OR.addObject(yagScreenA);
  OR.addObject(yagUnitB);
  OR.addObject(yagScreenB);

  OR.addObject(pipeAB);
  OR.addObject(pipeBB);
  OR.addObject(pipeCB);

  setFirstItems(pipeAA);
  setFirstItems(pipeBA);
  setFirstItems(pipeCA);
}

Segment26::~Segment26()
  /*!
    Destructor
   */
{}

void
Segment26::insertPrevSegment(Simulation& System,
			     const TDCsegment* prevSegPtr) const
  /*!
    Insert components that need to be in previous
    objects
    \param System :: Simulation
    \param prevSegPtr :: Previous segment points
   */
{
  ELog::RegMethod RegA("Segment26","insertPrevSegment");

  if (prevSegPtr && prevSegPtr->hasCell("BellowCell"))
    {
      pipeAA->insertAllInCell(System,prevSegPtr->getCell("BellowCell"));
      pipeBA->insertAllInCell(System,prevSegPtr->getCell("BellowCell"));
      pipeCA->insertAllInCell(System,prevSegPtr->getCell("BellowCell"));
      if (prevSegPtr->hasCell("MultiCell"))
	pipeCA->insertAllInCell(System,prevSegPtr->getCell("MultiCell"));
    }
  return;
}

void
Segment26::createSplitInnerZone()
  /*!
    Spilit the innerZone into three parts.
   */
{
  ELog::RegMethod RegA("Segment26","createSplitInnerZone");

  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();
  HeadRule HSurroundC=buildZone->getSurround();

//  const HeadRule HBack=buildZone->getBack();
//  ELog::EM<<"BACK == "<<HBack<<ELog::endDiag;

  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*pipeAA,2,*pipeBA,2);
  FB.createPairVector(*pipeBA,2,*pipeCA,2);

  ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
  ModelSupport::buildPlane(SMap,buildIndex+5015,FB.getCentre(),FB.getZ());
  SurfMap::addSurf("TopDivider",SMap.realSurf(buildIndex+5005));
  SurfMap::addSurf("LowDivider",SMap.realSurf(buildIndex+5015));

  const Geometry::Vec3D ZEffective(FA.getZ());

  HSurroundA.removeMatchedPlanes(ZEffective,0.9);   // remove base
  HSurroundB.removeMatchedPlanes(ZEffective,0.9);   // remove both
  HSurroundB.removeMatchedPlanes(-ZEffective,0.9);
  HSurroundC.removeMatchedPlanes(-ZEffective,0.9);  // remove top

  HSurroundA.addIntersection(SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5015));
  HSurroundC.addIntersection(-SMap.realSurf(buildIndex+5015));

  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {
      IZTop->setFront(pipeAA->getFullRule(-1));
      IZMid->setFront(pipeBA->getFullRule(-1));
      IZLower->setFront(pipeCA->getFullRule(-1));
    }
  else
    {
      IZTop->setFront(pipeAA->getFullRule(-1));
      IZMid->setFront(pipeAA->getFullRule(-1));
      IZLower->setFront(pipeAA->getFullRule(-1));
    }

  IZTop->setSurround(HSurroundA);
  IZMid->setSurround(HSurroundB);
  IZLower->setSurround(HSurroundC);

  return;
}

void
Segment26::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment26","buildObjects");

  int outerCellA,outerCellB,outerCellC;

  if (joinItems.size()>=3)
    {
      pipeAA->setFront(joinItems[0].display());
      pipeBA->setFront(joinItems[1].display());
      pipeCA->setFront(joinItems[2].display());
    }

  pipeAA->createAll(System,*this,0);
  pipeBA->createAll(System,*this,0);
  pipeCA->createAll(System,*this,0);

  createSplitInnerZone();

  // Local shielding wall

  shieldA->setCutSurf("Inner",
		      pipeAA->getFullRule("outerPipe") *
		      pipeBA->getFullRule("outerPipe") *
		      pipeCA->getFullRule("outerPipe"));
  shieldA->createAll(System, *pipeBA, "#front");


  // IZTop
  outerCellA = IZTop->createUnit(System, *shieldA, -1);

  attachSystem::ContainedGroup *CGPtr =
    dynamic_cast<attachSystem::ContainedGroup*>(pipeAA.get());
  CGPtr->insertAllInCell(System,outerCellA);

  outerCellA=IZTop->createUnit(System,*shieldA,2);
  shieldA->insertInCell(System,outerCellA);

  // IZMid
  outerCellB = IZMid->createUnit(System, *shieldA, -1);
  CGPtr = dynamic_cast<attachSystem::ContainedGroup*>(pipeBA.get());
  CGPtr->insertAllInCell(System,outerCellB);

  outerCellB=IZMid->createUnit(System,*shieldA,2);
  shieldA->insertInCell(System,outerCellB);

  // IZLower
  outerCellC = IZLower->createUnit(System, *shieldA, -1);
  CGPtr = dynamic_cast<attachSystem::ContainedGroup*>(pipeCA.get());
  CGPtr->insertAllInCell(System,outerCellC);

  outerCellC=IZLower->createUnit(System,*shieldA,2);
  shieldA->insertInCell(System,outerCellC);

  ////////////

  outerCellA=IZTop->createUnit(System,*pipeAA,2);
  outerCellB=IZMid->createUnit(System,*pipeBA,2);
  outerCellC=IZLower->createUnit(System,*pipeCA,2);

  pipeAA->insertAllInCell(System,outerCellA);
  pipeBA->insertAllInCell(System,outerCellB);
  pipeCA->insertAllInCell(System,outerCellC);

  // BELLOWS B:
  constructSystem::constructUnit
    (System,*IZTop,*pipeAA,"back",*bellowAA);
  constructSystem::constructUnit
    (System,*IZMid,*pipeBA,"back",*bellowBA);
  constructSystem::constructUnit
    (System,*IZLower,*pipeCA,"back",*bellowCA);

  // YAG SCREENS:
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

  yagScreenB->setBeamAxis(*yagUnitB,1);
  yagScreenB->createAll(System,*yagUnitB,-3);
  yagScreenB->insertInCell("Outer",System,outerCellB);
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("PlateA"));
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("Void"));
  yagScreenB->insertInCell("Payload",System,yagUnitB->getCell("Void"));

  constructSystem::constructUnit
    (System,*IZTop,*yagUnitA,"back",*pipeAB);
  constructSystem::constructUnit
    (System,*IZMid,*yagUnitB,"back",*pipeBB);
  constructSystem::constructUnit
    (System,*IZLower,*bellowCA,"back",*pipeCB);


  return;
}


void
Segment26::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment26","createLinks");

  setLinkCopy(0,*pipeAA,1);
  setLinkCopy(1,*pipeAB,2);

  setLinkCopy(2,*pipeBA,1);
  setLinkCopy(3,*pipeBB,2);

  setLinkCopy(4,*pipeCA,1);
  setLinkCopy(5,*pipeCB,2);

  FixedComp::nameSideIndex(0,"frontFlat");
  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"frontMid");
  FixedComp::nameSideIndex(3,"backMid");
  FixedComp::nameSideIndex(4,"frontLower");
  FixedComp::nameSideIndex(5,"backLower");

  // push back all the joing items:
  joinItems.push_back(FixedComp::getFullRule("backFlat"));
  joinItems.push_back(FixedComp::getFullRule("backMid"));
  joinItems.push_back(FixedComp::getFullRule("backLower"));

  buildZone->setBack(FixedComp::getFullRule("backMid"));

  buildZone->copyAllCells(*IZTop);
  buildZone->copyAllCells(*IZMid);
  buildZone->copyAllCells(*IZLower);

  return;
}

void
Segment26::buildFrontSpacer(Simulation& System)
  /*!
    Build the front spacer if needed
    \param System :: simulation for consructuion
   */
{
  ELog::RegMethod RegA("Segment26","buildFrontSpacer");

  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {
      HeadRule volume=buildZone->getFront();
      volume*=IZTop->getFront().complement();
      volume*=IZTop->getSurround();
      volume.addIntersection(SMap.realSurf(buildIndex+5015));
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZMid->getFront().complement();
      volume*=IZMid->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZLower->getFront().complement();
      volume*=IZLower->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
    }
  return;
}

void
Segment26::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment26","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  buildFrontSpacer(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
