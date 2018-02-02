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
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PortTube.h"

#include "OpticsHutch.h"
#include "CrossPipe.h"
#include "MonoVessel.h"
#include "MonoCrystals.h"
#include "GateValve.h"
#include "JawUnit.h"
#include "JawValve.h"
#include "FlangeMount.h"
#include "OpticsBeamline.h"
#include "makeBalder.h"

namespace xraySystem
{

makeBalder::makeBalder() :
  opticsHut(new OpticsHutch("BalderOptics")),
  opticsBeam(new OpticsBeamline("Balder"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(opticsHut);
  
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
 
  opticsHut->addInsertCell(voidCell);
  opticsHut->createAll(System,World::masterOrigin(),0);

  opticsBeam->addInsertCell(opticsHut->getCell("Void"));
  opticsBeam->createAll(System,World::masterOrigin(),0);
  
  return;
}


}   // NAMESPACE xraySystem

