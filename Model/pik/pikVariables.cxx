/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   pik/pikVariables.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

namespace setVariable
{

void
PIKVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("pikVariables[F]","PIKVariables");

 // REACTOR POOL
  Control.addVariable("PoolDepth",200.0);
  Control.addVariable("PoolHeight",600.0);
  Control.addVariable("PoolInnerShieldRadius",200.0);
  Control.addVariable("PoolInnerShieldWallThick",5.0);
  Control.addVariable("PoolOuterShieldRadius",320.0);
  Control.addVariable("PoolOuterShieldWidth",508.0);
  Control.addVariable("PoolInnerShieldMat","H2O");
  Control.addVariable("PoolInnerShieldWallMat","Stainless316L"); // guess
  Control.addVariable("PoolOuterShieldMat","Concrete");

  // iron-water protection tank
  Control.addVariable("PoolTankDepth",140.0);
  Control.addVariable("PoolTankHeight",165.0);
  Control.addVariable("PoolTankMat","Stainless316L"); // guess

  Control.addVariable("PoolTankNLayers",4);
  Control.addVariable("PoolTankRadius0",140.0);
  Control.addVariable("PoolTankRadius1",147.0);
  Control.addVariable("PoolTankRadius2",154.0);
  Control.addVariable("PoolTankRadius3",168.0);
  Control.addVariable("PoolTankThick0",5.0);
  Control.addVariable("PoolTankThick1",5.0);
  Control.addVariable("PoolTankThick2",5.0);
  Control.addVariable("PoolTankThick3",3.0);

  // reflector
  Control.addVariable("ReflectorZStep",-7.75);
  Control.addVariable("ReflectorOuterRadius",123.0);
  Control.addVariable("ReflectorRoofPitch",15.0);
  Control.addVariable("ReflectorFloorPitch",15.0);
  Control.addVariable("ReflectorHeight",206.104);
  Control.addVariable("ReflectorWallThick",3.0);
  Control.addVariable("ReflectorMat","D2O");
  Control.addVariable("ReflectorShieldMat","H2O");
  Control.addVariable("ReflectorWallMat","Stainless316L");

  // core
  Control.addVariable("CoreLength",10.0);
  Control.addVariable("CoreWidth",10.0);
  Control.addVariable("CoreHeight",100.0);
  Control.addVariable("CoreMainMat","Uranium");

  return;
}

}  // NAMESPACE setVariable
