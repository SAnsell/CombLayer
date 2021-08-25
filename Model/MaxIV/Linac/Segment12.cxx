/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment12.cxx
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

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "BeamDivider.h"
#include "DipoleDIBMag.h"
#include "IonPumpTube.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment12.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment12::Segment12(const std::string& Key) :
  TDCsegment(Key,6),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  beamA(new tdcSystem::BeamDivider(keyName+"BeamA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  bellowLA(new constructSystem::Bellows(keyName+"BellowLA")),
  ionPumpLA(new xraySystem::IonPumpTube(keyName+"IonPumpLA")),
  pipeLA(new constructSystem::VacuumPipe(keyName+"PipeLA")),
  bellowLB(new constructSystem::Bellows(keyName+"BellowLB")),

  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dipoleB(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  bellowRB(new constructSystem::Bellows(keyName+"BellowRB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(flatA);
  OR.addObject(dipoleA);
  OR.addObject(beamA);
  OR.addObject(shieldA);
  OR.addObject(bellowLA);
  OR.addObject(ionPumpLA);
  OR.addObject(pipeLA);
  OR.addObject(bellowLB);
  OR.addObject(flatB);
  OR.addObject(dipoleB);
  OR.addObject(bellowRB);

  setFirstItems(bellowA);
}

Segment12::~Segment12()
  /*!
    Destructor
   */
{}

void
Segment12::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment12","buildObjects");

  int outerCell;

  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  flatA->setFront(*bellowA,"back");
  flatA->createAll(System,*bellowA,"back");
  // insert-units : Origin : excludeSurf
  pipeMagGroup(System,*buildZone,flatA,
	       {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  beamA->setCutSurf("front",*flatA,"back");
  beamA->createAll(System,*flatA,"back");

  /////////// Local shielding
  shieldA->setCutSurf("Inner",*beamA,"outerBox");
  shieldA->createAll(System,*beamA,"front");
  outerCell=buildZone->createUnit(System,*shieldA,-1);

  beamA->insertInCell("Box",System,outerCell);
  beamA->insertInCell("FlangeA",System,outerCell);

  outerCell=buildZone->createUnit(System,*shieldA,2);
  shieldA->insertInCell(System,outerCell);

  pipeTerminateGroup(System,*buildZone,beamA,"exit",
   		     {"Box","Main","Exit","FlangeE"});
  outerCell = buildZone->createUnit(System,*beamA,"exit");
  /////////////

  bellowLA->setCutSurf("front",*beamA,"exit");
  bellowLA->createAll(System,*beamA,"exit");
  outerCell=pipeTerminate(System,*buildZone,bellowLA);
  beamA->insertInCell("Main",System,outerCell);

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*bellowLA,"back",*ionPumpLA);



  ionPumpLA->insertInCell(System,outerCell);
  // note this is a double insert
  beamA->insertInCell("Main",System,outerCell);

  int cellB,cellC;

  flatB->setFront(*beamA,"back");
  flatB->createAll(System,*beamA,"back");

  dipoleB->setCutSurf("Inner",*flatB,"outerPipe");
  dipoleB->createAll(System,*flatB,"Origin");

  cellB=buildZone->createUnit(System,*dipoleB,2); // cellB = cellA+1
  dipoleB->insertInCell(System,cellB);

  beamA->insertInCell("Main",System,cellB);
  beamA->insertInCell("FlangeB",System,cellB);


  cellC=pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  flatB->insertInCell("FlangeA",System,cellB);
  flatB->insertInCell("Pipe",System,cellB);

  pipeLA->addInsertCell("Main",cellB);
  pipeLA->addInsertCell("FlangeA",cellB);
  pipeLA->addAllInsertCell(cellC);
  pipeLA->addAllInsertCell(dipoleB->getCell("VoidMiddle"));

  pipeLA->setCutSurf("font",*ionPumpLA,"back");
  pipeLA->createAll(System,*ionPumpLA,"back");

  outerCell=constructSystem::constructUnit
    (System,*buildZone,*pipeLA,"back",*bellowLB);

  pipeLA->insertInCell("Main",System,outerCell);
  pipeLA->insertInCell("FlangeB",System,outerCell);

  bellowRB->setFront(*flatB,"back");
  bellowRB->createAll(System,*flatB,"back");
  bellowRB->insertInCell(System,outerCell);

  // transfer to segment 13
  CellMap::addCell("LastCell",outerCell);

  return;
}

void
Segment12::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*bellowA,1);

  setLinkCopy(1,*bellowLB,2);  // straigh exit
  setLinkCopy(2,*bellowRB,2);  // magnet exit

  FixedComp::nameSideIndex(1,"straightExit");
  FixedComp::nameSideIndex(2,"magnetExit");

  joinItems.push_back(FixedComp::getFullRule(2));
  joinItems.push_back(FixedComp::getFullRule(3));
  return;
}

void
Segment12::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment12","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
