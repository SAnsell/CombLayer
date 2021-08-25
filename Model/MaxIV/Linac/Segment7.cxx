/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment7.cxx
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
#include "StriplineBPM.h"
#include "LQuadF.h"
#include "CorrectorMag.h"
#include "EBeamStop.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "Segment6.h"
#include "Segment7.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment7::Segment7(const std::string& Key) :
  TDCsegment(Key,2),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),

  cMagHA(new xraySystem::CorrectorMag(keyName+"CMagHA")),
  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  bpm(new tdcSystem::StriplineBPM(keyName+"BPM")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(cMagHA);
  OR.addObject(QuadA);
  OR.addObject(bpm);
  OR.addObject(pipeB);
  OR.addObject(cMagVA);

  setFirstItems(pipeA);
}

Segment7::~Segment7()
  /*!
    Destructor
   */
{}

void
Segment7::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment7","buildObjects");



  int outerCell;

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");
  pipeA->createAll(System,*this,0);

  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",cMagHA);
  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",QuadA);
  outerCell = pipeTerminate(System,*buildZone,pipeA);

  for (const TDCsegment* sideSegment : sideVec)
    {
      // Insert shielding of L2SPF6BeamStop into the first zone cell
      const Segment6 *seg6 = dynamic_cast<const Segment6*>(sideSegment);
      if (seg6)
	{
	  const std::shared_ptr<EBeamStop> beamStop = seg6->getBeamStop();
	  if (beamStop->isShieldActive())
	    {
	      beamStop->insertAllInCell(System,outerCell-4);

	      // Insert L2SPF7PipeA into L2SPF6BeamStop
	      const std::vector<int>  cells= sideSegment->getCells("ShieldSideBackHole");
	      for (const int cn : cells)
		{
		  pipeA->insertInCell("FlangeA",System,cn);
		  pipeA->insertInCell("Main",System,cn);
		}
	    }
	}
    }



  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*bpm);

  //  pipeB->copyCutSurf("front",*bpm,"back");
  pipeB->createAll(System,*bpm,"back");

  pipeMagUnit(System,*buildZone,pipeB,"#front","outerPipe",cMagVA);
  pipeTerminate(System,*buildZone,pipeB);

  return;
}

void
Segment7::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*pipeB,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment7::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment7","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  CellMap::setCells("BlockVoid",buildZone->getCells("Unit"));

  return;
}


}   // NAMESPACE tdcSystem
