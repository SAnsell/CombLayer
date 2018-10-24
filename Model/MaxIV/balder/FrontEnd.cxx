/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/FrontEnd.cxx
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

#include "VacuumPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "Wiggler.h"
#include "SqrCollimator.h"
#include "FlangeMount.h"
#include "HeatDump.h"

#include "FrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
FrontEnd::FrontEnd(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),

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
  flightPipe(new constructSystem::VacuumPipe(newName+"FlightPipe")),
  shutterBox(new constructSystem::PortTube(newName+"ShutterBox")),
  shutters({
      std::make_shared<xraySystem::FlangeMount>(newName+"Shutter0"),
      std::make_shared<xraySystem::FlangeMount>(newName+"Shutter1")
	}),
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
  OR.addObject(flightPipe);
  OR.addObject(shutterBox);
  OR.addObject(shutters[0]);
  OR.addObject(shutters[1]);
  OR.addObject(exitPipe);
}
  
FrontEnd::~FrontEnd()
  /*!
    Destructor
   */
{}

void
FrontEnd::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  FixedOffset::populate(Control);
  return;
}

void
FrontEnd::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("FrontEnd","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}


void
FrontEnd::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("FrontEnd","buildObjects");

  wigglerBox->addInsertCell(ContainedComp::getInsertCells());
  wigglerBox->createAll(System,*this,0);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);

  dipolePipe->addInsertCell(ContainedComp::getInsertCells());
  dipolePipe->setFront(*wigglerBox,2);
  dipolePipe->createAll(System,*wigglerBox,2);

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(dipolePipe->getCell("Void"));
  eCutDisk->createAll(System,*dipolePipe,-2);

  bellowA->addInsertCell(ContainedComp::getInsertCells());
  bellowA->createAll(System,*dipolePipe,2);

  collTubeA->addAllInsertCell(ContainedComp::getInsertCells());
  collTubeA->setFront(*bellowA,2);  // needed to allow bend if miss aligned
  collTubeA->createAll(System,*bellowA,2);  

  collA->addInsertCell(collTubeA->getCell("Void"));
  collA->createAll(System,*collTubeA,0);

  bellowB->addInsertCell(ContainedComp::getInsertCells());
  bellowB->createAll(System,*collTubeA,2);

  collABPipe->addInsertCell(ContainedComp::getInsertCells());
  collABPipe->createAll(System,*bellowB,2);

  bellowC->addInsertCell(ContainedComp::getInsertCells());
  bellowC->createAll(System,*collABPipe,2);

  collTubeB->addAllInsertCell(ContainedComp::getInsertCells());
  collTubeB->createAll(System,*bellowC,2);

  collB->addInsertCell(collTubeB->getCell("Void"));
  collB->createAll(System,*collTubeB,0);


  collTubeC->addAllInsertCell(ContainedComp::getInsertCells());
  collTubeC->createAll(System,*collTubeB,2);

  collC->addInsertCell(collTubeC->getCell("Void"));
  collC->createAll(System,*collTubeC,0);


  collExitPipe->addInsertCell(ContainedComp::getInsertCells());
  collExitPipe->createAll(System,*collTubeC,2);


  // FAKE insertcell:
  heatBox->addAllInsertCell(ContainedComp::getInsertCells());
  heatBox->createAll(System,*collExitPipe,2);

  
  const constructSystem::portItem& PI=heatBox->getPort(0);    
  heatDump->addInsertCell("Inner",heatBox->getCell("Void"));
  heatDump->addInsertCell("Inner",PI.getCell("Void"));
  heatDump->addInsertCell("Outer",heatBox->getCell("OuterSpace"));
  heatDump->createAll(System,*heatBox,0,PI,2);

  flightPipe->addInsertCell(ContainedComp::getInsertCells());
  flightPipe->createAll(System,*heatBox,2);

  shutterBox->addAllInsertCell(ContainedComp::getInsertCells());
  shutterBox->delayPorts();
  shutterBox->createAll(System,*flightPipe,2);

  shutterBox->splitVoidPorts(System,"SplitVoid",1001,
  			     shutterBox->getCell("Void"),
			     {0,1});
  //  shutterBox->splitVoidPorts(System,"SplitOuter",2001,
  //			     shutterBox->getBuildCell(),
  //			     {0,1});

  shutterBox->createPorts(System);
  
  for(size_t i=0;i<shutters.size();i++)
    {
      const constructSystem::portItem& PI=shutterBox->getPort(i);
      shutters[i]->addInsertCell("Flange",shutterBox->getCell("SplitOuter",i));
      shutters[i]->addInsertCell("Body",PI.getCell("Void"));
      shutters[i]->addInsertCell("Body",shutterBox->getCell("SplitVoid",i));
      shutters[i]->setBladeCentre(PI,0);
      shutters[i]->createAll(System,PI,2);
    }

  exitPipe->addInsertCell(ContainedComp::getInsertCells());
  exitPipe->createAll(System,*shutterBox,2);
  
  lastComp=exitPipe;
  return;
}

void
FrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*wiggler,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
  
void 
FrontEnd::createAll(Simulation& System,
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
  ELog::RegMethod RControl("FrontEnd","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

