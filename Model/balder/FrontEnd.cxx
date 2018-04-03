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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
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

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PortTube.h"
#include "Wiggler.h"
#include "SqrCollimator.h"

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
  collTubeA(new constructSystem::PortTube(newName+"CollimatorTubeA")),
  collA(new xraySystem::SqrCollimator(newName+"CollA")),
  collABPipe(new constructSystem::VacuumPipe(newName+"CollABPipe")),
  collTubeB(new constructSystem::PortTube(newName+"CollimatorTubeB")),
  collB(new xraySystem::SqrCollimator(newName+"CollB")),  
  flightPipe(new constructSystem::VacuumPipe(newName+"FlightPipe"))
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
  OR.addObject(collTubeA);
  OR.addObject(collA);
  OR.addObject(collABPipe);
  OR.addObject(collTubeB);
  OR.addObject(collB);
  OR.addObject(flightPipe);
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
  wigglerBox->registerSpaceCut(0,2);
  wigglerBox->createAll(System,*this,0);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);

  dipolePipe->addInsertCell(ContainedComp::getInsertCells());
  dipolePipe->registerSpaceCut(1,2);
  dipolePipe->setFront(*wigglerBox,2);
  dipolePipe->createAll(System,*wigglerBox,2);

  collTubeA->addInsertCell(ContainedComp::getInsertCells());
  collTubeA->registerSpaceCut(1,2);
  collTubeA->setFront(*dipolePipe,2);
  collTubeA->createAll(System,*dipolePipe,2);

  collA->addInsertCell(collTubeA->getCell("Void"));
  collA->createAll(System,*collTubeA,0);

  collABPipe->addInsertCell(ContainedComp::getInsertCells());
  collABPipe->registerSpaceCut(1,2);
  collABPipe->setFront(*collTubeA,2);
  collABPipe->createAll(System,*collTubeA,2);

  collTubeB->addInsertCell(ContainedComp::getInsertCells());
  collTubeB->registerSpaceCut(1,2);
  collTubeB->setFront(*collABPipe,2);
  collTubeB->createAll(System,*collABPipe,2);

  collB->addInsertCell(collTubeB->getCell("Void"));
  collB->createAll(System,*collTubeB,0);

  flightPipe->addInsertCell(ContainedComp::getInsertCells());
  flightPipe->registerSpaceCut(1,2);
  flightPipe->setFront(*collTubeB,2);
  flightPipe->createAll(System,*collTubeB,2);
  
  lastComp=flightPipe;

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

