/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment28.cxx
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

#include "TDCsegment.h"
#include "Segment28.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment28::Segment28(const std::string& Key) :
  TDCsegment(Key,6),

  IZTop(new attachSystem::BlockZone(keyName+"IZTop")),
  IZFlat(new attachSystem::BlockZone(keyName+"IZFlat")),

  pipeAA(new constructSystem::VacuumPipe(keyName+"PipeAA")),
  pipeBA(new constructSystem::VacuumPipe(keyName+"PipeBA")),

  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),

  pipeAB(new constructSystem::VacuumPipe(keyName+"PipeAB")),
  pipeBB(new constructSystem::VacuumPipe(keyName+"PipeBB")),

  bellowAB(new constructSystem::Bellows(keyName+"BellowAB")),
  bellowBB(new constructSystem::Bellows(keyName+"BellowBB"))
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

  OR.addObject(pipeAB);
  OR.addObject(pipeBB);

  OR.addObject(bellowAB);
  OR.addObject(bellowBB);

  setFirstItems(pipeAA);
  setFirstItems(pipeBA);
}

Segment28::~Segment28()
  /*!
    Destructor
   */
{}


void
Segment28::createSplitInnerZone()
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment28","createSplitInnerZone");

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

  if (prevSegPtr && prevSegPtr->hasSurf("LowDivider"))
    {
      SurfMap::addSurf("LowDivider",prevSegPtr->getSurf("LowDivider"));
      SurfMap::addSurf("backLower",prevSegPtr->getLinkSurf("backLower"));
      HeadRule HR(SurfMap::getSurf("LowDivider"));
      HR.addUnion(SurfMap::getSurf("backLower"));
      HSurroundB.addIntersection(HR);
    }

  const Geometry::Vec3D ZEffective(FA.getZ());
  HSurroundA.removeMatchedPlanes(ZEffective,0.9);   // remove base
  HSurroundB.removeMatchedPlanes(-ZEffective,0.9);

  HSurroundA.addIntersection(SurfMap::getSurf("TopDivider"));
  HSurroundB.addIntersection(-SurfMap::getSurf("TopDivider"));

  IZTop->setFront(pipeAA->getFullRule(-1));
  IZFlat->setFront(pipeBA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZFlat->setSurround(HSurroundB);


  return;
}

void
Segment28::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment28","buildObjects");
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
  outerCellB=IZFlat->createUnit(System,*pipeBA,2);

  pipeAA->insertAllInCell(System,outerCellA);
  pipeBA->insertAllInCell(System,outerCellB);

  constructSystem::constructUnit
    (System,*IZTop,*pipeAA,"back",*bellowAA);
  constructSystem::constructUnit
    (System,*IZFlat,*pipeBA,"back",*bellowBA);

  constructSystem::constructUnit
    (System,*IZTop,*bellowAA,"back",*pipeAB);
  constructSystem::constructUnit
    (System,*IZFlat,*bellowBA,"back",*pipeBB);

  constructSystem::constructUnit
    (System,*IZTop,*pipeAB,"back",*bellowAB);
  constructSystem::constructUnit
    (System,*IZFlat,*pipeBB,"back",*bellowBB);

  outerCellA=IZTop->createUnit(System,*bellowBB,"back");
  CellMap::addCell("SpaceFiller",outerCellA);

  if (!prevSegPtr || !prevSegPtr->isBuilt())
    {
      HeadRule volume=buildZone->getFront();
      volume*=IZTop->getFront().complement();
      volume*=IZTop->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      volume=buildZone->getFront();
      volume*=IZFlat->getFront().complement();
      volume*=IZFlat->getSurround();
      makeCell("FrontSpace",System,cellIndex++,0,0.0,volume);
      buildZone->copyCells(*this,"FrontSpace");
    }
  return;
}

void
Segment28::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeAA,1);
  setLinkCopy(1,*bellowAB,2);
  setLinkCopy(2,*pipeBA,1);
  setLinkCopy(3,*bellowBB,2);


  FixedComp::nameSideIndex(0,"frontFlat");
  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"frontMid");
  FixedComp::nameSideIndex(3,"backMid");

  //    setLinkCopy(1,*triPipeA,2);
  joinItems.push_back(FixedComp::getFullRule("backFlat"));
  joinItems.push_back(FixedComp::getFullRule("backMid"));

  buildZone->setBack(FixedComp::getFullRule("backMid"));

  buildZone->copyCells(*this,"FrontSpace");
  buildZone->copyAllCells(*IZTop);
  buildZone->copyAllCells(*IZFlat);


  return;
}

void
Segment28::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment28","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
