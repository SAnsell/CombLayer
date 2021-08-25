/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: cosaxs/cosaxsOpticsLine.cxx
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
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "CrossPipe.h"
#include "TriggerTube.h"
#include "BremColl.h"
#include "BremMonoColl.h"
#include "MonoCrystals.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "Mirror.h"
#include "MonoBox.h"
#include "MonoShutter.h"
#include "DiffPumpXIADP03.h"
#include "cosaxsOpticsLine.h"

namespace xraySystem
{

// Note currently uncopied:

cosaxsOpticsLine::cosaxsOpticsLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),

  buildZone(Key+"BuildZone"),innerMat(0),

  pipeInit(new constructSystem::Bellows(newName+"InitBellow")),
  triggerPipe(new xraySystem::TriggerTube(newName+"TriggerUnit")),
  gateTubeA(new xraySystem::CylGateValve(newName+"GateTubeA")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  bremCollA(new xraySystem::BremColl(newName+"BremCollA")),
  filterBoxA(new constructSystem::PortTube(newName+"FilterBoxA")),
  filterStick(new xraySystem::FlangeMount(newName+"FilterStick")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  screenPipeA(new constructSystem::PipeTube(newName+"ScreenPipeA")),
  screenPipeB(new constructSystem::PipeTube(newName+"ScreenPipeB")),
  adaptorPlateA(new constructSystem::VacuumPipe(newName+"AdaptorPlateA")),
  diffPumpA(new xraySystem::DiffPumpXIADP03(newName+"DiffPumpA")),
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
  OR.addObject(gateTubeA);
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

cosaxsOpticsLine::~cosaxsOpticsLine()
  /*!
    Destructor
   */
{}

void
cosaxsOpticsLine::populate(const FuncDataBase& Control)
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
cosaxsOpticsLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("cosaxsOpticsLine","createSurface");

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

int
cosaxsOpticsLine::constructMonoShutter
(
 Simulation& System,
 const attachSystem::FixedComp& FC,
 const std::string& linkName)
/*!
    Construct a monoshutter system
    \param System :: Simulation for building
    \param masterCellPtr Pointer to mast cell
    \param FC :: FixedComp for start point
    \param linkNAme :: side index
    \return outerCell
   */
{
  ELog::RegMethod RegA("cosaxsOpticsLine","constructMonoShutter");

  int outerCell;

  constructSystem::constructUnit
    (System,buildZone,FC,linkName,*gateI);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*gateI,"back",*monoShutter);

  monoShutter->insertAllInCell(System,outerCell);
  monoShutter->splitObject(System,"-PortACut",outerCell);
  const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  monoShutter->splitObjectAbsolute(System,2001,outerCell,midPoint,midAxis);
  monoShutter->splitObject(System,"PortBCut",outerCell);
  cellIndex+=3;

  constructSystem::constructUnit
    (System,buildZone,*monoShutter,"back",*bellowJ);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowJ,"back",*gateJ);

  return outerCell;
}


int
cosaxsOpticsLine::constructDiag
(
 Simulation& System,
 constructSystem::PortTube& diagBoxItem,
 std::array<std::shared_ptr<constructSystem::JawFlange>,2>& jawComp,
 const attachSystem::FixedComp& FC,const std::string& linkName
 )
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
  ELog::RegMethod RegA("cosaxsOpticsLine","constructDiag");

  int outerCell=constructSystem::constructUnit
    (System,buildZone,FC,linkName,diagBoxItem);

  for(size_t index=0;index<2;index++)
    {
      const constructSystem::portItem& DPI=diagBoxItem.getPort(index);

      jawComp[index]->setFillRadius(DPI,"InnerRadius",DPI.getCell("Void"));
      jawComp[index]->addInsertCell(diagBoxItem.getCell("Void"));
      if (index)
	jawComp[index]->addInsertCell(jawComp[index-1]->getCell("Void"));
      jawComp[index]->createAll(System,DPI,"InnerPlate",diagBoxItem,"Origin");
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
cosaxsOpticsLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("cosaxsOpticsLine","buildObjects");

  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());
  
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  pipeInit->createAll(System,*this,0);

  outerCell=buildZone.createUnit(System,*pipeInit,"back");
  pipeInit->insertInCell(System,outerCell);
  if (preInsert)
    preInsert->insertAllInCell(System,outerCell);

  constructSystem::constructUnit
    (System,buildZone,*pipeInit,"back",*triggerPipe);

  constructSystem::constructUnit
    (System,buildZone,*triggerPipe,"back",*gateTubeA);

  constructSystem::constructUnit
    (System,buildZone,*gateTubeA,"back",*bellowA);

  constructSystem::constructUnit
    (System,buildZone,*bellowA,"back",*gateA);

  constructSystem::constructUnit
    (System,buildZone,*gateA,"back",*bremCollA);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bremCollA,"back",*filterBoxA);

  filterBoxA->splitObject(System,1001,outerCell,
  			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  cellIndex++;
  bremCollA->createExtension(System,filterBoxA->getCell("FrontPortVoid"));
  
  const constructSystem::portItem& PI=filterBoxA->getPort(3);
  filterStick->addInsertCell("Body",PI.getCell("Void"));
  filterStick->addInsertCell("Blade",PI.getCell("Void"));
  filterStick->addInsertCell("Body",filterBoxA->getCell("Void"));
  filterStick->addInsertCell("Blade",filterBoxA->getCell("Void"));
  filterStick->setBladeCentre(PI,0);
  filterStick->createAll(System,PI,PI.getSideIndex("-InnerPlate"));

  constructSystem::constructUnit
    (System,buildZone,*filterBoxA,"back",*gateB);

  constructSystem::constructUnit
    (System,buildZone,*gateB,"back",*screenPipeA);

  constructSystem::constructUnit
    (System,buildZone,*screenPipeA,"back",*screenPipeB);
  screenPipeB->intersectPorts(System,0,1);

  constructSystem::constructUnit
    (System,buildZone,*screenPipeB,"back",*adaptorPlateA);

  constructSystem::constructUnit
    (System,buildZone,*adaptorPlateA,"back",*diffPumpA);

  constructSystem::constructUnit
    (System,buildZone,*diffPumpA,"back",*bellowC);

  constructSystem::constructUnit
    (System,buildZone,*bellowC,"back",*gateC);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*gateC,"back",*monoBox);

  monoBox->splitObject(System,2001,outerCell,
		       Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  cellIndex++;

  
  monoXtal->addInsertCell(monoBox->getCell("Void"));
  monoXtal->createAll(System,*monoBox,0);

  constructSystem::constructUnit
    (System,buildZone,*monoBox,"back",*gateD);

  constructSystem::constructUnit
    (System,buildZone,*gateD,"back",*bellowD);

  outerCell=constructSystem::constructUnit
    (System,buildZone,*bellowD,"back",*diagBoxA);
  
  diagBoxA->splitVoidPorts(System,"SplitOuter",2001,
     			   diagBoxA->getCell("Void"),
   			   {0,1, 1,2});
  diagBoxA->splitObject(System,-11,outerCell);
  diagBoxA->splitObject(System,12,outerCell);
  diagBoxA->splitObject(System,-2001,outerCell);
  diagBoxA->splitObject(System,-2002,outerCell);
  diagBoxA->intersectVoidPorts(System,6,3);
  cellIndex+=4;

  
  bremMonoCollA->addInsertCell("Flange",diagBoxA->getCell("Void",0));
  bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",0));
  bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",1));
  bremMonoCollA->setCutSurf("front",diagBoxA->getSurf("VoidFront"));
  bremMonoCollA->setCutSurf("wallRadius",diagBoxA->getSurf("VoidCyl"));
  bremMonoCollA->setInOrg(monoXtal->getLinkPt(2));
  bremMonoCollA->createAll(System,*diagBoxA,0);

  constructSystem::constructUnit
    (System,buildZone,*diagBoxA,"back",*bellowE);

  constructSystem::constructUnit
    (System,buildZone,*bellowE,"back",*gateE);

  constructSystem::constructUnit
    (System,buildZone,*gateE,"back",*mirrorBoxA);
  mirrorBoxA->splitObject(System,3001,mirrorBoxA->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));


  mirrorFrontA->addInsertCell(mirrorBoxA->getCell("Void",0));
  mirrorFrontA->createAll(System,*mirrorBoxA,0);

  mirrorBackA->addInsertCell(mirrorBoxA->getCell("Void",1));
  mirrorBackA->createAll(System,*mirrorBoxA,0);

  constructSystem::constructUnit
    (System,buildZone,*mirrorBoxA,"back",*gateF);

  constructSystem::constructUnit
    (System,buildZone,*gateF,"back",*bellowF);

  constructDiag(System,*diagBoxB,jawCompB,*bellowF,"back");

  constructSystem::constructUnit
    (System,buildZone,*diagBoxB,"back",*bellowG);

  constructSystem::constructUnit
    (System,buildZone,*bellowG,"back",*gateG);


  constructSystem::constructUnit
    (System,buildZone,*gateG,"back",*mirrorBoxB);

  mirrorBoxB->splitObject(System,3001,mirrorBoxB->getCell("Void"),
			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));


  mirrorFrontB->addInsertCell(mirrorBoxB->getCell("Void",0));
  mirrorFrontB->createAll(System,*mirrorBoxB,0);
  mirrorBackB->addInsertCell(mirrorBoxB->getCell("Void",1));
  mirrorBackB->createAll(System,*mirrorBoxB,0);

  constructSystem::constructUnit
    (System,buildZone,*mirrorBoxB,"back",*gateH);

  constructSystem::constructUnit
    (System,buildZone,*gateH,"back",*bellowH);

  constructDiag(System,*diagBoxC,jawCompC,*bellowH,"back");

  constructSystem::constructUnit
    (System,buildZone,*diagBoxC,"back",*bellowI);

  constructMonoShutter(System,*bellowI,"back");


  buildZone.createUnit(System);
  buildZone.rebuildInsertCells(System);

  setCells("InnerVoid",buildZone.getCells("Unit"));
  setCell("LastVoid",buildZone.getCells("Unit").back());
  lastComp=gateJ;
  return;
}

void
cosaxsOpticsLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("cosaxsOpticsLine","createLinks");

  setLinkCopy(0,*pipeInit,1);
  setLinkCopy(1,*lastComp,2);
  return;
}


void
cosaxsOpticsLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("cosaxsOpticsLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem
