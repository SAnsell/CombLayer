/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/Segment12.cxx
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

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "BeamDivider.h"
#include "DipoleDIBMag.h"


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
  bellowLA(new constructSystem::Bellows(keyName+"BellowLA")),
  ionPumpLA(new constructSystem::BlankTube(keyName+"IonPumpLA")),
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

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);
  
  flatA->setFront(*bellowA,"back");
  flatA->createAll(System,*bellowA,"back");
  // insert-units : Origin : excludeSurf
  pipeMagGroup(System,*buildZone,flatA,
	       {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  beamA->setCutSurf("front",*flatA,"back");
  beamA->createAll(System,*flatA,"back");
  pipeTerminateGroup(System,*buildZone,beamA,"exit",
		     {"Exit","Box","FlangeE","FlangeA","Main"});

  pipeTerminateGroup(System,*buildZone,beamA,"exit",
		     {"Exit","Box","FlangeE","FlangeA","Main"});

  bellowLA->setCutSurf("front",*beamA,"exit");
  bellowLA->createAll(System,*beamA,"exit");
  outerCell=pipeTerminate(System,*buildZone,bellowLA);
  beamA->insertInCell("Main",System,outerCell);


  // FAKE INSERT REQUIRED
  ionPumpLA->addAllInsertCell(masterCell->getName());
  ionPumpLA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  ionPumpLA->createAll(System,*bellowLA,"back");

  const constructSystem::portItem& VPB=ionPumpLA->getPort(1);
  outerCell=buildZone->createOuterVoidUnit
    (System,masterCell,VPB,VPB.getSideIndex("OuterPlate"));
  ionPumpLA->insertAllInCell(System,outerCell);
  beamA->insertInCell("Main",System,outerCell);

  int cellA,cellB,cellC;
  cellA=pipeTerminateGroup(System,*buildZone,beamA,"back",
					{"Main","FlangeB"});
  flatB->setFront(*beamA,"back");
  flatB->createAll(System,*beamA,"back");

  // this creates two buildZone cells:
  cellB=pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleB);
  cellC=pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  pipeLA->addInsertCell(cellA);
  pipeLA->addInsertCell(cellB-1);
  pipeLA->addInsertCell(cellC);
  pipeLA->addInsertCell(dipoleB->getCell("VoidMiddle"));
  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,VPB,"OuterPlate",*pipeLA);

  // add last bellows
  bellowRB->addInsertCell(outerCell);
  bellowRB->setFront(*flatB,"back");
  bellowRB->createAll(System,*flatB,"back");

  outerCell=constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeLA,"back",*bellowLB);
  bellowRB->insertInCell(System,outerCell);

  // transfer to segment 13
  CellMap::addCell("LastCell",outerCell);
  buildZone->removeLastMaster(System);  
  return;
}

void
Segment12::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);

  setLinkSignedCopy(1,*bellowLB,2);  // straigh exit
  setLinkSignedCopy(2,*bellowRB,2);  // magnet exit

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

