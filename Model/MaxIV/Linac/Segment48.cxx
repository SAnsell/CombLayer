/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment48.cxx
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ContainedGroup.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "VacuumPipe.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "portItem.h"
#include "FixedOffset.h"
#include "EBeamStop.h"
#include "JawFlange.h"
#include "CrossWayTube.h"
#include "LocalShielding.h"
#include "LObjectSupport.h"

#include "TDCsegment.h"
#include "Segment48.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment48::Segment48(const std::string& Key) :
  TDCsegment(Key,2),
  beamStopA(new tdcSystem::EBeamStop(keyName+"BeamStopA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  beamStopB(new tdcSystem::EBeamStop(keyName+"BeamStopB")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  shieldB(new tdcSystem::LocalShielding(keyName+"ShieldB")),
  shieldC(new tdcSystem::LocalShielding(keyName+"ShieldC")),
  slitTube(new constructSystem::PortTube(keyName+"SlitTube")),
  jaws({
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit0"),
	std::make_shared<constructSystem::JawFlange>(keyName+"SlitTubeJawUnit1")
    }),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  mirrorChamberA(new xraySystem::CrossWayTube(keyName+"MirrorChamberA")),
  bellowC(new constructSystem::Bellows(keyName+"BellowC"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(beamStopA);
  OR.addObject(bellowA);
  OR.addObject(beamStopB);
  OR.addObject(pipeA);
  OR.addObject(shieldA);
  OR.addObject(shieldB);
  OR.addObject(shieldC);
  OR.addObject(slitTube);
  OR.addObject(bellowB);
  OR.addObject(mirrorChamberA);
  OR.addObject(bellowC);

  setFirstItems(beamStopA);
}

Segment48::~Segment48()
  /*!
    Destructor
   */
{}

void
Segment48::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment48","buildObjects");

  int outerCell;

  if (isActive("front"))
    beamStopA->copyCutSurf("front",*this,"front");
  beamStopA->createAll(System,*this,0);

  outerCell=buildZone->createUnit(System,*beamStopA,2);
  beamStopA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*buildZone,*beamStopA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*buildZone,*bellowA,"back",*beamStopB);

  pipeA->createAll(System,*beamStopB,"back");

  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",shieldA);
  pipeTerminate(System,*buildZone,pipeA);

  shieldB->createAll(System,*shieldA,"right");
  for (int i=2; i<=4; ++i)
    shieldB->insertInCell(System,outerCell+i);

  shieldC->createAll(System,*shieldB,"right");
  for (int i=0; i<=3; ++i)
    shieldC->insertInCell(System,outerCell+i);

  for (const TDCsegment* sideSegment : sideVec)
    {
      const std::vector<int> cellVec= sideSegment->getCells("BlockVoid");
      for (int i=0; i<4; ++i)
	shieldC->insertInCell(System,cellVec.rbegin()[i]);
      // for(const int cn : cellVec)
      // 	ELog::EM<<"BPtr= "<<cn<<ELog::endDiag;
    }

  constructSystem::constructUnit
    (System,*buildZone,*pipeA,"back",*slitTube);

  for(size_t index=0;index<2;index++)
    {
      const constructSystem::portItem& DPI=slitTube->getPort(index);
      jaws[index]->setFillRadius(DPI,"InnerRadius",DPI.getCell("Void"));

      jaws[index]->addInsertCell(slitTube->getCell("Void"));
      if (index)
  	jaws[index]->addInsertCell(jaws[index-1]->getCell("Void"));
      jaws[index]->createAll
  	(System,DPI,DPI.getSideIndex("InnerPlate"),*slitTube,0);
    }

  // simplify the DiagnosticBox inner cell
  slitTube->splitVoidPorts(System,"SplitOuter",2001,
  			  slitTube->getCell("Void"),{0,2});
  //////////////////////////////////////////////////////////////////////

  constructSystem::constructUnit
    (System,*buildZone,*slitTube,"back",*bellowB);

  constructSystem::constructUnit
    (System,*buildZone,*bellowB,"back",*mirrorChamberA);

  constructSystem::constructUnit
    (System,*buildZone,*mirrorChamberA,"back",*bellowC);

  return;
}

void
Segment48::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment48","createLinks");

  setLinkCopy(0,*beamStopA,1);
  setLinkCopy(1,*bellowC,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment48::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment48","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
