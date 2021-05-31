/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/makeTS2Bulk.cxx
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
#include <array>

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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "VoidVessel.h"
#include "World.h"
#include "BulkShield.h"

#include "makeTS2Bulk.h"

namespace moderatorSystem
{

makeTS2Bulk::makeTS2Bulk() :
  VObj(new shutterSystem::VoidVessel("void")),
  BulkObj(new shutterSystem::BulkShield("bulk"))
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeTS2Bulk","Constructor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(VObj);
  OR.addObject(BulkObj);
}

makeTS2Bulk::~makeTS2Bulk()
  /*!
    Destructor
   */
{}



void 
makeTS2Bulk::build(Simulation* SimPtr,
		   const mainSystem::inputParam& IParam,
		   int& excludeCell)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input from command line
   */
{
  // For output stream
  ELog::RegMethod RControl("makeTS2Bulk","build");

  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk"))) ) 
    {
      // void vessel

      VObj->createAll(*SimPtr,World::masterOrigin(),0);
      BulkObj->addInsertCell(excludeCell);
      BulkObj->createAll(*SimPtr,World::masterOrigin(),0);
      excludeCell=BulkObj->getCell("Torpedo");
      //      excludeCell=VObj->getCell("");      
    }
  else
    {
      // makeReflector RefObj;
      // RefObj->createAll(*SimPtr,IParam);
      // RefObj->getRef()->insertAllInCell(System,74123);
    }
	
  return;
}
  

}   // NAMESPACE moderatorSystem
