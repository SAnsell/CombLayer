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

#include "VacuumPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
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
  heatBox(new constructSystem::PortTube(newName+"HeatBox")),
  heatTopFlange(new xraySystem::FlangeMount(newName+"HeatTopFlange")),
  bellowD(new constructSystem::Bellows(newName+"BellowD")),
  gateTubeA(new constructSystem::PipeTube(newName+"GateTubeA")),
  ionPB(new constructSystem::CrossPipe(newName+"IonPB")),
  pipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  bellowE(new constructSystem::Bellows(newName+"BellowE")),
  aperturePipe(new constructSystem::VacuumPipe(newName+"AperturePipe")),
  bellowF(new constructSystem::Bellows(newName+"BellowF")),
  ionPC(new constructSystem::CrossPipe(newName+"IonPC"))
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
maxpeemFrontEnd::buildHeatTable(Simulation& System)
  /*!
    Build the heatDump table
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildAppTableOne");

  heatBox->addInsertCell(ContainedComp::getInsertCells()[1]);
  heatBox->setPortRotation(3,Geometry::Vec3D(1,0,0));
  heatBox->createAll(System,*heatPipe,2);

  heatTopFlange->addInsertCell("Flange",ContainedComp::getInsertCells()[1]);
  heatTopFlange->setBladeCentre(*heatBox,0);
  heatTopFlange->createAll(System,*heatBox,2);

  const constructSystem::portItem& PI=heatBox->getPort(1);  
  bellowD->addInsertCell(ContainedComp::getInsertCells()[1]);
  bellowD->createAll(System,PI,PI.getSideIndex("OuterPlate"));

  gateTubeA->addInsertCell(ContainedComp::getInsertCells()[1]);
  gateTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  gateTubeA->createAll(System,*bellowD,2);  

  return;
  const constructSystem::portItem& GPI=gateTubeA->getPort(1);
  ionPB->addInsertCell(ContainedComp::getInsertCells()[1]);
  ionPB->createAll(System,GPI,GPI.getSideIndex("OuterPlate"));

  //  pipeB->addInsertCell(ContainedComp::getInsertCells()[1]);
  //  pipeB->createAll(System,*ionPB,2);
  
  return;
  
}

void
maxpeemFrontEnd::buildAppTableOne(Simulation& System)
  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("maxpeemFrontEnd","buildAppTableOne");

  aperturePipe->addInsertCell(ContainedComp::getInsertCells()[1]);
  aperturePipe->createAll(System,*pipeB,2);

  // bellows AFTER movable aperture pipe
  bellowE->addInsertCell(ContainedComp::getInsertCells()[1]);
  bellowF->setFront(*pipeB,2);
  bellowE->setBack(*aperturePipe,1);
  bellowE->createAll(System,*pipeB,2);

  ionPC->addInsertCell(ContainedComp::getInsertCells()[1]);
  ionPC->createAll(System,*pipeB,2);
  
  // bellows AFTER aperature ionpump and ion pump
  bellowF->addInsertCell(ContainedComp::getInsertCells()[1]);
  bellowF->setFront(*aperturePipe,2);
  bellowF->setFront(*ionPC,1);
  bellowF->createAll(System,*pipeB,2);

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

  wigglerBox->addInsertCell(ContainedComp::getInsertCells()[0]);
  //  wigglerBox->registerSpaceCut(0,2);
  wigglerBox->createAll(System,*this,0);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);

  dipolePipe->addInsertCell(ContainedComp::getInsertCells()[0]);
  //  dipolePipe->registerSpaceCut(1,2);
  dipolePipe->setFront(*wigglerBox,2);
  dipolePipe->createAll(System,*wigglerBox,2);

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(dipolePipe->getCell("Void"));
  eCutDisk->createAll(System,*dipolePipe,-2);

  bellowA->addInsertCell(ContainedComp::getInsertCells()[0]);
  //  bellowA->registerSpaceCut(1,2);
  bellowA->createAll(System,*dipolePipe,2);


  collA->addInsertCell(ContainedComp::getInsertCells()[0]);
  collA->createAll(System,*bellowA,2);

  bellowB->addInsertCell(ContainedComp::getInsertCells()[0]);
  bellowB->createAll(System,*collA,2);

  ionPA->addInsertCell(ContainedComp::getInsertCells()[0]);
  ionPA->createAll(System,*bellowB,2);

  bellowC->addInsertCell(ContainedComp::getInsertCells()[0]);
  bellowC->createAll(System,*ionPA,2);

  heatPipe->addInsertCell(ContainedComp::getInsertCells());
  heatPipe->createAll(System,*bellowC,2);

  buildHeatTable(System);


  //  buildAppTableOne(System);
  
  lastComp=wigglerBox;
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
  buildObjects(System);
  createLinks();
  return;
}

}   // NAMESPACE xraySystem

