/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment47.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "CylGateValve.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "VacuumPipe.h"
#include "PrismaChamber.h"
#include "CrossWayTube.h"
#include "LocalShielding.h"
#include "LocalShieldingCell.h"

#include "TDCsegment.h"
#include "Segment47.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment47::Segment47(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::BlockZone(keyName+"IZThin")),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  prismaChamberA(new tdcSystem::PrismaChamber(keyName+"PrismaChamberA")),
  mirrorChamberA(new xraySystem::CrossWayTube(keyName+"MirrorChamberA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  mirrorChamberB(new xraySystem::CrossWayTube(keyName+"MirrorChamberB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  mirrorChamberC(new xraySystem::CrossWayTube(keyName+"MirrorChamberC")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  shieldCell(new tdcSystem::LocalShieldingCell(keyName+"ShieldCell",
					       keyName)),
  shieldF1(new tdcSystem::LocalShielding(keyName+"ShieldF1")),
  shieldF2(new tdcSystem::LocalShielding(keyName+"ShieldF2")),
  shieldF3(new tdcSystem::LocalShielding(keyName+"ShieldF3")),
  shieldF4(new tdcSystem::LocalShielding(keyName+"ShieldF4"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(prismaChamberA);
  OR.addObject(mirrorChamberA);
  OR.addObject(pipeB);
  OR.addObject(mirrorChamberB);
  OR.addObject(pipeC);
  OR.addObject(mirrorChamberC);
  OR.addObject(pipeD);
  OR.addObject(gateA);
  OR.addObject(bellowA);
  OR.addObject(pipeE);
  OR.addObject(shieldA);
  OR.addObject(shieldCell);
  OR.addObject(shieldF1);
  OR.addObject(shieldF2);
  OR.addObject(shieldF3);
  OR.addObject(shieldF4);

  setFirstItems(pipeA);
}

Segment47::~Segment47()
  /*!
    Destructor
   */
{}

void
Segment47::createSplitInnerZone()
  /*!
    Split the innerZone into two parts (assuming segment44 built)
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("Segment47","createSplitInnerZone");

  *IZThin=*buildZone;
  if (!sideVec.empty())
    {
      HeadRule HRcut;
      for(const TDCsegment* sideSegment : sideVec)
	{
	  if (sideSegment->hasSideIndex("buildZoneCut"))
	    HRcut.addUnion(sideSegment->getLinkSurf("buildZoneCut"));
	}
      HeadRule HSurroundB=buildZone->getSurround();
      HSurroundB.addIntersection(HRcut);

      IZThin->setSurround(HSurroundB);
    }
  return;
}


void
Segment47::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment47","buildObjects");

  int outerCell;

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);
  outerCell=IZThin->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,*IZThin,*pipeA,"back",*prismaChamberA);

  constructSystem::constructUnit
    (System,*IZThin,*prismaChamberA,"back",*mirrorChamberA);

  constructSystem::constructUnit
    (System,*IZThin,*mirrorChamberA,"back",*pipeB);

  constructSystem::constructUnit
    (System,*IZThin,*pipeB,"back",*mirrorChamberB);

  constructSystem::constructUnit
    (System,*IZThin,*mirrorChamberB,"back",*pipeC);

  constructSystem::constructUnit
    (System,*IZThin,*pipeC,"back",*mirrorChamberC);

  outerCell = constructSystem::constructUnit
    (System,*IZThin,*mirrorChamberC,"back",*pipeD);

  constructSystem::constructUnit
    (System,*IZThin,*pipeD,"back",*gateA);

  constructSystem::constructUnit
    (System,*IZThin,*gateA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*IZThin,*bellowA,"back",*pipeE);

  
  const MonteCarlo::Object* OCell=System.findObject(5750004);
  ELog::EM<<"Outer Cell ="<<*OCell<<ELog::endDiag; 

  // GIVE UNITS better names:
  shieldA->createAll(System,*pipeD, 2);
  shieldA->insertInCell(System,IZThin->getCell("Unit",8));
  shieldA->insertInCell(System,IZThin->getCell("Unit",9));

  shieldCell->createAll(System,*pipeD,2);
  for(size_t i=3;i<9;i++)
    {
      ELog::EM<<"Cell == "<<IZThin->getCell("Unit",i)<<ELog::endDiag;
      shieldCell->insertInCell(System,IZThin->getCell("Unit",i));
    }

  // legs [DO LATER]
  /*
  shieldF1->createAll(System,*shieldC, "back");
  shieldF1->insertInCell(System,outerCell-4);

  shieldF2->createAll(System,*shieldF1, "top");
  shieldF2->insertInCell(System,outerCell-3);

  shieldF3->createAll(System,*shieldF2, "back");
  shieldF3->insertInCell(System,outerCell-2);

  shieldF4->createAll(System,*shieldF3, "back");
  shieldF4->insertInCell(System,outerCell-1);
  */
  
  ELog::EM<<"Outer Cell ="<<*OCell<<ELog::endDiag; 

  return;
}

void
Segment47::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment47","createLinks");

  setLinkSignedCopy(0,*pipeA,1);
  setLinkSignedCopy(1,*pipeE,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  buildZone->setBack(FixedComp::getFullRule(2));
  return;
}

void
Segment47::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment47","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSplitInnerZone();
  buildObjects(System);
  createLinks();
  CellMap::setCells("BlockVoid",IZThin->getCells("Unit"));

  return;
}


}   // NAMESPACE tdcSystem
