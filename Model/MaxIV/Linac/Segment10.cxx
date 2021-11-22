/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment10.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "generateSurf.h"
#include "ModelSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LQuadF.h"
#include "CorrectorMag.h"
#include "GateValveCube.h"
#include "IonPumpTube.h"
#include "LocalShielding.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "InjectionHall.h"
#include "Segment10.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment10::Segment10(const std::string& Key) :
  TDCsegment(Key,2),
  IHall(nullptr),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  shieldB(new tdcSystem::LocalShielding(keyName+"ShieldB")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  gateValve(new constructSystem::GateValveCube(keyName+"Gate")),
  pumpA(new xraySystem::IonPumpTube(keyName+"PumpA")),

  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),

  QuadA(new tdcSystem::LQuadF(keyName+"QuadA")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"CMagVA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(shieldA);
  OR.addObject(shieldB);
  OR.addObject(bellowA);
  OR.addObject(gateValve);
  OR.addObject(pipeB);
  OR.addObject(bellowB);
  OR.addObject(pipeC);
  OR.addObject(QuadA);
  OR.addObject(cMagVA);

  setFirstItems(bellowA);
}

Segment10::~Segment10()
  /*!
    Destructor
   */
{}

void
Segment10::populate(const FuncDataBase& Control)
  /*!
    Get required variable
    \param Control :: DatatBase
   */
{
  ELog::RegMethod RegA("Segment10","populate");

  FixedRotate::populate(Control);

  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  return;
}

void
Segment10::createSurfaces()
  /*!
    Build surface(s) for wall hole.
  */
{
  ELog::RegMethod RegA("L2SFPsegment10","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,wallRadius);

  return;
}

void
Segment10::constructHole(Simulation& System)
  /*!
    Construct the hole in the wall
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Segment10","constructHole");

  if (IHall)
    {
      std::string Out;
      const HeadRule fbHR=IHall->combine("#MidAngleWall #TMidBack");

      Out=ModelSupport::getComposite(SMap,buildIndex," -7 " );
      makeCell("WallVoid",System,cellIndex++,0,0.0,Out+fbHR.display());

      pipeA->addInsertCell("Main",this->getCell("WallVoid"));
      pipeA->addInsertCell("Main",IHall->getCell("TVoidA"));
      pipeA->addInsertCell("FlangeB",IHall->getCell("TVoidA"));

      Out=ModelSupport::getComposite(SMap,buildIndex," 7 " );
      IHall->insertComponent(System,"MidTAngle",Out);
      //      IHall->insertComponent(System,"FKGMazeFrontWall",Out);
      //      IHall->insertComponent(System,"MidTAuxCyl",Out);

      // This might not be the best place for this:::

    }
  return;
}

void
Segment10::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment10","buildObjects");
  int outerCell;

  constructHole(System);

  if (isActive("front"))
    pipeA->copyCutSurf("front",*this,"front");

  pipeA->createAll(System,*this,0);
  shieldA->setCutSurf("Inner",*pipeA,"outerPipe");
  shieldA->createAll(System,*pipeA, "#front");

  outerCell=buildZone->createUnit(System, *shieldA, "#front");
  pipeA->insertInCell("FlangeA",System,outerCell);
  pipeA->insertInCell("Main",System,outerCell);

  outerCell=buildZone->createUnit(System, *shieldA, "back");
  shieldA->insertInCell(System,outerCell);

  shieldB->createAll(System, *shieldA, "top");
  shieldB->insertInCell(System,outerCell);

  outerCell=buildZone->createUnit(System);
  pipeA->insertInCell("Main",System,outerCell);

  if (!nextZone)
    ELog::EM<<"Failed to get nextZone"<<ELog::endDiag;

  // masterCell=nextZone->constructMasterCell(System,*pipeA,2);
  // allows the first surface of pipe to be the start of the masterCell
  outerCell=nextZone->createUnit(System,*pipeA,2);

  pipeA->insertInCell("Main",System,outerCell);
  pipeA->insertInCell("FlangeB",System,outerCell);
  pipeTerminate(System,*nextZone,pipeA);

  constructSystem::constructUnit
    (System,*nextZone,*pipeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,*nextZone,*bellowA,"back",*gateValve);

  constructSystem::constructUnit
    (System,*nextZone,*gateValve,"back",*pumpA);

  constructSystem::constructUnit
    (System,*nextZone,*pumpA,"back",*pipeB);


  constructSystem::constructUnit
    (System,*nextZone,*pipeB,"back",*bellowB);


  pipeC->createAll(System,*bellowB,"back");
  pipeMagUnit(System,*nextZone,pipeC,"#front","outerPipe",QuadA);
  pipeMagUnit(System,*nextZone,pipeC,"#front","outerPipe",cMagVA);
  pipeTerminate(System,*nextZone,pipeC);

  return;
}

void
Segment10::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*pipeA,1);
  setLinkCopy(1,*pipeC,2);

  //  setLinkCopy(1,*pipeA,2);

  joinItems.push_back(FixedComp::getFullRule(2));
  return;
}

void
Segment10::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment10","createAll");

  IHall=dynamic_cast<const InjectionHall*>(&FC);

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
