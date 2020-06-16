/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/L2SPFsegment25.cxx
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
#include "VacuumPipe.h"
#include "TriPipe.h"
#include "DipoleDIBMag.h"
#include "SixPortTube.h"
#include "subPipeUnit.h"
#include "MultiPipe.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "TDCsegment25.h"

namespace tdcSystem
{

// Note currently uncopied:

TDCsegment25::TDCsegment25(const std::string& Key) :
  TDCsegment(Key,2),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  triPipeA(new tdcSystem::TriPipe(keyName+"TriPipeA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  sixPortA(new tdcSystem::SixPortTube(keyName+"SixPortA"))
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

  setFirstItem(bellowA);
}

TDCsegment25::~TDCsegment25()
  /*!
    Destructor
   */
{}

void
TDCsegment25::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("TDCsegment25","buildObjects");

  int outerCell;

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

  
  buildZone->removeLastMaster(System);  

  return;
}

void
TDCsegment25::createLinks()
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
TDCsegment25::createAll(Simulation& System,
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
  ELog::RegMethod RControl("TDCsegment25","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
