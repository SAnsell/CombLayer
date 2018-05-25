/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: cosax/cosaxOpticsLine.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "CSGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PortTube.h"
#include "PipeTube.h"

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "BremColl.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "cosaxOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
cosaxOpticsLine::cosaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new constructSystem::CrossPipe(newName+"TriggerPipe")),
  gaugeA(new constructSystem::CrossPipe(newName+"GaugeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  bremCollA(new xraySystem::BremColl(newName+"BremCollA")),
  filterBoxA(new constructSystem::PortTube(newName+"FilterBoxA")),
  filterStick(new xraySystem::FlangeMount(newName+"FilterStick")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  screenPipeA(new constructSystem::PipeTube(newName+"ScreenPipeA")),
  screenPipeB(new constructSystem::PipeTube(newName+"ScreenPipeB")),
  primeJawBox(new constructSystem::VacuumBox(newName+"PrimeJawBox")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),  
  gateB(new constructSystem::GateValve(newName+"GateB")),
  monoBox(new constructSystem::VacuumBox(newName+"MonoBox"))
    /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(pipeInit);
  OR.addObject(triggerPipe);
  OR.addObject(gaugeA);
  OR.addObject(bellowA);
  OR.addObject(bremCollA);
  OR.addObject(filterBoxA);
  OR.addObject(filterStick);
  OR.addObject(gateA);
  OR.addObject(screenPipeA);
  OR.addObject(screenPipeB);
  OR.addObject(primeJawBox);
  OR.addObject(bellowC);
  OR.addObject(gateB);
  OR.addObject(monoBox);
}
  
cosaxOpticsLine::~cosaxOpticsLine()
  /*!
    Destructor
   */
{}

void
cosaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);
  return;
}

void
cosaxOpticsLine::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("cosaxOpticsLine","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}


void
cosaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("cosaxOpticsLine","buildObjects");
  
  pipeInit->addInsertCell(ContainedComp::getInsertCells());
  pipeInit->registerSpaceCut(1,2);
  pipeInit->createAll(System,*this,0);

  
  triggerPipe->addInsertCell(ContainedComp::getInsertCells());
  triggerPipe->registerSpaceCut(1,2);
  triggerPipe->createAll(System,*pipeInit,2);

  gaugeA->addInsertCell(ContainedComp::getInsertCells());
  gaugeA->registerSpaceCut(1,2);
  gaugeA->createAll(System,*triggerPipe,2);


  bellowA->addInsertCell(ContainedComp::getInsertCells());
  bellowA->registerSpaceCut(1,2);
  bellowA->createAll(System,*gaugeA,2);

  bremCollA->addInsertCell(ContainedComp::getInsertCells());
  bremCollA->registerSpaceCut(1,2);
  bremCollA->createAll(System,*bellowA,2);


  filterBoxA->addInsertCell(ContainedComp::getInsertCells());
  filterBoxA->registerSpaceCut(1,2);
  filterBoxA->createAll(System,*bremCollA,2);

  filterBoxA->splitObject(System,1001,filterBoxA->getCell("OuterSpace"),
  			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));


  const constructSystem::portItem& PI=filterBoxA->getPort(3);
  filterStick->addInsertCell("Flange",filterBoxA->getBuildCell());
  filterStick->addInsertCell("Body",PI.getCell("Void"));
  filterStick->addInsertCell("Body",filterBoxA->getCell("Void"));
  filterStick->setBladeCentre(PI,0);
  filterStick->createAll(System,PI,2);


  gateA->addInsertCell(ContainedComp::getInsertCells());
  gateA->registerSpaceCut(1,2);
  gateA->createAll(System,*filterBoxA,2);

  screenPipeA->addInsertCell(ContainedComp::getInsertCells());
  screenPipeA->registerSpaceCut(1,2);
  screenPipeA->createAll(System,*gateA,2);


  screenPipeB->addInsertCell(ContainedComp::getInsertCells());
  screenPipeB->registerSpaceCut(1,2);
  screenPipeB->createAll(System,*screenPipeA,2);
  screenPipeB->intersectPorts(System,0,1);


  primeJawBox->addInsertCell(ContainedComp::getInsertCells());
  primeJawBox->registerSpaceCut(1,2);
  primeJawBox->createAll(System,*screenPipeB,2);

  bellowC->addInsertCell(ContainedComp::getInsertCells());
  bellowC->registerSpaceCut(1,2);
  bellowC->createAll(System,*primeJawBox,2);


  gateB->addInsertCell(ContainedComp::getInsertCells());
  gateB->registerSpaceCut(1,2);
  gateB->createAll(System,*bellowC,2);

  monoBox->addInsertCell(ContainedComp::getInsertCells());
  monoBox->registerSpaceCut(1,2);
  monoBox->createAll(System,*gateB,2);

  lastComp=monoBox;

  return;
}

void
cosaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*pipeInit,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
  
void 
cosaxOpticsLine::createAll(Simulation& System,
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
  ELog::RegMethod RControl("cosaxOpticsLine","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

