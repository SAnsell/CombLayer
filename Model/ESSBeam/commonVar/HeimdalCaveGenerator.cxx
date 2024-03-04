/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/HeimdalCaveGenerator.cxx
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
#include "HeimdalCaveGenerator.h"

namespace setVariable
{

HeimdalCaveGenerator::HeimdalCaveGenerator() :
  length(470.0),height(500.0),width(600.0),corridorWidth(150.0),
  corridorLength(710.0),corridorTLen(200.0),
  doorLength(150.0),roofGap(180.0),
  beamWidth(160.0),beamHeight(178.5),
  midZStep(-10.0),midThick(25.0),
  midHoleRadius(180.0),
  mainThick(70.0),subThick(35.0),extGap(30.0),
  wallMat("Concrete"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}


  
HeimdalCaveGenerator::~HeimdalCaveGenerator() 
 /*!
   Destructor
 */
{}

  
void
HeimdalCaveGenerator::generateCave(FuncDataBase& Control,
				  const std::string& keyName) const
/*
  Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("HeimdalCaveGenerator","generatorCave");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"CorridorWidth",corridorWidth);
  Control.addVariable(keyName+"CorridorLength",corridorLength);
  Control.addVariable(keyName+"CorridorTLen",corridorTLen);

  Control.addVariable(keyName+"DoorLength",doorLength);
  Control.addVariable(keyName+"RoofGap",roofGap);

  Control.addVariable(keyName+"BeamWidth",beamWidth);
  Control.addVariable(keyName+"BeamHeight",beamHeight);

  Control.addVariable(keyName+"MidZStep",midZStep);
  Control.addVariable(keyName+"MidThick",midThick);
  Control.addVariable(keyName+"MidHoleRadius",midHoleRadius);

  Control.addVariable(keyName+"MainThick",mainThick);
  Control.addVariable(keyName+"SubThick",subThick);

  Control.addVariable(keyName+"ExtGap",extGap);

  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  
  return;

}

}  // NAMESPACE setVariable
