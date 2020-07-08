/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment27.cxx
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
#include "Segment27.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment27::Segment27(const std::string& Key) :
  TDCsegment(Key,6),
  IZTop(new attachSystem::InnerZone(*this,cellIndex)),
  IZFlat(new attachSystem::InnerZone(*this,cellIndex)),
  IZLower(new attachSystem::InnerZone(*this,cellIndex)),
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

  bellowAC(new constructSystem::Bellows(keyName+"BellowAC")),
  bellowBC(new constructSystem::Bellows(keyName+"BellowBC"))
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

  OR.addObject(bellowAC);
  OR.addObject(bellowBC);

  setFirstItems(bellowAA);
}

Segment27::~Segment27()
  /*!
    Destructor
   */
{}


void
Segment27::createSplitInnerZone(Simulation& System)
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment27","createSplitInnerZone");
  
  *IZTop = *buildZone;
  *IZFlat = *buildZone;
  *IZLower = *buildZone;

  
  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();
  HeadRule HSurroundC=buildZone->getSurround();

  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*bellowAA,-1,*bellowBA,-1);
  FB.createPairVector(*bellowBA,-1,*bellowCA,-1);
  ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
  ModelSupport::buildPlane(SMap,buildIndex+5015,FB.getCentre(),FB.getZ());
  
  const Geometry::Vec3D ZEffective(FA.getZ());
  HSurroundA.removeMatchedPlanes(ZEffective);   // remove base
  HSurroundB.removeMatchedPlanes(ZEffective);   // remove both
  HSurroundB.removeMatchedPlanes(-ZEffective); 
  HSurroundC.removeMatchedPlanes(-ZEffective);  // remove top
 
  HSurroundA.addIntersection(SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(SMap.realSurf(buildIndex+5015));
  HSurroundC.addIntersection(-SMap.realSurf(buildIndex+5015));

  IZTop->setFront(bellowAA->getFullRule(-1));
  IZFlat->setFront(bellowBA->getFullRule(-1));
  IZLower->setFront(bellowCA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZFlat->setSurround(HSurroundB);
  IZLower->setSurround(HSurroundC);

  IZTop->constructMasterCell(System);
  IZFlat->constructMasterCell(System);
  IZLower->constructMasterCell(System);


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
  ELog::EM<<"Center ="<<this->getLinkPt(0)<<ELog::endDiag;
  bellowAA->createAll(System,*this,0);
  bellowBA->createAll(System,*this,0);
  bellowCA->createAll(System,*this,0);

  createSplitInnerZone(System);

  MonteCarlo::Object* masterCellA=IZTop->getMaster();
  MonteCarlo::Object* masterCellB=IZFlat->getMaster();
  MonteCarlo::Object* masterCellC=IZLower->getMaster();

  outerCellA=IZTop->createOuterVoidUnit(System,masterCellA,*bellowAA,2);
  outerCellB=IZFlat->createOuterVoidUnit(System,masterCellB,*bellowBA,2);
  outerCellC=IZLower->createOuterVoidUnit(System,masterCellC,*bellowCA,2);

  bellowAA->insertInCell(System,outerCellA);
  bellowBA->insertInCell(System,outerCellB);
  bellowCA->insertInCell(System,outerCellC);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*bellowAA,"back",*pipeAA);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*bellowBA,"back",*pipeBA);
  constructSystem::constructUnit
    (System,*IZLower,masterCellC,*bellowCA,"back",*pipeCA);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*pipeAA,"back",*bellowAB);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*pipeBA,"back",*bellowBB);
  constructSystem::constructUnit
    (System,*IZLower,masterCellC,*pipeCA,"back",*bellowCB);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*bellowAB,"back",*yagUnitA);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*bellowBB,"back",*yagUnitB);
  constructSystem::constructUnit
    (System,*IZLower,masterCellC,*bellowCB,"back",*yagUnitC);

  
  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*yagUnitA,"back",*bellowAC);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*yagUnitB,"back",*bellowBC);
  
  IZTop->removeLastMaster(System);
  IZFlat->removeLastMaster(System);
  IZLower->removeLastMaster(System);  

  return;
}

void
Segment27::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment27","createLinks");
  
  setLinkSignedCopy(0,*bellowAA,1);
  setLinkSignedCopy(1,*bellowAC,2);

  setLinkSignedCopy(2,*bellowBA,1);
  setLinkSignedCopy(3,*bellowBC,2);

  setLinkSignedCopy(4,*bellowCA,1);
  setLinkSignedCopy(5,*yagUnitC,2);

  FixedComp::nameSideIndex(0,"frontFlat");
  FixedComp::nameSideIndex(1,"backFlat");
  FixedComp::nameSideIndex(2,"frontMid");
  FixedComp::nameSideIndex(3,"backMid");
  FixedComp::nameSideIndex(4,"frontLower");
  FixedComp::nameSideIndex(5,"backLower");

  joinItems.push_back(FixedComp::getFullRule(2));
    
  return;
}

void
Segment27::constructVoid(Simulation& System,
			 const attachSystem::FixedComp& FC) const
  /*!
    Creates the space for the InnerZone
  */
{
  ELog::RegMethod RegA("Segment27","constructVoid");

  const attachSystem::CellMap* CPtr=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  if (CPtr)
    {
      const HeadRule volHR=IZTop->getVolumeExclude();

      CPtr->insertComponent(System,"LongVoid",IZTop->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZFlat->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZLower->getVolumeExclude());
    }
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
  createLinks();
  constructVoid(System,FC);
  return;
}


}   // NAMESPACE tdcSystem
