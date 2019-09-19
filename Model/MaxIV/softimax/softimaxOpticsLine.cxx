/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: softimax/softimaxOpticsLine.cxx
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "CrossPipe.h"
#include "BremColl.h"
#include "BremMonoColl.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "MonoBox.h"
#include "MonoShutter.h"
#include "DiffPumpXIADP03.h"
#include "softimaxOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
softimaxOpticsLine::softimaxOpticsLine(const std::string& Key) :
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
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  bremCollA(new xraySystem::BremColl(newName+"BremCollA")),
  filterBoxA(new constructSystem::PortTube(newName+"FilterBoxA")),
  filterStick(new xraySystem::FlangeMount(newName+"FilterStick")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  screenPipeA(new constructSystem::PipeTube(newName+"ScreenPipeA")),
  screenPipeB(new constructSystem::PipeTube(newName+"ScreenPipeB")),
  adaptorPlateA(new constructSystem::VacuumPipe(newName+"AdaptorPlateA")),
  diffPumpA(new constructSystem::DiffPumpXIADP03(newName+"DiffPumpA")),
  primeJawBox(new constructSystem::VacuumBox(newName+"PrimeJawBox")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),  
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  monoBox(new xraySystem::MonoBox(newName+"MonoBox")),
  monoXtal(new xraySystem::MonoCrystals(newName+"MonoXtal")),
  gateD(new constructSystem::GateValveCube(newName+"GateD")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  diagBoxA(new constructSystem::PortTube(newName+"DiagBoxA")),
  bremMonoCollA(new xraySystem::BremMonoColl(newName+"BremMonoCollA")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  gateE(new constructSystem::GateValveCube(newName+"GateE")),
  mirrorBoxA(new constructSystem::VacuumBox(newName+"MirrorBoxA")),
  mirrorFrontA(new xraySystem::Mirror(newName+"MirrorFrontA")),
  mirrorBackA(new xraySystem::Mirror(newName+"MirrorBackA")),
  gateF(new constructSystem::GateValveCube(newName+"GateF")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),  
  diagBoxB(new constructSystem::PortTube(newName+"DiagBoxB")),
  jawCompB({
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit0"),
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit1")
	}),

  bellowG(new constructSystem::Bellows(newName+"BellowG")),  
  gateG(new constructSystem::GateValveCube(newName+"GateG")),
  mirrorBoxB(new constructSystem::VacuumBox(newName+"MirrorBoxB")),
  mirrorFrontB(new xraySystem::Mirror(newName+"MirrorFrontB")),
  mirrorBackB(new xraySystem::Mirror(newName+"MirrorBackB")),
  gateH(new constructSystem::GateValveCube(newName+"GateH")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),  
  diagBoxC(new constructSystem::PortTube(newName+"DiagBoxC")),
  jawCompC({
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit0"),
      std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit1")
	}),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  gateI(new constructSystem::GateValveCube(newName+"GateI")),
  monoShutter(new xraySystem::MonoShutter(newName+"MonoShutter")),
  
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  gateJ(new constructSystem::GateValveCube(newName+"GateJ"))
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
  OR.addObject(gateA);
  OR.addObject(bremCollA);
  OR.addObject(filterBoxA);
  OR.addObject(filterStick);
  OR.addObject(gateB);
  OR.addObject(screenPipeA);
  OR.addObject(screenPipeB);
  OR.addObject(adaptorPlateA);
  OR.addObject(diffPumpA);
  OR.addObject(primeJawBox);
  OR.addObject(bellowC);
  OR.addObject(gateC);
  OR.addObject(monoBox);
  OR.addObject(gateD);
  OR.addObject(bellowD);
  OR.addObject(diagBoxA);
  OR.addObject(bremMonoCollA);
  OR.addObject(bellowE);
  OR.addObject(gateE);
  OR.addObject(mirrorBoxA);
  OR.addObject(mirrorFrontA);
  OR.addObject(mirrorBackA);
  OR.addObject(gateF);
  OR.addObject(bellowF);
  OR.addObject(diagBoxB);
  OR.addObject(bellowG);
  OR.addObject(gateG);
  OR.addObject(mirrorBoxB);
  OR.addObject(mirrorFrontB);
  OR.addObject(mirrorBackB);
  OR.addObject(gateH);
  OR.addObject(bellowH);
  OR.addObject(diagBoxC);
  OR.addObject(gateI);
  OR.addObject(monoShutter);
  OR.addObject(bellowJ);
  OR.addObject(gateJ);
}
  
softimaxOpticsLine::~softimaxOpticsLine()
  /*!
    Destructor
   */
{}

void
softimaxOpticsLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}


void
softimaxOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("softimaxOpticsLine","createSurface");

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

int
softimaxOpticsLine::constructMonoShutter
  (Simulation& System,MonteCarlo::Object** masterCellPtr,
   const attachSystem::FixedComp& FC,const long int linkPt)
/*!
    Construct a monoshutter system
    \param System :: Simulation for building
    \param masterCellPtr Pointer to mast cell
    \param FC :: FixedComp for start point
    \param linkPt :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("softimaxOpticsLine","constructMonoShutter");

  int outerCell;
  
  gateI->setFront(FC,linkPt);
  gateI->createAll(System,FC,linkPt);
  outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*gateI,2);
  gateI->insertInCell(System,outerCell);

  monoShutter->addAllInsertCell((*masterCellPtr)->getName());
  monoShutter->setCutSurf("front",*gateI,2);
  monoShutter->createAll(System,*gateI,2);
  outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*monoShutter,2);

  monoShutter->insertAllInCell(System,outerCell);
  monoShutter->splitObject(System,"-PortACut",outerCell);
  const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  monoShutter->splitObjectAbsolute(System,2001,outerCell,midPoint,midAxis);
  monoShutter->splitObject(System,"PortBCut",outerCell);
  cellIndex+=3;

  bellowJ->setFront(*monoShutter,2);
  bellowJ->createAll(System,*monoShutter,2);
  outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*bellowJ,2);
  bellowJ->insertInCell(System,outerCell);


  gateJ->setFront(*bellowJ,2);
  gateJ->createAll(System,*bellowJ,2);
  outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*gateJ,2);
  gateJ->insertInCell(System,outerCell);
  
  return outerCell;
}


int
softimaxOpticsLine::constructDiag
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
  ELog::RegMethod RegA("softimaxOpticsLine","constructDiag");

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
softimaxOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("softimaxOpticsLine","buildObjects");

  int outerCell;
  
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));

  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System,*this);


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

  gateA->setFront(*bellowA,2);
  gateA->createAll(System,*bellowA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);


  bremCollA->setCutSurf("front",*gateA,2);
  bremCollA->createAll(System,*gateA,2);
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
  bremCollA->createExtension(System,filterBoxA->getCell("FrontPortVoid"));
  
  const constructSystem::portItem& PI=filterBoxA->getPort(3);
  filterStick->addInsertCell("Body",PI.getCell("Void"));
  filterStick->addInsertCell("Body",filterBoxA->getCell("Void"));
  filterStick->setBladeCentre(PI,0);
  filterStick->createAll(System,PI,PI.getSideIndex("-InnerPlate"));


  gateB->setFront(*filterBoxA,2);
  gateB->createAll(System,*filterBoxA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateB,2);
  gateB->insertInCell(System,outerCell);

  // fake insert
  screenPipeA->addAllInsertCell(masterCell->getName());
  screenPipeA->setFront(*gateB,2);
  screenPipeA->createAll(System,*gateB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeA,2);
  screenPipeA->insertAllInCell(System,outerCell);

  // fake insert
  screenPipeB->addAllInsertCell(masterCell->getName());
  screenPipeB->setFront(*screenPipeA,2);
  screenPipeB->createAll(System,*screenPipeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeB,2);
  screenPipeB->insertAllInCell(System,outerCell);
  screenPipeB->intersectPorts(System,0,1);

  adaptorPlateA->setFront(*screenPipeB,2);
  adaptorPlateA->createAll(System,*screenPipeB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*adaptorPlateA,2);
  adaptorPlateA->insertInCell(System,outerCell);


  diffPumpA->setCutSurf("front",*adaptorPlateA,2);
  diffPumpA->createAll(System,*adaptorPlateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diffPumpA,2);
  diffPumpA->insertInCell(System,outerCell);


  // primeJawBox->setFront(*screenPipeB,2);
  // primeJawBox->createAll(System,*screenPipeB,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*primeJawBox,2);
  // primeJawBox->insertInCell(System,outerCell);

  bellowC->setFront(*diffPumpA,2);
  bellowC->createAll(System,*diffPumpA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  gateC->setFront(*bellowC,2);
  gateC->createAll(System,*bellowC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateC,2);
  gateC->insertInCell(System,outerCell);

  // fake insert
  monoBox->addInsertCell(masterCell->getName());
  monoBox->setFront(*gateC,2);
  monoBox->createAll(System,*gateC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoBox,2);
  monoBox->insertInCell(System,outerCell);
  monoBox->splitObject(System,2001,outerCell,
		       Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  cellIndex++;

  monoXtal->addInsertCell(monoBox->getCell("Void"));
  monoXtal->createAll(System,*monoBox,0);

  gateD->setFront(*monoBox,2);
  gateD->createAll(System,*monoBox,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateD,2);
  gateD->insertInCell(System,outerCell);


  bellowD->setFront(*gateD,2);
  bellowD->createAll(System,*gateD,2);
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

  bremMonoCollA->addInsertCell("Flange",diagBoxA->getCell("Void",0));
  bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",0));
  bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",1));
  bremMonoCollA->setCutSurf("front",diagBoxA->getSurf("VoidFront"));
  bremMonoCollA->setCutSurf("wallRadius",diagBoxA->getSurf("VoidCyl"));
  bremMonoCollA->setInOrg(monoXtal->getLinkPt(2));
  bremMonoCollA->createAll(System,*diagBoxA,0);

  // ELog::EM<<"Early return here"<<ELog::endDiag;
  // setCell("LastVoid",masterCell->getName());
  // lastComp=gateJ;
  // return;

  
  bellowE->setFront(*diagBoxA,2);  
  bellowE->createAll(System,*diagBoxA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);

  gateE->setFront(*bellowE,2);  
  gateE->createAll(System,*bellowE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateE,2);
  gateE->insertInCell(System,outerCell);
  
  mirrorBoxA->setFront(*gateE,2);  
  mirrorBoxA->createAll(System,*gateE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorBoxA,2);
  mirrorBoxA->insertInCell(System,outerCell);
  
  mirrorBoxA->splitObject(System,3001,mirrorBoxA->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  
  mirrorFrontA->addInsertCell(mirrorBoxA->getCell("Void",0));
  mirrorFrontA->createAll(System,*mirrorBoxA,0);

  mirrorBackA->addInsertCell(mirrorBoxA->getCell("Void",1));
  mirrorBackA->createAll(System,*mirrorBoxA,0);

  gateF->setFront(*mirrorBoxA,2);  
  gateF->createAll(System,*mirrorBoxA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateF,2);
  gateF->insertInCell(System,outerCell);

  bellowF->setFront(*gateF,2);  
  bellowF->createAll(System,*gateF,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowF,2);
  bellowF->insertInCell(System,outerCell);

  constructDiag(System,&masterCell,*diagBoxB,jawCompB,*bellowF,2);

  bellowG->setFront(*diagBoxB,2);  
  bellowG->createAll(System,*diagBoxB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowG,2);
  bellowG->insertInCell(System,outerCell);


  gateG->setFront(*bellowG,2);  
  gateG->createAll(System,*bellowG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateG,2);
  gateG->insertInCell(System,outerCell);

  mirrorBoxB->setFront(*gateG,2);  
  mirrorBoxB->createAll(System,*gateG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorBoxB,2);
  mirrorBoxB->insertInCell(System,outerCell);

  mirrorBoxB->splitObject(System,3001,mirrorBoxB->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  
  
  mirrorFrontB->addInsertCell(mirrorBoxB->getCell("Void",0));
  mirrorFrontB->createAll(System,*mirrorBoxB,0);
  mirrorBackB->addInsertCell(mirrorBoxB->getCell("Void",1));
  mirrorBackB->createAll(System,*mirrorBoxB,0);

  gateH->setFront(*mirrorBoxB,2);  
  gateH->createAll(System,*mirrorBoxB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateH,2);
  gateH->insertInCell(System,outerCell);

  bellowH->setFront(*gateH,2);  
  bellowH->createAll(System,*gateH,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowH,2);
  bellowH->insertInCell(System,outerCell);

  constructDiag(System,&masterCell,*diagBoxC,jawCompC,*bellowH,2);

  bellowI->setFront(*diagBoxC,2);
  bellowI->createAll(System,*diagBoxC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowI,2);
  bellowI->insertInCell(System,outerCell);

  constructMonoShutter(System,&masterCell,*bellowI,2);

  setCell("LastVoid",masterCell->getName());
  lastComp=gateJ;
  return;
}

void
softimaxOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("softimaxOpticsLine","createLinks");
  
  setLinkSignedCopy(0,*pipeInit,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
  
void 
softimaxOpticsLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("softimaxOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

