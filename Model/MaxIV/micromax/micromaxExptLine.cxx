/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: micromax/micromaxExptLine.cxx
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
#include "Exception.h"
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
#include "VirtualTube.h"
#include "PipeTube.h"
#include "portItem.h"
#include "FlangePlate.h"

#include "VacuumPipe.h"
#include "VacuumBox.h"
#include "Mirror.h"
#include "CRLTube.h"
#include "CylGateValve.h"
#include "CRLTube.h"
#include "HPJaws.h"
#include "FourPortTube.h"
#include "SixPortTube.h"
#include "ViewScreenTube.h"
#include "CooledScreen.h"
#include "RoundMonoShutter.h"


#include "micromaxExptLine.h"

namespace xraySystem
{

// Note currently uncopied:

  micromaxExptLine::micromaxExptLine(const std::string& baseName, const std::string& Key) :
  attachSystem::CopiedComp(baseName+Key,baseName+Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  baseName(baseName),
  buildZone(baseName+Key+"BuildZone"),
  outerMat(0),exptType("Sample"),

  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  viewTube(new xraySystem::ViewScreenTube(newName+"ViewTube")),
  cooledScreen(new xraySystem::CooledScreen(newName+"CooledScreen")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  hpJaws(new xraySystem::HPJaws(newName+"HPJaws")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  gateTubeB(new xraySystem::CylGateValve(newName+"GateTubeB")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),

  crlPipeA(new constructSystem::VacuumPipe(newName+"CRLPipeA")),
  crlTubeA(new xraySystem::CRLTube(newName+"CRLTubeA")),
  crlPipeB(new constructSystem::VacuumPipe(newName+"CRLPipeB")),
  crlTubeB(new xraySystem::CRLTube(newName+"CRLTubeB")),
  crlPipeD(new constructSystem::VacuumPipe(newName+"CRLPipeD")),

  endPipe(new constructSystem::VacuumPipe(newName+"EndPipe")),

  mirrorBoxA(new constructSystem::VacuumBox(newName+"MirrorBoxA")),
  mirrorFrontA(new xraySystem::Mirror(newName+"MirrorFrontA")),
  mirrorBackA(new xraySystem::Mirror(newName+"MirrorBackA")),

  monoShutterB(new xraySystem::RoundMonoShutter(newName+"RMonoShutterB")),

  diffractTube(new constructSystem::VacuumPipe(newName+"DiffractTube")),

  byPassTube(new constructSystem::VacuumPipe(newName+"ByPassTube")),

  endWindow(new constructSystem::FlangePlate(newName+"EndWindow")),
  sampleTube(new constructSystem::VacuumPipe(newName+"SampleTube")),
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
  OR.addObject(gateTubeB);
  OR.addObject(pipeC);

  OR.addObject(crlPipeA);
  OR.addObject(crlTubeA);
  OR.addObject(crlPipeB);
  OR.addObject(crlTubeB);
  OR.addObject(crlPipeD);

  OR.addObject(endPipe);
  OR.addObject(mirrorBoxA);
  OR.addObject(mirrorFrontA);
  OR.addObject(mirrorBackA);
  OR.addObject(sample);
  OR.addObject(endWindow);

  OR.addObject(diffractTube);

  OR.addObject(monoShutterB);
}

micromaxExptLine::~micromaxExptLine()
  /*!
    Destructor
   */
{}

void
micromaxExptLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database
   */
{
  ELog::RegMethod RegA("micromaxExptLine","populate");

  attachSystem::FixedRotate::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",outerMat);
  exptType=Control.EvalDefPair<std::string>(keyName+"ExptType",baseName+"ExptType", "Sample");

  return;
}

void
micromaxExptLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("micromaxExptLine","createSurface");

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
micromaxExptLine::constructCRL(Simulation& System,
			       const attachSystem::FixedComp& initFC,
			       const std::string& sideName)
 /*!
    Sub build of the post first mono system.
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructCRL");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*crlPipeA);

  constructSystem::constructUnit
    (System,buildZone,*crlPipeA,"back",*crlTubeA);

  constructSystem::constructUnit
    (System,buildZone,*crlTubeA,"back",*crlPipeB);

  constructSystem::constructUnit
    (System,buildZone,*crlPipeB,"back",*crlTubeB);

  constructSystem::constructUnit
    (System,buildZone,*crlTubeB,"back",*crlPipeD);

  return;
}

void
micromaxExptLine::constructSampleStage(Simulation& System,
				       const attachSystem::FixedComp& initFC,
				       const std::string& sideName)
 /*!
    Sub build of the sample in direct line geometry
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxExptLine","constructSampleStage");

  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*endWindow);

  // both build absolute
  sample->setNoInsert();
  sample->createAll(System,initFC,sideName);

  monoShutterB->createAll(System,*endPipe,"back");

  outerCell=buildZone.createUnit(System,*monoShutterB,"#front");
  sample->insertInCell(System,outerCell);
  outerCell=buildZone.createUnit(System,*monoShutterB,"back");

  monoShutterB->insertAllInCell(System,outerCell);
  monoShutterB->splitObject(System,"-TopPlate",outerCell);
  monoShutterB->splitObject(System,"MidCutB",outerCell);

  return;
}

void
micromaxExptLine::constructByPassStage(Simulation& System)
  /*!
    Sub build of the bypass geometry (beam to expthut-2)
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("micromaxExptLine","constructByPassStage");

  int outerCell,outerCellA;
  // First place mirrors into beam

  monoShutterB->createAll(System,*endPipe,"back");
  outerCellA=buildZone.createUnit(System,*monoShutterB,"#front");
  sample->insertInCell(System,outerCellA);
  outerCell=buildZone.createUnit(System,*monoShutterB,"back");

  monoShutterB->insertAllInCell(System,outerCell);
  monoShutterB->splitObject(System,"-TopPlate",outerCell);
  monoShutterB->splitObject(System,"MidCutB",outerCell);

  byPassTube->setFront(*mirrorBoxA,"back");
  byPassTube->setBack(*monoShutterB,"front");
  byPassTube->createAll(System,*endPipe,"back");
  byPassTube->insertAllInCell(System,outerCellA);

  return;
}

void
micromaxExptLine::constructDiffractionStage
    (Simulation& System,
     const attachSystem::FixedComp& initFC,
     const std::string& sideName)
 /*!
    Sub build of the diffractoin in out-line line geometry
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("micromaxExptLine","constructDiffractionStage");

  int outerCell;
  // First place mirrors into beam
  mirrorFrontA->addInsertCell(mirrorBoxA->getCell("Void",0));
  mirrorFrontA->createAll(System,*mirrorBoxA,0);

  mirrorBackA->addInsertCell(mirrorBoxA->getCell("Void",1));
  mirrorBackA->createAll(System,*mirrorBoxA,0);

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*diffractTube);

  // both build absolute
  sample->setNoInsert();
  sample->createAll(System,initFC,sideName);

  monoShutterB->createAll(System,*endPipe,"back");
  outerCell=buildZone.createUnit(System,*monoShutterB,"#front");
  sample->insertInCell(System,outerCell);
  outerCell=buildZone.createUnit(System,*monoShutterB,"back");

  monoShutterB->insertAllInCell(System,outerCell);
  monoShutterB->splitObject(System,"-TopPlate",outerCell);
  monoShutterB->splitObject(System,"MidCutB",outerCell);

  return;
}

void
micromaxExptLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("micromaxExptLine","buildObjects");

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

  constructSystem::constructUnit
    (System,buildZone,*pipeB,"back",*gateTubeB);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeB,"back",*pipeC);

  constructCRL(System,*pipeC,"back");

  constructSystem::constructUnit
    (System,buildZone,*crlPipeD,"back",*endPipe);


  if (exptType == "RodTest") {
    buildZone.createUnit(System);
    buildZone.rebuildInsertCells(System);
    setCell("LastVoid",buildZone.getLastCell("Unit"));
    lastComp=endPipe;
    return;
  }

  constructSystem::constructUnit
    (System,buildZone,*endPipe,"back",*mirrorBoxA);

  mirrorBoxA->splitObject(System,3001,mirrorBoxA->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));


  // main exits built

  if (exptType=="Sample")
    constructSampleStage(System,*mirrorBoxA,"back");
  else if (exptType=="Diffraction")
    constructDiffractionStage(System,*mirrorBoxA,"back");
  else if (exptType=="ByPass")
    constructByPassStage(System);
  else
    throw ColErr::InContainerError<std::string>(exptType,"exptType");

  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);
  setCell("LastVoid",buildZone.getLastCell("Unit"));
  lastComp=monoShutterB;

  return;
}

void
micromaxExptLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("micromaxExptLine","createLinks");

  setLinkCopy(0,*bellowA,1);
  setLinkCopy(1,*lastComp,2);
  return;
}

void
micromaxExptLine::insertSample(Simulation& System,
			     const int cellNumber) const
  /*!
    PRocess the insertion of the sample [UGLY]
    \param Ssytem :: Simulation
   */
{
  ELog::RegMethod RegA("micromaxExptLine","insertSample");

  sample->insertInCell(System,cellNumber);
  return;
}


void
micromaxExptLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("micromaxExptLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem
