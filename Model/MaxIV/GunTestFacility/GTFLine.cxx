/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Model/MaxIV/GunTestFacility/GTFLine.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "NameStack.h"
#include "RegMethod.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "FixedOffset.h"
#include "ContainedGroup.h"
#include "BlockZone.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"

#include "IonPumpGammaVacuum.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Solenoid.h"

#include "GTFLine.h"

namespace MAXIV
{

// Note currently uncopied:

GTFLine::GTFLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(Key+"BuildZone"),
  ionPump(std::make_shared<IonPumpGammaVacuum>("IonPump")),
  extension(std::make_shared<constructSystem::VacuumPipe>("Extension")),
  pipeA(std::make_shared<constructSystem::VacuumPipe>("PipeA")),
  solenoid(std::make_shared<xraySystem::Solenoid>("Solenoid"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ionPump);
  OR.addObject(extension);
  OR.addObject(pipeA);
  OR.addObject(solenoid);
}

GTFLine::~GTFLine()
  /*!
    Destructor
   */
{}

void
GTFLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database
   */
{
  ELog::RegMethod RegA("GTFLine","populate");

  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  return;
}


void
GTFLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("GTFLine","createSurface");

  if (outerLeft>Geometry::zeroTol && isActive("floor"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      const HeadRule HR=
	ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 -6");

     buildZone.setSurround(HR*getRule("floor"));
     buildZone.setFront(getRule("front"));
     buildZone.setMaxExtent(getRule("back"));
     buildZone.setInnerMat(innerMat);
    }
  return;
}

void
GTFLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("GTFLine","buildObjects");

  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());

  ionPump->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*ionPump,2);
  ionPump->insertInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*ionPump,"back",*extension);

  pipeA->setFront(*extension,"back");
  pipeA->createAll(System, *extension, "back");

  solenoid->setCutSurf("Inner",*pipeA,"outerPipe");
  solenoid->createAll(System,*pipeA,"#front");

  // outerCell is the buildZone cell between pipeA start and solenoid start
  outerCell=buildZone.createUnit(System, *solenoid, "#front");
  pipeA->insertAllInCell(System,outerCell);

  // outerCell is the buildZone cell with colenoid
  outerCell=buildZone.createUnit(System, *solenoid, "back");
  solenoid->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System, *pipeA, 2);
  pipeA->insertAllInCell(System,outerCell);

  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=pipeA;

  return;
}

void
GTFLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("GTFLine","createLinks");

  setLinkCopy(0,*ionPump,1);
  setLinkCopy(1,*lastComp,2);
  return;
}

void
GTFLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("GTFLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}

}
