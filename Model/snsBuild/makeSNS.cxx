/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   snsBuild/makeSNS.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <utility>
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
#include "LayerComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "TargetBase.h"
#include "targetOuter.h"
#include "RefPlug.h"

#include "makeSNS.h"

namespace snsSystem
{

makeSNS::makeSNS() :
  tarOuterObj(new targetOuter("tarFront")),
  refObj(new RefPlug("RefPlug"))
 /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeSNS","makeSNS");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(tarOuterObj);
  OR.addObject(refObj);
}


makeSNS::~makeSNS()
  /*!
    Destructor
  */
{}


void 
makeSNS::build(Simulation* SimPtr,
	       const mainSystem::inputParam&)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeSNS","build");

  int voidCell(74123);
  tarOuterObj->addInsertCell(voidCell);
  tarOuterObj->createAll(*SimPtr,World::masterOrigin());

  refObj->addInsertCell(voidCell);
  refObj->createAll(*SimPtr,World::masterOrigin(),0);

  attachSystem::addToInsertSurfCtrl(*SimPtr,*refObj,*tarOuterObj);

  // LineVoid->createAll(*SimPtr,World::masterOrigin());
  // attachSystem::addToInsertSurfCtrl(*SimPtr,*Hall,*LineVoid);

  // makeMagnets(*SimPtr);
  return;
}


}   // NAMESPACE ts1System

