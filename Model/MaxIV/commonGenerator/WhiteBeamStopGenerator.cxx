/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/WhiteBeamStopGenerator.cxx
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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

#include "WhiteBeamStopGenerator.h"

// References
// [1] S3716 DanMAX Diagnostics Functional Specification Rev05_NEW.pdf

namespace setVariable
{

WhiteBeamStopGenerator::WhiteBeamStopGenerator() :
  length(4.6),  // [1]
  width(1.7),   // [1]
  height(1.2),  // [1]
  angle(30.0),  // [1]
  inBeam(false),
  offBeamOffset(6.0), // adjust to fully contain WBS in the port when off beam to simplify intersections
  mat("Copper") // [1]
  /*!
    Constructor and defaults
  */
{}

WhiteBeamStopGenerator::~WhiteBeamStopGenerator()
 /*!
   Destructor
 */
{}

void
WhiteBeamStopGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("WhiteBeamStopGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Angle",angle);
  Control.addVariable(keyName+"InBeam",static_cast<int>(inBeam));
  Control.addVariable(keyName+"OffBeamOffset",offBeamOffset);
  Control.addVariable(keyName+"Mat",mat);

  return;

}


}  // namespace setVariable
