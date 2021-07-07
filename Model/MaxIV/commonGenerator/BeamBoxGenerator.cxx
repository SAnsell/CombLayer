/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/BeamBoxGenerator.cxx
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

#include "BeamBoxGenerator.h"

namespace setVariable
{

BeamBoxGenerator::BeamBoxGenerator() :
  width(180.0),height(80.0),length(130.0),
  fullCut(40.0),innerCut(80.0),
  backThick(8.0),b4cThick(0.5),
  backExtension(80.0),wallThick(11.0),
  innerMat("Void"),backMat("HighDensPoly"),
  mainMat("HighDensPoly"),b4cMat("B4C")
  /*!
    Constructor and defaults
  */
{}

BeamBoxGenerator::~BeamBoxGenerator()
 /*!
   Destructor
 */
{}

void
BeamBoxGenerator::generateBox(FuncDataBase& Control,
			      const std::string& keyName,
			      const double yStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("BeamBoxGenerator","generateBox");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FullCut",fullCut);
  Control.addVariable(keyName+"InnerCut",innerCut);
  Control.addVariable(keyName+"BackThick",backThick);
  Control.addVariable(keyName+"B4CThick",b4cThick);
  Control.addVariable(keyName+"BackExtension",backExtension);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"BackMat",backMat);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"B4CMat",b4cMat);
  
  return;  
}

}  // namespace setVariable
