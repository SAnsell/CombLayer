/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: formax/formaxOpticsLine.cxx
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
#include "RoundMonoShutter.h"
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

#include "formaxOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
formaxOpticsLine::formaxOpticsLine(const std::string& Key) :
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
  whiteCollA(new xraySystem::SquareFMask(newName+"WhiteCollA")),
  bremHolderA(new xraySystem::IonGauge(newName+"BremHolderA")),
  bremCollA(new xraySystem::BremBlock(newName+"BremCollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  bremPipeB(new constructSystem::VacuumPipe(newName+"BremPipeB")),
  diagBoxA(new constructSystem::PortTube(newName+"DiagBoxA")),
  jaws({
      std::make_shared<xraySystem::BeamPair>(newName+"DiagBoxAJawX"),
      std::make_shared<xraySystem::BeamPair>(newName+"DiagBoxAJawZ")
    }),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  gateTubeB(new xraySystem::CylGateValve(newName+"GateTubeB")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  MLMVessel(new constructSystem::VacuumBox(newName+"MLMVessel")),
  MLM(new xraySystem::MLMono(newName+"MLM")),
  
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  gateTubeC(new xraySystem::CylGateValve(newName+"GateTubeC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),

  monoVessel(new xraySystem::DCMTank(newName+"MonoVessel")),
  mbXstals(new xraySystem::MonoBlockXstals(newName+"MBXstals")),

  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  gateTubeD(new xraySystem::CylGateValve(newName+"GateTubeD")),

  bremTubeA(new xraySystem::BremTube(newName+"BremTubeA")),
  bremCollB(new xraySystem::BremBlock(newName+"BremCollB")),
  hpJawsA(new xraySystem::HPJaws(newName+"HPJawsA")),

  mirrorBoxA(new constructSystem::VacuumBox(newName+"MirrorBoxA")),
  mirrorFrontA(new xraySystem::Mirror(newName+"MirrorFrontA")),
  mirrorBackA(new xraySystem::Mirror(newName+"MirrorBackA")),

  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  gateTubeE(new xraySystem::CylGateValve(newName+"GateTubeE")),
  viewTube(new xraySystem::ViewScreenTube(newName+"ViewTube")),
  yagScreen(new tdcSystem::YagScreen(newName+"YagScreen")),

  bremTubeB(new constructSystem::PipeTube(newName+"BremTubeB")),
  bremCollC(new xraySystem::BremBlock(newName+"BremCollC")),  
  hpJawsB(new xraySystem::HPJaws(newName+"HPJawsB")),
  
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),

  gateTubeF(new xraySystem::CylGateValve(newName+"GateTubeF")),
  viewTubeB(new xraySystem::ViewScreenTube(newName+"ViewTubeB")),

  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  monoAdaptorA(new constructSystem::VacuumPipe(newName+"MonoAdaptorA")),
  monoShutter(new xraySystem::RoundMonoShutter(newName+"RMonoShutter")),
  monoAdaptorB(new constructSystem::VacuumPipe(newName+"MonoAdaptorB")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF"))
  
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
  OR.addObject(whiteCollA);
  OR.addObject(bellowB);
  OR.addObject(bremHolderA);
  OR.addObject(bremCollA);
  OR.addObject(bremPipeB);
  OR.addObject(diagBoxA);
  OR.addObject(jaws[0]);
  OR.addObject(jaws[1]);

  OR.addObject(gateTubeB);
  OR.addObject(pipeB);
  OR.addObject(bellowC);
  OR.addObject(MLMVessel);
  OR.addObject(MLM);

  OR.addObject(bellowD);
  OR.addObject(pipeC);
  OR.addObject(gateTubeC);
  OR.addObject(bellowE);

  OR.addObject(monoVessel);
  OR.addObject(mbXstals);

  OR.addObject(bellowF);
  OR.addObject(pipeD);
  OR.addObject(gateTubeD);

  OR.addObject(bremTubeA);
  OR.addObject(bremCollB);
  OR.addObject(hpJawsA);

  OR.addObject(mirrorBoxA);
  OR.addObject(mirrorFrontA);
  OR.addObject(mirrorBackA);

  OR.addObject(bellowG);
  OR.addObject(gateTubeE);
  OR.addObject(viewTube);
  OR.addObject(yagScreen);
  OR.addObject(bremTubeB);
  OR.addObject(bremCollC);
  OR.addObject(hpJawsB);
  
  OR.addObject(bellowH);
  OR.addObject(pipeE);
  OR.addObject(bellowI);

  OR.addObject(gateTubeF);
  OR.addObject(viewTubeB);

  OR.addObject(bellowJ);
  OR.addObject(monoAdaptorA);
  OR.addObject(monoShutter);
  OR.addObject(monoAdaptorB);
  OR.addObject(pipeF);
  
}
  
formaxOpticsLine::~formaxOpticsLine()
  /*!
    Destructor
   */
{}

void
formaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database 
   */
{
  ELog::RegMethod RegA("formaxOpticsLine","populate");
  
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}


void
formaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","createSurface");

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
formaxOpticsLine::constructMirrorMono(Simulation& System,
				      const attachSystem::FixedComp& initFC, 
				      const std::string& sideName)
/*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructMirrorMono");

  int outerCell;

  MLMVessel->createAll(System,initFC,sideName);
  outerCell=buildZone.createUnit(System,*MLMVessel,2);
  MLMVessel->insertInCell(System,outerCell);
  
  MLM->addInsertCell(MLMVessel->getCell("Void"));
  MLM->createAll(System,*MLMVessel,0);

  return;
}

void
formaxOpticsLine::constructHDCM(Simulation& System,
				const attachSystem::FixedComp& initFC, 
				const std::string& sideName)
/*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructHDCM");

  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*monoVessel);
  
  mbXstals->addInsertCell(monoVessel->getCell("Void"));
  mbXstals->createAll(System,*monoVessel,0);

  return;
}

void
formaxOpticsLine::constructDiag2(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructDiag2");


  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*bremTubeA);

  bremCollB->addInsertCell(bremTubeA->getCell("Void"));
  bremCollB->createAll(System,*bremTubeA,0);

  hpJawsA->setFlangeJoin();
  constructSystem::constructUnit
    (System,buildZone,*bremTubeA,"back",*hpJawsA);

  return;
}

void
formaxOpticsLine::constructDiag3(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructDiag3");


  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*bellowG);

  constructSystem::constructUnit
    (System,buildZone,*bellowG,"back",*gateTubeE);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeE,"back",*viewTube);

  bremTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  bremTubeB->setOuterVoid();
  bremTubeB->createAll(System,*viewTube,"back");

  const constructSystem::portItem& VPB=bremTubeB->getPort(1);
  int outerCell=buildZone.createUnit(System,VPB,"OuterPlate");
  bremTubeB->insertAllInCell(System,outerCell);

  bremCollC->addInsertCell(bremTubeB->getCell("Void"));
  bremCollC->createAll(System,*bremTubeB,0);

  hpJawsB->setFlangeJoin();
  constructSystem::constructUnit
    (System,buildZone,VPB,"OuterPlate",*hpJawsB);

  constructSystem::constructUnit
    (System,buildZone,*hpJawsB,"back",*bellowH);

  constructSystem::constructUnit
    (System,buildZone,*bellowH,"back",*pipeE);

  constructSystem::constructUnit
    (System,buildZone,*pipeE,"back",*bellowI);

  return;
}

void
formaxOpticsLine::constructDiag4(Simulation& System,
				 const attachSystem::FixedComp& initFC, 
				 const std::string& sideName)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param initFC :: Start point
    \param sideName :: start link point
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructDiag4");


  constructSystem::constructUnit
    (System,buildZone,initFC,sideName,*gateTubeF);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeF,"back",*viewTubeB);
  
}

void
formaxOpticsLine::constructMonoShutter(Simulation& System,
				       const attachSystem::FixedComp& FC,
				       const std::string& linkName)
  /*!
    Construct a monoshutter system
    \param System :: Simulation for building
    \param FC :: FixedComp for start point
    \param linkName :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("formaxOpticsLine","constructMonoShutter");


  constructSystem::constructUnit
    (System,buildZone,FC,linkName,*bellowJ);

  constructSystem::constructUnit
    (System,buildZone,*bellowJ,"back",*monoAdaptorA);

  int outerCell=constructSystem::constructUnit
    (System,buildZone,*monoAdaptorA,"back",*monoShutter);

  monoShutter->splitObject(System,"-TopPlate",outerCell);

  monoShutter->splitObject(System,"MidCutB",outerCell);

  constructSystem::constructUnit
    (System,buildZone,*monoShutter,"back",*monoAdaptorB);

  constructSystem::constructUnit
    (System,buildZone,*monoAdaptorB,"back",*pipeF);
  
  return;
}


void
formaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("formaxOpticsLine","buildObjects");

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
    (System,buildZone,*bellowA,"back",*whiteCollA);

  constructSystem::constructUnit
    (System,buildZone,*whiteCollA,"back",*bremHolderA);

  bremCollA->addInsertCell(bremHolderA->getCell("Void"));
  bremCollA->createAll(System,*bremHolderA,0);

  constructSystem::constructUnit
    (System,buildZone,*bremHolderA,"back",*bellowB);

  // split later:
  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowB,"back",*diagBoxA);
  diagBoxA->intersectPorts(System,0,1);
  diagBoxA->intersectPorts(System,4,3);


  for(size_t i=0;i<jaws.size();i++)
    {
      const constructSystem::portItem& PI=diagBoxA->getPort(i);
      jaws[i]->createAll(System,*diagBoxA,0,
			 PI,PI.getSideIndex("InnerPlate"));

      const int surfNum(1501+10*static_cast<int>(i));
      diagBoxA->splitObjectAbsolute(System,surfNum,
				    diagBoxA->getCell("Void",i),
				    jaws[i]->getCentre(),
				    diagBoxA->getY());
      jaws[i]->insertInCell("SupportA",System,PI.getCell("Void"));
      jaws[i]->insertInCell("SupportB",System,PI.getCell("Void"));
      cellIndex++;
    }
  
  jaws[0]->insertInCell("SupportB",System,diagBoxA->getCell("Void",0));
  jaws[0]->insertInCell("SupportA",System,diagBoxA->getCell("Void",1));
  jaws[0]->insertInCell("BlockB",System,diagBoxA->getCell("Void",0));
  jaws[0]->insertInCell("BlockA",System,diagBoxA->getCell("Void",1));

  jaws[1]->insertInCell("SupportA",System,diagBoxA->getCell("Void",2));
  jaws[1]->insertInCell("SupportB",System,diagBoxA->getCell("Void",1));
  jaws[1]->insertInCell("BlockA",System,diagBoxA->getCell("Void",2));
  jaws[1]->insertInCell("BlockB",System,diagBoxA->getCell("Void",1));

  // split on port:

  diagBoxA->splitVoidPorts(System,"OuterSplit",2501,outerCell,
			  {1,2});
  outerCell=diagBoxA->getCell("OuterSplit",1);
  diagBoxA->splitVoidPorts(System,"OuterSplit",2601,outerCell,
			  {2,3});

  // exit:

  constructSystem::constructUnit
    (System,buildZone,*diagBoxA,"back",*pipeB);
  constructSystem::constructUnit
    (System,buildZone,*pipeB,"back",*gateTubeB);
  constructSystem::constructUnit
    (System,buildZone,*gateTubeB,"back",*bellowC);

  constructMirrorMono(System,*bellowC,"back");


  constructSystem::constructUnit
    (System,buildZone,*MLMVessel,"back",*bellowD);
  constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*pipeC);
  constructSystem::constructUnit
    (System,buildZone,*pipeC,"back",*gateTubeC);
  constructSystem::constructUnit
    (System,buildZone,*gateTubeC,"back",*bellowE);

  constructHDCM(System,*bellowE,"back");

  constructSystem::constructUnit
    (System,buildZone,*monoVessel,"back",*bellowF);
  constructSystem::constructUnit
    (System,buildZone,*bellowF,"back",*pipeD);
  constructSystem::constructUnit
    (System,buildZone,*pipeD,"back",*gateTubeD);
  

  constructDiag2(System,*gateTubeD,"back");


  constructSystem::constructUnit
    (System,buildZone,*hpJawsA,"back",*mirrorBoxA);

  mirrorBoxA->splitObject(System,3001,mirrorBoxA->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  
  mirrorFrontA->addInsertCell(mirrorBoxA->getCell("Void",0));
  mirrorFrontA->createAll(System,*mirrorBoxA,0);

  mirrorBackA->addInsertCell(mirrorBoxA->getCell("Void",1));
  mirrorBackA->createAll(System,*mirrorBoxA,0);

  constructDiag3(System,*mirrorBoxA,"back");


  constructDiag4(System,*bellowI,"back");

  constructMonoShutter(System,*viewTubeB,"back");


  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=pipeF;

  return;
}

void
formaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("formaxOpticsLine","createLinks");
  
  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}
  
  
void 
formaxOpticsLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("formaxOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

