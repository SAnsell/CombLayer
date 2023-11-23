/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/LegoBrickGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "LegoBrickGenerator.h"

namespace setVariable
{

LegoBrickGenerator::LegoBrickGenerator() :
  width(100.0),height(20.0),depth(35.0),
  mat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
LegoBrickGenerator::~LegoBrickGenerator() 
 /*!
   Destructor
 */
{}
  
void
LegoBrickGenerator::generateBrick
( FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("LegoBrickGenerator","generatorBrick");
  
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Mat",mat);
  
  return;
}

}  // NAMESPACE setVariable