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
#include "TriPipe.h"
#include "DipoleDIBMag.h"
#include "SixPortTube.h"
#include "subPipeUnit.h"
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
  TDCsegment(Key,2),
  IZTop(*this,cellIndex),IZFlat(*this,cellIndex),
  IZLower(*this,cellIndex),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  triPipeA(new tdcSystem::TriPipe(keyName+"TriPipeA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  sixPortA(new tdcSystem::SixPortTube(keyName+"SixPortA")),
  multiPipe(new tdcSystem::MultiPipe(keyName+"MultiPipe")),
  bellowUp(new constructSystem::Bellows(keyName+"BellowUp")),
  bellowFlat(new constructSystem::Bellows(keyName+"BellowFlat")),
  bellowDown(new constructSystem::Bellows(keyName+"BellowDown")),
  pipeUpA(new constructSystem::VacuumPipe(keyName+"PipeUpA")),
  pipeFlatA(new constructSystem::VacuumPipe(keyName+"PipeFlatA")),
  pipeDownA(new constructSystem::VacuumPipe(keyName+"PipeDownA")),

  bellowUpB(new constructSystem::Bellows(keyName+"BellowUpB")),
  bellowFlatB(new constructSystem::Bellows(keyName+"BellowFlatB")),
  bellowDownB(new constructSystem::Bellows(keyName+"BellowDownB")),

  yagUnitUp(new tdcSystem::YagUnit(keyName+"YagUnitUp")),
  yagUnitFlat(new tdcSystem::YagUnit(keyName+"YagUnitFlat")),
  yagScreenUp(new tdcSystem::YagScreen(keyName+"YagScreenUp")),
  yagScreenFlat(new tdcSystem::YagScreen(keyName+"YagScreenFlat")),

  pipeUpB(new constructSystem::VacuumPipe(keyName+"PipeUpB")),
  pipeFlatB(new constructSystem::VacuumPipe(keyName+"PipeFlatB")),
  pipeDownB(new constructSystem::VacuumPipe(keyName+"PipeDownB"))

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
  OR.addObject(bellowUp);
  OR.addObject(bellowFlat);
  OR.addObject(bellowDown);

  OR.addObject(pipeUpA);
  OR.addObject(pipeFlatA);
  OR.addObject(pipeDownA);

  OR.addObject(bellowUpB);
  OR.addObject(bellowFlatB);
  OR.addObject(bellowDownB);

  OR.addObject(yagUnitUp);
  OR.addObject(yagScreenUp);
  OR.addObject(yagUnitFlat);
  OR.addObject(yagScreenFlat);

  OR.addObject(pipeUpB);
  OR.addObject(pipeFlatB);
  OR.addObject(pipeDownB);

  setFirstItem(bellowA);
}

Segment25::~Segment25()
  /*!
    Destructor
   */
{}


void
Segment25::createSplitInnerZone(Simulation& System)
  /*!
    Spilit the innerZone into three parts.
    \param System :: Simulatio to use
   */
{
  ELog::RegMethod RegA("Segment25","createSplitInnerZone");
  
  IZTop= *buildZone;
  IZFlat= *buildZone;
  IZLower= *buildZone;

  HeadRule HSurroundA=buildZone->getSurround();
  HeadRule HSurroundB=buildZone->getSurround();
  HeadRule HSurroundC=buildZone->getSurround();
  // create surfaces
  attachSystem::FixedUnit FA("FA");
  attachSystem::FixedUnit FB("FB");
  FA.createPairVector(*bellowUp,2,*bellowFlat,2);
  FB.createPairVector(*bellowFlat,2,*bellowDown,2);

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

  IZTop.setSurround(HSurroundA);
  IZFlat.setSurround(HSurroundB);
  IZLower.setSurround(HSurroundC);

  IZTop.constructMasterCell(System);
  IZFlat.constructMasterCell(System);
  IZLower.constructMasterCell(System);

  return;
}

void
Segment25::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment25","buildObjects");

  int outerCell,outerCellA,outerCellB,outerCellC;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  //  if (isActive("front"))
    //    bellowA->copyCutSurf("front",*this,"front");
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
  bellowUp->createAll(System,*multiPipe,2);
  bellowFlat->addInsertCell(outerCellMulti);
  bellowFlat->createAll(System,*multiPipe,3);

  bellowDown->addInsertCell(outerCellMulti);
  bellowDown->createAll(System,*multiPipe,4);

  const int outerCellBellow=
    buildZone->createOuterVoidUnit(System,masterCell,*bellowUp,2);
  bellowUp->insertInCell(System,outerCellBellow);
  bellowFlat->insertInCell(System,outerCellBellow);

  createSplitInnerZone(System);

  
  // PIPE:
  pipeFlatA->addInsertCell(outerCellBellow);
  pipeDownA->addInsertCell(outerCellMulti);
  pipeDownA->addInsertCell(outerCellBellow);
  pipeUpA->createAll(System,*bellowUp,"back");
  pipeFlatA->createAll(System,*bellowFlat,"back");
  pipeDownA->createAll(System,*bellowDown,"back");

  
  MonteCarlo::Object* masterCellA=IZTop.getMaster();
  MonteCarlo::Object* masterCellB=IZFlat.getMaster();
  MonteCarlo::Object* masterCellC=IZLower.getMaster();

  outerCellA=IZTop.createOuterVoidUnit(System,masterCellA,*pipeUpA,2);
  outerCellB=IZFlat.createOuterVoidUnit(System,masterCellB,*pipeFlatA,2);
  outerCellC=IZLower.createOuterVoidUnit(System,masterCellC,*pipeDownA,2);

  pipeUpA->insertInCell(System,outerCellA);
  pipeFlatA->insertInCell(System,outerCellB);
  pipeDownA->insertInCell(System,outerCellC);

  // BELLOWS B:
  constructSystem::constructUnit
    (System,IZTop,masterCellA,*pipeUpA,"back",*bellowUpB);
  constructSystem::constructUnit
    (System,IZFlat,masterCellB,*pipeFlatA,"back",*bellowFlatB);
  constructSystem::constructUnit
    (System,IZLower,masterCellC,*pipeDownA,"back",*bellowDownB);

  // YAG SCREENS:
  constructSystem::constructUnit
    (System,IZTop,masterCellA,*bellowUpB,"back",*yagUnitUp);

  constructSystem::constructUnit
    (System,IZFlat,masterCellB,*bellowFlatB,"back",*yagUnitFlat);



  // BELLOWS B:
  constructSystem::constructUnit
    (System,IZTop,masterCellA,*yagUnitUp,"back",*pipeUpB);
  constructSystem::constructUnit
    (System,IZFlat,masterCellB,*yagUnitFlat,"back",*pipeFlatB);
  constructSystem::constructUnit
    (System,IZLower,masterCellC,*bellowDownB,"back",*pipeDownB);

  buildZone->refrontMasterCell(masterCell,IZFlat.getDivider());
  System.removeCell(masterCell->getName());
  //  buildZone->removeLastMaster(System);
  //  IZTop.removeLastMaster(System);
  //IZFlat.removeLastMaster(System);
  //IZLower.removeLastMaster(System);  

  HeadRule Multi=IZTop.getDivider()+IZFlat.getDivider();
  ELog::EM<<"Front == "<<IZTop.getDivider()<<ELog::endDiag;

  
  return;
}

void
Segment25::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowA,2);
  //    setLinkSignedCopy(1,*triPipeA,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  
  
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

  return;
}


}   // NAMESPACE tdcSystem
