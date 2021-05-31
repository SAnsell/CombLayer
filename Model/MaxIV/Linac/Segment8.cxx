/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment8.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "EBeamStop.h"

#include "AttachSupport.h"
#include "TDCsegment.h"
#include "Segment7.h"
#include "Segment8.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment8::Segment8(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  beamStop(new tdcSystem::EBeamStop(keyName+"EBeam")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(beamStop);
  OR.addObject(bellowB);
  OR.addObject(pipeA);

  setFirstItems(bellowA);
}

Segment8::~Segment8()
  /*!
    Destructor
   */
{}

void
Segment8::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment8","buildObjects");

  int outerCell;

  if (isActive("front"))
    bellowA->copyCutSurf("front",*this,"front");
  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*beamStop);

  constructSystem::constructUnit
    (System,*buildZone,*beamStop,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,*bellowB,"back",*pipeA);

  if (beamStop->isShieldActive())
    {
      beamStop->insertAllInCell(System,outerCell);
      beamStop->insertAllInCell(System,outerCell+2);
      beamStop->insertAllInCell(System,outerCell+3);

      attachSystem::addToInsertControl(System,*beamStop,*bellowA);
      attachSystem::addToInsertControl(System,*beamStop,*bellowB);
      attachSystem::addToInsertControl(System,*beamStop,*pipeA,"FlangeA");
      attachSystem::addToInsertControl(System,*beamStop,*pipeA,"Main");

      for (const TDCsegment* sideSegment : sideVec)
	{
	  const std::vector<int> cellVec= sideSegment->getCells("BlockVoid");
	  beamStop->insertAllInCell(System,cellVec.end()[-1]);
	  const Segment7 *seg7 = dynamic_cast<const Segment7*>(sideSegment);

	  attachSystem::addToInsertControl(System,*beamStop,*seg7->getPipeB(),"FlangeB");
	  attachSystem::addToInsertControl(System,*beamStop,*seg7->getPipeB(),"Main");
	}
    }

  return;
}

void
Segment8::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*pipeA,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment8::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment8","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
