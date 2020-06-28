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
#include "Segment28.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment28::Segment28(const std::string& Key) :
  TDCsegment(Key,2),
  IZTop(new attachSystem::InnerZone(*this,cellIndex)),
  IZFlat(new attachSystem::InnerZone(*this,cellIndex)),


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

  setFirstItem(pipeAA);
}

Segment28::~Segment28()
  /*!
    Destructor
   */
{}


void
Segment28::createSplitInnerZone(Simulation& System)
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment28","createSplitInnerZone");
  
  *IZTop = *buildZone;
  *IZFlat = *buildZone;

  
  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();

  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*bellowAA,-1,*bellowBA,-1);
  ModelSupport::buildPlane(SMap,buildIndex+5005,FA.getCentre(),FA.getZ());
  
  const Geometry::Vec3D ZEffective(FA.getZ());
  HSurroundA.removeMatchedPlanes(ZEffective);   // remove base
  HSurroundB.removeMatchedPlanes(-ZEffective); 
 
  HSurroundA.addIntersection(SMap.realSurf(buildIndex+5005));
  HSurroundB.addIntersection(-SMap.realSurf(buildIndex+5005));

  IZTop->setFront(bellowAA->getFullRule(-1));
  IZFlat->setFront(bellowBA->getFullRule(-1));

  IZTop->setSurround(HSurroundA);
  IZFlat->setSurround(HSurroundB);

  IZTop->constructMasterCell(System);
  IZFlat->constructMasterCell(System);

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

  pipeAA->createAll(System,*this,0);
  pipeBA->createAll(System,*this,0);

  createSplitInnerZone(System);

  MonteCarlo::Object* masterCellA=IZTop->getMaster();
  MonteCarlo::Object* masterCellB=IZFlat->getMaster();

  outerCellA=IZTop->createOuterVoidUnit(System,masterCellA,*pipeAA,2);
  outerCellB=IZFlat->createOuterVoidUnit(System,masterCellB,*pipeBA,2);

  pipeAA->insertInCell(System,outerCellA);
  pipeBA->insertInCell(System,outerCellB);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*pipeAA,"back",*bellowAA);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*pipeBA,"back",*bellowBA);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*bellowAA,"back",*pipeAB);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*bellowBA,"back",*pipeBB);

  constructSystem::constructUnit
    (System,*IZTop,masterCellA,*pipeAB,"back",*bellowAB);
  constructSystem::constructUnit
    (System,*IZFlat,masterCellB,*pipeBB,"back",*bellowBB);

  
  IZTop->removeLastMaster(System);
  IZFlat->removeLastMaster(System);

  return;
}

void
Segment28::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeAA,1);
  setLinkSignedCopy(1,*bellowBA,2);
  //    setLinkSignedCopy(1,*triPipeA,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  
  
  return;
}

void
Segment28::constructVoid(Simulation& System,
			 const attachSystem::FixedComp& FC) const
  /*!
    Creates the space for the InnerZone
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Segment28","constructVoid");

  const attachSystem::CellMap* CPtr=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  if (CPtr)
    {
      CPtr->insertComponent(System,"LongVoid",IZTop->getVolumeExclude());
      CPtr->insertComponent(System,"LongVoid",IZFlat->getVolumeExclude());
    }
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
  constructVoid(System,FC);
  return;
}


}   // NAMESPACE tdcSystem
