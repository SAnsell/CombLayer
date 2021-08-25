/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment49.cxx
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
#include "BaseVisit.h"
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
#include "generateSurf.h"
#include "ModelSupport.h"
#include "generalConstruct.h"

#include "CylGateValve.h"
#include "VacuumPipe.h"
#include "LObjectSupport.h"
#include "InjectionHall.h"
#include "LocalShielding.h"

#include "TDCsegment.h"
#include "Segment49.h"


namespace tdcSystem
{

// Note currently uncopied:

Segment49::Segment49(const std::string& Key) :
  TDCsegment(Key,2),
  IHall(nullptr),
  gateA(new xraySystem::CylGateValve(keyName+"GateA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  shieldA(new tdcSystem::LocalShielding(keyName+"ShieldA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  gateB(new xraySystem::CylGateValve(keyName+"GateB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gateA);
  OR.addObject(pipeA);
  OR.addObject(shieldA);
  OR.addObject(pipeB);
  OR.addObject(gateB);

  setFirstItems(gateA);
}

Segment49::~Segment49()
  /*!
    Destructor
   */
{}

void
Segment49::populate(const FuncDataBase& Control)
  /*!
    Get required variable
    \param Control :: DatatBase
   */
{
  ELog::RegMethod RegA("Segment49","populate");

  FixedRotate::populate(Control);

  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");

  return;
}

void
Segment49::createSurfaces()
  /*!
    Build surface(s) for wall hole.
  */
{
  ELog::RegMethod RegA("Segment49","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,wallRadius);

  return;
}

void
Segment49::constructHole(Simulation& System)
  /*!
    Construct the hole in the wall
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Segment49","constructHole");

  if (IHall)
    {
      std::string Out;
      const HeadRule fbHR=IHall->combine("BackWallFront #BackWallBack");

      Out=ModelSupport::getComposite(SMap,buildIndex," -7 " );
      makeCell("WallVoid",System,cellIndex++,0,0.0,Out+fbHR.display());

      pipeB->addInsertCell("Main",this->getCell("WallVoid"));
      pipeB->addInsertCell("Main",IHall->getCell("C080016"));
      pipeB->addInsertCell("FlangeB",IHall->getCell("C080016"));

      Out=ModelSupport::getComposite(SMap,buildIndex," 7 " );
      const size_t nLayers = IHall->getBackWallNLayers();
      if (nLayers<=1)
	{
	  IHall->insertComponent(System,"BackWallConcrete",Out);
	  IHall->insertComponent(System,"BackWallIron",Out);
	}
      else
	{
	  for (size_t i=0; i<nLayers; ++i) {
	    const std::string istr = std::to_string(i);
	    IHall->insertComponent(System,"BackWallConcrete"+istr,Out);
	    IHall->insertComponent(System,"BackWallIron"+istr,Out);
	  }
	}
    }

  return;
}

void
Segment49::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment49","buildObjects");

  int outerCell;

  constructHole(System);

  if (isActive("front"))
    gateA->copyCutSurf("front",*this,"front");

  gateA->createAll(System,*this,0);
  outerCell=buildZone->createUnit(System,*gateA,2);
  gateA->insertInCell(System,outerCell);

  pipeA->createAll(System,*gateA,2);

  pipeMagUnit(System,*buildZone,pipeA,"#front","outerPipe",shieldA);
  pipeTerminate(System,*buildZone,pipeA);

  pipeB->createAll(System,*pipeA,"back");
  outerCell=buildZone->createUnit(System);
  pipeB->insertAllInCell(System,outerCell);

  if (!nextZone)
    ELog::EM<<"Failed to get nextZone"<<ELog::endDiag;

  // this creates spfBehindBackWall zone
  outerCell=nextZone->createUnit(System,*pipeB,2);
  pipeB->insertAllInCell(System,outerCell);
  pipeTerminate(System,*nextZone,pipeB);

  constructSystem::constructUnit
    (System,*nextZone,*pipeB,"back",*gateB);

  return;
}

void
Segment49::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment49","createLinks");

  setLinkCopy(0,*gateA,1);
  setLinkCopy(1,*gateB,2);

  joinItems.push_back(FixedComp::getFullRule("back"));

  return;
}

void
Segment49::createAll(Simulation& System,
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
  ELog::RegMethod RControl("Segment49","createAll");

  IHall=dynamic_cast<const InjectionHall*>(&FC);

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
