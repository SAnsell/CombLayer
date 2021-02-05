/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/FMUndulatorGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "FMUndulatorGenerator.h"

namespace setVariable
{

FMUndulatorGenerator::FMUndulatorGenerator() :
  vGap(1.1),poleWidth(6.0),poleDepth(0.4),
  magnetWidth(9.0),magnetDepth(2.5), 
  baseDepth(1.2),baseExtraLen(4.5),
  midWidth(6.0),midDepth(2.0),
  mainWidth(11.5),mainDepth(2.5),
  voidMat("Void"),magnetMat("NbFeB"),
  supportMat("Aluminium")
  /*!
    Constructor and defaults
  */
{}

FMUndulatorGenerator::~FMUndulatorGenerator()
 /*!
   Destructor
 */
{}


void
FMUndulatorGenerator::generateUndulator(FuncDataBase& Control,
					  const std::string& keyName,
					  const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param length :: total magnet lneght
  */
{
  ELog::RegMethod RegA("FMUndulatorGenerator","generateFMUndulator");

  Control.addVariable(keyName+"VGap",vGap);
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"PoleWidth",poleWidth);
  Control.addVariable(keyName+"PoleDepth",poleDepth);

  Control.addVariable(keyName+"MagnetWidth",magnetWidth);
  Control.addVariable(keyName+"MagnetDepth",magnetDepth);

  Control.addVariable(keyName+"BaseDepth",baseDepth);
  Control.addVariable(keyName+"BaseExtraLen",baseExtraLen);

  Control.addVariable(keyName+"MidWidth",midWidth);
  Control.addVariable(keyName+"MidDepth",midDepth);

  Control.addVariable(keyName+"MainWidth",mainWidth);
  Control.addVariable(keyName+"MainDepth",mainDepth);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MagnetMat",magnetMat);
  Control.addVariable(keyName+"SupportMat",supportMat);
      
      

  return;

}

///\cond TEMPLATE



///\endcond TEMPLATE

}  // NAMESPACE setVariable
