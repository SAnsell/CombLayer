/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/MagnetGenerator.cxx
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
#include "CFFlanges.h"

#include "MagnetGenerator.h"

namespace setVariable
{

MagnetGenerator::MagnetGenerator() :
  length(20.0),width(10.0),height(5.0),
  KFactor({0,0,0,0})
  /*!
    Constructor and defaults
  */
{}

MagnetGenerator::~MagnetGenerator()
 /*!
   Destructor
 */
{}


void
MagnetGenerator::generate(FuncDataBase& Control,
			  const std::string& unitName,
			  const std::string& fcUnit,
			  const std::string& linkPt,
			  const double yAngle) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param Unit :: FixedComp for active units / origin
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generate");

  const std::string keyName="MagUnit"+unitName;

  Control.addVariable(keyName+"YAngle",angle);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  
  Control.addVariable(keyName+"FixedComp",fcUnit);
  Control.addVariable(keyName+"LinkPt",linkPt);

  for(size_t i=0;i<4;i++)
    Control.addVariable(keyName+"KFactor"+std::to_string(i),KFactor[i]);

  Control.addVariable(keyName+"NActiveCell",activeCells.size());
  size_t index(0);
  for(const std::string& CN : activeCells)
    {
      Control.addVariable
	(keyName+"ActiveCell"+std::to_string(index),KFactor[i]);
      index++;
    }
    
  return;

}

}  // namespace setVariable
