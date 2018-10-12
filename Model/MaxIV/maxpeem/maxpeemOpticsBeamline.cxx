/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/maxpeemOpticsBeamline.cxx
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
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "CSGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
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
#include "FlangeMount.h"
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
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  pipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  pumpTubeA(new constructSystem::PipeTube(newName+"PumpTubeA")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  M1Tube(new constructSystem::PipeTube(newName+"M1Tube")),
  M1Mirror(new xraySystem::Mirror(newName+"M1Mirror")),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  screenA(new xraySystem::PipeShield(newName+"ScreenA")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube")),
  jaws({
      std::make_shared<xraySystem::FlangeMount>(newName+"JawMinusX"),
      std::make_shared<xraySystem::FlangeMount>(newName+"JawPlusX"),
      std::make_shared<xraySystem::FlangeMount>(newName+"JawMinusZ"),
      std::make_shared<xraySystem::FlangeMount>(newName+"JawPlusZ")}),  
  pipeE(new constructSystem::VacuumPipe(newName+"PipeE")),
  gateB(new constructSystem::GateValve(newName+"GateB")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  pipeF(new constructSystem::VacuumPipe(newName+"PipeF")),
  monoB(new xraySystem::GrateMonoBox(newName+"MonoBox")),
  monoXtal(new xraySystem::GratingMono(newName+"MonoXtal")),
  pipeG(new constructSystem::VacuumPipe(newName+"PipeG")),
  gateC(new constructSystem::GateValve(newName+"GateC")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  viewTube(new constructSystem::PipeTube(newName+"ViewTube")),
  slitsB(new constructSystem::JawValve(newName+"SlitsB")),
  pumpTubeB(new constructSystem::PipeTube(newName+"PumpTubeB")),
  offPipeC(new constructSystem::OffsetFlangePipe(newName+"OffPipeC")),
  M3Tube(new constructSystem::PipeTube(newName+"M3Tube")),
  M3Mirror(new xraySystem::Mirror(newName+"M3Mirror")),
  offPipeD(new constructSystem::OffsetFlangePipe(newName+"OffPipeD")),
  splitter(new xraySystem::TwinPipe(newName+"Splitter")),
  screenB(new xraySystem::PipeShield(newName+"ScreenB")),
  bellowAA(new constructSystem::Bellows(newName+"BellowAA")),
  gateAA(new constructSystem::GateValve(newName+"GateAA")),
  pumpTubeAA(new constructSystem::PortTube(newName+"PumpTubeAA")),
  bellowBA(new constructSystem::Bellows(newName+"BellowBA")),
  gateBA(new constructSystem::GateValve(newName+"GateBA")),
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
  OR.addObject(gateTubeA);
  OR.addObject(bellowB);
  OR.addObject(pipeA);
  OR.addObject(florTubeA);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(pumpTubeA);
  OR.addObject(offPipeA);
  OR.addObject(M1Tube);
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

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*180.0,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  return;
}

  
void
maxpeemOpticsBeamline::constructMasterCell(Simulation& System,
					   const HeadRule& cutHR)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param cutHR :: Start of cut fixed comp
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","constructMasterCell");

  std::string Out;

  masterCellA=0;
  const int CN=getCell("MasterVoid");
  if (CN)
    {
      MonteCarlo::Object* MCell  = System.findQhull(CN);    
      if (MCell && cutHR.hasRule())
	{
	  const int SN=getRule("back").getPrimarySurface();
	  MCell->removeSurface(SN);
	  MCell->addSurfString(cutHR.display());
	  
	  Out=cutHR.complement().display()+getRuleStr("back")+
	    getRuleStr("beam");
	  makeCell("MasterVoid",System,cellIndex++,0,0.0,Out);
	  masterCellA=System.findQhull(cellIndex-1);
	}
    }
  else
    throw ColErr::InContainerError<int>(CN,"Cell number for masterCell");
  return;
}

int
maxpeemOpticsBeamline::constructDivideCell(Simulation& System,
					   const bool plusSide,
					   const attachSystem::FixedComp& FFC,
					   const long int frontIndex,
					   const attachSystem::FixedComp& BFC,
					   const long backIndex)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param FFC :: Front FC
    \param BFC :: Back FC
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","constructDivideCell");

  std::string Out;
  Out=getRuleStr("beam");
  Out += (plusSide) ? getRuleStr("middle") : getComplementStr("middle");
  
  // make inner cell

  std::string Inner = Out;
  Inner += FFC.getLinkString(frontIndex);
  Inner += BFC.getLinkString(-backIndex);
  makeCell("MasterVoid",System,cellIndex++,0,0.0,Inner);
  // outer cell and set 
  Out+=BFC.getLinkString(backIndex);
  Out+=getRuleStr("back");
  if (plusSide)
    masterCellB->procString(Out);
  else
    masterCellA->procString(Out);

  // return current build cell:
  return cellIndex-1;
}

  
int
maxpeemOpticsBeamline::createDoubleVoidUnit(Simulation& System,
					    HeadRule& divider,
					    const attachSystem::FixedComp& FC,
					    const long int sideIndex)
 /*!
    Construct two outer void objects in the main pipe
    \param System :: Simulation
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline",
		       "createDoubleOuterVoidUnit");

    // construct an cell based on previous cell:
  std::string Out;
  
  const HeadRule& backHR=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    ExternalCut::getRule("back");
  
  Out=getRuleStr("beam");
  Out+=divider.display()+backHR.display();
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  
  // make the master cell valid :
  makeCell("masterCellB",System,cellIndex++,0,0.0,Out);
  masterCellB = System.findQhull(cellIndex-1);

  if (!ExternalCut::isActive("middle"))
    {
      const Geometry::Vec3D DPoint(FC.getLinkPt(sideIndex));
      Geometry::Vec3D crossX,crossY,crossZ;
      FC.selectAltAxis(sideIndex,crossX,crossY,crossZ);
      ModelSupport::buildPlane(SMap,buildIndex+10,DPoint,crossX);
      ExternalCut::setCutSurf("middle",SMap.realSurf(buildIndex+10));
    }
  
  refrontMasterCell(masterCellA,masterCellB,FC,sideIndex);
  return cellIndex-2;
}

int
maxpeemOpticsBeamline::createOuterVoidUnit(Simulation& System,
					   MonteCarlo::Object* masterCell,
					   HeadRule& divider,
					   const attachSystem::FixedComp& FC,
					   const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell number
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","createOuterVoid");

  // construct an cell based on previous cell:
  std::string Out;
  
  const HeadRule& backHR=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    ExternalCut::getRule("back");
  
  Out=getRuleStr("beam");
  Out+=divider.display()+backHR.display();
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  divider=backHR;

  // make the master cell valid:
  
  divider.makeComplement();

  refrontMasterCell(masterCell,FC,sideIndex);
  return cellIndex-1;
}

int
maxpeemOpticsBeamline::createOuterVoidUnit(Simulation& System,
					   MonteCarlo::Object* masterCell,
					   const HeadRule& middle,
					   HeadRule& divider,
					   const attachSystem::FixedComp& FC,
					   const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell number
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","createOuterVoid");

  // construct an cell based on previous cell:
  std::string Out;
  
  const HeadRule& backHR=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    ExternalCut::getRule("back");
  
  Out=getRuleStr("beam");
  Out+=divider.display()+backHR.display()+middle.display();
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  divider=backHR;

  // make the master cell valid:
  
  divider.makeComplement();

  refrontMasterCell(masterCell,FC,sideIndex);
  masterCell->addSurfString(middle.display());
  return cellIndex-1;
}



void
maxpeemOpticsBeamline::refrontMasterCell(MonteCarlo::Object* MCell,
					 const attachSystem::FixedComp& FC,
					 const long int sideIndex) const
  /*!
    This horrific function to re-build MCell so that it is correct
    as createOuterVoid consumes the front of the master cell
    \param MCell :: master cell object
    \param middel
    \param FC :: FixedComp
    \param sideIndex :: side index for back of FC object
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","refrontMasterCell");

  std::string Out;  
  Out=getRuleStr("beam")+getRuleStr("back")+
    FC.getLinkString(sideIndex);
  MCell->procString(Out);
  return;
}


void
maxpeemOpticsBeamline::refrontMasterCell(MonteCarlo::Object* MCellNeg,
					 MonteCarlo::Object* MCellPlus,
					 const attachSystem::FixedComp& FC,
					 const long int sideIndex) const
  /*!
    This horrifc function to re-build a double MCell 
    based on centre point split 

    \param MCellNeg :: Based on the negative side
    \param MCellPlus :: Based on the positive side
    \param FC :: FixedComp
    \param sideIndex :: side index for back of FC object
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","refrontMasterCell");

  std::string Out;  
  Out=getRuleStr("beam")+getRuleStr("back")+
    FC.getLinkString(sideIndex);

  MCellNeg->procString(Out+getComplementStr("middle"));
  MCellPlus->procString(Out+getRuleStr("middle"));

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
  this->insertComponent(System,"OuterVoid",voidN,PT,"FrontFlange",0);
  this->insertComponent(System,"OuterVoid",voidN,PT,"BackFlange",0);
  this->insertComponent(System,"OuterVoid",voidN+2,PT,"FrontFlange",0);
  this->insertComponent(System,"OuterVoid",voidN+2,PT,"BackFlange",0);
  return;
}

void
maxpeemOpticsBeamline::buildSplitter(Simulation& System,
				     HeadRule& divider,
				     const attachSystem::FixedComp& initFC,
				     const long int sideIndex)
  /*!
    Sub build of the spliter package
    \param System :: Simulation to use
    \param dividerA :: Divider object main divider
    \param initFC :: Start point
    \param sideIndex :: start link point
  */

{
  ELog::RegMethod RegA("maxpeemOpticsBeamLine","buildSplitter");

  int cellA,cellB;

  offPipeD->createAll(System,initFC,sideIndex);
  cellA=createDoubleVoidUnit(System,divider,*offPipeD,2);
  offPipeD->insertInCell(System,cellA);

  splitter->createAll(System,*offPipeD,2);
  cellA=constructDivideCell(System,0,*offPipeD,2,*splitter,2);
  cellB=constructDivideCell(System,1,*offPipeD,2,*splitter,3);  

  splitter->insertInCell("Flange",System,cellA);
  splitter->insertInCell("Flange",System,cellB);
  splitter->insertInCell("PipeA",System,cellA);
  splitter->insertInCell("PipeB",System,cellB);
  
  // now build left/ right
  // LEFT
  bellowAA->createAll(System,*splitter,2);
  cellA=constructDivideCell(System,0,*splitter,2,*bellowAA,2);
  bellowAA->insertInCell(System,cellA);

  gateAA->createAll(System,*bellowAA,2);
  cellA=constructDivideCell(System,0,*bellowAA,2,*gateAA,2);
  gateAA->insertInCell(System,cellA);

  pumpTubeAA->addInsertCell(masterCellA->getName());
  pumpTubeAA->createAll(System,*gateAA,2);
  cellA=constructDivideCell(System,0,*gateAA,2,*pumpTubeAA,2);
  pumpTubeAA->insertInCell(System,cellA);
    
  // RIGHT
  bellowBA->createAll(System,*splitter,3);
  cellB=constructDivideCell(System,1,*splitter,3,*bellowBA,2);
  bellowBA->insertInCell(System,cellB);

  gateBA->createAll(System,*bellowBA,2);
  cellB=constructDivideCell(System,1,*bellowBA,2,*gateBA,2);
  gateBA->insertInCell(System,cellB);

  pumpTubeBA->addInsertCell(masterCellB->getName());
  pumpTubeBA->createAll(System,*gateBA,2);
  cellB=constructDivideCell(System,1,*gateBA,2,*pumpTubeBA,2);
  pumpTubeBA->insertInCell(System,cellB);

  
  return;
}


void
maxpeemOpticsBeamline::buildM3Mirror(Simulation& System,
				     HeadRule& divider,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC, 
				     const long int sideIndex)
  /*!
    Sub build of the m3-mirror package
    \param System :: Simulation to use
    \param divider :: Divider object
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildM3Mirror");

  int outerCell;
  
  // FAKE insertcell: required
  viewTube->addInsertCell(masterCell->getName());
  viewTube->createAll(System,initFC,sideIndex);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*viewTube,2);
  viewTube->insertInCell(System,outerCell);

  slitsB->createAll(System,*viewTube,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*slitsB,2);
  slitsB->insertInCell(System,outerCell);

  // FAKE insertcell: required
  
  pumpTubeB->addInsertCell(masterCell->getName());
  pumpTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeB->createAll(System,*slitsB,2);


  const constructSystem::portItem& CPI=pumpTubeB->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,divider,
				CPI,CPI.getSideIndex("OuterPlate"));
  pumpTubeB->insertInCell(System,outerCell);
  pumpTubeB->splitObjectAbsolute(System,1501,outerCell,
				 pumpTubeB->getLinkPt(0),
				 Geometry::Vec3D(0,0,1));
  cellIndex++;
  
  offPipeC->createAll(System,CPI,CPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*offPipeC,2);
  offPipeC->insertInCell(System,outerCell);

  M3Tube->createAll(System,*offPipeC,offPipeC->getSideIndex("FlangeBCentre"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*M3Tube,2);
  M3Tube->insertInCell(System,outerCell);

  M3Mirror->addInsertCell(M3Tube->getCell("Void"));
  M3Mirror->createAll(System,*M3Tube,0);

  return;
}

void
maxpeemOpticsBeamline::buildMono(Simulation& System,
				 HeadRule& divider,
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
  outerCell=createOuterVoidUnit(System,masterCell,divider,*monoB,2);
  monoB->insertInCell(System,outerCell);
  
  monoXtal->addInsertCell(monoB->getCell("Void"));
  monoXtal->createAll(System,*monoB,0);

  pipeG->createAll(System,*monoB,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeG,2);
  pipeG->insertInCell(System,outerCell);

  gateC->createAll(System,*pipeG,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*gateC,2);
  gateC->insertInCell(System,outerCell);

  bellowE->createAll(System,*gateC,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*bellowE,2);
  bellowE->insertInCell(System,outerCell);

  return;
}


void
maxpeemOpticsBeamline::buildSlitPackage(Simulation& System,
					HeadRule& divider,
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
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildSlitPackage");

  int outerCell;
  
  pipeD->createAll(System,initFC,sideIndex);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeD,2);
  pipeD->insertInCell(System,outerCell);

  // FAKE insertcell: required
  slitTube->addInsertCell(masterCell->getName());
  slitTube->createAll(System,*pipeD,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*slitTube,2);
  slitTube->insertInCell(System,outerCell);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
			   slitTube->getCell("Void"),
			   Geometry::Vec3D(0,1,0));

  ELog::EM<<"--------------"<<ELog::endDiag;
  slitTube->splitObject(System,1501,outerCell,
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  cellIndex++;  // remember creates an extra cell in  primary
  ELog::EM<<"--------------"<<ELog::endDiag;
  for(size_t i=0;i<jaws.size();i++)
    {
      const constructSystem::portItem& PI=slitTube->getPort(i);
      jaws[i]->addInsertCell("Flange",outerCell);
      jaws[i]->addInsertCell("Body",PI.getCell("Void"));
      jaws[i]->addInsertCell("Body",slitTube->getCell("SplitVoid",i));
      jaws[i]->setBladeCentre(PI,0);
      jaws[i]->createAll(System,PI,2);
    }


  

  pipeE->createAll(System,*slitTube,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeE,2);
  pipeE->insertInCell(System,outerCell);

  const constructSystem::portItem& SPI=slitTube->getPort(3);
  // this needs teh plate as well if constructed
  SPI.insertCellMapInCell(System,"Flange",0,outerCell);
  
  gateB->createAll(System,*pipeE,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*gateB,2);
  gateB->insertInCell(System,outerCell);

  bellowD->createAll(System,*gateB,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*bellowD,2);
  bellowD->insertInCell(System,outerCell);

  pipeF->createAll(System,*bellowD,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeF,2);
  pipeF->insertInCell(System,outerCell);
  

  return;
}
 
void
maxpeemOpticsBeamline::buildM1Mirror(Simulation& System,
				     HeadRule& divider,
				     MonteCarlo::Object* masterCell,
				     const attachSystem::FixedComp& initFC, 
				     const long int sideIndex)
  /*!
    Sub build of the m1-mirror package
    \param System :: Simulation to use
    \param divider :: Divider object
    \param masterCell :: Main master volume
    \param initFC :: Start point
    \param sideIndex :: start link point
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildM1Mirror");

  int outerCell;

  M1Tube->setFront(initFC,sideIndex);
  M1Tube->createAll(System,initFC,sideIndex);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*M1Tube,2);
  M1Tube->insertInCell(System,outerCell);


  M1Mirror->addInsertCell(M1Tube->getCell("Void"));
  M1Mirror->createAll(System,*M1Tube,0);
  
  offPipeB->createAll(System,*M1Tube,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);

  gateA->createAll(System,*offPipeB,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*gateA,2);
  gateA->insertInCell(System,outerCell);

  pipeC->createAll(System,*gateA,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  screenA->addInsertCell(outerCell);
  screenA->setCutSurf("inner",*pipeC,"pipeOuterTop");
  screenA->createAll(System,*pipeC,0);
  
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

  HeadRule divider;
  
  int outerCell;
  // dummy space for first itme
  // This is a mess but want to preserve insert items already
  // in the hut beam port
  bellowA->createAll(System,*this,0);
  divider=bellowA->getFullRule(1);
  constructMasterCell(System,divider);

  divider.makeComplement();
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowA,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*ionPA,2);
  ionPA->insertInCell(System,outerCell);
  
  // FAKE insertcell: required
  gateTubeA->addInsertCell(masterCellA->getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*ionPA,2);  

  
  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,
  				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertInCell(System,outerCell);

  bellowB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  insertFlanges(System,*gateTubeA);

     

  pipeA->createAll(System,*bellowB,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  florTubeA->addInsertCell(masterCellA->getName());
  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*pipeA,2);  

  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,
  				FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertInCell(System,outerCell);

  bellowC->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  insertFlanges(System,*florTubeA);
  
  pipeB->createAll(System,*bellowC,2);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*pipeB,2);
  pipeB->insertInCell(System,outerCell);


  // FAKE insertcell: required
  pumpTubeA->addInsertCell(masterCellA->getName());
  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*pipeB,2);

  const constructSystem::portItem& CPI=pumpTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCellA,divider,
  				CPI,CPI.getSideIndex("OuterPlate"));
  pumpTubeA->insertInCell(System,outerCell);
  pumpTubeA->intersectPorts(System,1,2);

  offPipeA->createAll(System,CPI,CPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCellA,divider,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);

  buildM1Mirror(System,divider,masterCellA,*offPipeA,
		offPipeA->getSideIndex("FlangeBCentre"));
  buildSlitPackage(System,divider,masterCellA,*pipeC,2);
  buildMono(System,divider,masterCellA,*pipeF,2);

    
  buildM3Mirror(System,divider,masterCellA,*bellowE,2);
  
  buildSplitter(System,divider,*M3Tube,2);
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
					  const std::vector<int>& hutCell,
					  const int outCell)
  /*!
    Construct outgoing tracks
    \param System :: Simulation
    \parma hutCell :: Cells for construction in hut
    \parma outCell :: Final out cel
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","buildOutgoingPipes");

  outPipeA->addInsertCell(masterCellA->getName());
  outPipeA->addInsertCell(hutCell);
  outPipeA->addInsertCell(outCell);
  outPipeA->createAll(System,*pumpTubeAA,2);

  outPipeB->addInsertCell(masterCellB->getName());
  outPipeB->addInsertCell(hutCell);
  outPipeB->addInsertCell(outCell);
  outPipeB->createAll(System,*pumpTubeBA,2);
  
  screenB->addInsertCell(masterCellA->getName());
  screenB->addInsertCell(masterCellB->getName());

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
  bellowA->setFront(FC,sideIndex);
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

