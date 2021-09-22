/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/TableGenerator.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "TableGenerator.h"

namespace setVariable
{

TableGenerator::TableGenerator() :
  thick(1.5),width(48.0),legSize(5.0),
  clearance(5.0),
  plateMat("Stainless304"),legMat("Stainless304"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

TableGenerator::~TableGenerator() 
 /*!
   Destructor
 */
{}
  
void
TableGenerator::generateTable(FuncDataBase& Control,
			      const std::string& keyName,
			      const double zStep,
			      const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param zStep :: Vertical lift down from beam centre line
    \param length :: length of tabel 
  */
{
  ELog::RegMethod RegA("TableGenerator","generatorMount");

  Control.addVariable(keyName+"ZStep",zStep);

  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"LegSize",legSize);
  Control.addVariable(keyName+"Clearance",clearance);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PlateMat",plateMat);
  Control.addVariable(keyName+"LegMat",legMat);
       
  return;

}

  
}  // NAMESPACE setVariable
