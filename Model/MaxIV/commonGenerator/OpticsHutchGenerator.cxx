/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/OpticsHutchGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
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

#include "XRayHutchBaseGenerator.h"
#include "OpticsHutchGenerator.h"

namespace setVariable
{


OpticsHutchGenerator::OpticsHutchGenerator() :
  XRayHutchBaseGenerator(),
  wallShineThick(0.6), wallShineLength(59.0),
  wallShineOutThick(1.2), wallShineOutLength(20.0),
  roofShineLength(20.0),
  roofShineThick(0.6)
  /*!
    Constructor and defaults
  */
{
  // by default the back wall plate is active in optics hutches:
  backPlateActive = true;
  backPlateThick = 7.0;
  backPlateWidth = 200.0;
  backPlateHeight = 200.0;
}

void
OpticsHutchGenerator::generateHut(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param length :: Full length of hut
  */
{
  ELog::RegMethod RegA("OpticsHutchGenerator","generateOpticsHut");

  XRayHutchBaseGenerator::generateHut(Control, keyName, length);

  Control.addVariable(keyName+"WallShineThick",wallShineThick);
  Control.addVariable(keyName+"WallShineLength",wallShineLength);
  Control.addVariable(keyName+"WallShineOutThick",wallShineOutThick);
  Control.addVariable(keyName+"WallShineOutLength",wallShineOutLength);
  Control.addVariable(keyName+"RoofShineLength",roofShineLength);
  Control.addVariable(keyName+"RoofShineThick",roofShineThick);

  for(size_t i=0;i<holeRadius.size();i++)
    {
      const std::string iStr("Hole"+std::to_string(i));
      Control.addVariable(keyName+iStr+"XStep",holeOffset[i].X());
      Control.addVariable(keyName+iStr+"ZStep",holeOffset[i].Z());
      Control.addVariable(keyName+iStr+"Radius",holeRadius[i]);
    }

  return;

}

}  // NAMESPACE setVariable
