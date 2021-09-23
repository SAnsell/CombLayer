/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: micromax/micromaxOpticsLine.cxx
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
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"
#include "Line.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "CrossPipe.h"
#include "BremColl.h"
#include "BremMonoColl.h"
#include "BremBlock.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "MonoBox.h"
#include "MonoShutter.h"
#include "TriggerTube.h"
#include "CylGateValve.h"
#include "SquareFMask.h"
#include "IonGauge.h"
#include "BeamPair.h"
#include "MonoBlockXstals.h"
#include "MLMono.h"
#include "DCMTank.h"
#include "BremTube.h"
#include "HPJaws.h"
#include "ViewScreenTube.h"
#include "YagScreen.h"
#include "Table.h"

#include "micromaxOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
micromaxOpticsLine::micromaxOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),
  
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  bremCollA(new xraySystem::SquareFMask(newName+"BremCollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  bremBlockTube(new constructSystem::PipeTube(newName+"BremBlockTube")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  attnTube(new constructSystem::PortTube(newName+"AttnTube")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  tableA(new xraySystem::Table(newName+"TableA")),
  monoVessel(new xraySystem::DCMTank(newName+"MonoVessel")),
  mbXstals(new xraySystem::MonoBlockXstals(newName+"MBXstals")),
  monoBremTube(new xraySystem::BremTube(newName+"MonoBremTube")),
  bremCollB(new xraySystem::BremBlock(newName+"BremCollB")),
  hpJawsA(new xraySystem::HPJaws(newName+"HPJawsA"))

  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(pipeInit);
  OR.addObject(triggerPipe);

  OR.addObject(gateTubeA);
  OR.addObject(pipeA);
  OR.addObject(bellowA);
  OR.addObject(bremCollA);
  OR.addObject(bellowB);
  OR.addObject(bremBlockTube);
  OR.addObject(bellowC);
  OR.addObject(viewTube);
  OR.addObject(bellowD);
  OR.addObject(attnTube);
  OR.addObject(bellowE);
  OR.addObject(tableA);
  OR.addObject(monoVessel);
  OR.addObject(mbXstals);
  OR.addObject(monoBremTube);
  OR.addObject(bremCollB);
  OR.addObject(hpJawsA);
  
    
}
  
micromaxOpticsLine::~micromaxOpticsLine()
  /*!
    Destructor
   */
{}

void
micromaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database 
   */
{
  ELog::RegMethod RegA("micromaxOpticsLine","populate");
  
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}


void
micromaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","createSurface");

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
micromaxOpticsLine::constructHDCM(Simulation& System,
				  const attachSystem::FixedComp& initFC, 
				  const std::string& sideName)
/*!
    Sub build of the mono package
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructHDCM");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoVessel);
  
  mbXstals->addInsertCell(monoVessel->getCell("Void"));
  mbXstals->createAll(System,*monoVessel,0);

  return;
}

void
micromaxOpticsLine::constructDiag2(Simulation& System,
				   const attachSystem::FixedComp& initFC, 
				   const std::string& sideName)
/*!
    Sub build of the post first mono system.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructDiag2");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoBremTube);

  bremCollB->addInsertCell(monoBremTube->getCell("Void"));
  bremCollB->createAll(System,*monoBremTube,0);

  hpJawsA->setFlangeJoin();
  constructSystem::constructUnit
    (System,buildZone,*monoBremTube,"back",*hpJawsA);

  return;
}

void
micromaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","buildObjects");

  int outerCell;
  
  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*pipeA);

  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*bellowA);
  
  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*bremCollA);

  constructSystem::constructUnit
    (System,buildZone,*bremCollA,"back",*bellowB);

  bremBlockTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  bremBlockTube->setOuterVoid();
  bremBlockTube->createAll(System,*bellowB,"back");

  const constructSystem::portItem& VPB=bremBlockTube->getPort(1);
  outerCell=buildZone.createUnit
    (System,VPB,VPB.getSideIndex("OuterPlate"));
  bremBlockTube->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*bellowC);

  viewTube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  viewTube->setOuterVoid();
  viewTube->createAll(System,*bellowC,"back");

  const constructSystem::portItem& VPC=viewTube->getPort(1);
  outerCell=buildZone.createUnit
    (System,VPC,VPC.getSideIndex("OuterPlate"));
  viewTube->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,VPC,"OuterPlate",*bellowD);

  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*attnTube);

  constructSystem::constructUnit
    (System,buildZone,*attnTube,"back",*bellowE);

  tableA->addHole(*viewTube,"Origin","OuterRadius");
  tableA->addHole(attnTube->getPort(1),"Origin","OuterRadius");
  tableA->createAll(System,*bellowA,0);
  tableA->insertInCells(System,buildZone.getCells());

  constructHDCM(System,*bellowE,"back");

  constructDiag2(System,*monoVessel,"back");
  
  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=bellowC;

  return;
}

void
micromaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("micromaxOpticsLine","createLinks");
  
  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
   
void 
micromaxOpticsLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("micromaxOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

