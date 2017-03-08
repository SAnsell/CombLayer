/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t3Model/makeTS3.cxx
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
#include "support.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "ConicInfo.h"
#include "ModBase.h"
#include "CylMod.h"
#include "WallCut.h"
#include "makeTS3.h"

namespace ts3System
{

makeTS3::makeTS3() :
  CentObj(new constructSystem::CylMod("CentH2")),
  WCut(new constructSystem::WallCut("WCut",0))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(CentObj);
  OR.addObject(WCut);
}

makeTS3::~makeTS3()
  /*!
    Destructor
   */
{}

void 
makeTS3::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("makeTS3","build");

  int voidCell(74123);

  CentObj->addInsertCell(voidCell);
  CentObj->createAll(System,World::masterOrigin(),0,0);

  HeadRule WBoundary;
  WCut->addInsertCell(CentObj->getCell(CentObj->getKeyName(),5));
  WBoundary.addIntersection(CentObj->getLayerSurf(5,3));
  WBoundary.makeComplement();
  WBoundary.addIntersection(CentObj->getLayerSurf(4,3));
  WCut->createAll(System,*CentObj,0,WBoundary);
  
}

}   // NAMESPACE ts3System
