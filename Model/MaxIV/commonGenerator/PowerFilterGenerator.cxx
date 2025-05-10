/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/PowerFilterGenerator.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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

#include "PowerFilterGenerator.h"

namespace setVariable
{

PowerFilterGenerator::PowerFilterGenerator() :
  maxLength(0.6), // all dimensions: TDR, page 39 and table 8.1
  baseLength(0.7),
  wedgeAngle(3), // TDR, caption of fig 8.2 on page 39
  width(0.3),height(12.0),baseHeight(1.0),
  mat("Silicon300K"),wallMat("Stainless316L")
  /*!
    Constructor and defaults
  */
{}

PowerFilterGenerator::~PowerFilterGenerator()
 /*!
   Destructor
 */
{}

void
PowerFilterGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("PowerFilterGenerator","generate");

  Control.addVariable(keyName+"MaxLength",maxLength);
  Control.addVariable(keyName+"BaseLength",baseLength);
  Control.addVariable(keyName+"WedgeAngle",wedgeAngle);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"BaseHeight",baseHeight);
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"WallMat",wallMat);

  return;

}


}  // namespace setVariable
