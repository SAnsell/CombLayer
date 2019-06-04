/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/maxpeemOpticsBeamline.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "OffsetFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "PipeShield.h"

#include "BlockStand.h"
#include "CrossPipe.h"
#include "GateValveCube.h"
#include "JawUnit.h"
#include "JawValveCube.h"
#include "BeamMount.h"
#include "GrateMonoBox.h"
#include "GratingMono.h"
#include "TwinPipe.h"
#include "Mirror.h"
#include "maxpeemOpticsBeamline.h"

namespace xraySystem
{

// Note currently uncopied:
  
maxpeemOpticsBeamline::maxpeemOpticsBeamline(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  gateRing(new constructSystem::GateValveCube(newName+"GateRing")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  screenExtra(new xraySystem::PipeShield(newName+"ScreenExtra")),
  pumpTubeA(new constructSystem::PipeTube(newName+"PumpTubeA")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  M1Stand(new xraySystem::BlockStand(newName+"M1Stand")),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  gateA(new constructSystem::GateValveCube(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusZ"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusZ")}),  
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  gateB(new constructSystem::GateValveCube(newName+"GateB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  monoB(new xraySystem::GrateMonoBox(newName+"MonoBox")),
  monoXtal(new xraySystem::GratingMono(newName+"MonoXtal")),
  pipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  gateC(new constructSystem::GateValveCube(newName+"GateC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  slitsB(new constructSystem::JawValveCube(newName+"SlitsB")),
  pumpTubeB(new constructSystem::PipeTube(newName+"PumpTubeB")),
  offPipeC(new constructSystem::OffsetFlangePipe(newName+"OffPipeC")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  M3Mirror(new xraySystem::Mirror(newName+"M3Mirror")),
  offPipeD(new constructSystem::OffsetFlangePipe(newName+"OffPipeD")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),
  screenB(new xraySystem::PipeShield(newName+"ScreenB")),
  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  gateAA(new constructSystem::GateValveCube(newName+"GateAA")),
  pumpTubeAA(new constructSystem::PortTube(newName+"PumpTubeAA")),
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  gateBA(new constructSystem::GateValveCube(newName+"GateBA")),
  pumpTubeBA(new constructSystem::PortTube(newName+"PumpTubeBA")),
  outPipeA(new constructSystem::VacuumPipe(newName+"OutPipeA")),
  outPipeB(new constructSystem::VacuumPipe(newName+"OutPipeB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(const auto& JM : jaws)
    OR.addObject(JM);

  OR.addObject(bellowA);
  OR.addObject(ionPA);
  OR.addObject(gateRing);
  OR.addObject(gateTubeA);
  OR.addObject(bellowB);
  OR.addObject(pipeA);
  OR.addObject(florTubeA);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(screenExtra);
  OR.addObject(pumpTubeA);
  OR.addObject(offPipeA);
  OR.addObject(M1Tube);
  OR.addObject(M1Mirror);
  OR.addObject(M1Stand);
  OR.addObject(offPipeB);
  OR.addObject(gateA);
  OR.addObject(pipeC);
  OR.addObject(screenA);
  OR.addObject(pipeD);
  OR.addObject(slitTube);
  OR.addObject(pipeE);
  OR.addObject(gateB);
  OR.addObject(bellowD);
  OR.addObject(pipeF);
  OR.addObject(monoB);
  OR.addObject(monoXtal);
  OR.addObject(pipeG);
  OR.addObject(gateC);
  OR.addObject(bellowE);
  OR.addObject(viewTube);
  OR.addObject(slitsB);
  OR.addObject(pumpTubeB);
  OR.addObject(offPipeC);
  OR.addObject(M3Tube);
  OR.addObject(M3Mirror);
  OR.addObject(offPipeD);
  OR.addObject(splitter);
  OR.addObject(bellowAA);
  OR.addObject(bellowBA);
  OR.addObject(gateAA);
  OR.addObject(gateBA);
  OR.addObject(pumpTubeAA);
  OR.addObject(pumpTubeBA);
  OR.addObject(outPipeA);
  OR.addObject(outPipeB);
}
  
maxpeemOpticsBeamline::~maxpeemOpticsBeamline()
  /*!
    Destructor
   */
{}

void
maxpeemOpticsBeamline::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","populate");
  FixedOffset::populate(Control);

  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  return;
}

void
maxpeemOpticsBeamline::createUnitVector(const attachSystem::FixedComp& FC,
					const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
maxpeemOpticsBeamline::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    {

      if (isActive("floor"))
	{
	  std::string Out;
	  ModelSupport::buildPlane
	    (SMap,buildIndex+3,Origin-X*outerRadius,X);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+4,Origin+X*outerRadius,X);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+6,Origin+Z*outerRadius,Z);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
	  const HeadRule HR(Out+getRuleStr("floor"));
	  buildZone.setSurround(HR);
	}
      else
	{
	  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
	  buildZone.setSurround(HeadRule(-SMap.realSurf(buildIndex+7)));
	}
    }

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*180.0,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  return;
}



void
maxpeemOpticsBeamline::insertFlanges(Simulation& System,
				     const constructSystem::PipeTube& PT)
  /*!
    Boilerplate function to insert the flanges from pipetubes
    that extend past the linkzone in to ther neighbouring regions.
    \param System :: Simulation to use
    \param PT :: PipeTube
   */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","insertFlanges");
  
  const size_t voidN=this->getNItems("OuterVoid")-3;

  // inserting into the outerVoid +1 / -1 
  this->insertComponent(System,"OuterVoid",voidN,PT.getCC("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN,PT.getCC("FlangeB"));
  this->insertComponent(System,"OuterVoid",voidN+2,PT.getCC("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN+2,PT.getCC("FlangeB"));
  return;
}

void
maxpeemOpticsBeamline::buildSplitter(Simulation& System,
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

  buildZone.constructMiddleSurface(SMap,buildIndex+10,*offPipeD,2);

  
  attachSystem::InnerZone leftZone=buildZone.buildMiddleZone(-1);
  attachSystem::InnerZone rightZone=buildZone.buildMiddleZone(1);

  // No need for insert -- note removal of old master cell
  System.removeCell(masterCellA->getName());
  
  masterCellA=leftZone.constructMasterCell(System);
  masterCellB=rightZone.constructMasterCell(System);
  splitter->createAll(System,*offPipeD,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*splitter,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*splitter,3);

  splitter->insertInCell("Flange",System,cellA);
  splitter->insertInCell("PipeA",System,cellA);

  splitter->insertInCell("Flange",System,cellB);
  splitter->insertInCell("PipeB",System,cellB);


  // now build left/ right
  // LEFT
  bellowAA->createAll(System,*splitter,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*bellowAA,2);
  bellowAA->insertInCell(System,cellA);

  gateAA->createAll(System,*bellowAA,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*gateAA,2);
  gateAA->insertInCell(System,cellA);

  // make build necessary
  pumpTubeAA->addAllInsertCell(masterCellA->getName());
  pumpTubeAA->createAll(System,*gateAA,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*pumpTubeAA,2);
  pumpTubeAA->insertAllInCell(System,cellA);


  // RIGHT
  bellowBA->createAll(System,*splitter,3);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBA,2);
  bellowBA->insertInCell(System,cellB);

  gateBA->createAll(System,*bellowBA,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*gateBA,2);
  gateBA->insertInCell(System,cellB);

  pumpTubeBA->addAllInsertCell(masterCellB->getName());
  pumpTubeBA->createAll(System,*gateBA,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*pumpTubeBA,2);
  pumpTubeBA->insertAllInCell(System,cellB);

    // Get last two cells
  setCell("LeftVoid",masterCellA->getName());
  setCell("RightVoid",masterCellB->getName());
  
  return;
}


void
maxpeemOpticsBeamline::buildM3Mirror(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC, 
				     const long int sideIndex)
  /*!
    Sub build of the m3-mirror package
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildM3Mirror");

  int outerCell;
  
  // FAKE insertcell: required
  viewTube->addAllInsertCell(masterCell->getName());
  viewTube->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*viewTube,2);
  viewTube->insertAllInCell(System,outerCell);

  slitsB->createAll(System,*viewTube,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*slitsB,2);
  slitsB->insertInCell(System,outerCell);
  
  // FAKE insertcell: required  
  pumpTubeB->addAllInsertCell(masterCell->getName());
  pumpTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeB->createAll(System,*slitsB,2);


  const constructSystem::portItem& CPI=pumpTubeB->getPort(1);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,
				CPI,CPI.getSideIndex("OuterPlate"));
  pumpTubeB->insertAllInCell(System,outerCell);
  pumpTubeB->splitObjectAbsolute(System,1501,outerCell,
				 pumpTubeB->getLinkPt(0),
				 Geometry::Vec3D(0,0,1));
  cellIndex++;
  
  offPipeC->createAll(System,CPI,CPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeC,2);
  offPipeC->insertInCell(System,outerCell);

  M3Tube->createAll(System,*offPipeC,offPipeC->getSideIndex("FlangeBCentre"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*M3Tube,2);
  M3Tube->insertAllInCell(System,outerCell);

  M3Mirror->addInsertCell(M3Tube->getCell("Void"));
  M3Mirror->createAll(System,*M3Tube,0);

  return;
}

void
maxpeemOpticsBeamline::buildMono(Simulation& System,
				 MonteCarlo::Object* masterCell,
				 const attachSystem::FixedComp& initFC, 
				 const long int sideIndex)
  /*!
    Sub build of the slit package unit
    \param System :: Simulation to use
    \param divider :: Divider object
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildMono");

  int outerCell;
  
  monoB->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoB,2);
  monoB->insertInCell(System,outerCell);
  
  monoXtal->addInsertCell(monoB->getCell("Void"));
  monoXtal->createAll(System,*monoB,0);

  pipeG->createAll(System,*monoB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeG,2);
  pipeG->insertInCell(System,outerCell);

  gateC->createAll(System,*pipeG,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateC,2);
  gateC->insertInCell(System,outerCell);

  bellowE->createAll(System,*gateC,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);

  return;
}


void
maxpeemOpticsBeamline::buildSlitPackage(Simulation& System,
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
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildSlitPackage");

  int outerCell;
  
  pipeD->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeD,2);
  pipeD->insertInCell(System,outerCell);

  // FAKE insertcell: required
  slitTube->addAllInsertCell(masterCell->getName());
  slitTube->createAll(System,*pipeD,2);
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
      jaws[i]->addInsertCell("Support",PI.getCell("Void"));
      jaws[i]->addInsertCell("Support",slitTube->getCell("SplitVoid",i));
      jaws[i]->addInsertCell("Block",slitTube->getCell("SplitVoid",i));
      jaws[i]->createAll(System,*slitTube,0,
			 PI,PI.getSideIndex("InnerPlate"));
    }


  

  pipeE->createAll(System,*slitTube,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeE,2);
  pipeE->insertInCell(System,outerCell);

  const constructSystem::portItem& SPI=slitTube->getPort(3);
  // this needs teh plate as well if constructed
  SPI.insertCellMapInCell(System,"Flange",0,outerCell);
  
  gateB->createAll(System,*pipeE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateB,2);
  gateB->insertInCell(System,outerCell);

  bellowD->createAll(System,*gateB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);

  pipeF->createAll(System,*bellowD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeF,2);
  pipeF->insertInCell(System,outerCell);
  

  return;
}
 
void
maxpeemOpticsBeamline::buildM1Mirror(Simulation& System,
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
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildM1Mirror");

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
  
  offPipeB->createAll(System,*M1Tube,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);
  offPipeB->setCell("OuterVoid",outerCell);
  
  gateA->createAll(System,*offPipeB,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);
  gateA->setCell("OuterVoid",outerCell);
  
  pipeC->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  screenA->addAllInsertCell(outerCell);
  screenA->setCutSurf("inner",*pipeC,"pipeOuterTop");
  screenA->createAll(System,*pipeC,0);
  screenA->insertInCell("Wings",System,gateA->getCell("OuterVoid"));
  screenA->insertInCell("Wings",System,offPipeB->getCell("OuterVoid"));
  
  return;
}

void
maxpeemOpticsBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildObjects");

  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));
  MonteCarlo::Object* masterCellA=
    buildZone.constructMasterCell(System,*this);

  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  bellowA->createAll(System,*this,0);
  // dump cell for initPipe 
  outerCell=buildZone.createOuterVoidUnit(System,masterCellA,*bellowA,-1);
  // real cell for bellowA
  outerCell=buildZone.createOuterVoidUnit(System,masterCellA,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCellA,*ionPA,2);
  ionPA->insertInCell(System,outerCell);

  gateRing->createAll(System,*ionPA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCellA,*gateRing,2);
  gateRing->insertInCell(System,outerCell);
  gateRing->setCell("OuterVoid",outerCell);

  // FAKE insertcell: required
  gateTubeA->addAllInsertCell(masterCellA->getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*gateRing,2);  
  
  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertAllInCell(System,outerCell);

  bellowB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  insertFlanges(System,*gateTubeA);

  pipeA->createAll(System,*bellowB,2);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  florTubeA->addAllInsertCell(masterCellA->getName());
  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*pipeA,2);  

  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertAllInCell(System,outerCell);

  bellowC->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  insertFlanges(System,*florTubeA);
  
  pipeB->createAll(System,*bellowC,2);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,*pipeB,2);
  pipeB->insertInCell(System,outerCell);

  screenExtra->addAllInsertCell(outerCell);
  screenExtra->setCutSurf("inner",*pipeB,"pipeOuterTop");
  screenExtra->createAll(System,*pipeB,0);
  
  // FAKE insertcell: required
  pumpTubeA->addAllInsertCell(masterCellA->getName());
  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*pipeB,2);

  const constructSystem::portItem& CPI=pumpTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCellA,CPI,CPI.getSideIndex("OuterPlate"));
  pumpTubeA->insertAllInCell(System,outerCell);
  pumpTubeA->intersectPorts(System,1,2);

  offPipeA->createAll(System,CPI,CPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit(System,masterCellA,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);


  buildM1Mirror(System,masterCellA,*offPipeA,
		offPipeA->getSideIndex("FlangeBCentre"));
  buildSlitPackage(System,masterCellA,*pipeC,2);
  buildMono(System,masterCellA,*pipeF,2);

    
  buildM3Mirror(System,masterCellA,*bellowE,2);

  MonteCarlo::Object* masterCellB(0);
  buildSplitter(System,masterCellA,masterCellB,*M3Tube,2);
  
  lastComp=offPipeA;

  return;
}

void
maxpeemOpticsBeamline::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
void
maxpeemOpticsBeamline::buildOutGoingPipes(Simulation& System,
					  const int leftCell,
					  const int rightCell,
					  const std::vector<int>& hutCells)
  /*!
    Construct outgoing tracks
    \param System :: Simulation
    \param leftCell :: additional left cell for insertion
    \param rightCell :: additional right cell for insertion
    \param hutCell :: Cells for construction in hut [common to both pipes]
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildOutgoingPipes");

  outPipeA->addInsertCell(hutCells);
  outPipeA->addInsertCell(leftCell);
  outPipeA->createAll(System,*pumpTubeAA,2);
  
  outPipeB->addInsertCell(hutCells);
  outPipeB->addInsertCell(rightCell);
  outPipeB->createAll(System,*pumpTubeBA,2);

  screenB->addAllInsertCell(leftCell);
  screenB->addAllInsertCell(rightCell);

  screenB->setCutSurf("inner",outPipeA->getSurfRule("OuterRadius"));
  screenB->setCutSurf("innerTwo",outPipeB->getSurfRule("OuterRadius"));

  screenB->createAll(System,*outPipeA,0);

  return;
}


void 
maxpeemOpticsBeamline::createAll(Simulation& System,
				 const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("maxpeemOpticsBeamline","build");

  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  bellowA->setFront(FC,sideIndex);
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

