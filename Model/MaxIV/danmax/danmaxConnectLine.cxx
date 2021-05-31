/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/danmaxConnectLine.cxx
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "SqrShield.h"

#include "danmaxConnectLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
danmaxConnectLine::danmaxConnectLine(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  buildZone(Key+"BuildZone"),
  connectShield(new xraySystem::SqrShield(keyName+"ConnectShield")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  flangeA(new constructSystem::VacuumPipe(keyName+"FlangeA")),
  ionPumpA(new constructSystem::PipeTube(keyName+"IonPumpA")),
  flangeB(new constructSystem::VacuumPipe(keyName+"FlangeB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(flangeA);
  OR.addObject(bellowA);
  OR.addObject(ionPumpA);
  OR.addObject(bellowB);
  OR.addObject(flangeB);
  OR.addObject(pipeB);
}
  
danmaxConnectLine::~danmaxConnectLine()
  /*!
    Destructor
   */
{}


  
void
danmaxConnectLine::buildObjects(Simulation& System,
				const attachSystem::FixedComp& beamFC,
				const long int sideIndex)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \param beamFC :: Connection point that is the beam axis
    \param sideIndex :: link point for beam axis
  */
{
  ELog::RegMethod RegA("danmaxConnectLine","buildObjects");
  // First build construction zone
  int outerCell;

  // Outer volume with shielding
  connectShield->setInsertCell(getInsertCells());
  connectShield->setFront(FrontBackCut::getFrontRule());
  connectShield->setBack(FrontBackCut::getBackRule());
  connectShield->createAll(System,beamFC,sideIndex);
  
  buildZone.setFront(FrontBackCut::getFrontRule());
  buildZone.setMaxExtent(FrontBackCut::getBackRule());

  buildZone.setSurround(connectShield->getInnerVoid());
  buildZone.setInnerMat(connectShield->getInnerMat());

      
  // insert first tube:
  constructSystem::constructUnit
    (System,buildZone,beamFC,beamFC.getSideName(sideIndex),*pipeA);

  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*flangeA);

  constructSystem::constructUnit
    (System,buildZone,*flangeA,"back",*ionPumpA);

  constructSystem::constructUnit
    (System,buildZone,*ionPumpA,"back",*flangeB);

  constructSystem::constructUnit
    (System,buildZone,*flangeB,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*pipeB);


  outerCell=buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCell("FirstVoid",buildZone.getCells("Unit").front());
  setCell("LastVoid",buildZone.getCells("Unit").back());

  JPipe->addAllInsertCell(outerCell);
  JPipe->setFront(*pipeB,2);
  JPipe->createAll(System,*pipeB,2);
    
  return;
}

  
  
void 
danmaxConnectLine::createAll(Simulation& System,
			     const attachSystem::FixedComp& beamFC,
			     const long int sideIndex)
  
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param beamFC :: Fixed component for pipe/beam
    \param beamName :: link point for beam
   */
{
  // For output stream
  ELog::RegMethod RControl("danmaxConnectLine","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(beamFC,sideIndex);
  buildObjects(System,beamFC,sideIndex);    
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem

