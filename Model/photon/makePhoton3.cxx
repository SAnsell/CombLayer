/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/makePhoton3.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "ModContainer.h"
#include "VacuumVessel.h"
#include "CylLayer.h"
#include "TableSupport.h"
#include "He3Tubes.h"
#include "TubeCollimator.h"

#include "makePhoton3.h"

namespace photonSystem
{

makePhoton3::makePhoton3() :
  Chamber(new photonSystem::VacuumVessel("Chamber")),
  ModContObj(new photonSystem::ModContainer("MetalCont")),
  ModObj(new photonSystem::CylLayer("PrimMod")),
  BaseSupport(new photonSystem::TableSupport("BaseSupport")),
  leftTubes(new photonSystem::He3Tubes("LeftTubes")),
  rightTubes(new photonSystem::He3Tubes("RightTubes")),
  leftColl(new photonSystem::TubeCollimator("LeftColl"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(Chamber);
  OR.addObject(ModContObj);
  OR.addObject(ModObj);
  OR.addObject(BaseSupport);
  OR.addObject(leftTubes);
  OR.addObject(rightTubes);
  OR.addObject(leftColl);

}



makePhoton3::~makePhoton3()
  /*!
    Destructor
   */
{}

void 
makePhoton3::build(Simulation& System,
		   const mainSystem::inputParam&)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  ELog::RegMethod RControl("makePhoton3","build");

  int voidCell(74123);

  Chamber->addInsertCell(voidCell);
  Chamber->createAll(System,World::masterOrigin(),0);
  
  ModContObj->addInsertCell(Chamber->getCell("Void",0));
  ModContObj->createAll(System,*Chamber,0);

  ModObj->addInsertCell(ModContObj->getCell("Void"));
  ModObj->setCutSurf("Outer",*ModContObj,9);    
  ModObj->createAll(System,ModContObj->getBackFlange(),7);

  BaseSupport->addInsertCell(Chamber->getCell("Void"));
  BaseSupport->createAll(System,*Chamber,0);
    
  leftTubes->addInsertCell(voidCell);
  leftTubes->createAll(System,*ModObj,-1);

  leftColl->addInsertCell(voidCell);
  leftColl->createAll(System,*leftTubes,0);
  
  return;
}


}   // NAMESPACE photonSystem

