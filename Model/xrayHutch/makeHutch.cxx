/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayHutch/makeHutch.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "xrayHutch.h"
#include "xrayTarget.h"

#include "makeHutch.h"

namespace xrayHutSystem
{

makeHutch::makeHutch() :
  hut(new xrayHutSystem::xrayHutch("Hut")),
  target(new xrayHutSystem::xrayTarget("Target"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(hut);
  OR.addObject(target);
}

makeHutch::~makeHutch()
  /*!
    Destructor
   */
{}

void 
makeHutch::build(Simulation& System,
		 const mainSystem::inputParam&)
/*!
  Carry out the full build
  \param System :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makeHutch","build");

  int voidCell(74123);

  hut->setInsertCell(voidCell);
  hut->createAll(System,World::masterOrigin(),0);

  target->setInsertCell(hut->getCell("InnerVoid"));
  target->createAll(System,*hut,"Origin");
  return;
}
    

}   // NAMESPACE pipeSystem

