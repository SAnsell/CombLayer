/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: cosaxs/cosaxsExptLine.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "generateSurf.h"


#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValveCylinder.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawValveBase.h"
#include "JawValveCylinder.h"
#include "MonoBox.h"
#include "FilterHolder.h"
#include "DiffPumpXIADP03.h"
#include "cosaxsSampleArea.h"
#include "cosaxsTube.h"

#include "cosaxsExptLine.h"

namespace xraySystem
{

// Note currently uncopied:

cosaxsExptLine::cosaxsExptLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  doubleSlitA(new constructSystem::JawValveCylinder(newName+"DoubleSlitA")),
  doubleSlitB(new constructSystem::JawValveCylinder(newName+"DoubleSlitB")),
  diagUnit(new xraySystem::MonoBox(newName+"DiagnosticUnit")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  diffPump(new xraySystem::DiffPumpXIADP03(newName+"DiffPump")),
  telescopicSystem(new constructSystem::VacuumPipe(newName+"TelescopicSystem")),
  sampleArea(new xraySystem::cosaxsSampleArea(newName+"SampleArea")),
  tube(new xraySystem::cosaxsTube(newName+"Tube"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeInit);
  OR.addObject(gateA);
  OR.addObject(doubleSlitA);
  OR.addObject(doubleSlitB);
  OR.addObject(diagUnit);
  OR.addObject(gateB);
  OR.addObject(diffPump);
  OR.addObject(telescopicSystem);
  OR.addObject(sampleArea);
  OR.addObject(tube);
}

cosaxsExptLine::~cosaxsExptLine()
  /*!
    Destructor
   */
{}

void
cosaxsExptLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  ELog::RegMethod RegA("cosaxsExptLine","populate");

  FixedOffset::populate(Control);

  outerLength=Control.EvalVar<double>(keyName+"OuterLength");

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  nFilterHolders=Control.EvalDefVar<size_t>(keyName+"NFilterHolders",3);

  return;
}


void
cosaxsExptLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("cosaxsExptLine","createSurface");

  if (outerLeft>Geometry::zeroTol &&  isActive("floor"))
    {
      std::string Out;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("floor"));
      buildZone.setSurround(HR);
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*outerLength,Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  return;
}

void
cosaxsExptLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("cosaxsExptLine","buildObjects");

  int outerCell;

  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));
  buildZone.setInsertCells(this->getInsertCells());
  
  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System);

  pipeInit->createAll(System,*this,0);

  // front surface of the build zone - must have it
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,-1);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);

  gateA->setFront(*pipeInit,2);
  gateA->createAll(System,*pipeInit,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  doubleSlitA->setFront(*gateA,2);
  doubleSlitA->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*doubleSlitA,2);
  doubleSlitA->insertInCell(System,outerCell);

  doubleSlitB->setFront(*doubleSlitA,2);
  doubleSlitB->createAll(System,*doubleSlitA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*doubleSlitB,2);
  doubleSlitB->insertInCell(System,outerCell);

  diagUnit->setFront(*doubleSlitB,2);
  diagUnit->createAll(System,*doubleSlitB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diagUnit,2);
  diagUnit->insertInCell(System,outerCell);

  // filter holders
  ModelSupport::objectRegister& OR=ModelSupport::objectRegister::Instance();
  for (size_t i=0; i<nFilterHolders; i++)
    {
      std::shared_ptr<xraySystem::FilterHolder>
	fh(new xraySystem::FilterHolder(newName+"DiagnosticUnitFilterHolder"+
					std::to_string(i+1)));
      OR.addObject(fh);
      fh->addInsertCell(diagUnit->getCell("Void"));
      if (i==0)
	fh->createAll(System,*diagUnit,-2);
      else
	fh->createAll(System,*filterHolder[i-1],2);
      filterHolder.push_back(fh);
    }

  gateB->setFront(*diagUnit,2);
  gateB->createAll(System,*diagUnit,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateB,2);
  gateB->insertInCell(System,outerCell);

  //diffPump->setCutSurf("front",*gateB,2);
  diffPump->createAll(System,*gateB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diffPump,2);
  diffPump->insertInCell(System,outerCell);

  telescopicSystem->setFront(*diffPump,-7);
  telescopicSystem->createAll(System,*diffPump,-7);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,*telescopicSystem,2);
  telescopicSystem->insertAllInCell(System,outerCell);
  telescopicSystem->insertAllInCell(System,diffPump->getCell("FlangeFrontVoid"));


  // GOOD ABOVE this point:
  
  tube->delayPorts();
  tube->createAll(System,*this,0);


  sampleArea->setFront(*telescopicSystem,2);
  sampleArea->setBack(*tube,1);
  sampleArea->createAll(System,*telescopicSystem,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*sampleArea,2);
  sampleArea->insertInCell(System,outerCell);

  
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*tube,2);

  
  tube->insertInCell(System,outerCell);

  //  tube->createPorts(System);  
  setCell("SurroundVoid",outerCell);
  lastComp=tube;

  return;
}

void
cosaxsExptLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("cosaxsExptLine","createLinks");

  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}


void
cosaxsExptLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("cosaxsExptLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

