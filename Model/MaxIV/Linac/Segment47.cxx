/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment47.cxx
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
#include "FixedOffset.h"
#include "InjectionHall.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Importance.h"
#include "Object.h"

#include "TDCsegment.h"
#include "Segment47.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment47::Segment47(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::BlockZone(keyName+"IZThin")),
  IHall(nullptr),

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
  shieldB(new tdcSystem::LocalShielding(keyName+"ShieldB")),
  shieldC(new tdcSystem::LocalShielding(keyName+"ShieldC")),
  shieldD(new tdcSystem::LocalShielding(keyName+"ShieldD")),
  shieldE(new tdcSystem::LocalShielding(keyName+"ShieldE")),
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
  OR.addObject(shieldB);
  OR.addObject(shieldC);
  OR.addObject(shieldD);
  OR.addObject(shieldE);
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

	  if (sideSegment->getKeyName() == "SPF45")
	    back45 = sideSegment->getLinkSurf("buildZoneCut");
	  else if (sideSegment->getKeyName() == "SPF46")
	    roof46 = sideSegment->getLinkSurf("buildZoneCut");
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

  HeadRule HR;
  int outerCell;
  MonteCarlo::Object *obj(nullptr), *objB(nullptr), *objB1(nullptr);
  const int IHFloor(IHall->getSurf("Floor"));

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);
  outerCell=IZThin->createUnit(System,*pipeA,2);
  pipeA->insertAllInCell(System,outerCell);

  obj = System.findObject(outerCell);
  obj->removeSurface(IHFloor);
  obj->removeSurface(back45);

  outerCell = constructSystem::constructUnit
    (System,*IZThin,*pipeA,"back",*prismaChamberA);

  obj = System.findObject(outerCell);
  obj->removeSurface(IHFloor);
  obj->removeSurface(back45);

  outerCell = constructSystem::constructUnit
    (System,*IZThin,*prismaChamberA,"back",*mirrorChamberA);

  obj = System.findObject(outerCell);
  obj->removeSurface(IHFloor);
  obj->removeSurface(back45);

  outerCell = constructSystem::constructUnit
    (System,*IZThin,*mirrorChamberA,"back",*pipeB);
  objB = System.findObject(outerCell);

  outerCell=constructSystem::constructUnit
    (System,*IZThin,*pipeB,"back",*mirrorChamberB);
  objB1 = System.findObject(outerCell);

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

  shieldA->createAll(System,*pipeD, 2);
  for (int i=0; i<2; ++i)
    shieldA->insertInCell(System,outerCell+i);

  // vertical side wall along the beam line behind pipeC
  shieldB->createAll(System,*pipeD, 2);
  HR.reset();
  HR.addIntersection(shieldB->getLinkSurf("#left")); // 3
  HR.addIntersection(shieldB->getLinkSurf("#front")); // 1
  HR.addIntersection(shieldB->getLinkSurf("#right")); // -4
  HR.addIntersection(shieldB->getLinkSurf("#bottom"));
  HR.addIntersection(shieldB->getLinkSurf("#top"));
  HR.makeComplement();
  HR.populateSurf();
  objB->addIntersection(HR);

  HR.reset();
  HR.addIntersection(shieldB->getLinkSurf("#left")); // 3
  HR.addIntersection(shieldB->getLinkSurf("#right")); // -4
  HR.addIntersection(shieldB->getLinkSurf("#bottom"));
  HR.addIntersection(shieldB->getLinkSurf("#top"));
  HR.makeComplement();
  HR.populateSurf();
  objB1->addIntersection(HR);

  for (int i=-2; i<=0; ++i)
    shieldB->insertInCell(System,outerCell+i);

  // floor
  shieldC->createAll(System,*shieldB, "bottom");
  HR.reset();
  HR.addIntersection(shieldC->getLinkSurf("#left")); // 5890003
  HR.addIntersection(shieldC->getLinkSurf("#right")); // -4
  HR.addIntersection(shieldC->getLinkSurf("#front")); // -5650005
  HR.addIntersection(shieldC->getLinkSurf("#back"));
  HR.addIntersection(shieldC->getLinkSurf("#bottom")); // 5880001
  HR.makeComplement();
  HR.populateSurf();
  objB->addIntersection(HR);

  HR.reset();
  HR.addIntersection(shieldC->getLinkSurf("#left")); // 5890003
  HR.addIntersection(shieldC->getLinkSurf("#right")); // -4
  HR.addIntersection(shieldC->getLinkSurf("#front")); // -5650005
  HR.addIntersection(shieldC->getLinkSurf("#back"));
  HR.makeComplement();
  HR.populateSurf();
  objB1->addIntersection(HR);

  for (int i=-2; i<=-1; ++i)
    shieldC->insertInCell(System,outerCell+i);

  // // vertical wall
  // shieldD->createAll(System,*shieldB, "front");
  // HR.reset();
  // HR.addIntersection(shieldD->getLinkSurf("#left")); // -4 -5880003
  // HR.addIntersection(shieldD->getLinkSurf("#right")); // 3
  // HR.addIntersection(shieldD->getLinkSurf("#back")); // 1
  // HR.addIntersection(shieldD->getLinkSurf("#bottom"));
  // HR.addIntersection(shieldD->getLinkSurf("#top"));
  // HR.makeComplement();
  // HR.populateSurf();
  // objB->addIntersection(HR);

  // HR.reset();
  // HR.addIntersection(shieldD->getLinkSurf("#left")); // -4 -5880003
  // HR.addIntersection(shieldD->getLinkSurf("#right")); // 3
  // HR.addIntersection(shieldD->getLinkSurf("#front"));
  // HR.addIntersection(shieldD->getLinkSurf("#bottom"));
  // HR.addIntersection(shieldD->getLinkSurf("#top"));
  // HR.makeComplement();
  // HR.populateSurf();
  // objB1->addIntersection(HR);

  // for (int i=-2; i<=-3; ++i)
  //   shieldD->insertInCell(System,outerCell+i);

  // roof
  // shieldE->createAll(System,*shieldB, "top");
  // HR.reset();
  // HR.addIntersection(shieldE->getLinkSurf("#left")); // -3
  // HR.addIntersection(shieldE->getLinkSurf("#right")); // 4
  // HR.addIntersection(shieldE->getLinkSurf("#back")); // 2
  // HR.addIntersection(shieldE->getLinkSurf("#front")); // 6
  // HR.addIntersection(shieldE->getLinkSurf("#bottom")); //
  // HR.makeComplement();
  // HR.populateSurf();
  // objB->addIntersection(HR);

  // HR.reset();
  // HR.addIntersection(shieldE->getLinkSurf("#left")); // -3
  // HR.addIntersection(shieldE->getLinkSurf("#right")); // 4
  // HR.addIntersection(shieldE->getLinkSurf("#back")); // 2
  // HR.addIntersection(shieldE->getLinkSurf("#front")); // 6
  // HR.makeComplement();
  // HR.populateSurf();
  // objB1->addIntersection(HR);

  // for (int i=-2; i<=-2; ++i)
  //   shieldE->insertInCell(System,outerCell+i);
  // legs
  shieldF1->createAll(System,*shieldC, "back");
  shieldF1->insertInCell(System,outerCell-4);

  shieldF2->createAll(System,*shieldF1, "top");
  shieldF2->insertInCell(System,outerCell-3);

  shieldF3->createAll(System,*shieldF2, "back");
  shieldF3->insertInCell(System,outerCell-2);

  shieldF4->createAll(System,*shieldF3, "back");
  shieldF4->insertInCell(System,outerCell-1);

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

  IHall=dynamic_cast<const InjectionHall*>(&FC);

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSplitInnerZone();
  buildObjects(System);
  createLinks();
  CellMap::setCells("BlockVoid",IZThin->getCells("Unit"));

  return;
}


}   // NAMESPACE tdcSystem
