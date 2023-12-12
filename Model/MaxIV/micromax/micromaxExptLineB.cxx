/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: micromax/micromaxExptLineB.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "Line.h"
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

#include "GeneralPipe.h"
#include "Bellows.h"


#include "VacuumPipe.h"
#include "CylGateValve.h"
#include "HPJaws.h"
#include "ViewScreenTube.h"
#include "CooledScreen.h"

#include "micromaxExptLineB.h"

namespace xraySystem
{

// Note currently uncopied:
  
micromaxExptLineB::micromaxExptLineB(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),
  outerMat(0),

  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  viewTube(new xraySystem::ViewScreenTube(newName+"ViewTube")),
  cooledScreen(new xraySystem::CooledScreen(newName+"CooledScreen")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  hpJaws(new xraySystem::HPJaws(newName+"HPJaws")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),

  sample(new insertSystem::insertSphere(newName+"Sample"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(gateTubeA);
  OR.addObject(bellowA);
  OR.addObject(viewTube); 
  OR.addObject(cooledScreen);
  OR.addObject(pipeA);
  OR.addObject(hpJaws);
  OR.addObject(pipeB);
  OR.addObject(sample);
}
  
micromaxExptLineB::~micromaxExptLineB()
  /*!
    Destructor
   */
{}

void
micromaxExptLineB::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database 
   */
{
  ELog::RegMethod RegA("micromaxExptLineB","populate");
  
  attachSystem::FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",outerMat);

  beamStopYStep=Control.EvalVar<double>(keyName+"BeamStopYStep");
  beamStopThick=Control.EvalVar<double>(keyName+"BeamStopThick");
  beamStopRadius=Control.EvalVar<double>(keyName+"BeamStopRadius");

  beamStopMat=ModelSupport::EvalMat<int>(Control,keyName+"BeamStopMat");
  return;
}

void
micromaxExptLineB::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("micromaxExptLineB","createSurface");

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

  ModelSupport::buildCylinder
    (SMap,buildIndex+107,Origin+Y*beamStopYStep,Y,beamStopRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin+Y*(beamStopYStep-beamStopThick/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(beamStopYStep+beamStopThick/2.0),Y);

  
  
  return;
}

void
micromaxExptLineB::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("micromaxExptLineB","buildObjects");

  int outerCell;
  
  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  gateTubeA->createAll(System,*this,0);
  outerCell=buildZone.createUnit(System,*gateTubeA,2);
  gateTubeA->insertInCell(System,outerCell);
  
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowA);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*viewTube);

  cooledScreen->setBeamAxis(*viewTube,1);
  cooledScreen->createAll(System,*viewTube,4);
  cooledScreen->insertInCell("Outer",System,outerCell);
  cooledScreen->insertInCell("Connect",System,viewTube->getCell("Plate"));
  cooledScreen->insertInCell("Connect",System,viewTube->getCell("Void"));
  cooledScreen->insertInCell("Payload",System,viewTube->getCell("Void"));

  constructSystem::constructUnit
    (System,buildZone,*viewTube,"back",*pipeA);

  constructSystem::constructUnit
    (System,buildZone,*pipeA,"back",*hpJaws);

  constructSystem::constructUnit
    (System,buildZone,*hpJaws,"back",*pipeB);

  sample->setNoInsert();
  sample->createAll(System,*pipeB,"back");

  outerCell=buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);
  sample->insertInCell(System,outerCell);

  HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 -107 ");
  CellMap::makeCell("BeamStop",System,cellIndex++,beamStopMat,0.0,HR);
  insertCellMapInCell(System,"BeamStop",outerCell);

  setCell("LastVoid",buildZone.getLastCell("Unit"));
  lastComp=pipeB;

  return;
}

void
micromaxExptLineB::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("micromaxExptLineB","createLinks");
  
  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*lastComp,2);
  return;
}

void
micromaxExptLineB::insertSample(Simulation& System,
			     const int cellNumber) const
  /*!
    PRocess the insertion of the sample [UGLY]
    \param Ssytem :: Simulation
   */
{
  ELog::RegMethod RegA("micromaxExptLineB","insertSample");

  sample->insertInCell(System,cellNumber);
  return;
}

  
void 
micromaxExptLineB::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("micromaxExptLineB","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

