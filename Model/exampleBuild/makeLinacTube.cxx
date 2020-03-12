/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: makeLinacTube/makeLinacTube.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "insertObject.h"
#include "insertPlate.h"

#include "VacuumPipe.h"
#include "ShieldRoom.h"
#include "vacTube.h"
#include "makeLinacTube.h"

namespace exampleSystem
{

makeLinacTube::makeLinacTube() :
  shieldRoom(new ShieldRoom("LinacRoom")),
  VTube(new vacTube("LTube")),
  scorePlane(new insertSystem::insertPlate("ScorePlane"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(shieldRoom);
  OR.addObject(VTube);
  OR.addObject(scorePlane);
}

makeLinacTube::~makeLinacTube()
  /*!
    Destructor
   */
{}

void 
makeLinacTube::build(Simulation& System,
		     const attachSystem::FixedComp& FCOrigin,
		     const long int sideIndex)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: R3Ring position
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RControl("makeLinacTube","build");

 
  shieldRoom->addInsertCell(74123);
  shieldRoom->createAll(System,FCOrigin,sideIndex);

  VTube->addInsertCell(shieldRoom->getCell("Void"));
  VTube->setCutSurf("front",shieldRoom->getSignedSurf("Front"));
  VTube->setCutSurf("back",shieldRoom->getSignedSurf("#Back"));

  VTube->createAll(System,*shieldRoom,0);

  scorePlane->addInsertCell(74123);
  scorePlane->setNoInsert();
  scorePlane->createAll(System,*shieldRoom,0);

  
  return;
}


}   // NAMESPACE exampleSystem

