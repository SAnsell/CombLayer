/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/IonChamberGenerator.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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

#include "IonChamberGenerator.h"

// References:
// [1] A new method to calculate the response of the WENDI-II rem counter using the FLUKA Monte Carlo Code, http://dx.doi.org/10.1016/j.nima.2012.05.097
// [2] Test of the rem-counter WENDI-II from Eberline in different energy-dispersed neutron fields, https://cds.cern.ch/record/731243/files/ext-2004-085.pdf
// [3] WENDI-II data sheet http://localhost:8080/maxiv/doc/misc/54085td-e_reva.pdf/view

namespace setVariable
{

IonChamberGenerator::IonChamberGenerator() :
  radius(8.25),
  height(20.0),
  airMat("Void")
  /*!
    Constructor and defaults
  */
{}

IonChamberGenerator::~IonChamberGenerator()
 /*!
   Destructor
 */
{}

void
IonChamberGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("IonChamberGenerator","generate");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"AirMat",airMat);

  return;

}


}  // namespace setVariable
