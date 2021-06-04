/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/LBeamStopGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

#include "NBeamStopGenerator.h"

namespace setVariable
{

NBeamStopGenerator::NBeamStopGenerator() :
  fullLength(120.0),
  radii({2.0, 5.0, 20.0}),
  len({{1.0,50.0},{7.0,60.0,},{12.0}}),
  mat({{"Void","Tungsten","Stainless304"},
       {"Void","Poly","Stainless304"},
       {"Void","Stainless304"}})
  /*!
    Constructor and defaults
  */
{}

NBeamStopGenerator::~NBeamStopGenerator()
 /*!
   Destructor
 */
{}

void
NBeamStopGenerator::generateBStop(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("NBeamStopGenerator","generateBStop");

  if (!radii.empty())
    {
      Control.addVariable(keyName+"Length",fullLength);
      Control.addVariable(keyName+"OuterRadius",radii.back());
      Control.addVariable(keyName+"NLayers",mat.size());
      for(size_t i=0;i<radii.size();i++)
	{
	  const std::string layerName(keyName+"Layer"+std::to_string(i));
	  Control.addVariable(layerName+"Radius",radii[i]);
	  size_t j;
	  for(j=0;j<len[i].size();j++)
	    {
	      std::string UNum(std::to_string(j));
	      Control.addVariable(layerName+"Length"+UNum,len[i][j]);
	      Control.addVariable(layerName+"Mat"+UNum,mat[i][j]);	  
	    }
	  Control.addVariable(layerName+"Mat"+std::to_string(j),mat[i][j]);	  
	}
    }
  return;  
}

}  // namespace setVariable
