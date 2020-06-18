/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment32.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "generalConstruct.h"
#include "LObjectSupport.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "DipoleDIBMag.h"

#include "TDCsegment.h"
#include "Segment32.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment32::Segment32(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  dmA(new tdcSystem::DipoleDIBMag(keyName+"DMA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  dmB(new tdcSystem::DipoleDIBMag(keyName+"DMB")),
  bellow(new constructSystem::Bellows(keyName+"Bellow"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(dmA);
  OR.addObject(pipeB);
  OR.addObject(pipeC);
  OR.addObject(dmB);
  OR.addObject(bellow);

  setFirstItem(pipeA);
}

Segment32::~Segment32()
  /*!
    Destructor
   */
{}



void
Segment32::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment32","buildObjects");

  MonteCarlo::Object* masterCell=buildZone->getMaster();

  pipeA->createAll(System,*this,0);
  pipeMagUnit(System,*buildZone,pipeA,"Origin","outerPipe",dmA);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*pipeB);

  pipeC->createAll(System,*pipeB,"back");
  pipeMagUnit(System,*buildZone,pipeC,"Origin","outerPipe",dmB);
  pipeTerminate(System,*buildZone,pipeC);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeC,"back",*bellow);

  buildZone->removeLastMaster(System);

  return;
}

void
Segment32::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment32","createLinks");

  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*bellow,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));

  return;
}

void
Segment32::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment32","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
