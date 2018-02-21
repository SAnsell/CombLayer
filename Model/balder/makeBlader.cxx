/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/makeBalder.cxx
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
#include "ContainedGroup.h"
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
#include "LeadPipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PortTube.h"

#include "OpticsHutch.h"
#include "ExperimentalHutch.h"
#include "CrossPipe.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "FlangeMount.h"
#include "FrontEndCave.h"
#include "Wiggler.h"
#include "OpticsBeamline.h"
#include "ConnectZone.h"
#include "makeBalder.h"

namespace xraySystem
{

makeBalder::makeBalder() :
  frontEnd(new FrontEndCave("BalderFrontEnd")),
  wigglerBox(new constructSystem::VacuumBox("BalderWigglerBox",1)),
  wiggler(new Wiggler("BalderWiggler")),
  joinPipe(new constructSystem::VacuumPipe("BalderJoinPipe")),
  opticsHut(new OpticsHutch("BalderOptics")),
  opticsBeam(new OpticsBeamline("Balder")),
  joinPipeB(new constructSystem::LeadPipe("BalderJoinPipeB")),
  connectZone(new ConnectZone("BalderConnect")),
  joinPipeC(new constructSystem::LeadPipe("BalderJoinPipeC")),
  exptHut(new ExperimentalHutch("BalderExpt"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(frontEnd);
  OR.addObject(wigglerBox);
  OR.addObject(wiggler);
  OR.addObject(joinPipe);
  OR.addObject(opticsHut);
  OR.addObject(joinPipeB);
  OR.addObject(joinPipeC);
  OR.addObject(exptHut);
}

makeBalder::~makeBalder()
  /*!
    Destructor
   */
{}

void 
makeBalder::build(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeBalder","build");

  int voidCell(74123);
 
  frontEnd->addInsertCell(voidCell);
  frontEnd->createAll(System,World::masterOrigin(),0);

  wigglerBox->addInsertCell(frontEnd->getCell("Void"));
  wigglerBox->createAll(System,*frontEnd,0);

  wiggler->addInsertCell(wigglerBox->getCell("Void"));
  wiggler->createAll(System,*wigglerBox,0);

  opticsHut->addInsertCell(voidCell);
  opticsHut->createAll(System,*frontEnd,2);

  joinPipe->addInsertCell(frontEnd->getCell("Void"));
  joinPipe->addInsertCell(frontEnd->getCell("FrontWallHole"));
  joinPipe->addInsertCell(opticsHut->getCell("Void"));
  joinPipe->setFront(*wigglerBox,2);
  joinPipe->createAll(System,*wigglerBox,2);

  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->createAll(System,*joinPipe,2);

  joinPipeB->addInsertCell(voidCell);
  joinPipeB->addInsertCell(opticsHut->getCell("Void"));
  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->createAll(System,*opticsBeam,2);

  connectZone->addInsertCell(voidCell);
  connectZone->createAll(System,*joinPipeB,2);

  exptHut->addInsertCell(voidCell);
  exptHut->createAll(System,*frontEnd,2);
  
  joinPipeC->addInsertCell(voidCell);
  joinPipeC->addInsertCell(exptHut->getCell("Void"));
  joinPipeC->addInsertCell(exptHut->getCell("EnteranceHole"));
  joinPipeC->setFront(*connectZone,2);
  joinPipeC->createAll(System,*connectZone,2);

  return;
}


}   // NAMESPACE xraySystem

