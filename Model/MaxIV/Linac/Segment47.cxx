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
#include "ContainedGroup.h"
#include "InnerZone.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "VacuumPipe.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "BlankTube.h"
#include "portItem.h"

#include "TDCsegment.h"
#include "Segment47.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment47::Segment47(const std::string& Key) :
  TDCsegment(Key,2),
  IZThin(new attachSystem::InnerZone(*this,cellIndex)),
  
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  prismaChamberA(new constructSystem::BlankTube(keyName+"PrismaChamberA")),
  mirrorChamberA(new constructSystem::PipeTube(keyName+"MirrorChamberA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  mirrorChamberB(new constructSystem::PipeTube(keyName+"MirrorChamberB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  mirrorChamberC(new constructSystem::PipeTube(keyName+"MirrorChamberC")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE"))
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

  setFirstItems(pipeA);
}

Segment47::~Segment47()
  /*!
    Destructor
   */
{}

void
Segment47::createSplitInnerZone(Simulation& System)
  /*!
    Split the innerZone into two parts (assuming segment44 built)
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("Segment47","createSplitInnerZone");

  //  *IZThin = *buildZone;
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
      IZThin->clearDivider();
      IZThin->setInsertCells(buildZone->getInsertCells());
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
/* OLD INNERZONE 

  int outerCell;
  MonteCarlo::Object* masterCell=IZThin->getMaster();

  if (!masterCell)
    masterCell=IZThin->constructMasterCell(System);

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);
  outerCell=IZThin->createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  
  const constructSystem::portItem& PC =
      buildIonPump2Port(System,*IZThin,masterCell,*pipeA,"back",*prismaChamberA);

  const constructSystem::portItem& MCA =
    buildIonPump2Port(System,*IZThin,masterCell,PC,"OuterPlate",*mirrorChamberA,true);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,MCA,"back",*pipeB);

  const constructSystem::portItem& MCB =
    buildIonPump2Port(System,*IZThin,masterCell,*pipeB,"back",*mirrorChamberB,true);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,MCB,"back",*pipeC);

  const constructSystem::portItem& MCC =
    buildIonPump2Port(System,*IZThin,masterCell,*pipeC,"back",*mirrorChamberC,true);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,MCC,"back",*pipeD);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*pipeD,"back",*gateA);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*gateA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*IZThin,masterCell,*bellowA,"back",*pipeE);

  IZThin->removeLastMaster(System);
*/
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

  createSplitInnerZone(System);
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
