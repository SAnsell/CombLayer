/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: micromax/micromaxOpticsLine.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

#include "CrossPipe.h"
#include "BremColl.h"

#include "GateValveCube.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "MonoBox.h"
#include "MonoCrystals.h"
#include "MonoShutter.h"
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
  buildZone(*this,cellIndex),
  
  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new constructSystem::CrossPipe(newName+"TriggerPipe")),
  gaugeA(new constructSystem::CrossPipe(newName+"GaugeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  bremCollA(new xraySystem::BremColl(newName+"BremCollA")),
  filterBoxA(new constructSystem::PortTube(newName+"FilterBoxA")),
  filterStick(new xraySystem::FlangeMount(newName+"FilterStick")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  screenPipeA(new constructSystem::PipeTube(newName+"ScreenPipeA")),
  screenPipeB(new constructSystem::PipeTube(newName+"ScreenPipeB")),
  primeJawBox(new constructSystem::VacuumBox(newName+"PrimeJawBox")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),  
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  monoBox(new xraySystem::MonoBox(newName+"MonoBox")),
  monoXtal(new xraySystem::MonoCrystals(newName+"MonoXtal")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  diagBoxA(new constructSystem::PortTube(newName+"DiagBoxA")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  gateD(new constructSystem::GateValveCube(newName+"GateD")),
  mirrorA(new constructSystem::VacuumBox(newName+"MirrorA")),
  gateE(new constructSystem::GateValveCube(newName+"GateE")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),  
  diagBoxB(new constructSystem::PortTube(newName+"DiagBoxB")),
  jawCompB({
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit0"),
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit1")
	}),

  bellowG(new constructSystem::Bellows(newName+"BellowG")),  
  gateF(new constructSystem::GateValveCube(newName+"GateF")),
  mirrorB(new constructSystem::VacuumBox(newName+"MirrorB")),
  gateG(new constructSystem::GateValveCube(newName+"GateG")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),  
  diagBoxC(new constructSystem::PortTube(newName+"DiagBoxC")),
  jawCompC({
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit0"),
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit1")
	}),
  
  monoShutter(new xraySystem::MonoShutter(newName+"MonoShutter")),

  screenA(new xraySystem::PipeShield(newName+"ScreenA"))
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
  OR.addObject(monoXtal);
  OR.addObject(gateC);
  OR.addObject(bellowD);
  OR.addObject(diagBoxA);
  OR.addObject(bellowE);
  OR.addObject(gateD);
  OR.addObject(mirrorA);
  OR.addObject(gateE);
  OR.addObject(bellowF);
  OR.addObject(diagBoxB);
  OR.addObject(bellowG);
  OR.addObject(gateF);
  OR.addObject(mirrorB);
  OR.addObject(gateG);
  OR.addObject(bellowH);
  OR.addObject(diagBoxC);
  OR.addObject(monoShutter);
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
    \param Control :: Database of variables
  */
{
  FixedOffset::populate(Control);
  
  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);

  return;
}


int
micromaxOpticsLine::constructDiag
  (Simulation& System,
   MonteCarlo::Object** masterCellPtr,
   constructSystem::PortTube& diagBoxItem,
   std::array<std::shared_ptr<constructSystem::JawFlange>,2>& jawComp,
   const attachSystem::FixedComp& FC,const long int linkPt)
/*!
    Construct a diagnostic box
    \param System :: Simulation for building
    \param diagBoxItem :: Diagnostic box item
    \param jawComp :: Jaw componets to build in diagnostic box
    \param FC :: FixedComp for start point
    \param linkPt :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("micromaxOpticsLine","constructDiag");

  int outerCell;

  // fake insert

  diagBoxItem.addAllInsertCell((*masterCellPtr)->getName());  
  diagBoxItem.setFront(FC,linkPt);
  diagBoxItem.createAll(System,FC,linkPt);
  outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,diagBoxItem,2);
  diagBoxItem.insertAllInCell(System,outerCell);


  for(size_t index=0;index<2;index++)
    {
      const constructSystem::portItem& DPI=diagBoxItem.getPort(index);
      jawComp[index]->setFillRadius
	(DPI,DPI.getSideIndex("InnerRadius"),DPI.getCell("Void"));
      
      jawComp[index]->addInsertCell(diagBoxItem.getCell("Void"));
      if (index)
	jawComp[index]->addInsertCell(jawComp[index-1]->getCell("Void"));
      jawComp[index]->createAll
	(System,DPI,DPI.getSideIndex("InnerPlate"),diagBoxItem,0);
    }
  
  diagBoxItem.splitVoidPorts(System,"SplitOuter",2001,
			     diagBoxItem.getCell("Void"),{0,2});
  diagBoxItem.splitObject(System,-11,outerCell);
  diagBoxItem.splitObject(System,12,outerCell);
  diagBoxItem.splitObject(System,2001,outerCell);
  cellIndex+=3;
    
  return outerCell;
}
  
void
micromaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("micromaxOpticsLine","createSurface");

  if (outerLeft>Geometry::zeroTol &&  isActive("floor"))
    {
      std::string Out;
      ModelSupport::buildPlane
	(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane
	(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("floor"));
      buildZone.setSurround(HR);
    }
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
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));
    buildZone.setInsertCells(this->getInsertCells());
  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System);


  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);
  // dump cell for joinPipe
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,-1);
  // real cell for initPipe
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,2);
  pipeInit->insertInCell(System,outerCell);

  triggerPipe->setFront(*pipeInit,2);
  triggerPipe->createAll(System,*pipeInit,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*triggerPipe,2);
  triggerPipe->insertInCell(System,outerCell);
  
  gaugeA->setFront(*triggerPipe,2);
  gaugeA->createAll(System,*triggerPipe,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gaugeA,2);
  gaugeA->insertInCell(System,outerCell);

  bellowA->setFront(*gaugeA,2);
  bellowA->createAll(System,*gaugeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  bremCollA->setCutSurf("front",*bellowA,2);
  bremCollA->createAll(System,*bellowA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bremCollA,2);
  bremCollA->insertInCell("Main",System,outerCell);


  filterBoxA->addAllInsertCell(masterCell->getName());
  filterBoxA->setFront(*bremCollA,2);
  filterBoxA->createAll(System,*bremCollA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*filterBoxA,2);
  filterBoxA->insertAllInCell(System,outerCell);
  filterBoxA->splitObject(System,1001,outerCell,
  			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  cellIndex++;
  
  const constructSystem::portItem& PI=filterBoxA->getPort(3);
  filterStick->addInsertCell("Body",PI.getCell("Void"));
  filterStick->addInsertCell("Body",filterBoxA->getCell("Void"));
  filterStick->setBladeCentre(PI,0);
  filterStick->createAll(System,PI,PI.getSideIndex("-InnerPlate"));


  gateA->setFront(*filterBoxA,2);
  gateA->createAll(System,*filterBoxA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  // fake insert
  screenPipeA->addAllInsertCell(masterCell->getName());
  screenPipeA->setFront(*gateA,2);
  screenPipeA->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeA,2);
  screenPipeA->insertAllInCell(System,outerCell);

  // fake insert
  screenPipeB->addAllInsertCell(masterCell->getName());
  screenPipeB->setFront(*screenPipeA,2);
  screenPipeB->createAll(System,*screenPipeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeB,2);
  screenPipeB->insertAllInCell(System,outerCell);
  screenPipeB->intersectPorts(System,0,1);


  primeJawBox->setFront(*screenPipeB,2);
  primeJawBox->createAll(System,*screenPipeB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*primeJawBox,2);
  primeJawBox->insertInCell(System,outerCell);

  bellowC->setFront(*primeJawBox,2);
  bellowC->createAll(System,*primeJawBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  gateB->setFront(*bellowC,2);
  gateB->createAll(System,*bellowC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateB,2);
  gateB->insertInCell(System,outerCell);

  // fake insert
  monoBox->addInsertCell(masterCell->getName());
  monoBox->setFront(*gateB,2);
  monoBox->createAll(System,*gateB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoBox,2);
  monoBox->insertInCell(System,outerCell);
  monoBox->splitObject(System,2001,outerCell,
		       Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  cellIndex++;

  monoXtal->addInsertCell(monoBox->getCell("Void"));
  monoXtal->createAll(System,*monoBox,0);

  
  gateC->setFront(*monoBox,2);
  gateC->createAll(System,*monoBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateC,2);
  gateC->insertInCell(System,outerCell);

  bellowD->setFront(*gateC,2);
  bellowD->createAll(System,*gateC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);


  // fake insert
  diagBoxA->addAllInsertCell(masterCell->getName());
  diagBoxA->setFront(*bellowD,2);
  diagBoxA->createAll(System,*bellowD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diagBoxA,2);
  diagBoxA->insertAllInCell(System,outerCell);

  
  diagBoxA->splitVoidPorts(System,"SplitOuter",2001,
     			   diagBoxA->getCell("Void"),
   			   {0,1, 1,2});
  diagBoxA->splitObject(System,-11,outerCell);
  diagBoxA->splitObject(System,12,outerCell);
  diagBoxA->splitObject(System,-2001,outerCell);
  diagBoxA->splitObject(System,-2002,outerCell);

  //  diagBoxA->intersectPorts(System,3,6);
  diagBoxA->intersectVoidPorts(System,6,3);
  cellIndex+=4;
  
  
  bellowE->setFront(*diagBoxA,2);  
  bellowE->createAll(System,*diagBoxA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);

  gateD->setFront(*bellowE,2);  
  gateD->createAll(System,*bellowE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateD,2);
  gateD->insertInCell(System,outerCell);
  
  mirrorA->setFront(*gateD,2);  
  mirrorA->createAll(System,*gateD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorA,2);
  mirrorA->insertInCell(System,outerCell);

  gateE->setFront(*mirrorA,2);  
  gateE->createAll(System,*mirrorA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateE,2);
  gateE->insertInCell(System,outerCell);

  bellowF->setFront(*gateE,2);  
  bellowF->createAll(System,*gateE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowF,2);
  bellowF->insertInCell(System,outerCell);


  constructDiag(System,&masterCell,*diagBoxB,jawCompB,*bellowF,2);

  bellowG->setFront(*diagBoxB,2);  
  bellowG->createAll(System,*diagBoxB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowG,2);
  bellowG->insertInCell(System,outerCell);


  gateF->setFront(*bellowG,2);  
  gateF->createAll(System,*bellowG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateF,2);
  gateF->insertInCell(System,outerCell);

  mirrorB->setFront(*gateF,2);  
  mirrorB->createAll(System,*gateF,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorB,2);
  mirrorB->insertInCell(System,outerCell);

  gateG->setFront(*mirrorB,2);  
  gateG->createAll(System,*mirrorB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateG,2);
  gateG->insertInCell(System,outerCell);

  bellowH->setFront(*gateG,2);  
  bellowH->createAll(System,*gateG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowH,2);
  bellowH->insertInCell(System,outerCell);
   
  constructDiag(System,&masterCell,*diagBoxC,jawCompC,*bellowH,2);

  monoShutter->addAllInsertCell(masterCell->getName());
  monoShutter->setCutSurf("front",*diagBoxC,2);
  monoShutter->createAll(System,*diagBoxC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoShutter,2);

  monoShutter->insertAllInCell(System,outerCell);

  monoShutter->splitObject(System,"-PortACut",outerCell);
  const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  monoShutter->splitObjectAbsolute(System,2001,outerCell,midPoint,midAxis);
  monoShutter->splitObject(System,"PortBCut",outerCell);

  //  screenA->addAllInsertCell(masterCell->getName());  
  //  screenA->createAll(System,*diagBoxC,2);
  
  lastComp=diagBoxC;
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
  //  pipeInit->setFront(FC,sideIndex);
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

