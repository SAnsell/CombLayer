/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/balderFrontEnd.cxx
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"

#include "VacuumPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValve.h"
#include "OffsetFlangePipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "CrossPipe.h"
#include "Wiggler.h"
#include "SqrCollimator.h"
#include "BeamMount.h"
#include "HeatDump.h"

#include "balderFrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
balderFrontEnd::balderFrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),


  wigglerBox(new constructSystem::VacuumBox(newName+"WigglerBox",1)),
  wiggler(new Wiggler(newName+"Wiggler")),
  dipolePipe(new constructSystem::VacuumPipe(newName+"DipolePipe")),
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  collTubeA(new constructSystem::PipeTube(newName+"CollimatorTubeA")),
  collA(new xraySystem::SqrCollimator(newName+"CollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  collABPipe(new constructSystem::VacuumPipe(newName+"CollABPipe")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  collTubeB(new constructSystem::PipeTube(newName+"CollimatorTubeB")),
  collB(new xraySystem::SqrCollimator(newName+"CollB")),
  collTubeC(new constructSystem::PipeTube(newName+"CollimatorTubeC")),
  collC(new xraySystem::SqrCollimator(newName+"CollC")),
  eCutDisk(new insertSystem::insertCylinder(newName+"ECutDisk")),  
  collExitPipe(new constructSystem::VacuumPipe(newName+"CollExitPipe")),
  heatBox(new constructSystem::PortTube(newName+"HeatBox")),
  heatDump(new xraySystem::HeatDump(newName+"HeatDump")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  ionPB(new constructSystem::CrossPipe(newName+"IonPB")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),

  gateA(new constructSystem::GateValve(newName+"GateA")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  gateTubeB(new constructSystem::PipeTube(newName+"GateTubeB")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  shutterBox(new constructSystem::PipeTube(newName+"ShutterBox")),
  shutters({
      std::make_shared<xraySystem::BeamMount>(newName+"Shutter0"),
	std::make_shared<xraySystem::BeamMount>(newName+"Shutter1")
	}),
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  bellowK(new constructSystem::Bellows(newName+"BellowK")) ,

  exitPipe(new constructSystem::VacuumPipe(newName+"ExitPipe"))
   
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(wigglerBox);
  OR.addObject(wiggler);
  OR.addObject(dipolePipe);
  OR.addObject(bellowA);
  OR.addObject(collTubeA);
  OR.addObject(collA);
  OR.addObject(bellowB);
  OR.addObject(collABPipe);
  OR.addObject(bellowC);    
  OR.addObject(collTubeB);
  OR.addObject(collB);
  OR.addObject(collTubeC);
  OR.addObject(collC);
  OR.addObject(eCutDisk);
  OR.addObject(collExitPipe);
  OR.addObject(heatBox);
  OR.addObject(heatDump);    
  OR.addObject(shutterBox);
  OR.addObject(shutters[0]);
  OR.addObject(shutters[1]);
  OR.addObject(exitPipe);
}
  
balderFrontEnd::~balderFrontEnd()
  /*!
    Destructor
   */
{}

void
balderFrontEnd::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);
  return;
}

void
balderFrontEnd::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("balderFrontEnd","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
balderFrontEnd::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("balderFrontEnd","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*180.0,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  return;
}

int
balderFrontEnd::createOuterVoidUnit(Simulation& System,
				     MonteCarlo::Object& masterCell,
				     const attachSystem::FixedComp& FC,
				     const long int sideIndex)
/*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param masterCell :: full master cell
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("balderFrontEnd","createOuterVoid");

  static HeadRule divider;
  // construct an cell based on previous cell:
  std::string Out;
  
  if (!divider.hasRule())
    divider=FrontBackCut::getFrontRule();

  const HeadRule& backHR=
    (sideIndex) ? FC.getFullRule(-sideIndex) :
    FrontBackCut::getBackRule();
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=divider.display()+backHR.display();
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  divider=backHR;

  // make the master cell valid:
  
  divider.makeComplement();

  refrontMasterCell(masterCell,FC,sideIndex);
  return cellIndex-1;
}

void
balderFrontEnd::refrontMasterCell(MonteCarlo::Object& MCell,
				   const attachSystem::FixedComp& FC,
				   const long int sideIndex) const
  /*!
    This horrifc function to re-build MCell so that it is correct
    as createOuterVoid consumes the front of the master cell
    \param MCell :: master cell object
    \param FC :: FixedComp
    \param sideIndex :: side index for back of FC object
  */
{
  ELog::RegMethod RegA("balderFrontEnd","refrontMasterCell");

  std::string Out;  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=backRule()+FC.getLinkString(sideIndex);
  MCell.procString(Out);
  return;
}
 
MonteCarlo::Object&
balderFrontEnd::constructMasterCell(Simulation& System)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \return cell object
  */
{
  ELog::RegMethod RegA("balderFrontEnd","constructMasterCell");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=frontRule()+backRule();
  makeCell("MasterVoid",System,cellIndex++,0,0.0,Out);
  addOuterSurf(Out);
  insertObjects(System);

  return *System.findQhull(cellIndex-1);
}

void
balderFrontEnd::insertFlanges(Simulation& System,
			       const constructSystem::PipeTube& PT)
  /*!
    Boilerplate function to insert the flanges from pipetubes
    that extend past the linkzone in to ther neighbouring regions.
    \param System :: Simulation to use
    \param PT :: PipeTube
   */
{
  ELog::RegMethod RegA("balderFrontEnd","insertFlanges");
  
  const size_t voidN=this->getNItems("OuterVoid")-3;

  this->insertComponent(System,"OuterVoid",voidN,
			PT.getFullRule("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN,
			PT.getFullRule("FlangeB"));
  this->insertComponent(System,"OuterVoid",voidN+2,
			PT.getFullRule("FlangeA"));
  this->insertComponent(System,"OuterVoid",voidN+2,
			PT.getFullRule("FlangeB"));
  return;
}

void
balderFrontEnd::buildHeatTable(Simulation& System,
			       MonteCarlo::Object& masterCell,
			       const attachSystem::FixedComp& preFC,
			       const long int preSideIndex)

  /*!
    Build the heatDump table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: initial Fixedcomp 
    \param preSideIndex :: link point on initial FC
  */
{
  ELog::RegMethod RegA("balderFrontEnd","buildHeatTable");

  int outerCell;
  // FAKE insertcell:
  heatBox->addAllInsertCell(masterCell.getName());
  heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  heatBox->createAll(System,preFC,preSideIndex);

  const constructSystem::portItem& PIA=heatBox->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				PIA,PIA.getSideIndex("OuterPlate"));
  heatBox->insertAllInCell(System,outerCell);
    
  // cant use heatbox here because of port rotation
  
  heatDump->addInsertCell("Inner",heatBox->getCell("Void"));
  heatDump->addInsertCell("Outer",outerCell);
  heatDump->createAll(System,PIA,0,*heatBox,2);


  //  const constructSystem::portItem& PI=heatBox->getPort(1);  
  bellowD->createAll(System,PIA,PIA.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);


  // FAKE insertcell:
  gateTubeA->addAllInsertCell(masterCell.getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*bellowD,2);  

  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertAllInCell(System,outerCell);
  
  ionPB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*ionPB,2);
  ionPB->insertInCell(System,outerCell);

  insertFlanges(System,*gateTubeA);
  
  pipeB->createAll(System,*ionPB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertInCell(System,outerCell);
  
  return;
  
}

void
balderFrontEnd::buildShutterTable(Simulation& System,
				   MonteCarlo::Object& masterCell,
				   const attachSystem::FixedComp& preFC,
				   const long int preSideIndex)
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell for insertion
    \param preFC :: initial Fixedcomp 
    \param preSideIndex :: link point on initial FC
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildShutterTable");
  int outerCell;
  
  gateA->createAll(System,preFC,preSideIndex);
  outerCell=createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  // bellows 
  bellowI->createAll(System,*gateA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowI,2);
  bellowI->insertInCell(System,outerCell);

  florTubeA->addAllInsertCell(masterCell.getName());
  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,2);
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertAllInCell(System,outerCell);
  
  // bellows 
  bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*bellowJ,2);
  bellowJ->insertInCell(System,outerCell);

  insertFlanges(System,*florTubeA);

  // FAKE insertcell:
  gateTubeB->addAllInsertCell(masterCell.getName());
  gateTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeB->createAll(System,*bellowJ,2);  
  const constructSystem::portItem& GPI=gateTubeB->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeB->insertAllInCell(System,outerCell);

  offPipeA->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);

  insertFlanges(System,*gateTubeB);

  shutterBox->delayPorts();
  shutterBox->createAll(System,*offPipeA,
			offPipeA->getSideIndex("FlangeBCentre"));
  outerCell=createOuterVoidUnit(System,masterCell,*shutterBox,2);
  shutterBox->insertAllInCell(System,outerCell);
  
  cellIndex=shutterBox->splitVoidPorts(System,"SplitVoid",1001,
				       shutterBox->getCell("Void"),
				       {0,1});
  cellIndex=
    shutterBox->splitVoidPorts(System,"SplitOuter",2001,
			       outerCell,{0,1});
  shutterBox->addAllInsertCell(outerCell);
  shutterBox->createPorts(System);

  for(size_t i=0;i<shutters.size();i++)
    {
      const constructSystem::portItem& PI=shutterBox->getPort(i);
      shutters[i]->addInsertCell("Support",PI.getCell("Void"));
      shutters[i]->addInsertCell("Support",shutterBox->getCell("SplitVoid",i));
      shutters[i]->addInsertCell("Block",shutterBox->getCell("SplitVoid",i));

      shutters[i]->createAll(System,*offPipeA,
			     offPipeA->getSideIndex("FlangeACentre"),
			     PI,PI.getSideIndex("InnerPlate"));
    }

  offPipeB->createAll(System,*shutterBox,2);
  outerCell=createOuterVoidUnit(System,masterCell,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);
    
  // bellows 
  bellowK->createAll(System,*offPipeB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowK,2);
  bellowK->insertInCell(System,outerCell);
  
  return;
}

void
balderFrontEnd::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("balderFrontEnd","buildObjects");

  int outerCell;
  MonteCarlo::Object& masterCell=constructMasterCell(System);
  
  wigglerBox->createAll(System,*this,0);
  outerCell=createOuterVoidUnit(System,masterCell,*wigglerBox,2);
  wigglerBox->insertInCell(System,outerCell);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->insertInCell(System,outerCell);

  return;
  dipolePipe->setFront(*wigglerBox,2);
  dipolePipe->createAll(System,*wigglerBox,2);
  outerCell=createOuterVoidUnit(System,masterCell,*dipolePipe,2);
  dipolePipe->insertInCell(System,outerCell);

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(dipolePipe->getCell("Void"));
  eCutDisk->createAll(System,*dipolePipe,-2);

  bellowA->createAll(System,*dipolePipe,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  collTubeA->setFront(*bellowA,2);
  collTubeA->createAll(System,*bellowA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collTubeA,2);
  collTubeA->insertAllInCell(System,outerCell);
  
  collA->addInsertCell(collTubeA->getCell("Void"));
  collA->createAll(System,*collTubeA,0);

  bellowB->createAll(System,*collTubeA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  collABPipe->createAll(System,*bellowB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collABPipe,2);
  collABPipe->insertInCell(System,outerCell);

  bellowC->createAll(System,*collABPipe,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);
  

  collTubeB->setFront(*bellowC,2);
  collTubeB->createAll(System,*bellowC,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collTubeB,2);
  collTubeB->insertAllInCell(System,outerCell);

  collB->addInsertCell(collTubeB->getCell("Void"));
  collB->createAll(System,*collTubeB,0);


  collTubeC->setFront(*collTubeB,2);
  collTubeC->createAll(System,*collTubeB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collTubeC,2);
  collTubeC->insertAllInCell(System,outerCell);

  collC->addInsertCell(collTubeC->getCell("Void"));
  collC->createAll(System,*collTubeC,0);


  collExitPipe->setFront(*collTubeC,2);
  collExitPipe->createAll(System,*collTubeC,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collExitPipe,2);
  collExitPipe->insertInCell(System,outerCell);

  buildHeatTable(System,masterCell,*collExitPipe,2);
  buildShutterTable(System,masterCell,*pipeB,2);


  exitPipe->createAll(System,*bellowK,2);
  outerCell=createOuterVoidUnit(System,masterCell,*exitPipe,2);
  exitPipe->insertInCell(System,outerCell);
  
  lastComp=exitPipe;
  return;
}

void
balderFrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*wiggler,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
  
void 
balderFrontEnd::createAll(Simulation& System,
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
  ELog::RegMethod RControl("balderFrontEnd","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();

  std::string Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=frontRule()+backRule();

  addOuterSurf(Out);
  insertObjects(System);

  return;
}


}   // NAMESPACE xraySystem

