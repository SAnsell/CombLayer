/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: formax/formaxExptLine.cxx
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
#include "BlockZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"
#include "insertObject.h"
#include "insertSphere.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValveCylinder.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "portItem.h"

#include "VacuumPipe.h"
#include "MonoBox.h"
#include "BoxJaws.h"
#include "ConnectorTube.h"
#include "CLRTube.h"
#include "FourPortTube.h"
#include "SixPortTube.h"

#include "formaxDetectorTube.h"

#include "formaxExptLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
formaxExptLine::formaxExptLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),
  outerMat(0),
  
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  filterBoxA(new xraySystem::MonoBox(newName+"FilterBoxA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  crossA(new xraySystem::FourPortTube(newName+"CrossA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  jawBox(new xraySystem::BoxJaws(newName+"JawBox")),
  connectA(new xraySystem::ConnectorTube(newName+"ConnectA")),
  clrTubeA(new xraySystem::CLRTube(newName+"CLRTubeA")),
  connectB(new xraySystem::ConnectorTube(newName+"ConnectB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  sixPortA(new tdcSystem::SixPortTube(newName+"SixPortA")),
  cylGateA(new constructSystem::GateValveCylinder(newName+"CylGateA")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  sixPortB(new tdcSystem::SixPortTube(newName+"SixPortB")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  cylGateB(new constructSystem::GateValveCylinder(newName+"CylGateB")),
  sixPortC(new tdcSystem::SixPortTube(newName+"SixPortC")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  connectC(new xraySystem::ConnectorTube(newName+"ConnectC")),
  clrTubeB(new xraySystem::CLRTube(newName+"CLRTubeB")),
  connectD(new xraySystem::ConnectorTube(newName+"ConnectD")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  crossB(new xraySystem::FourPortTube(newName+"CrossB")), 
  adjustPipe(new constructSystem::VacuumPipe(newName+"AdjustPipe")),
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  jawBoxB(new xraySystem::BoxJaws(newName+"JawBoxB")),
  connectE(new xraySystem::ConnectorTube(newName+"ConnectE")),
  endPipe(new constructSystem::VacuumPipe(newName+"EndPipe")),
  sample(new insertSystem::insertSphere(newName+"Sample"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(bellowA);
  OR.addObject(filterBoxA);
  OR.addObject(bellowB);
  OR.addObject(crossA);
  OR.addObject(bellowC);
  OR.addObject(jawBox);
  OR.addObject(connectA);
  OR.addObject(clrTubeA);
  OR.addObject(connectB);
  OR.addObject(pipeA);
  OR.addObject(sixPortA);
  OR.addObject(cylGateA);
  OR.addObject(pipeB);
  OR.addObject(bellowD);
  OR.addObject(sixPortB);
  OR.addObject(pipeC);
  OR.addObject(cylGateB);
  OR.addObject(sixPortC);
  OR.addObject(pipeD);
  OR.addObject(connectC);
  OR.addObject(clrTubeB);
  OR.addObject(connectD);
  OR.addObject(viewTube);
  OR.addObject(bellowE);
  OR.addObject(crossB);
  OR.addObject(adjustPipe);
  OR.addObject(pipeE);
  OR.addObject(jawBoxB);
  OR.addObject(connectE);
  OR.addObject(endPipe);
  OR.addObject(sample);
}
  
formaxExptLine::~formaxExptLine()
  /*!
    Destructor
   */
{}

void
formaxExptLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database 
   */
{
  ELog::RegMethod RegA("formaxExptLine","populate");
  
  attachSystem::FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",0);
  
  return;
}

void
formaxExptLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("formaxExptLine","createSurface");

  if (outerLeft>Geometry::zeroTol &&  isActive("floor"))
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
     buildZone.setInnerMat(outerMat);
    }
  return;
}

void
formaxExptLine::constructViewScreen(Simulation& System,
				    const attachSystem::FixedComp& initFC, 
				    const std::string& sideName)
  /*!
    Sub build of the first viewing package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructViewScreen");

  // FAKE insertcell: required

  int prevCell(buildZone.getLastCell("Unit"));
    
  viewTube->setOuterVoid();
  

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*viewTube);
  viewTube->intersectPorts(System,0,2);
  viewTube->intersectPorts(System,1,2);
  viewTube->intersectPorts(System,0,3);
  viewTube->intersectPorts(System,1,3);

  const constructSystem::portItem& VPA=viewTube->getPort(2);
  VPA.insertInCell(System,prevCell);
  return;

}

void
formaxExptLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("formaxExptLine","buildObjects");

  int outerCell;
  
  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  bellowA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*bellowA,2);
  bellowA->insertInCell(System,outerCell);
  
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*filterBoxA);

  constructSystem::constructUnit
    (System,buildZone,*filterBoxA,"back",*bellowB);

  constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*crossA);

  constructSystem::constructUnit
    (System,buildZone,*crossA,"back",*bellowC);

  constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*jawBox);

  constructSystem::constructUnit
    (System,buildZone,*jawBox,"back",*connectA);

  constructSystem::constructUnit
    (System,buildZone,*connectA,"back",*clrTubeA);

  constructSystem::constructUnit
    (System,buildZone,*clrTubeA,"back",*connectB);
  
  constructSystem::constructUnit
    (System,buildZone,*connectB,"back",*pipeA);

  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*sixPortA);

  constructSystem::constructUnit
    (System,buildZone,*sixPortA,"back",*cylGateA);

  constructSystem::constructUnit
    (System,buildZone,*cylGateA,"back",*pipeB);

  constructSystem::constructUnit
    (System,buildZone,*pipeB,"back",*bellowD);
  
  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*sixPortB);

  constructSystem::constructUnit
    (System,buildZone,*sixPortB,"back",*pipeC);

  constructSystem::constructUnit
    (System,buildZone,*pipeC,"back",*cylGateB);

  constructSystem::constructUnit
    (System,buildZone,*cylGateB,"back",*sixPortC);

  constructSystem::constructUnit
    (System,buildZone,*sixPortC,"back",*pipeD);

  constructSystem::constructUnit
    (System,buildZone,*pipeD,"back",*connectC);

  constructSystem::constructUnit
    (System,buildZone,*connectC,"back",*clrTubeB);

  constructSystem::constructUnit
    (System,buildZone,*clrTubeB,"back",*connectD);

  constructViewScreen(System,*connectD,"back");

  constructSystem::constructUnit
    (System,buildZone,*viewTube,"back",*bellowE);

  constructSystem::constructUnit
    (System,buildZone,*bellowE,"back",*crossB);

  constructSystem::constructUnit
    (System,buildZone,*crossB,"back",*adjustPipe);

  constructSystem::constructUnit
    (System,buildZone,*adjustPipe,"back",*pipeE);

  constructSystem::constructUnit
    (System,buildZone,*pipeE,"back",*connectE);

  constructSystem::constructUnit
    (System,buildZone,*connectE,"back",*endPipe);
  
  //  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  sample->setNoInsert();
  sample->createAll(System,*endPipe,"back");
  
  setCell("LastVoid",buildZone.getLastCell("Unit"));
  lastComp=endPipe;

  return;
}

void
formaxExptLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("formaxExptLine","createLinks");
  
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*lastComp,2);
  return;
}

void
formaxExptLine::insertSample(Simulation& System,
			     const int cellNumber) const
  /*!
    PRocess the insertion of the sample [UGLY]
    \param Ssytem :: Simulation
   */
{
  ELog::RegMethod RegA("formaxExptLine","insertSample");

  sample->insertInCell(System,cellNumber);
  return;
}

  
void 
formaxExptLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("formaxExptLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

