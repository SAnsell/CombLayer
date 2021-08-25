/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment3.cxx
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
#include "FixedOffset.h"
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

#include "OffsetFlangePipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "DipoleDIBMag.h"
#include "CorrectorMag.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment3.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment3::Segment3(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeA(new constructSystem::OffsetFlangePipe(keyName+"PipeA")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  shieldB(new tdcSystem::LocalShielding(keyName+"ShieldB")),
  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dipoleB(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
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
  OR.addObject(pipeA);
  OR.addObject(cMagHA);
  OR.addObject(cMagVA);
  OR.addObject(shieldA);
  OR.addObject(shieldB);
  OR.addObject(flatB);
  OR.addObject(dipoleB);
  OR.addObject(bellowB);

  setFirstItems(bellowA);
}

Segment3::~Segment3()
  /*!
    Destructor
   */
{}

void
Segment3::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment3","buildObjects");
  int outerCell;

  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  flatA->setFront(*bellowA,"back");
  flatA->createAll(System,*bellowA,"back");
  pipeMagGroup(System,*buildZone,flatA,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  pipeA->setFront(*flatA,"back");
  pipeA->createAll(System,*flatA,"back");
  correctorMagnetPair(System,*buildZone,pipeA,cMagHA,cMagVA);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",shieldA);
  pipeTerminate(System,*buildZone,pipeA);

  shieldB->createAll(System,*shieldA, "left");
  for (int i=2; i<=9; ++i)
    shieldB->insertInCell(System,outerCell+i);


  flatB->setFront(*pipeA,"back");
  flatB->createAll(System,*pipeA,"back");
  pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleB);
  pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,*flatB,"back",*bellowB);

  return;
}

void
Segment3::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*bellowB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment3::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment3","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
