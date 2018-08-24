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
  offPipeB(new constructSystem::OffsetFlangePipe(newName+"OffPipeB")),
  gateA(new constructSystem::GateValve(newName+"GateA")),
  pipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  pipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  slitTube(new constructSystem::PipeTube(newName+"SlitTube"))
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
  OR.addObject(florTubeA);
  OR.addObject(bellowC);
  OR.addObject(pipeB);
  OR.addObject(pumpTubeA);
  OR.addObject(offPipeA);
  OR.addObject(M1Tube);
  OR.addObject(offPipeB);
  OR.addObject(gateA);
  OR.addObject(pipeC);
  OR.addObject(pipeD);
  OR.addObject(slitTube);
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
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*180.0,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  return;
}

  
MonteCarlo::Object&
maxpeemOpticsBeamline::constructMasterCell
(Simulation& System,const HeadRule& cutHR)
 /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param cutHR :: Start of cut fixed comp
    \return cell object
  */
{
  ELog::RegMethod RegA("maxpeemOpticsBeamline","constructMasterCell");

  std::string Out;

  const int CN=getCell("MasterVoid");
  MonteCarlo::Object& MCell=*System.findQhull(CN);    
  if (cutHR.hasRule())
    {
      const int SN=getRule("back").getPrimarySurface();
      MCell.removeSurface(SN);
      MCell.addSurfString(cutHR.display());

      Out=cutHR.complement().display()+getRuleStr("back")+getRuleStr("beam");
      makeCell("MasterVoid",System,cellIndex++,0,0.0,Out);
      
      return *System.findQhull(cellIndex-1);
    }
  
  return MCell;
}

int
maxpeemOpticsBeamline::createOuterVoidUnit(Simulation& System,
					   MonteCarlo::Object& masterCell,
					   HeadRule& divider,
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


void
maxpeemOpticsBeamline::refrontMasterCell(MonteCarlo::Object& MCell,
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
  ELog::RegMethod RegA("maxpeemOpticsBeamline","refrontMasterCell");

  std::string Out;  
  Out=getRuleStr("beam")+getRuleStr("back")+
    FC.getLinkString(sideIndex);
  MCell.procString(Out);
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
maxpeemOpticsBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \param MCell :: Master cell to use
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
  MonteCarlo::Object& masterCell=constructMasterCell(System,divider);
  divider.makeComplement();
  outerCell=createOuterVoidUnit(System,masterCell,divider,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  ionPA->createAll(System,*bellowA,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*ionPA,2);
  ionPA->insertInCell(System,outerCell);
    
  // FAKE insertcell: reqruired
  gateTubeA->addInsertCell(masterCell.getName());
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*ionPA,2);  

  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,divider,
  				GPI,GPI.getSideIndex("OuterPlate"));
  gateTubeA->insertInCell(System,outerCell);

  bellowB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*bellowB,2);
  bellowB->insertInCell(System,outerCell);

  pipeA->createAll(System,*bellowB,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeA,2);
  pipeA->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  florTubeA->addInsertCell(masterCell.getName());
  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*pipeA,2);  

  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,divider,
  				FPI,FPI.getSideIndex("OuterPlate"));
  florTubeA->insertInCell(System,outerCell);

  bellowC->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*bellowC,2);
  bellowC->insertInCell(System,outerCell);

  pipeB->createAll(System,*bellowC,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeB,2);
  pipeB->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  pumpTubeA->addInsertCell(masterCell.getName());
  pumpTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  pumpTubeA->createAll(System,*pipeB,2);

  const constructSystem::portItem& CPI=pumpTubeA->getPort(1);
  outerCell=createOuterVoidUnit(System,masterCell,divider,
  				CPI,CPI.getSideIndex("OuterPlate"));
  pumpTubeA->insertInCell(System,outerCell);
  pumpTubeA->intersectPorts(System,1,2);

  offPipeA->createAll(System,CPI,CPI.getSideIndex("OuterPlate"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*offPipeA,2);
  offPipeA->insertInCell(System,outerCell);

  M1Tube->createAll(System,*offPipeA,offPipeA->getSideIndex("FlangeBCentre"));
  outerCell=createOuterVoidUnit(System,masterCell,divider,*M1Tube,2);
  M1Tube->insertInCell(System,outerCell);

  offPipeB->createAll(System,*M1Tube,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*offPipeB,2);
  offPipeB->insertInCell(System,outerCell);

  gateA->createAll(System,*offPipeB,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*gateA,2);
  gateA->insertInCell(System,outerCell);

  pipeC->createAll(System,*gateA,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeC,2);
  pipeC->insertInCell(System,outerCell);

  pipeD->createAll(System,*pipeC,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*pipeD,2);
  pipeD->insertInCell(System,outerCell);

  // FAKE insertcell: reqruired
  slitTube->addInsertCell(masterCell.getName());
  slitTube->createAll(System,*pipeD,2);
  outerCell=createOuterVoidUnit(System,masterCell,divider,*slitTube,2);
  slitTube->insertInCell(System,outerCell);

  slitTube->splitVoidPorts(System,"SplitVoid",1001,
			   slitTube->getCell("Void"),
			   Geometry::Vec3D(0,1,0));

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

