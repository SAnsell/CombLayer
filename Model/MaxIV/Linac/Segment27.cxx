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
  TDCsegment(Key,2),
  IZTop(new attachSystem::InnerZone(*this,cellIndex)),
  IZFlat(new attachSystem::InnerZone(*this,cellIndex)),
  IZLower(new attachSystem::InnerZone(*this,cellIndex)),
  bellowAA(new constructSystem::Bellows(keyName+"BellowAA")),
  bellowBA(new constructSystem::Bellows(keyName+"BellowBA")),
  bellowCA(new constructSystem::Bellows(keyName+"BellowCA"))
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

  setFirstItem(bellowAA);
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
  ELog::EM<<"FA == "<<FA.getZ()<<ELog::endDiag;
  ELog::EM<<"FB == "<<FB.getZ()<<ELog::endDiag;
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

  bellowAA->createAll(System,*this,0);
  bellowBA->createAll(System,*this,0);
  bellowCA->createAll(System,*this,0);

  for(const int CN : buildZone->getInsertCell())
    ELog::EM<<"CN == "<<CN<<ELog::endDiag;
  
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

  System.removeCell(1030001);
  IZTop->removeLastMaster(System);
  IZFlat->removeLastMaster(System);
  IZLower->removeLastMaster(System);
  return;

  
  return;
}

void
Segment27::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowAA,1);
  setLinkSignedCopy(1,*bellowAA,2);
  //    setLinkSignedCopy(1,*triPipeA,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  
  
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

  return;
}


}   // NAMESPACE tdcSystem
