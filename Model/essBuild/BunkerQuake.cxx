/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerQuake.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "BunkerQUnit.h"
#include "BunkerQuake.h"


namespace essSystem
{

BunkerQuake::BunkerQuake(const std::string& bunkerName) :
  attachSystem::FixedUnit(bunkerName+"Quake",0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param bunkerName :: Name of the bunker object that is building this roof
  */
{}


BunkerQuake::~BunkerQuake() 
  /*!
    Destructor
  */
{}
 
void
BunkerQuake::buildAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int orgIndex,
                       const long int axisIndex)

  /*!
    Generic function to initialize everything
    \param System :: Simulation to build object in
    \param FC :: Central origin
    \param orgIndex :: origin point [lower part of roof]
    \param axisIndex :: axis index direction
  */
{
  ELog::RegMethod RegA("BunkerQuake","createAll");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();

  const size_t NPath=Control.EvalVar<size_t>(keyName+"NPath");
  for(size_t i=0;i<NPath;i++)
    {
      QUnit.push_back(std::shared_ptr<BunkerQUnit>
		      (new BunkerQUnit(keyName+std::to_string(i))));
      OR.addObject(QUnit.back());
      QUnit.back()->createAll(System,FC,orgIndex,axisIndex);
    }
  return;
}

}  // NAMESPACE essSystem
