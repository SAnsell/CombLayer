/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/makeTS2.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "inputParam.h"
#include "objectRegister.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "World.h"

#include "BulkShield.h"
#include "VoidVessel.h"
#include "ReflectorAssembly.h"

#include "makeTS2.h"

namespace moderatorSystem
{

makeTS2::makeTS2() :
  RefObj(new ReflectorAssembly("ReflectorAssembly")),
  VObj(new shutterSystem::VoidVessel("void")),
  BulkObj(new shutterSystem::BulkShield("bulk"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(RefObj);
  OR.addObject(VObj);
  OR.addObject(BulkObj);
}

makeTS2::~makeTS2()
/*!
  Destructor
 */
{}

void 
makeTS2::build(Simulation* SimPtr,
	       const mainSystem::inputParam& IParam)
  
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeTS2","build");

  int voidCell(74123);

  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk"))) ) 
    {
      BulkObj->addInsertCell(voidCell);
      BulkObj->createAll(*SimPtr,World::masterOrigin(),0);
      
      VObj->addInsertCell(BulkObj->getCell("Torpedo"));
      VObj->addInsertCell(BulkObj->getCell("Shutter"));
      VObj->createAll(*SimPtr,World::masterOrigin(),0);
      
      BulkObj->setCutSurf("Inner",VObj->getOuterSurf());
      BulkObj->createTorpedoes(*SimPtr);

      RefObj->addInsertCell(VObj->getCell("Void"));
      RefObj->createAll(*SimPtr,World::masterOrigin(),0);
    }
  else
    {
      RefObj->addInsertCell(voidCell);
      RefObj->createAll(*SimPtr,World::masterOrigin(),0);
    }

  
  /*
  

  // this needs to be SELECTED
  /*
  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk"))) ) 
    {
      if (!IParam.compValue("E",std::string("chipIR")))  
	chipObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
      if (!IParam.compValue("E",std::string("zoom")))  
	zoomObj.build(*SimPtr,IParam,*bulkObj.getBulkShield());
    }
  * /
  // Insert pipes [make part of makeTS2Bulk]
  refObj.insertPipeObjects(*SimPtr,IParam);
  */
  // refObj.insertInCell(*SimPtr,excludeCell)
  return;
}

}   // NAMESPACE moderatorSystem

