/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/gtfVariables.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include <cassert>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"


namespace setVariable
{
  void HallVariables(FuncDataBase& Control, const std::string& name)
  {
    // References:
    // [1] 221413_radiation_caclulation.STEP

    Control.addVariable(name+"MainRoomLength",650.0); // [1]
    Control.addVariable(name+"MainRoomWidth",460.0); // [1]
    Control.addVariable(name+"Depth",130.0); // [1]
    Control.addVariable(name+"Height",230.0); // [1]
    Control.addVariable(name+"BackWallThick",100.0); // [1]
    Control.addVariable(name+"MidWallThick",100.0); // [1]
    Control.addVariable(name+"WallMat","Concrete"); // guess
  }

  void GunTestFacilityVariables(FuncDataBase& Control)
  /*!
    Set Gun Test Facility Building variables
    \param Control :: Database to use
    \param name :: name prefix
  */
  {
    ELog::RegMethod RegA("GunTestFacilityVariables", "gtfVariables");

    const std::string name = "GTFHall";
    HallVariables(Control, name);

  }
}
