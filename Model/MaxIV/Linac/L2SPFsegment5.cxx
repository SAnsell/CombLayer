/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/L2SPFsegment5.cxx
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
#include "InnerZone.h"
#include "generalConstruct.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "DipoleDIBMag.h"
#include "BeamDivider.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment5.h"

namespace tdcSystem
{

// Note currently uncopied:

L2SPFsegment5::L2SPFsegment5(const std::string& Key) :
  TDCsegment(Key,2),

  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  beamA(new tdcSystem::BeamDivider(keyName+"BeamA")),
  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dipoleB(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(flatA);
  OR.addObject(dipoleA);
  OR.addObject(flatB);
  OR.addObject(beamA);
  OR.addObject(dipoleB);
  OR.addObject(bellowA);

  setFirstItem(flatA);
}

L2SPFsegment5::~L2SPFsegment5()
  /*!
    Destructor
   */
{}

void
L2SPFsegment5::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment5","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    flatA->copyCutSurf("front",*this,"front");
  flatA->createAll(System,*this,0);

  // insert-units : Origin : excludeSurf
  pipeMagGroup(System,*buildZone,flatA,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*flatA,"back",*beamA);

  flatB->setFront(*beamA,"back");
  flatB->createAll(System,*beamA,"back");
  // insert-units : Origin : excludeSurf
  pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleB);
  pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*flatB,"back",*bellowA);

  buildZone->removeLastMaster(System);  

  return;
}

void
L2SPFsegment5::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*flatA,1);
  setLinkSignedCopy(1,*bellowA,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void
L2SPFsegment5::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment5","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
