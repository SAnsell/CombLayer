/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment27.cxx
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

#include "Line.h"
#include "ContainedGroup.h"
#include "YagScreen.h"
#include "YagUnit.h"
#include "LBeamStop.h"

#include "TDCsegment.h"
#include "Segment27.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment27::Segment27(const std::string& Key) :
  TDCsegment(Key,6),

  IZTop(new attachSystem::BlockZone(keyName+"IZTop")),
  IZFlat(new attachSystem::BlockZone(keyName+"IZFlat")),
  IZLower(new attachSystem::BlockZone(keyName+"IZLower")),

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
  yagUnitC(new tdcSystem::YagUnit(keyName+"YagUnitC")),

  yagScreenA(new tdcSystem::YagScreen(keyName+"YagScreenA")),
  yagScreenB(new tdcSystem::YagScreen(keyName+"YagScreenB")),
  yagScreenC(new tdcSystem::YagScreen(keyName+"YagScreenC")),

  bellowAC(new constructSystem::Bellows(keyName+"BellowAC")),
  bellowBC(new constructSystem::Bellows(keyName+"BellowBC")),

  beamStopC(new tdcSystem::LBeamStop(keyName+"BeamStopC"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

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
  OR.addObject(yagUnitB);
  OR.addObject(yagUnitC);

  OR.addObject(yagScreenA);
  OR.addObject(yagScreenB);
  OR.addObject(yagScreenC);

  OR.addObject(bellowAC);
  OR.addObject(bellowBC);

  OR.addObject(beamStopC);

  setFirstItems(bellowAA);
  setFirstItems(bellowBA);
  setFirstItems(bellowCA);
}

Segment27::~Segment27()
  /*!
    Destructor
   */
{}


void
Segment27::createSplitInnerZone()
  /*!
    Split the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment27","createSplitInnerZone");

  *IZTop=*buildZone;
  *IZFlat=*buildZone;
  *IZLower=*buildZone;

  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();
  HeadRule HSurroundC=buildZone->getSurround();

  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*bellowAA,-1,*bellowBA,-1);
  FB.createPairVector(*bellowBA,-1,*bellowCA,-1);

  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {
      // create surfaces
      ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
      ModelSupport::buildPlane(SMap,buildIndex+5015,FB.getCentre(),FB.getZ());
      SurfMap::addSurf("TopDivider",SMap.realSurf(buildIndex+5005));
      SurfMap::addSurf("LowDivider",SMap.realSurf(buildIndex+5015));
    }
  else
    {
      SurfMap::addSurf("TopDivider",prevSegPtr->getSurf("TopDivider"));
      SurfMap::addSurf("LowDivider",prevSegPtr->getSurf("LowDivider"));
    }


  const Geometry::Vec3D ZEffective(FA.getZ());
  HSurroundA.removeMatchedPlanes(ZEffective,0.9);   // remove base
  HSurroundB.removeMatchedPlanes(ZEffective,0.9);   // remove both
  HSurroundB.removeMatchedPlanes(-ZEffective,0.9);
  HSurroundC.removeMatchedPlanes(-ZEffective,0.9);  // remove top

  HSurroundA.addIntersection(SurfMap::getSurf("TopDivider"));
  HSurroundB.addIntersection(-SurfMap::getSurf("TopDivider"));
  HSurroundB.addIntersection(SurfMap::getSurf("LowDivider"));
  HSurroundC.addIntersection(-SurfMap::getSurf("LowDivider"));

  IZTop->setFront(bellowAA->getFullRule(-1));
  IZFlat->setFront(bellowBA->getFullRule(-1));
  IZLower->setFront(bellowCA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZFlat->setSurround(HSurroundB);
  IZLower->setSurround(HSurroundC);

  return;
}

void
Segment27::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment27","buildObjects");

  int outerCellA,outerCellB,outerCellC;

  if (joinItems.size()>=3)
    {
      bellowAA->setFront(joinItems[0].display());
      bellowBA->setFront(joinItems[1].display());
      bellowCA->setFront(joinItems[2].display());
    }

  bellowAA->createAll(System,*this,0);
  bellowBA->createAll(System,*this,0);
  bellowCA->createAll(System,*this,0);

  createSplitInnerZone();

  outerCellA=IZTop->createUnit(System,*bellowAA,2);
  outerCellB=IZFlat->createUnit(System,*bellowBA,2);
  outerCellC=IZLower->createUnit(System,*bellowCA,2);

  bellowAA->insertInCell(System,outerCellA);
  bellowBA->insertInCell(System,outerCellB);
  bellowCA->insertInCell(System,outerCellC);

  constructSystem::constructUnit
    (System,*IZTop,*bellowAA,"back",*pipeAA);
  constructSystem::constructUnit
    (System,*IZFlat,*bellowBA,"back",*pipeBA);
  constructSystem::constructUnit
    (System,*IZLower,*bellowCA,"back",*pipeCA);

  constructSystem::constructUnit
    (System,*IZTop,*pipeAA,"back",*bellowAB);
  constructSystem::constructUnit
    (System,*IZFlat,*pipeBA,"back",*bellowBB);
  constructSystem::constructUnit
    (System,*IZLower,*pipeCA,"back",*bellowCB);

  outerCellA = constructSystem::constructUnit
    (System,*IZTop,*bellowAB,"back",*yagUnitA);

  yagScreenA->setBeamAxis(*yagUnitA,1);
  yagScreenA->createAll(System,*yagUnitA,-3);
  yagScreenA->insertInCell("Outer",System,outerCellA);
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("PlateA"));
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("Void"));
  yagScreenA->insertInCell("Payload",System,yagUnitA->getCell("Void"));

  outerCellB = constructSystem::constructUnit
    (System,*IZFlat,*bellowBB,"back",*yagUnitB);

  yagScreenB->setBeamAxis(*yagUnitB,1);
  yagScreenB->createAll(System,*yagUnitB,-3);
  yagScreenB->insertInCell("Outer",System,outerCellB);
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("PlateA"));
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("Void"));
  yagScreenB->insertInCell("Payload",System,yagUnitB->getCell("Void"));

  outerCellC = constructSystem::constructUnit
    (System,*IZLower,*bellowCB,"back",*yagUnitC);

  yagScreenC->setBeamAxis(*yagUnitC,1);
  yagScreenC->createAll(System,*yagUnitC,-3);
  yagScreenC->insertInCell("Outer",System,outerCellC);
  yagScreenC->insertInCell("Connect",System,yagUnitC->getCell("PlateA"));
  yagScreenC->insertInCell("Connect",System,yagUnitC->getCell("Void"));
  yagScreenC->insertInCell("Payload",System,yagUnitC->getCell("Void"));

  constructSystem::constructUnit
    (System,*IZTop,*yagUnitA,"back",*bellowAC);
  constructSystem::constructUnit
    (System,*IZFlat,*yagUnitB,"back",*bellowBC);

  constructSystem::constructUnit
  (System,*IZLower,*yagUnitC,"back",*beamStopC);

  outerCellA=IZTop->createUnit(System,*beamStopC,"back");
  CellMap::addCell("SpaceFiller",outerCellA);

  outerCellB=IZFlat->createUnit(System,*beamStopC,"back");
  CellMap::addCell("SpaceFiller",outerCellB);

  return;
}

void
Segment27::buildFrontSpacer(Simulation& System)
  /*!
    Build the front spacer if needed
   */
{
  ELog::RegMethod RegA("Segment27","buildFrontSpacer");

  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {

      HeadRule volume=buildZone->getFront();
      volume*=IZTop->getFront().complement();
      volume*=IZTop->getSurround();
      volume.addIntersection(SMap.realSurf(buildIndex+5015));
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZFlat->getFront().complement();
      volume*=IZFlat->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZLower->getFront().complement();
      volume*=IZLower->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
    }
    return;
}

void
Segment27::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment27","createLinks");

  setLinkCopy(0,*bellowAA,1);
  setLinkCopy(1,*bellowAC,2);

  setLinkCopy(2,*bellowBA,1);
  setLinkCopy(3,*bellowBC,2);

  setLinkCopy(4,*bellowCA,1);
  setLinkCopy(5,*beamStopC,2);

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

  buildZone->setBack(FixedComp::getFullRule("backLower"));

  buildZone->copyCells(*this,"FrontSpace");
  buildZone->copyAllCells(*IZTop);
  buildZone->copyAllCells(*IZFlat);
  buildZone->copyAllCells(*IZLower);

  return;
}

void
Segment27::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment27","build");

  FixedRotate::populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  buildObjects(System);
  buildFrontSpacer(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
