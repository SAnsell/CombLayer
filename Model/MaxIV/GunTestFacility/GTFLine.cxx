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
#include "Bellows.h"
#include "VacuumPipe.h"
#include "YagUnit.h"
#include "YagScreen.h"
#include "GTFGateValve.h"
#include "Solenoid.h"
#include "CurrentTransformer.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "FlangePlate.h"

#include "LObjectSupport.h"
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
  ionPumpA(std::make_shared<IonPumpGammaVacuum>("IonPumpA")),
  extension(std::make_shared<constructSystem::VacuumPipe>("Extension")),
  pipeA(std::make_shared<constructSystem::VacuumPipe>("PipeA")),
  solenoid(std::make_shared<xraySystem::Solenoid>("Solenoid")),
  gate(new constructSystem::GTFGateValve("Gate")),
  pipeB(std::make_shared<constructSystem::VacuumPipe>("PipeB")),
  bellowA(std::make_shared<constructSystem::Bellows>("BellowA")),
  mon(std::make_shared<xraySystem::CurrentTransformer>("CurrentTransformer")),
  laserChamber(std::make_shared<constructSystem::PipeTube>("LaserChamber")),
  laserChamberBackPlate(std::make_shared<constructSystem::FlangePlate>("LaserChamberBackPlate")),
  ionPumpB(std::make_shared<IonPumpGammaVacuum>("IonPumpB")),
  pipeC(std::make_shared<constructSystem::VacuumPipe>("PipeC")),
  yagUnitA(new tdcSystem::YagUnit("YagUnitA")),
  yagScreenA(new tdcSystem::YagScreen("YagScreenA")),
  bellowB(std::make_shared<constructSystem::Bellows>("BellowB")),
  yagUnitB(new tdcSystem::YagUnit("YagUnitB")),
  yagScreenB(new tdcSystem::YagScreen("YagScreenB")),
  bellowC(std::make_shared<constructSystem::Bellows>("BellowC")),
  yagUnitC(new tdcSystem::YagUnit("YagUnitC")),
  yagScreenC(new tdcSystem::YagScreen("YagScreenC")),
  bellowD(std::make_shared<constructSystem::Bellows>("BellowD"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ionPumpA);
  OR.addObject(extension);
  OR.addObject(pipeA);
  OR.addObject(solenoid);
  OR.addObject(gate);
  OR.addObject(pipeB);
  OR.addObject(bellowA);
  OR.addObject(mon);
  OR.addObject(laserChamber);
  OR.addObject(laserChamberBackPlate);
  OR.addObject(pipeC);
  OR.addObject(yagUnitA);
  OR.addObject(yagScreenA);
  OR.addObject(bellowB);
  OR.addObject(yagUnitB);
  OR.addObject(yagScreenB);
  OR.addObject(bellowC);
  OR.addObject(yagUnitC);
  OR.addObject(yagScreenC);
  OR.addObject(bellowD);
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

void GTFLine::constructYAG(Simulation& System, attachSystem::BlockZone& buildZone,
			   const attachSystem::FixedComp& linkUnit,
			   const std::string& sideName,
			   tdcSystem::YagUnit& yag,
			   tdcSystem::YagScreen& screen)
/*!
  Construct YAG unit and its screen
 */
{
  int outerCell = constructSystem::constructUnit(System, buildZone, linkUnit, sideName, yag);
  screen.setBeamAxis(yag, 1);
  screen.createAll(System, yag, -3);
  screen.insertInCell("Outer",   System, outerCell);
  screen.insertInCell("Connect", System, yag.getCell("PlateA"));
  screen.insertInCell("Connect", System, yag.getCell("Void"));
  screen.insertInCell("Payload", System, yag.getCell("Void"));
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

  int outerCell, outerCell1;

  buildZone.addInsertCells(this->getInsertCells());

  ionPumpA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*ionPumpA,2);
  ionPumpA->insertInCell(System,outerCell);

  constructSystem::constructUnit(System,buildZone,*ionPumpA,"back",*extension);

  pipeA->setFront(*extension,"back");
  pipeA->createAll(System, *extension, "back");

  tdcSystem::pipeMagUnit(System,buildZone,pipeA,"#front","outerPipe",solenoid);
  outerCell = tdcSystem::pipeTerminate(System,buildZone,pipeA);

  outerCell1 = constructSystem::constructUnit(System,buildZone,*pipeA,"back",*gate);

  gate->insertInCell("Shaft", System, outerCell);

  buildZone.createUnit(System,*gate,2);

  gate->insertInCell("Shaft", System, outerCell1+1);

  outerCell = constructSystem::constructUnit(System,buildZone,*gate,"back",*pipeB);

  gate->insertInCell("Shaft", System, outerCell);

  constructSystem::constructUnit(System,buildZone,*pipeB,"back",*bellowA);
  constructSystem::constructUnit(System,buildZone,*bellowA,"back",*mon);
  constructSystem::constructUnit(System,buildZone,*mon,"back",*laserChamber);

  laserChamberBackPlate->setCutSurf("plate", *laserChamber, "back");
  laserChamberBackPlate->createAll(System,*laserChamber,"back");
  // const constructSystem::portItem& BPI=laserChamberBackPlate->getPort(0);
  // outerCell=buildZone.createUnit(System,BPI,"OuterPlate");
  outerCell=buildZone.createUnit(System,*laserChamberBackPlate,"back");
  laserChamberBackPlate->insertInCell(System,outerCell);

  laserChamber->insertPortInCell(System,3,outerCell);

  const constructSystem::portItem& PI=laserChamber->getPort(3);
  ionPumpB->createAll(System,PI,PI.getSideIndex("#OuterPlate"));
  ionPumpB->insertInCell(System,outerCell);
  ionPumpB->insertInCell(System,outerCell-1);
  ionPumpB->insertInCell(System,outerCell-2);

  constructSystem::constructUnit(System,buildZone,*laserChamberBackPlate,"back",*pipeC);
  ionPumpB->insertInCell(System,outerCell+1);

  // Emittance meter (movable)

  ELog::EM << "TODO: skipping two objects: ~screen chamber and coils - must be added" << ELog::endWarn;

  constructYAG(System,buildZone,*pipeC,"back",*yagUnitA,*yagScreenA);

  ELog::EM << "add Solenoid after yagScreenA" << ELog::endWarn;

  constructSystem::constructUnit(System,buildZone,*yagUnitA,"back",*bellowB);

  ELog::EM << "Make below densities to be a function of their length" << ELog::endWarn;
  ELog::EM << "Total emittance meter length should be constant with different bellow lengths" << ELog::endWarn;

  constructYAG(System,buildZone,*bellowB,"back",*yagUnitB,*yagScreenB);

  constructSystem::constructUnit(System,buildZone,*yagUnitB,"back",*bellowC);
  constructYAG(System,buildZone,*bellowC,"back",*yagUnitC,*yagScreenC);

  constructSystem::constructUnit(System,buildZone,*yagUnitC,"back",*bellowD);


  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=laserChamberBackPlate;

  return;
}

void
GTFLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("GTFLine","createLinks");

  setLinkCopy(0,*ionPumpA,1);
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
