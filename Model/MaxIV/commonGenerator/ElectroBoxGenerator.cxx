/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/ElectroBoxGenerator.cxx
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

#include "OpticsHutGenerator.h"
#include "ElectroBoxGenerator.h"

namespace setVariable
{


ElectroBoxGenerator::ElectroBoxGenerator() :
  voidWidth(17.0),voidLength(36.0),voidHeight(48.4),
  frontThick(4.0),backThick(4.0),sideThick(4.0),
  skinThick(0.2),voidMat("Poly"),skinMat("Stainless304"),
  wallMat("Lead")
  /*!
    Constructor and defaults
  */
{}
  
void
ElectroBoxGenerator::generateBox(FuncDataBase& Control,
				 const std::string& boxName,
				 const double xStep,
				 const double yStep) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param xStep :: distance from beam centre line
    \param yStep :: distance from undulator centre point
  */
{
  ELog::RegMethod RegA("ElectroBoxGenerator","generateBox");

  Control.addVariable(boxName+"XStep",xStep);
  Control.addVariable(boxName+"YStep",yStep);
  
  Control.addVariable(boxName+"VoidWidth",voidWidth);
  Control.addVariable(boxName+"VoidLength",voidLength);
  Control.addVariable(boxName+"VoidHeight",voidHeight);

  Control.addVariable(boxName+"FrontThick",frontThick);
  Control.addVariable(boxName+"BackThick",backThick);
  Control.addVariable(boxName+"SideThick",sideThick);
  Control.addVariable(boxName+"SkinThick",skinThick);

  Control.addVariable(boxName+"VoidMat",voidMat);
  Control.addVariable(boxName+"SkinMat",skinMat);
  Control.addVariable(boxName+"WallMat",wallMat);
 
  
  return;

}

}  // NAMESPACE setVariable
