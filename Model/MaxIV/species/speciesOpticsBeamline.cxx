/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: species/speciesOpticsBeamline.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "ExternalCut.h"

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

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "BeamMount.h"
#include "TankMonoVessel.h"
#include "GratingMono.h"
#include "TwinPipe.h"
#include "Mirror.h"
#include "speciesOpticsBeamline.h"

namespace xraySystem
{

// Note currently uncopied:
  
speciesOpticsBeamline::speciesOpticsBeamline(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusX"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawMinusZ"),
      std::make_shared<xraySystem::BeamMount>(newName+"JawPlusZ")}),  
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  screenB(new xraySystem::PipeShield(newName+"ScreenB")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  monoVessel(new xraySystem::TankMonoVessel(newName+"MonoVessel")),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  mirrorJaws(new constructSystem::JawValve(newName+"MirrorJaws")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),

  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  pumpTubeAA(new constructSystem::PipeTube(newName+"PumpTubeAA")),
  gateAA(new constructSystem::GateValve(newName+"GateAA")),
  bellowAB(new constructSystem::Bellows(newName+"BellowAB")),
  gateAB(new constructSystem::GateValve(newName+"GateAB")),
  bellowAC(new constructSystem::Bellows(newName+"BellowAC")),
  
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  pumpTubeBA(new constructSystem::PipeTube(newName+"PumpTubeBA")),
  gateBA(new constructSystem::GateValve(newName+"GateBA")),
  bellowBB(new constructSystem::Bellows(newName+"BellowBB")),
  gateBB(new constructSystem::GateValve(newName+"GateBB")),
  bellowBC(new constructSystem::Bellows(newName+"BellowBC")),


  outPipeA(new constructSystem::VacuumPipe(newName+"OutPipeA")),
  outPipeB(new constructSystem::VacuumPipe(newName+"OutPipeB"))

  
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(ionPA);
  OR.addObject(gateTubeA);
  OR.addObject(bellowB);
  OR.addObject(pipeA);
  OR.addObject(M1Tube);
  OR.addObject(M1Mirror);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(offPipeA);
  OR.addObject(offPipeB);
  OR.addObject(mirrorJaws);
}
  
speciesOpticsBeamline::~speciesOpticsBeamline()
  /*!
    Destructor
   */
{}

void
speciesOpticsBeamline::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","populate");
  FixedOffset::populate(Control);

  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  return;
}

void
speciesOpticsBeamline::createUnitVector(const attachSystem::FixedComp& FC,
					const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
speciesOpticsBeamline::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","createSurfaces");
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
speciesOpticsBeamline::insertFlanges(Simulation& System,
				     const constructSystem::PipeTube& PT)
  /*!
    Boilerplate function to insert the flanges from pipetubes
    that extend past the linkzone in to ther neighbouring regions.
    \param System :: Simulation to use
    \param PT :: PipeTube
   */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","insertFlanges");
  
  const size_t voidN=this->getNItems("OuterVoid")-3;

  // inserting into the outerVoid +1 / -1 
  this->insertComponent(System,"OuterVoid",voidN,PT.getCC("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN,PT.getCC("FlangeB"));
  this->insertComponent(System,"OuterVoid",voidN+2,PT.getCC("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN+2,PT.getCC("FlangeB"));
  return;
}


void
speciesOpticsBeamline::buildFrontTable(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC, 
				     const long int sideIndex)
  /*!
    Sub build of the first part of the beamline
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","buildFrontTable");

  int outerCell;
  // dummy space for first item
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  bellowA->createAll(System,initFC,sideIndex);
  // dump cell for initPipe 
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowA,-1);
  // real cell for bellowA
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*ionPA,2);
  ionPA->insertInCell(System,outerCell);
  
  // FAKE insertcell: required
  gateTubeA->addAllInsertCell(masterCell->getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*ionPA,2);  

  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertAllInCell(System,outerCell);

  bellowB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  insertFlanges(System,*gateTubeA);
  
  return;
}


void
speciesOpticsBeamline::buildM1Mirror(Simulation& System,
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
  ELog::RegMethod RegA("speciesOpticsBeamline","buildM1Mirror");

  int outerCell;

  pipeA->createAll(System,initFC,sideIndex);
  outerCell= buildZone.createOuterVoidUnit(System,masterCell,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  M1Tube->addAllInsertCell(masterCell->getName());
  M1Tube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  M1Tube->createAll(System,*pipeA,2);

  
  const constructSystem::portItem& API=M1Tube->getPort(1);

  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,API,API.getSideIndex("OuterPlate"));
  M1Tube->insertAllInCell(System,outerCell);
  
  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);


  bellowC->setAxisControl(3,pipeA->getZ());
  bellowC->createAll(System,API,API.getSideIndex("OuterPlate"));
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  pipeB->createAll(System,*bellowC,2);
  outerCell= buildZone.createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertInCell(System,outerCell);

  screenA->addAllInsertCell(outerCell);
  screenA->setCutSurf("inner",*pipeB,"pipeOuterTop");
  screenA->createAll(System,*pipeB,0);
    
  return;
}


void
speciesOpticsBeamline::buildSlitPackage(Simulation& System,
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
  ELog::RegMethod RegA("speciesOpticsBeamline","buildSlitPackage");

  int outerCell;
  
  gateA->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  pipeC->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  // FAKE insertcell: required
  slitTube->addAllInsertCell(masterCell->getName());
  slitTube->createAll(System,*pipeC,2);
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

  pipeD->createAll(System,*slitTube,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeD,2);
  pipeD->insertInCell(System,outerCell);

  const constructSystem::portItem& SPI=slitTube->getPort(3);
  // this needs the plate as well if constructed
  SPI.insertCellMapInCell(System,"Flange",0,outerCell);
  
  
  screenB->addAllInsertCell(outerCell);
  screenB->setCutSurf("inner",*pipeD,"pipeOuterTop");
  screenB->createAll(System,*pipeD,0);

  return;
}

void
speciesOpticsBeamline::buildMono(Simulation& System,
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
  ELog::RegMethod RegA("speciesOpticsBeamline","buildMono");

  int outerCell;

  offPipeA->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);
  
  monoVessel->createAll(System,*offPipeA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*monoVessel,2);
  monoVessel->insertInCell(System,outerCell);

  offPipeB->createAll(System,*monoVessel,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);

  return;
}

void
speciesOpticsBeamline::buildM3Mirror(Simulation& System,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC, 
				     const long int sideIndex)
  /*!
    Sub build of the m3-mirror unit
    \param System :: Simulation to use
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","buildM3Mirror");

  int outerCell;

  bellowD->createAll(System,initFC,sideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);

  pipeE->createAll(System,*bellowD,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeE,2);
  pipeE->insertInCell(System,outerCell);

  bellowE->createAll(System,*pipeE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);
  
  pipeF->createAll(System,*bellowE,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeF,2);
  pipeF->insertInCell(System,outerCell);

  mirrorJaws->createAll(System,*pipeF,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*mirrorJaws,2);
  mirrorJaws->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  M3Tube->addAllInsertCell(masterCell->getName());
  M3Tube->setPortRotation(3,Geometry::Vec3D(1,0,0));
  M3Tube->createAll(System,*mirrorJaws,2);

  const constructSystem::portItem& API=M3Tube->getPort(1);
  outerCell=buildZone.createOuterVoidUnit
    (System,masterCell,API,API.getSideIndex("OuterPlate"));
  M3Tube->insertAllInCell(System,outerCell);

  return;
}

void
speciesOpticsBeamline::buildSplitter(Simulation& System,
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
  ELog::RegMethod RegA("speciesOpticsBeamLine","buildSplitter");

  int cellA(0),cellB(0);
  

  buildZone.constructMiddleSurface(SMap,buildIndex+10,initFC,sideIndex);

  attachSystem::InnerZone leftZone=buildZone.buildMiddleZone(-1);
  attachSystem::InnerZone rightZone=buildZone.buildMiddleZone(1);

  // No need for insert -- note removal of old master cell
  System.removeCell(masterCellA->getName());
  
  masterCellA=leftZone.constructMasterCell(System);
  masterCellB=rightZone.constructMasterCell(System);
  splitter->createAll(System,initFC,sideIndex);
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

  // make build necessary
  pumpTubeAA->addAllInsertCell(masterCellA->getName());
  pumpTubeAA->createAll(System,*bellowAA,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*pumpTubeAA,2);
  pumpTubeAA->insertAllInCell(System,cellA);

  gateAA->createAll(System,*pumpTubeAA,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*gateAA,2);
  gateAA->insertInCell(System,cellA);

  bellowAB->createAll(System,*gateAA,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*bellowAB,2);
  bellowAB->insertInCell(System,cellA);

  gateAB->createAll(System,*bellowAB,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*gateAB,2);
  gateAB->insertInCell(System,cellA);

  bellowAC->createAll(System,*gateAB,2);
  cellA=leftZone.createOuterVoidUnit(System,masterCellA,*bellowAC,2);
  bellowAC->insertInCell(System,cellA);

  // RIGHT
  bellowBA->createAll(System,*splitter,3);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBA,2);
  bellowBA->insertInCell(System,cellB);  

  // make build necessary
  pumpTubeBA->addAllInsertCell(masterCellB->getName());
  pumpTubeBA->createAll(System,*bellowBA,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*pumpTubeBA,2);
  pumpTubeBA->insertAllInCell(System,cellB);

  gateBA->createAll(System,*pumpTubeBA,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*gateBA,2);
  gateBA->insertInCell(System,cellB);

  bellowBB->createAll(System,*gateBA,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBB,2);
  bellowBB->insertInCell(System,cellB);

  gateBB->createAll(System,*bellowBB,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*gateBB,2);
  gateBB->insertInCell(System,cellB);

  bellowBC->createAll(System,*gateBB,2);
  cellB=rightZone.createOuterVoidUnit(System,masterCellB,*bellowBC,2);
  bellowBC->insertInCell(System,cellB);

  return;
}

void
speciesOpticsBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("speciesOpticsBeamline","buildObjects");

  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));  
  MonteCarlo::Object* masterCellA=
    buildZone.constructMasterCell(System,*this);

  buildFrontTable(System,masterCellA,*this,0);
  buildM1Mirror(System,masterCellA,*bellowB,2);
  buildSlitPackage(System,masterCellA,*pipeB,2);
  buildMono(System,masterCellA,*pipeD,2);
  buildM3Mirror(System,masterCellA,*offPipeB,2);

  MonteCarlo::Object* masterCellB(0);
  const constructSystem::portItem& API=M3Tube->getPort(1);
  const long int sideIndex=API.getSideIndex("OuterPlate");
  buildSplitter(System,masterCellA,masterCellB,API,sideIndex);

  lastComp=bellowB;

  return;
}

void
speciesOpticsBeamline::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  

void 
speciesOpticsBeamline::createAll(Simulation& System,
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
  ELog::RegMethod RControl("speciesOpticsBeamline","build");

  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  bellowA->setFront(FC,sideIndex);

  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE xraySystem

