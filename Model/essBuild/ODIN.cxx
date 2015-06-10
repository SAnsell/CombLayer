/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODIN.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "stringCombine.h"
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "ODIN.h"

namespace essSystem
{

ODIN::ODIN() :
  BladeChopper(new constructSystem::DiskChopper("odinBlade")),
  GuideA(new beamlineSystem::GuideLine("ODINgA")),
  T0Chopper(new constructSystem::DiskChopper("odinTZero")),
  GuideB(new beamlineSystem::GuideLine("ODINgA"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(BladeChopper);
  OR.addObject(GuideA);
  OR.addObject(T0Chopper);
  OR.addObject(GuideB);
}




ODIN::~ODIN()
  /*!
    Destructor
  */
{}


void 
ODIN::build(Simulation& System,const attachSystem::TwinComp& GItem,
	    const attachSystem::CellMap& Bunker,const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param Bunkdr :: Bunker cell map [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("ODIN","build");
  
  BladeChopper->addInsertCell(Bunker.getCell("MainVoid"));
  BladeChopper->createAll(System,GItem,2);

  GuideA->addInsertCell(Bunker.getCell("MainVoid"));
  GuideA->createAll(System,BladeChopper->getKey("Main"),2,
		    BladeChopper->getKey("Beam"),2);
  
  T0Chopper->addInsertCell(Bunker.getCell("MainVoid"));
  T0Chopper->createAll(System,GuideA->getKey("Guide0"),2);

  GuideB->addInsertCell(Bunker.getCell("MainVoid"));
  GuideB->createAll(System,T0Chopper->getKey("Main"),2,
		    T0Chopper->getKey("Beam"),2);

  return;
}


}   // NAMESPACE essSystem

