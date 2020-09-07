/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment29.cxx
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
#include "Object.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
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
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumPipe.h"
#include "YagUnit.h"
#include "YagScreen.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment29.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment29::Segment29(const std::string& Key) :
  TDCsegment(Key,6),
  IZTop(new attachSystem::InnerZone(*this,cellIndex)),
  IZMid(new attachSystem::InnerZone(*this,cellIndex)),

  pipeAA(new constructSystem::VacuumPipe(keyName+"PipeAA")),
  pipeBA(new constructSystem::VacuumPipe(keyName+"PipeBA")),

  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),


  yagUnitA(new tdcSystem::YagUnit(keyName+"YagUnitA")),
  yagUnitB(new tdcSystem::YagUnit(keyName+"YagUnitB")),

  yagScreenA(new tdcSystem::YagScreen(keyName+"YagScreenA")),
  yagScreenB(new tdcSystem::YagScreen(keyName+"YagScreenB"))

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

  setFirstItems(pipeAA);
  setFirstItems(pipeBA);
}

Segment29::~Segment29()
  /*!
    Destructor
   */
{}


void
Segment29::createSplitInnerZone(Simulation& System)
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment29","createSplitInnerZone");

  *IZTop = *buildZone;
  *IZMid = *buildZone;

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
  HSurroundA.removeMatchedPlanes(ZEffective);   // remove base
  HSurroundB.removeMatchedPlanes(-ZEffective);

  HSurroundA.addIntersection(SurfMap::getSurf("TopDivider"));
  HSurroundB.addIntersection(-SurfMap::getSurf("TopDivider"));

  IZTop->setFront(pipeAA->getFullRule(-1));
  IZMid->setFront(pipeBA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZMid->setSurround(HSurroundB);

  IZTop->constructMasterCell(System);
  IZMid->constructMasterCell(System);

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

  pipeAA->createAll(System,*this,0);
  pipeBA->createAll(System,*this,0);

  createSplitInnerZone(System);

  MonteCarlo::Object* masterCellA=IZTop->getMaster();
  MonteCarlo::Object* masterCellB=IZMid->getMaster();

  outerCellA=IZTop->createOuterVoidUnit(System,masterCellA,*pipeAA,2);
  outerCellB=IZMid->createOuterVoidUnit(System,masterCellB,*pipeBA,2);

  pipeAA->insertInCell(System,outerCellA);
  pipeBA->insertInCell(System,outerCellB);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*pipeAA,"back",*bellowAA);
  constructSystem::constructUnit
    (System,*IZMid,masterCellB,*pipeBA,"back",*bellowBA);

  outerCellA = constructSystem::constructUnit
    (System,*IZTop,masterCellA,*bellowAA,"back",*yagUnitA);

  yagScreenA->setBeamAxis(*yagUnitA,1);
  yagScreenA->createAll(System,*yagUnitA,-3);
  yagScreenA->insertInCell("Outer",System,outerCellA);
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("PlateA"));
  yagScreenA->insertInCell("Connect",System,yagUnitA->getCell("Void"));
  yagScreenA->insertInCell("Payload",System,yagUnitA->getCell("Void"));

  outerCellB = constructSystem::constructUnit
    (System,*IZMid,masterCellB,*bellowBA,"back",*yagUnitB);

  yagScreenB->setBeamAxis(*yagUnitA,1);
  yagScreenB->createAll(System,*yagUnitB,-3);
  yagScreenB->insertInCell("Outer",System,outerCellB);
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("PlateA"));
  yagScreenB->insertInCell("Connect",System,yagUnitB->getCell("Void"));
  yagScreenB->insertInCell("Payload",System,yagUnitB->getCell("Void"));

  IZTop->removeLastMaster(System);
  IZMid->removeLastMaster(System);

  return;
}

void
Segment29::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeAA,1);
  setLinkSignedCopy(1,*yagUnitA,2);
  setLinkSignedCopy(2,*pipeBA,1);
  setLinkSignedCopy(3,*yagUnitB,2);


  FixedComp::nameSideIndex(0,"frontFlat");
  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"frontMid");
  FixedComp::nameSideIndex(3,"backMid");

  //    setLinkSignedCopy(1,*triPipeA,2);
  joinItems.push_back(FixedComp::getFullRule("backFlat"));
  joinItems.push_back(FixedComp::getFullRule("backMid"));

  return;
}

void
Segment29::constructVoid(Simulation& System,
			 const attachSystem::FixedComp& FC) const
  /*!
    Creates the space for the InnerZone
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Segment29","constructVoid");

  const attachSystem::CellMap* CPtr=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  if (CPtr)
    {
      CPtr->insertComponent(System,"LongVoid",IZTop->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZMid->getVolumeExclude());
    }
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
  constructVoid(System,FC);
  return;
}


}   // NAMESPACE tdcSystem
