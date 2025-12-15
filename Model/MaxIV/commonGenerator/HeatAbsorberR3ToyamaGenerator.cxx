/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/HeatAbsorberR3ToyamaGenerator.cxx
 *
 * Copyright (c) 2004-2025 by Udo Friman-Gayer
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
#include <complex>
#include <fstream>
#include <map>
#include <vector>

#include "CFFlanges.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "HeatAbsorberR3ToyamaGenerator.h"

namespace setVariable
{

HeatAbsorberR3ToyamaGenerator::HeatAbsorberR3ToyamaGenerator() :
  // Most of the dimensions are explicitly given in Ref. [1],
  // but some have been measured with a ruler. This is clearly indicated in the
  // comments below.
  //
  // All dimensions of the flanges not set here are assumed to correspond to the
  // CF63 standard.
  length(26.5), // [1]
  connectorInnerRadius(2.0), // measured in [1]
  absorberLength(22.0), // [1]
  absorberWidth(8.0), // [1]
  absorberHeight(8.0), // [1]
  absorberConnectorLength(1.0), // measured in [1] 
  gapWidth(1.0), // [1]
  gapMinHeight(0.9), // [1]
  gapMaxHeight(2.6), // [1]
  inOutRange(1.7), // [1]
  closed(false),
  mainMat("Copper"), // TODO: Should be GLIDCOP, grade not given in [1].
  voidMat("Void"),
  pipeMat("SteelUnknownGrade") // TODO
  /*!
    Constructor and defaults
  */
{}

HeatAbsorberR3ToyamaGenerator::~HeatAbsorberR3ToyamaGenerator()
 /*!
   Destructor
 */
{}

void
HeatAbsorberR3ToyamaGenerator::generate(FuncDataBase& Control,
				     const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("HeatAbsorberR3ToyamaGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"ConnectorInnerRadius",connectorInnerRadius);
  Control.addVariable(keyName+"AbsorberLength",absorberLength);
  Control.addVariable(keyName+"AbsorberWidth",absorberWidth);
  Control.addVariable(keyName+"AbsorberHeight",absorberHeight);
  Control.addVariable(keyName+"AbsorberConnectorLength",absorberConnectorLength);
  Control.addVariable(keyName+"GapWidth",gapWidth);
  Control.addVariable(keyName+"GapMinHeight",gapMinHeight);
  Control.addVariable(keyName+"GapMaxHeight",gapMaxHeight);
  Control.addVariable(keyName+"InOutRange",inOutRange);
  Control.addVariable(keyName+"Closed",static_cast<int>(closed));
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);

}


}  // namespace setVariable
