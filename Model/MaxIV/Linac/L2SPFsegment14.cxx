/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/L2SPFsegment14.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"
#include "LObjectSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"

#include "DipoleDIBMag.h"
#include "GateValveCube.h"

#include "TDCsegment.h"
#include "L2SPFsegment14.h"

namespace tdcSystem
{

// Note currently uncopied:

L2SPFsegment14::L2SPFsegment14(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  dm1(new tdcSystem::DipoleDIBMag(keyName+"DM1")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  dm2(new tdcSystem::DipoleDIBMag(keyName+"DM2")),
  gateA(new constructSystem::GateValveCube(keyName+"GateA")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(dm1);
  OR.addObject(pipeB);
  OR.addObject(pipeC);
  OR.addObject(dm2);
  OR.addObject(gateA);
  OR.addObject(bellowB);
}

L2SPFsegment14::~L2SPFsegment14()
  /*!
    Destructor
   */
{}



void
L2SPFsegment14::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment14","buildObjects");

  int outerCell;
  MonteCarlo::Object* masterCell=buildZone->getMaster();

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  // constructSystem::constructUnit
  //   (System,*buildZone,masterCell,*bellowA,"back",*pipeA);

  pipeA->createAll(System,*bellowA,"back");
  pipeMagUnit(System,*buildZone,pipeA,"Origin",dm1);
  pipeTerminate(System,*buildZone,pipeA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeA,"back",*pipeB);

  pipeC->createAll(System,*pipeB,"back");
  pipeMagUnit(System,*buildZone,pipeC,"Origin",dm2);
  pipeTerminate(System,*buildZone,pipeC);
  
  constructSystem::constructUnit
    (System,*buildZone,masterCell,*pipeC,"back",*gateA);

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*gateA,"back",*bellowB);

  buildZone->removeLastMaster(System);  

  return;
}

void
L2SPFsegment14::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("L2SPFsegment14","createLinks");
  
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowB,2);
  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void
L2SPFsegment14::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment14","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
