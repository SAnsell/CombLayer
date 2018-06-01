/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: balder/COSAXS.cxx
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"
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
#include "CrossPipe.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "FrontEndCave.h"
#include "FrontEnd.h"
#include "cosaxsOpticsLine.h"
#include "ConnectZone.h"
#include "COSAXS.h"

namespace xraySystem
{

COSAXS::COSAXS(const std::string& KN) :
  attachSystem::CopiedComp("Balder",KN),
  frontCave(new FrontEndCave(newName+"FrontEnd")),
  frontBeam(new FrontEnd(newName+"FrontBeam")),
  joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe")),
  opticsHut(new OpticsHutch(newName+"OpticsHut")),
  opticsBeam(new cosaxsOpticsLine(newName+"OpticsLine")),
  joinPipeB(new constructSystem::VacuumPipe(newName+"JoinPipeB"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(frontCave);
  OR.addObject(frontBeam);
  OR.addObject(joinPipe);
  
  OR.addObject(opticsHut);
  OR.addObject(opticsBeam);
  OR.addObject(joinPipeB);
  
}

COSAXS::~COSAXS()
  /*!
    Destructor
   */
{}

void 
COSAXS::build(Simulation& System,
		  const attachSystem::FixedComp& FCOrigin,
		  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
   */
{
  // For output stream
  ELog::RegMethod RControl("COSAXS","build");

  int voidCell(74123);
 
  frontCave->addInsertCell(voidCell);
  frontCave->createAll(System,FCOrigin,sideIndex);
  const HeadRule caveVoid=frontCave->getCellHR(System,"Void");
  
  frontBeam->addInsertCell(frontCave->getCell("Void"));
  frontBeam->createAll(System,*frontCave,-1);

  opticsHut->addInsertCell(voidCell);
  opticsHut->createAll(System,*frontCave,2);

  joinPipe->addInsertCell(frontCave->getCell("Void"));
  joinPipe->addInsertCell(frontCave->getCell("FrontWallHole"));
  joinPipe->addInsertCell(opticsHut->getCell("Void"));
  
  joinPipe->setPrimaryCell(opticsHut->getCell("Void"));
  joinPipe->setFront(*frontBeam,2);
  joinPipe->setSpaceLinkCopy(0,*opticsHut,1);
  joinPipe->registerSpaceCut(0,2);
  joinPipe->createAll(System,*frontBeam,2);

  joinPipe->clear();
  joinPipe->setPrimaryCell(caveVoid);
  joinPipe->registerSpaceCut(1,0);
  joinPipe->insertObjects(System);

  System.removeCell(frontCave->getCell("Void"));

  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->createAll(System,*joinPipe,2);

  joinPipeB->addInsertCell(opticsHut->getCell("ExitHole"));
  joinPipeB->setPrimaryCell(opticsHut->getCell("Void"));
  joinPipeB->setFront(*opticsBeam,2);
  joinPipeB->setSpaceLinkCopy(1,*opticsHut,
			 opticsHut->getSideIndex("-innerBack"));
  joinPipeB->registerSpaceCut(1,0);
  joinPipeB->createAll(System,*opticsBeam,2);

  System.removeCell(opticsHut->getCell("Void"));

  return;
}


}   // NAMESPACE xraySystem

