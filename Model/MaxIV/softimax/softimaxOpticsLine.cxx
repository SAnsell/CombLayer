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
// #include <iomanip>
// #include <iostream>
#include <sstream>
// #include <cmath>
// #include <complex>
// #include <list>
#include <vector>
#include <set>
#include <map>
// #include <string>
// #include <algorithm>
// #include <iterator>
#include <memory>

// #include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
// #include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
// #include "MatrixBase.h"
// #include "Matrix.h"
#include "Vec3D.h"
// #include "inputParam.h"
// #include "Surface.h"
// #include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
// #include "Rules.h"
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
// #include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
// #include "World.h"
// #include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "generalConstruct.h"

// #include "insertObject.h"
// #include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "FlangeMount.h"
// #include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "BlockStand.h"
#include "CrossPipe.h"
#include "BremColl.h"
// #include "BremMonoColl.h"
// #include "MonoVessel.h"
// #include "MonoCrystals.h"
#include "GateValveCube.h"
// #include "JawUnit.h"
#include "JawFlange.h"
// #include "FlangeMount.h"
#include "TankMonoVessel.h"
#include "GratingUnit.h"
#include "Mirror.h"
#include "BeamPair.h"
#include "TwinPipe.h"
// #include "MonoBox.h"
// #include "MonoShutter.h"
// #include "DiffPumpXIADP03.h"
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
  pumpM1(new constructSystem::PipeTube(newName+"PumpM1")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  M1Stand(new xraySystem::BlockStand(newName+"M1Stand")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pumpTubeA(new constructSystem::PipeTube(newName+"PumpTubeA")),
  bremCollA(new xraySystem::BremColl(newName+"BremCollA")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  slitTube(new constructSystem::PortTube(newName+"SlitTube")),
  jaws({
	std::make_shared<xraySystem::BeamPair>(newName+"JawX"),
	std::make_shared<xraySystem::BeamPair>(newName+"JawZ")
    }),
  monoVessel(new xraySystem::TankMonoVessel(newName+"MonoVessel")),
  grating(new xraySystem::GratingUnit(newName+"Grating")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  pumpTubeB(new constructSystem::PipeTube(newName+"PumpTubeB")),
  gateD(new constructSystem::GateValveCube(newName+"GateD")),
  joinPipeA(new constructSystem::VacuumPipe(newName+"JoinPipeA")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  pumpTubeM3(new constructSystem::PipeTube(newName+"PumpTubeM3")),
  pumpTubeM3Baffle(new xraySystem::FlangeMount(newName+"PumpTubeM3Baffle")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  M3Mirror(new xraySystem::Mirror(newName+"M3Mirror")),
  M3Stand(new xraySystem::BlockStand(newName+"M3Stand")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  gateE(new constructSystem::GateValveCube(newName+"GateE")),
  pumpTubeC(new constructSystem::PipeTube(newName+"PumpTubeC")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  vacPiece(new constructSystem::PipeTube(newName+"VacPiece")),
  gateF(new constructSystem::GateValveCube(newName+"GateF")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  M3STXMTube(new constructSystem::PipeTube(newName+"M3STXMTube")),
  offPipeD(new constructSystem::OffsetFlangePipe(newName+"OffPipeD")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter"))



  // filterBoxA(new constructSystem::PortTube(newName+"FilterBoxA")),
  // filterStick(new xraySystem::FlangeMount(newName+"FilterStick")),
  // screenPipeA(new constructSystem::PipeTube(newName+"ScreenPipeA")),
  // screenPipeB(new constructSystem::PipeTube(newName+"ScreenPipeB")),
  // diffPumpA(new constructSystem::DiffPumpXIADP03(newName+"DiffPumpA")),
  // primeJawBox(new constructSystem::VacuumBox(newName+"PrimeJawBox")),
  // gateC(new constructSystem::GateValveCube(newName+"GateC")),
  // monoBox(new xraySystem::MonoBox(newName+"MonoBox")),
  // monoXtal(new xraySystem::MonoCrystals(newName+"MonoXtal")),
  // diagBoxA(new constructSystem::PortTube(newName+"DiagBoxA")),
  // bremMonoCollA(new xraySystem::BremMonoColl(newName+"BremMonoCollA")),
  // bellowE(new constructSystem::Bellows(newName+"BellowE")),
  // mirrorBoxA(new constructSystem::VacuumBox(newName+"MirrorBoxA")),
  // mirrorFrontA(new xraySystem::Mirror(newName+"MirrorFrontA")),
  // mirrorBackA(new xraySystem::Mirror(newName+"MirrorBackA")),
  // diagBoxB(new constructSystem::PortTube(newName+"DiagBoxB")),
  // jawCompB({
  //     std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit0"),
  //     std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxBJawUnit1")
  // 	}),

  // gateG(new constructSystem::GateValveCube(newName+"GateG")),
  // mirrorBoxB(new constructSystem::VacuumBox(newName+"MirrorBoxB")),
  // mirrorFrontB(new xraySystem::Mirror(newName+"MirrorFrontB")),
  // mirrorBackB(new xraySystem::Mirror(newName+"MirrorBackB")),
  // gateH(new constructSystem::GateValveCube(newName+"GateH")),
  // bellowH(new constructSystem::Bellows(newName+"BellowH")),
  // diagBoxC(new constructSystem::PortTube(newName+"DiagBoxC")),
  // jawCompC({
  //     std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit0"),
  //     std::make_shared<constructSystem::JawFlange>(newName+"DiagBoxCJawUnit1")
  // 	}),
  // gateI(new constructSystem::GateValveCube(newName+"GateI")),
  // monoShutter(new xraySystem::MonoShutter(newName+"MonoShutter")),

  // gateJ(new constructSystem::GateValveCube(newName+"GateJ"))
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
  OR.addObject(pumpM1);
  OR.addObject(gateA);
  OR.addObject(bellowB);
  OR.addObject(M1Tube);
  OR.addObject(M1Mirror);
  OR.addObject(M1Stand);
  OR.addObject(bellowC);
  OR.addObject(pumpTubeA);
  OR.addObject(bremCollA);
  OR.addObject(gateB);
  OR.addObject(bellowD);
  OR.addObject(slitTube);
  OR.addObject(jaws[0]);
  OR.addObject(jaws[1]);
  OR.addObject(monoVessel);
  OR.addObject(gateC);
  OR.addObject(bellowE);
  OR.addObject(pumpTubeB);
  OR.addObject(gateD);
  OR.addObject(joinPipeA);
  OR.addObject(bellowF);
  OR.addObject(pumpTubeM3);
  OR.addObject(pumpTubeM3Baffle);
  OR.addObject(bellowG);
  OR.addObject(M3Tube);
  OR.addObject(M3Mirror);
  OR.addObject(M3Stand);
  OR.addObject(bellowH);
  OR.addObject(gateE);
  OR.addObject(pumpTubeC);
  OR.addObject(vacPiece);
  OR.addObject(gateF);
  OR.addObject(bellowJ);
  OR.addObject(M3STXMTube);
  OR.addObject(offPipeD);
  OR.addObject(splitter);


  // OR.addObject(filterBoxA);
  // OR.addObject(filterStick);
  // OR.addObject(screenPipeA);
  // OR.addObject(screenPipeB);
  // OR.addObject(diffPumpA);
  // OR.addObject(primeJawBox);
  // OR.addObject(monoBox);
  // OR.addObject(diagBoxA);
  // OR.addObject(bremMonoCollA);
  // OR.addObject(mirrorBoxA);
  // OR.addObject(mirrorFrontA);
  // OR.addObject(mirrorBackA);
  // OR.addObject(diagBoxB);
  // OR.addObject(gateG);
  // OR.addObject(mirrorBoxB);
  // OR.addObject(mirrorFrontB);
  // OR.addObject(mirrorBackB);
  // OR.addObject(gateH);
  // OR.addObject(bellowH);
  // OR.addObject(diagBoxC);
  // OR.addObject(gateI);
  // OR.addObject(monoShutter);
  // OR.addObject(gateJ);
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

  int outerCell(0);

  // gateI->setFront(FC,linkPt);
  // gateI->createAll(System,FC,linkPt);
  // outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*gateI,2);
  // gateI->insertInCell(System,outerCell);

  // monoShutter->addAllInsertCell((*masterCellPtr)->getName());
  // monoShutter->setCutSurf("front",*gateI,2);
  // monoShutter->createAll(System,*gateI,2);
  // outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*monoShutter,2);

  // monoShutter->insertAllInCell(System,outerCell);
  // monoShutter->splitObject(System,"-PortACut",outerCell);
  // const Geometry::Vec3D midPoint(monoShutter->getLinkPt(3));
  // const Geometry::Vec3D midAxis(monoShutter->getLinkAxis(-3));
  // monoShutter->splitObjectAbsolute(System,2001,outerCell,midPoint,midAxis);
  // monoShutter->splitObject(System,"PortBCut",outerCell);
  // cellIndex+=3;


  // gateJ->setFront(*bellowJ,2);
  // gateJ->createAll(System,*bellowJ,2);
  // outerCell=buildZone.createOuterVoidUnit(System,*masterCellPtr,*gateJ,2);
  // gateJ->insertInCell(System,outerCell);

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
softimaxOpticsLine::buildM1Mirror(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC,
				     const long int sideIndex)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM1Mirror");

  int outerCell;

  M1Tube->setFront(initFC,sideIndex);
  M1Tube->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*M1Tube,2);
  M1Tube->insertAllInCell(System,outerCell);

  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);

  M1Stand->setCutSurf("floor",this->getRule("floor"));
  M1Stand->setCutSurf("front",*M1Tube,-1);
  M1Stand->setCutSurf("back",*M1Tube,-2);
  M1Stand->addInsertCell(outerCell);
  M1Stand->createAll(System,*M1Tube,0);

  // gateA->createAll(System,*offPipeB,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  // gateA->insertInCell(System,outerCell);
  // gateA->setCell("OuterVoid",outerCell);

  // pipeC->createAll(System,*gateA,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  // pipeC->insertInCell(System,outerCell);

  // screenA->addAllInsertCell(outerCell);
  // screenA->setCutSurf("inner",*pipeC,"pipeOuterTop");
  // screenA->createAll(System,*pipeC,0);
  // screenA->insertInCell("Wings",System,gateA->getCell("OuterVoid"));
  // screenA->insertInCell("Wings",System,offPipeB->getCell("OuterVoid"));

  return;
}

void
softimaxOpticsLine::buildM3Mirror(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC,
				     const long int sideIndex)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM3Mirror");

  int outerCell;

  M3Tube->setFront(initFC,sideIndex);
  M3Tube->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*M3Tube,2);
  M3Tube->insertAllInCell(System,outerCell);

  M3Mirror->addInsertCell(M3Tube->getCell("Void"));
  M3Mirror->createAll(System,*M3Tube,0);

  M3Stand->setCutSurf("floor",this->getRule("floor"));
  M3Stand->setCutSurf("front",*M3Tube,-1);
  M3Stand->setCutSurf("back",*M3Tube,-2);
  M3Stand->addInsertCell(outerCell);
  M3Stand->createAll(System,*M3Tube,0);

  // gateA->createAll(System,*offPipeB,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  // gateA->insertInCell(System,outerCell);
  // gateA->setCell("OuterVoid",outerCell);

  // pipeC->createAll(System,*gateA,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  // pipeC->insertInCell(System,outerCell);

  // screenA->addAllInsertCell(outerCell);
  // screenA->setCutSurf("inner",*pipeC,"pipeOuterTop");
  // screenA->createAll(System,*pipeC,0);
  // screenA->insertInCell("Wings",System,gateA->getCell("OuterVoid"));
  // screenA->insertInCell("Wings",System,offPipeB->getCell("OuterVoid"));

  return;
}

void
softimaxOpticsLine::buildM3STXMMirror(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC,
				     const long int sideIndex)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildM3STXMMirror");

  int outerCell;

  M3STXMTube->setFront(initFC,sideIndex);
  M3STXMTube->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*M3STXMTube,2);
  M3STXMTube->insertAllInCell(System,outerCell);

  return;
}

void
softimaxOpticsLine::constructSlitTube(Simulation& System,
				      MonteCarlo::Object* masterCell,
				      const attachSystem::FixedComp& initFC,
				      const std::string& sideName)
  /*!
    Build the Baffle before Monochromator
    \param System :: Simuation to use
   */
{
  ELog::RegMethod RegA("softimaxOpticsLine","buildSlitTube");

  int outerCell;

  // FAKE insertcell: required
  slitTube->addAllInsertCell(masterCell->getName());
  slitTube->createAll(System,initFC,sideName);
  slitTube->intersectPorts(System,0,1);
  slitTube->intersectPorts(System,1,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*slitTube,2);
  slitTube->insertAllInCell(System,outerCell);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
			   slitTube->getCell("Void"),
			   Geometry::Vec3D(0,1,0));


  slitTube->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  cellIndex++;  // remember creates an extra cell in  primary


  for(size_t i=0;i<jaws.size();i++)
    {
      const constructSystem::portItem& PI=slitTube->getPort(i);
      jaws[i]->addInsertCell("SupportA",PI.getCell("Void"));
      jaws[i]->addInsertCell("SupportA",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("SupportB",PI.getCell("Void"));
      jaws[i]->addInsertCell("SupportB",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("BlockA",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("BlockB",slitTube->getCell("SplitVoid",i));
      jaws[i]->createAll(System,*slitTube,0,
			 PI,PI.getSideIndex("InnerPlate"));
    }

  return;
}

void
softimaxOpticsLine::buildMono(Simulation& System,
				 MonteCarlo::Object* masterCell,
				 const attachSystem::FixedComp& initFC,
				 const long int sideIndex)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("softimaxOpticsBeamline","buildMono");

  int outerCell;

  // offPipeA->createAll(System,initFC,sideIndex);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeA,2);
  // offPipeA->insertInCell(System,outerCell);

  // FAKE insertcell: required
  monoVessel->addInsertCell(masterCell->getName());
  monoVessel->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoVessel,2);
  monoVessel->insertInCell(System,outerCell);

  grating->addInsertCell(monoVessel->getCell("Void"));
  grating->copyCutSurf("innerCylinder",*monoVessel,"innerRadius");
  grating->createAll(System,*monoVessel,0);

  // offPipeB->createAll(System,*monoVessel,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeB,2);
  // offPipeB->insertInCell(System,outerCell);

  return;
}

void
softimaxOpticsLine::buildSplitter(Simulation& System,
				     MonteCarlo::Object* masterCellA,
				     MonteCarlo::Object* masterCellB,
				     const attachSystem::FixedComp& initFC,
				     const long int sideIndex)
  /*!
    Sub build of the spliter package
    \param System :: Simulation to use
    \param masterCellA :: Current master cell
    \param masterCellB :: Secondary master cell
    \param initFC :: Start point
    \param sideIndex :: start link point
  */

{
  ELog::RegMethod RegA("maxpeemOpticsBeamLine","buildSplitter");

  int cellA(0),cellB(0);

  offPipeD->createAll(System,initFC,sideIndex);
  cellA=buildZone.createOuterVoidUnit(System,masterCellA,*offPipeD,2);
  offPipeD->insertInCell(System,cellA);


  //////////////////// 1: build splitter without creating two outer void units
  splitter->createAll(System,*offPipeD,2);
  cellA=buildZone.createOuterVoidUnit(System,masterCellA,*splitter,2);
  splitter->insertInCell("Flange",System,cellA);
  splitter->insertInCell("PipeA",System,cellA);
  splitter->insertInCell("Flange",System,cellA);
  splitter->insertInCell("PipeB",System,cellA);


  //////////////////// 2: build splitter with creating two outer void units
  ////////////////////////////////////////////////////////////////////////////////////
  // buildZone.constructMiddleSurface(SMap,buildIndex+10,*offPipeD,2);

  // attachSystem::InnerZone leftZone=buildZone.buildMiddleZone(-1);
  // attachSystem::InnerZone rightZone=buildZone.buildMiddleZone(1);

  // // No need for insert -- note removal of old master cell
  // System.removeCell(masterCellA->getName());

  // masterCellA=leftZone.constructMasterCell(System);
  // masterCellB=rightZone.constructMasterCell(System);
  // splitter->createAll(System,*offPipeD,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*splitter,2);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*splitter,3);

  // splitter->insertInCell("Flange",System,cellA);
  // splitter->insertInCell("PipeA",System,cellA);

  // splitter->insertInCell("Flange",System,cellB);
  // splitter->insertInCell("PipeB",System,cellB);
  ////////////////////////////////////////////////////////////////////////////////////





  // // now build left/ right
  // // LEFT
  // bellowAA->createAll(System,*splitter,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*bellowAA,2);
  // bellowAA->insertInCell(System,cellA);

  // gateAA->createAll(System,*bellowAA,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*gateAA,2);
  // gateAA->insertInCell(System,cellA);

  // // make build necessary
  // pumpTubeAA->addAllInsertCell(masterCellA->getName());
  // pumpTubeAA->createAll(System,*gateAA,2);
  // cellA=leftZone.createOuterVoidUnit(System,masterCellA,*pumpTubeAA,2);
  // pumpTubeAA->insertAllInCell(System,cellA);


  // // RIGHT
  // bellowBA->createAll(System,*splitter,3);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBA,2);
  // bellowBA->insertInCell(System,cellB);

  // gateBA->createAll(System,*bellowBA,2);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*gateBA,2);
  // gateBA->insertInCell(System,cellB);

  // pumpTubeBA->addAllInsertCell(masterCellB->getName());
  // pumpTubeBA->createAll(System,*gateBA,2);
  // cellB=rightZone.createOuterVoidUnit(System,masterCellB,*pumpTubeBA,2);
  // pumpTubeBA->insertAllInCell(System,cellB);

  //   // Get last two cells
  // setCell("LeftVoid",masterCellA->getName());
  // setCell("RightVoid",masterCellB->getName());

  return;
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

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gaugeA,"back",*bellowA);

  // FAKE insertcell: required
  pumpM1->addAllInsertCell(masterCell->getName());
  pumpM1->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpM1->createAll(System,*bellowA,"back");
  pumpM1->intersectPorts(System,1,2);

  ///////////// split for FLUKA
  const constructSystem::portItem& VP0=pumpM1->getPort(0);
  const constructSystem::portItem& VP1=pumpM1->getPort(1);
  const constructSystem::portItem& VP2=pumpM1->getPort(2);
  //  const constructSystem::portItem& VP3=pumpM1->getPort(3);
  const constructSystem::portItem& VP4=pumpM1->getPort(4);
  //  const constructSystem::portItem& VP5=pumpM1->getPort(5);
  const constructSystem::portItem& VP6=pumpM1->getPort(6);

  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,VP1,VP1.getSideIndex("OuterPlate"));
  const Geometry::Vec3D  Axis12=pumpM1->getY()*(VP1.getY()+VP2.getY())/2.0;
  const Geometry::Vec3D  Axis26=pumpM1->getY()*(VP2.getY()+VP6.getY())/2.0;

  this->splitObjectAbsolute(System,1501,outerCell,
			    (VP1.getCentre()+VP4.getCentre())/2.0,
  			    Z);
  this->splitObjectAbsolute(System,1502,outerCell+1,
    			    pumpM1->getCentre(),
			    VP4.getY());
  this->splitObjectAbsolute(System,1503,outerCell,
			    pumpM1->getCentre(),Axis12);
  this->splitObjectAbsolute(System,1504,outerCell+3,
   			    pumpM1->getCentre(),Axis26);

  const std::vector<int> cellUnit=this->getCells("OuterVoid");
  pumpM1->insertMainInCell(System,cellUnit);
  //  VP0.insertInCell(System,this->getCell("OuterVoid"));

  pumpM1->insertPortInCell
    (System,{{outerCell+4},{outerCell},{outerCell+3},{outerCell},
	     {outerCell+2},{outerCell+1},
	     {outerCell+4}});

  cellIndex+=5;
  /////////////////////////////////////////

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,VP1,"OuterPlate",*gateA);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gateA,"back",*bellowB);

  buildM1Mirror(System,masterCell,*bellowB,2);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*M1Tube,"back",*bellowC);

  // FAKE insertcell: required
  pumpTubeA->addAllInsertCell(masterCell->getName());
  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*bellowC,2);

  const constructSystem::portItem& CPI1=pumpTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,CPI1,CPI1.getSideIndex("OuterPlate"));
  pumpTubeA->insertAllInCell(System,outerCell);
  //  pumpTubeA->intersectPorts(System,1,2);

  bremCollA->setCutSurf("front",CPI1,CPI1.getSideIndex("OuterPlate"));
  bremCollA->createAll(System,CPI1,CPI1.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bremCollA,2);
  bremCollA->insertInCell("Main",System,outerCell);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*bremCollA,"back",*gateB);

  bremCollA->createExtension(System,gateB->getCell("Void")); // !!! UGLY - it does not actually intersect gateB

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gateB,"back",*bellowD);

  constructSlitTube(System,masterCell,*bellowD,"back");

  buildMono(System,masterCell,*slitTube,2);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*monoVessel,"back",*gateC);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gateC,"back",*bellowE);

  // FAKE insertcell: required
  pumpTubeB->addAllInsertCell(masterCell->getName());
  pumpTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeB->createAll(System,*bellowE,2);

  const constructSystem::portItem& pumpTubeBCPI=pumpTubeB->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,pumpTubeBCPI,pumpTubeBCPI.getSideIndex("OuterPlate"));
  pumpTubeB->insertAllInCell(System,outerCell);
  //  pumpTubeB->intersectPorts(System,1,2);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,pumpTubeBCPI,"OuterPlate",*gateD);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gateD,"back",*joinPipeA);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*joinPipeA,"back",*bellowF);

  /////////////////// M3 Pump and baffle
  // FAKE insertcell: required
  pumpTubeM3->addAllInsertCell(masterCell->getName());
  pumpTubeM3->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeM3->createAll(System,*bellowF,"back");

  const constructSystem::portItem& GPI=pumpTubeM3->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,GPI,GPI.getSideIndex("OuterPlate"));
  pumpTubeM3->insertAllInCell(System,outerCell);

  pumpTubeM3Baffle->addInsertCell("Body",pumpTubeM3->getCell("Void"));
  pumpTubeM3Baffle->setBladeCentre(*pumpTubeM3,0);
  pumpTubeM3Baffle->createAll(System,*pumpTubeM3,std::string("InnerBack"));

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,GPI,"OuterPlate",*bellowG);

  buildM3Mirror(System,masterCell,*bellowG,2);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*M3Tube,"back",*bellowH);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*bellowH,"back",*gateE);


  //// pumpTubeC
  // FAKE insertcell: required
  pumpTubeC->addAllInsertCell(masterCell->getName());
  pumpTubeC->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeC->createAll(System,*gateE,2);

  const constructSystem::portItem& pumpTubeCCPI=pumpTubeC->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,pumpTubeCCPI,pumpTubeCCPI.getSideIndex("OuterPlate"));
  pumpTubeC->insertAllInCell(System,outerCell);
  //  pumpTubeC->intersectPorts(System,1,2);
  ///////////////////////////////////////////////

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,pumpTubeCCPI,"back",*bellowI);

  vacPiece->setFront(*bellowI,2);
  vacPiece->createAll(System,*bellowI,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*vacPiece,2);
  vacPiece->insertAllInCell(System,outerCell);
  // TODO: since PipeTube has insertAllInCell but other classes have
  // insertInCell I can't use constructUnit with PipeTube.
  // why dont' fix method names for PipeTube? so we could use next 2 lines:
  // xrayConstruct::constructUnit
  //   (System,buildZone,masterCell,*bellowI,"back",*);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*vacPiece,"back",*gateF);

  xrayConstruct::constructUnit
    (System,buildZone,masterCell,*gateF,"back",*bellowJ);

  buildM3STXMMirror(System,masterCell,*bellowJ,2);

  MonteCarlo::Object* masterCellB(0);
  buildSplitter(System,masterCell,masterCellB,*M3STXMTube,2);



  // filterBoxA->addAllInsertCell(masterCell->getName());
  // filterBoxA->setFront(*bremCollA,2);
  // filterBoxA->createAll(System,*bremCollA,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*filterBoxA,2);
  // filterBoxA->insertAllInCell(System,outerCell);
  // filterBoxA->splitObject(System,1001,outerCell,
  // 			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  // cellIndex++;
  // bremCollA->createExtension(System,filterBoxA->getCell("FrontPortVoid"));

  // const constructSystem::portItem& PI=filterBoxA->getPort(3);
  // filterStick->addInsertCell("Body",PI.getCell("Void"));
  // filterStick->addInsertCell("Body",filterBoxA->getCell("Void"));
  // filterStick->setBladeCentre(PI,0);
  // filterStick->createAll(System,PI,PI.getSideIndex("-InnerPlate"));



  // // fake insert
  // screenPipeA->addAllInsertCell(masterCell->getName());
  // screenPipeA->setFront(*gateB,2);
  // screenPipeA->createAll(System,*gateB,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeA,2);
  // screenPipeA->insertAllInCell(System,outerCell);

  // // fake insert
  // screenPipeB->addAllInsertCell(masterCell->getName());
  // screenPipeB->setFront(*screenPipeA,2);
  // screenPipeB->createAll(System,*screenPipeA,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*screenPipeB,2);
  // screenPipeB->insertAllInCell(System,outerCell);
  // screenPipeB->intersectPorts(System,0,1);


  // diffPumpA->setCutSurf("front",*adaptorPlateA,2);
  // diffPumpA->createAll(System,*adaptorPlateA,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diffPumpA,2);
  // diffPumpA->insertInCell(System,outerCell);


  // // primeJawBox->setFront(*screenPipeB,2);
  // // primeJawBox->createAll(System,*screenPipeB,2);
  // // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*primeJawBox,2);
  // // primeJawBox->insertInCell(System,outerCell);



  // // fake insert
  // monoBox->addInsertCell(masterCell->getName());
  // monoBox->setFront(*gateC,2);
  // monoBox->createAll(System,*gateC,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoBox,2);
  // monoBox->insertInCell(System,outerCell);
  // monoBox->splitObject(System,2001,outerCell,
  // 		       Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));
  // cellIndex++;

  // monoXtal->addInsertCell(monoBox->getCell("Void"));
  // monoXtal->createAll(System,*monoBox,0);


  // // fake insert
  // diagBoxA->addAllInsertCell(masterCell->getName());
  // diagBoxA->setFront(*bellowD,2);
  // diagBoxA->createAll(System,*bellowD,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*diagBoxA,2);
  // diagBoxA->insertAllInCell(System,outerCell);


  // diagBoxA->splitVoidPorts(System,"SplitOuter",2001,
  //    			   diagBoxA->getCell("Void"),
  //  			   {0,1, 1,2});
  // diagBoxA->splitObject(System,-11,outerCell);
  // diagBoxA->splitObject(System,12,outerCell);
  // diagBoxA->splitObject(System,-2001,outerCell);
  // diagBoxA->splitObject(System,-2002,outerCell);

  // //  diagBoxA->intersectPorts(System,3,6);
  // diagBoxA->intersectVoidPorts(System,6,3);
  // cellIndex+=4;

  // bremMonoCollA->addInsertCell("Flange",diagBoxA->getCell("Void",0));
  // bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",0));
  // bremMonoCollA->addInsertCell("Main",diagBoxA->getCell("Void",1));
  // bremMonoCollA->setCutSurf("front",diagBoxA->getSurf("VoidFront"));
  // bremMonoCollA->setCutSurf("wallRadius",diagBoxA->getSurf("VoidCyl"));
  // bremMonoCollA->setInOrg(monoXtal->getLinkPt(2));
  // bremMonoCollA->createAll(System,*diagBoxA,0);

  // // ELog::EM<<"Early return here"<<ELog::endDiag;
  // // setCell("LastVoid",masterCell->getName());
  // // lastComp=gateJ;
  // // return;




  // mirrorBoxA->setFront(*gateE,2);
  // mirrorBoxA->createAll(System,*gateE,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorBoxA,2);
  // mirrorBoxA->insertInCell(System,outerCell);

  // mirrorBoxA->splitObject(System,3001,mirrorBoxA->getCell("Void"),
  // 			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));

  // mirrorFrontA->addInsertCell(mirrorBoxA->getCell("Void",0));
  // mirrorFrontA->createAll(System,*mirrorBoxA,0);

  // mirrorBackA->addInsertCell(mirrorBoxA->getCell("Void",1));
  // mirrorBackA->createAll(System,*mirrorBoxA,0);

  // constructDiag(System,&masterCell,*diagBoxB,jawCompB,*bellowF,2);

  // gateG->setFront(*bellowG,2);
  // gateG->createAll(System,*bellowG,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateG,2);
  // gateG->insertInCell(System,outerCell);

  // mirrorBoxB->setFront(*gateG,2);
  // mirrorBoxB->createAll(System,*gateG,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorBoxB,2);
  // mirrorBoxB->insertInCell(System,outerCell);

  // mirrorBoxB->splitObject(System,3001,mirrorBoxB->getCell("Void"),
  // 			  Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0));


  // mirrorFrontB->addInsertCell(mirrorBoxB->getCell("Void",0));
  // mirrorFrontB->createAll(System,*mirrorBoxB,0);
  // mirrorBackB->addInsertCell(mirrorBoxB->getCell("Void",1));
  // mirrorBackB->createAll(System,*mirrorBoxB,0);

  // gateH->setFront(*mirrorBoxB,2);
  // gateH->createAll(System,*mirrorBoxB,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateH,2);
  // gateH->insertInCell(System,outerCell);

  // bellowH->setFront(*gateH,2);
  // bellowH->createAll(System,*gateH,2);
  // outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowH,2);
  // bellowH->insertInCell(System,outerCell);

  // constructDiag(System,&masterCell,*diagBoxC,jawCompC,*bellowH,2);

  // constructMonoShutter(System,&masterCell,*bellowI,2);

  setCell("LastVoid",masterCell->getName());
  lastComp=bellowA; //gateJ;
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
