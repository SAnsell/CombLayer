/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment30.cxx
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
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "VacuumPipe.h"
#include "portItem.h"
#include "BlankTube.h"
#include "CorrectorMag.h"
#include "LObjectSupport.h"

#include "TDCsegment.h"
#include "Segment30.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment30::Segment30(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::InnerZone(*this,cellIndex)),
  gauge(new constructSystem::PipeTube(keyName+"Gauge")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellow(new constructSystem::Bellows(keyName+"Bellow")),
  ionPump(new constructSystem::BlankTube(keyName+"IonPump")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagV(new tdcSystem::CorrectorMag(keyName+"CMagV"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gauge);
  OR.addObject(pipeA);
  OR.addObject(bellow);
  OR.addObject(ionPump);
  OR.addObject(pipeB);
  OR.addObject(cMagV);

  setFirstItems(gauge);
}

Segment30::~Segment30()
  /*!
    Destructor
   */
{}

void
Segment30::createSplitInnerZone(Simulation& System)
  /*!
    Split the innerZone into two parts (assuming segment13 built)
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment27","createSplitInnerZone");
  
  *IZThin = *buildZone;
  
  
  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();

  const Geometry::Vec3D sideOrg(sideSegment->getCentre());
  const Geometry::Vec3D sideY((sideSegment->getY()+Y).unit());

  const Geometry::Vec3D midX=(sideY*Z);
  ELog::EM<<"Side - "<<sideOrg<<ELog::endDiag;
  ELog::EM<<"Side - "<<midX<<ELog::endDiag;
  ELog::EM<<"Side - "<<sideY<<ELog::endDiag;

  ELog::EM<<"Cells = "<<ELog::endDiag;
  for(const std::string& N : sideSegment->getNames())
    ELog::EM<<"Cell = "<<N<<ELog::endDiag;

  
  for(const int CN : sideSegment->getCells("BuildVoid"))
    ELog::EM<<"Cell = "<<CN<<ELog::endDiag;


  ModelSupport::buildPlane(SMap,buildIndex+5005,(sideOrg+Origin)/2.0,midX);
  //  for(const int CN : sideSegment->getCells()

  
  /*
  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*bellowAA,-1,*bellowBA,-1);
  FB.createPairVector(*bellowBA,-1,*bellowCA,-1);

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
  */

  return;
}
  

void
Segment30::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment30","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();
  ELog::EM<<"Master cell == "<<Origin<<ELog::endErr;

  gauge->addAllInsertCell(masterCell->getName());
  // Gauge
  if (isActive("front"))
    gauge->copyCutSurf("front", *this, "front");
  gauge->createAll(System,*this,0);

  ELog::EM<<"Master cell == "<<masterCell<<ELog::endErr;


  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System,*gauge,-1);

  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*gauge,2);
  gauge->insertAllInCell(System,outerCell);


  constructSystem::constructUnit
    (System,*buildZone,masterCell,*gauge,"back",*pipeA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*bellow);
  buildZone->removeLastMaster(System);
  return;

  const constructSystem::portItem& ionPumpBackPort =
    buildIonPump2Port(System,*buildZone,masterCell,*bellow,"back",*ionPump);

  pipeB->createAll(System,ionPumpBackPort,"OuterPlate");
  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagV);
  pipeTerminate(System,*buildZone,pipeB);

  buildZone->removeLastMaster(System);

  return;
}

void
Segment30::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment30","createLinks");

  setLinkSignedCopy(0,*gauge,1);

  //  setLinkSignedCopy(1,*pipeB,2);

  setLinkSignedCopy(1,*gauge,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment30::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment30","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSplitInnerZone(System);
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
