/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/maxpeemFrontEnd.cxx
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "CSGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "VacuumPipe.h"
#include "OffsetFlangePipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "GateValve.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "CrossPipe.h"
#include "Wiggler.h"
#include "SquareFMask.h"
#include "FlangeMount.h"
#include "HeatDump.h"

#include "maxpeemFrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
maxpeemFrontEnd::maxpeemFrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),

  wigglerBox(new constructSystem::VacuumBox(newName+"WigglerBox",1)),
  wiggler(new Wiggler(newName+"Wiggler")),
  dipolePipe(new constructSystem::VacuumPipe(newName+"DipolePipe")),
  eCutDisk(new insertSystem::insertCylinder(newName+"ECutDisk")),  
  bellowA(new constructSystem::Bellows(newName+"BellowA")),
  collA(new xraySystem::SquareFMask(newName+"CollA")),
  bellowB(new constructSystem::Bellows(newName+"BellowB")),
  ionPA(new constructSystem::CrossPipe(newName+"IonPA")),
  bellowC(new constructSystem::Bellows(newName+"BellowC")),
  heatPipe(new constructSystem::VacuumPipe(newName+"HeatPipe")),
  heatBox(new constructSystem::PipeTube(newName+"HeatBox")),
  heatTopFlange(new xraySystem::FlangeMount(newName+"HeatTopFlange")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  ionPB(new constructSystem::CrossPipe(newName+"IonPB")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  aperturePipe(new constructSystem::VacuumPipe(newName+"AperturePipe")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  ionPC(new constructSystem::CrossPipe(newName+"IonPC")),
  bellowG(new constructSystem::Bellows(newName+"BellowG")),
  aperturePipeB(new constructSystem::VacuumPipe(newName+"AperturePipeB")),
  bellowH(new constructSystem::Bellows(newName+"BellowH")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  bellowI(new constructSystem::Bellows(newName+"BellowI")),
  florTubeA(new constructSystem::PipeTube(newName+"FlorTubeA")),
  bellowJ(new constructSystem::Bellows(newName+"BellowJ")),
  gateTubeB(new constructSystem::PipeTube(newName+"GateTubeB")),
  offPipeA(new constructSystem::OffsetFlangePipe(newName+"OffPipeA")),
  shutterBox(new constructSystem::PipeTube(newName+"ShutterBox")),
  shutters({
      std::make_shared<xraySystem::FlangeMount>(newName+"Shutter0"),
      std::make_shared<xraySystem::FlangeMount>(newName+"Shutter1")
	})
  
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(wigglerBox);
  OR.addObject(wiggler);
  OR.addObject(dipolePipe);
  OR.addObject(eCutDisk);
  OR.addObject(bellowA);
  OR.addObject(collA);
  OR.addObject(bellowB);
  OR.addObject(ionPA);
  OR.addObject(bellowC);
  OR.addObject(heatPipe);
  OR.addObject(heatBox);
  OR.addObject(heatTopFlange);
  OR.addObject(bellowD);
  OR.addObject(gateTubeA);
  OR.addObject(ionPB);
  OR.addObject(pipeB);
  OR.addObject(bellowE);
  OR.addObject(aperturePipe);
  OR.addObject(bellowF);
  OR.addObject(ionPC);
  OR.addObject(bellowG);
  OR.addObject(aperturePipeB);
  OR.addObject(bellowH);
  OR.addObject(pipeC);
  OR.addObject(gateA);
  OR.addObject(bellowI);
  OR.addObject(florTubeA);
  OR.addObject(bellowJ);
  OR.addObject(gateTubeB);
  OR.addObject(offPipeA);
  OR.addObject(shutterBox);
  OR.addObject(shutters[0]);
  OR.addObject(shutters[1]);

}
  
maxpeemFrontEnd::~maxpeemFrontEnd()
  /*!
    Destructor
   */
{}

void
maxpeemFrontEnd::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);
  outerRadius=Control.EvalDefVar<double>(keyName+"OuterRadius",0.0);
  return;
}

void
maxpeemFrontEnd::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
maxpeemFrontEnd::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  return;
}

int
maxpeemFrontEnd::createOuterVoidUnit(Simulation& System,
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
  ELog::RegMethod RegA("ConnectZone","createOuterVoid");

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
maxpeemFrontEnd::refrontMasterCell(MonteCarlo::Object& MCell,
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
  ELog::RegMethod RegA("maxpeemFrontEnd","refrontMasterCell");

  std::string Out;  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=backRule()+FC.getLinkString(sideIndex);
  MCell.procString(Out);
  return;
}
 
MonteCarlo::Object&
maxpeemFrontEnd::constructMasterCell(Simulation& System)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \return cell object
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","constructMasterCell");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  Out+=frontRule()+backRule();
  makeCell("MasterVoid",System,cellIndex++,0,0.0,Out);
  addOuterSurf(Out);
  insertObjects(System);

  return *System.findQhull(cellIndex-1);

}
   
void
maxpeemFrontEnd::buildHeatTable(Simulation& System,
				MonteCarlo::Object& masterCell)
  /*!
    Build the heatDump table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildHeatTable");

  int outerCell;
  // FAKE insertcell:
  heatBox->addInsertCell(masterCell.getName());
  heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  heatBox->createAll(System,*heatPipe,2);

  const constructSystem::portItem& PI=heatBox->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				PI,PI.getSideIndex("OuterPlate"));
  heatBox->insertInCell(System,outerCell);
    
  heatTopFlange->addInsertCell("Flange",outerCell);
  heatTopFlange->setBladeCentre(*heatBox,0);
  heatTopFlange->createAll(System,*heatBox,2);

  //  const constructSystem::portItem& PI=heatBox->getPort(1);  
  bellowD->createAll(System,PI,PI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*bellowD,2);
  bellowD->insertInCell(System,outerCell);


  // FAKE insertcell:
  gateTubeA->addInsertCell(masterCell.getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*bellowD,2);  

  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertInCell(System,outerCell);
  
  ionPB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*ionPB,2);
  ionPB->insertInCell(System,outerCell);

  pipeB->createAll(System,*ionPB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*pipeB,2);
  pipeB->insertInCell(System,outerCell);
  
  return;
  
}

void
maxpeemFrontEnd::buildApertureTable(Simulation& System,
				    MonteCarlo::Object& masterCell)
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildApertureTable");

  int outerCell;
  // NOTE order for master cell [Next 4 object
  aperturePipe->createAll(System,*pipeB,2);

  // bellows AFTER movable aperture pipe
  bellowE->setFront(*pipeB,2);
  bellowE->setBack(*aperturePipe,1);
  bellowE->createAll(System,*pipeB,2);

  ionPC->createAll(System,*pipeB,2);

  // bellows AFTER aperature ionpump and ion pump
  bellowF->setFront(*aperturePipe,2);
  bellowF->setBack(*ionPC,1);
  bellowF->createAll(System,*pipeB,2);

  // now do insert:
  outerCell=createOuterVoidUnit(System,masterCell,*bellowE,2);
  bellowE->insertInCell(System,outerCell);
    
  outerCell=createOuterVoidUnit(System,masterCell,*aperturePipe,2);
  aperturePipe->insertInCell(System,outerCell);
  
  outerCell=createOuterVoidUnit(System,masterCell,*bellowF,2);
  bellowF->insertInCell(System,outerCell);

  outerCell=createOuterVoidUnit(System,masterCell,*ionPC,2);
  ionPC->insertInCell(System,outerCell);


  // Next 4 objects need to be build before insertion
  aperturePipeB->createAll(System,*ionPC,2);

  // bellows AFTER movable aperture pipe
  bellowG->setFront(*ionPC,2);
  bellowG->setBack(*aperturePipeB,1);
  bellowG->createAll(System,*ionPC,2);

  pipeC->createAll(System,*ionPC,2);

  // bellows AFTER movable aperture pipe
  bellowH->setFront(*aperturePipeB,2);
  bellowH->setBack(*pipeC,1);
  bellowH->createAll(System,*ionPC,2);


  // now do insert:
  outerCell=createOuterVoidUnit(System,masterCell,*bellowG,2);
  bellowG->insertInCell(System,outerCell);
    
  outerCell=createOuterVoidUnit(System,masterCell,*aperturePipeB,2);
  aperturePipeB->insertInCell(System,outerCell);
  
  outerCell=createOuterVoidUnit(System,masterCell,*bellowH,2);
  bellowH->insertInCell(System,outerCell);

  outerCell=createOuterVoidUnit(System,masterCell,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  
  return;
}

void
maxpeemFrontEnd::buildShutterTable(Simulation& System,
				   MonteCarlo::Object& masterCell)
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param masterCell :: Main cell for insertion
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildShutterTable");
  int outerCell;
  
  gateA->createAll(System,*pipeC,2);
  outerCell=createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  // bellows 
  bellowI->createAll(System,*gateA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowI,2);
  bellowI->insertInCell(System,outerCell);

  // FAKE insertcell:
  florTubeA->addInsertCell(masterCell.getName());
  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,2);
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertInCell(System,outerCell);

  // bellows 
  bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*bellowJ,2);
  bellowJ->insertInCell(System,outerCell);

  // FAKE insertcell:
  gateTubeB->addInsertCell(masterCell.getName());
  gateTubeB->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeB->createAll(System,*bellowJ,2);  
  const constructSystem::portItem& GPI=gateTubeB->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,
				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeB->insertInCell(System,outerCell);


  offPipeA->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);


  shutterBox->delayPorts();
  shutterBox->createAll(System,*offPipeA,
			offPipeA->getSideIndex("FlangeBCentre"));
  outerCell=createOuterVoidUnit(System,masterCell,*shutterBox,2);
  shutterBox->insertInCell(System,outerCell);

  /*
  shutterBox->splitVoidPorts(System,"SplitVoid",1001,
  			     shutterBox->getCell("Void"),
			     {0,1});
  //  shutterBox->splitVoidPorts(System,"SplitOuter",2001,
  //                            shutterBox->getBuildCell(),
  //			         {0,1});

  shutterBox->createPorts(System);
  
  for(size_t i=0;i<shutters.size();i++)
    {
      const constructSystem::portItem& PI=shutterBox->getPort(i);
      shutters[i]->addInsertCell("Flange",ContainedComp::getInsertCells()[1]);

      // shutters[i]->addInsertCell("Flange",shutterBox->getCell("SplitOuter",i));
      shutters[i]->addInsertCell("Body",PI.getCell("Void"));
      shutters[i]->addInsertCell("Body",shutterBox->getCell("SplitVoid",i));
      shutters[i]->setBladeCentre(PI,0);
      shutters[i]->createAll(System,PI,2);
    }
  */
  return;
}

  
void
maxpeemFrontEnd::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildObjects");

  int outerCell;
  //  wigglerBox->addInsertCell(ContainedComp::getInsertCells()[0]);
  //  wigglerBox->registerSpaceCut(0,2);
  MonteCarlo::Object& masterCell=constructMasterCell(System);
  
  wigglerBox->createAll(System,*this,0);
  outerCell=createOuterVoidUnit(System,masterCell,*wigglerBox,2);
  wigglerBox->insertInCell(System,outerCell);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);

  dipolePipe->setFront(*wigglerBox,2);
  dipolePipe->createAll(System,*wigglerBox,2);
  outerCell=createOuterVoidUnit(System,masterCell,*dipolePipe,2);
  dipolePipe->insertInCell(System,outerCell);

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(dipolePipe->getCell("Void"));
  eCutDisk->createAll(System,*dipolePipe,-2);

  //  bellowA->registerSpaceCut(1,2);
  bellowA->createAll(System,*dipolePipe,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  collA->createAll(System,*bellowA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*collA,2);
  collA->insertInCell(System,outerCell);
  
  bellowB->createAll(System,*collA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowB,2);
  outerCell=createOuterVoidUnit(System,masterCell,*ionPA,2);
  ionPA->insertInCell(System,outerCell);

  bellowC->createAll(System,*ionPA,2);
  outerCell=createOuterVoidUnit(System,masterCell,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  heatPipe->createAll(System,*bellowC,2);
  outerCell=createOuterVoidUnit(System,masterCell,*heatPipe,2);
  heatPipe->insertInCell(System,outerCell);

  lastComp=heatPipe;

  buildHeatTable(System,masterCell);  
  buildApertureTable(System,masterCell);
  buildShutterTable(System,masterCell);
  lastComp=pipeC;
  
  return;
}

void
maxpeemFrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*wiggler,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
void 
maxpeemFrontEnd::createAll(Simulation& System,
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
  ELog::RegMethod RControl("maxpeemFrontEnd","build");

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

