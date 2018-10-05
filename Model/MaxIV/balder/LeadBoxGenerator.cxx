/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/LeadBoxGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "LeadBoxGenerator.h"

namespace setVariable
{

LeadBoxGenerator::LeadBoxGenerator() :
  height(8.0),depth(8.0),
  width(16.0),length(8.0),
  wallThick(0.5),portGap(0.2),
  wallMat("Lead")
  /*!
    Constructor and defaults
  */
{}

  
LeadBoxGenerator::~LeadBoxGenerator() 
 /*!
   Destructor
 */
{}

  
void
LeadBoxGenerator::setMain(const double W,const double H)
  /*!
    Set the surface radius
    \param W :: Width
    \param H :: height+depth [equal divided]
   */
{
  width=W;
  height=H/2.0;
  depth=H/2.0;
  return;
}
				  
void
LeadBoxGenerator::generateBox(FuncDataBase& Control,
			      const std::string& keyName,
			      const double yStep,
			      const double len) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param len :: length
  */
{
  ELog::RegMethod RegA("LeadBoxGenerator","generateBox");
  
  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Length",len);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"PortGap",portGap);

  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat","Void");
       
  return;

}

  
}  // NAMESPACE setVariable
