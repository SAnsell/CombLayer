/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/HeatAbsorberR3ToyamaGenerator.cxx
 *
 * Copyright (c) 2025 by Udo Friman-Gayer
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
#include "PipeGenerator.h"

#include "HeatAbsorberR3ToyamaGenerator.h"

namespace setVariable
{

HeatAbsorberR3ToyamaGenerator::HeatAbsorberR3ToyamaGenerator() :
  length(26.5),absorberLength(22.0),absorberWidth(8.0),absorberHeight(8.0),
  gapWidth(1.0),gapMinHeight(0.9),gapMaxHeight(2.6),closed(false),
  mainMat("SteelUnknownGrade"),voidMat("Void")
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
  Control.addVariable(keyName+"AbsorberLength",absorberLength);
  Control.addVariable(keyName+"AbsorberWidth",absorberWidth);
  Control.addVariable(keyName+"AbsorberHeight",absorberHeight);
  Control.addVariable(keyName+"GapWidth",gapWidth);
  Control.addVariable(keyName+"GapMinHeight",gapMinHeight);
  Control.addVariable(keyName+"GapMaxHeight",gapMaxHeight);
  Control.addVariable(keyName+"Closed",static_cast<int>(closed));
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  setVariable::PipeGenerator pipeGen;

  const double frontBackPipeLength = (length-absorberLength)/2.0;
  pipeGen.setCF<setVariable::CF63>();
  pipeGen.setFlangeLength(setVariable::CF63::flangeLength, 0.0);
  pipeGen.generatePipe(Control, keyName+"FrontPipe", frontBackPipeLength);
  pipeGen.setFlangeLength(0.0, setVariable::CF63::flangeLength);
  pipeGen.generatePipe(Control, keyName+"BackPipe", frontBackPipeLength);

}


}  // namespace setVariable
