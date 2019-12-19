/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: danmax/danmaxConnectLine.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "LeadPipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "SqrShield.h"

#include "danmaxConnectLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
danmaxConnectLine::danmaxConnectLine(const std::string& Key) :
  attachSystem::FixedOffsetUnit(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
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
				const attachSystem::FixedComp& FC,
				const std::string& sideName,
				const attachSystem::FixedComp& beamFC,
				const std::string& beamName)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \parma FC :: Connection point
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("danmaxConnectLine","buildObjects");
  // First build construction zone
  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));  

  connectShield->setInsertCell(getInsertCells());
  connectShield->setFront(*this);
  connectShield->setBack(*this);
  connectShield->createAll(System,beamFC,beamName);
  
  buildZone.setSurround(connectShield->getInnerVoid());

  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System);

  buildZone.createOuterVoidUnit(System,masterCell,beamFC,beamName);
  
  // insert first tube:
  xrayConstruct::constructUnit
    (System,buildZone,masterCell,beamFC,beamName,*pipeA);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*pipeA,"back",*bellowA);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*bellowA,"back",*flangeA);

  
  ionPumpA->addAllInsertCell(masterCell->getName());
  ionPumpA->setFront(*flangeA,2);
  ionPumpA->createAll(System,*flangeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPumpA,2);
  ionPumpA->insertAllInCell(System,outerCell);


  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*ionPumpA,"back",*flangeB);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*flangeB,"back",*bellowB);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*bellowB,"back",*pipeB);

  outerCell=buildZone.createFinalVoidUnit(System,masterCell,*pipeB,2);

  JPipe->addInsertCell(outerCell);
  JPipe->setFront(*pipeB,2);
  JPipe->createAll(System,*pipeB,2);
    


  // JPipe->createAll(System,*pipeB,2);
  // JPipe->insertInCell(System,masterCell->getName());

  return;
}

// void
// danmaxConnectLine::createLinks()
//   /*!
//     Create a front/back link
//   */
// {
//   setLinkSignedCopy(0,*bellowA,1);
//   setLinkSignedCopy(1,*bellowC,2);
//   return;
// }
  
  
void 
danmaxConnectLine::construct(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const std::string& sideName,
			     const attachSystem::FixedComp& beamFC,
			     const std::string& beamName)
  
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component (hutch wall)
    \param sideName :: link point (hutch wall)
    \param beamFC :: Fixed component for pipe/beam
    \param beamName :: link point for beam
   */
{
  // For output stream
  ELog::RegMethod RControl("danmaxConnectLine","createAll");

  buildObjects(System,FC,sideName,beamFC,beamName);    
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem

