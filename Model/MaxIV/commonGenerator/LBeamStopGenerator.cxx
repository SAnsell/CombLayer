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

#include "LBeamStopGenerator.h"

namespace setVariable
{

LBeamStopGenerator::LBeamStopGenerator() :
  length(50.0),innerVoidLen(15.0),
  innerLength(5.0),innerRadius(4.0),
  midVoidLen(7.5),midLength(22.5),
  midRadius(7.0),outerRadius(20.0),
  midNLayers(1),
  outerNLayers(1),
  voidMat("Void"),innerMat("Graphite"),
  midMat("Poly"),outerMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

LBeamStopGenerator::LBeamStopGenerator(const std::string&) :
  length(50.0),innerVoidLen(15.0),
  innerLength(5.0),innerRadius(4.0),
  midVoidLen(7.5),midLength(22.5),
  midRadius(7.0),outerRadius(20.0),
  midNLayers(1),
  outerNLayers(1),
  voidMat("Void"),innerMat("Graphite"),
  midMat("Poly"),outerMat("Stainless304L")
  /*!
    Constructor and defaults
  */
{}

LBeamStopGenerator::~LBeamStopGenerator()
 /*!
   Destructor
 */
{}

void
LBeamStopGenerator::generateBStop(FuncDataBase& Control,
				  const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("LBeamStopGenerator","generate");


  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"InnerVoidLen",innerVoidLen);
  Control.addVariable(keyName+"InnerLength",innerLength);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"MidVoidLen",midVoidLen);
  Control.addVariable(keyName+"MidLength",midLength);
  Control.addVariable(keyName+"MidRadius",midRadius);
  Control.addVariable(keyName+"MidNLayers",midNLayers);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"OuterNLayers",outerNLayers);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"MidMat",midMat);
  Control.addVariable(keyName+"OuterMat",outerMat);


  return;

}

}  // namespace setVariable
