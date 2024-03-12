/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/WendiGenerator.cxx
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

#include "WendiGenerator.h"

// References:
// [1] A new method to calculate the response of the WENDI-II rem counter using the FLUKA Monte Carlo Code, http://dx.doi.org/10.1016/j.nima.2012.05.097
// [2] Test of the rem-counter WENDI-II from Eberline in different energy-dispersed neutron fields, https://cds.cern.ch/record/731243/files/ext-2004-085.pdf
// [3] WENDI-II data sheet http://localhost:8080/maxiv/doc/misc/54085td-e_reva.pdf/view

namespace setVariable
{

WendiGenerator::WendiGenerator() :
  radius(11.43), // [1], fig. 1
  cRadius(1.32), // [1, section 3]
  height(21.0),  // [1], fig. 1
  wRadius(4.0),  // [3], fig. 3
  wThick(1.5),   // [3], fig. 3, [1, section 3]
  wHeight(11.5), // [1], fig. 1
  wOffset(4.0),  // [1], fig. 1
  cOffset(1.5),  // guess
  heRadius(1.22), // [1, section 3]
  heWallThick(0.05), // [1, section 3]
  heHeight(7.36),  // [1, section 3]
  rubberThick(0.75), // guess based on [2, fig. 1]
  bottomInsulatorRadius(0.38), // [1, section 3],
  bottomInsulatorHeight(0.5), // [1, section 3],
  topInsulatorRadius(0.67), // [1, section 3],
  topInsulatorHeight(1.08), // [1, section 3],
  modMat("Poly"),// [1], fig. 1
  wMat("Tungsten%Void%55.169"), // to adjust density to 10.71 g/cmq as of [1, section 3],
  heWallMat("Stainless304"), // guess, [1, section 3]: "stainless steel",
  heMat("He3_2Atm"), // [3, fig. 3],
  rubberMat("BoratedPolyTDC"), // TODO, rough assumption, [2]: 25% boron,
  dummy(0)
  /*!
    Constructor and defaults
  */
{}

WendiGenerator::~WendiGenerator()
 /*!
   Destructor
 */
{}

void
WendiGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("WendiGenerator","generate");

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"CounterTubeRadius",cRadius);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"TungstenRadius",wRadius);
  Control.addVariable(keyName+"TungstenThick",wThick);
  Control.addVariable(keyName+"TungstenHeight",wHeight);
  Control.addVariable(keyName+"TungstenOffset",wOffset);
  Control.addVariable(keyName+"CounterOffset",cOffset);
  Control.addVariable(keyName+"HeRadius",heRadius);
  Control.addVariable(keyName+"HeWallThick",heWallThick);
  Control.addVariable(keyName+"HeHeight",heHeight);
  Control.addVariable(keyName+"RubberThick",rubberThick);
  Control.addVariable(keyName+"BottomInsulatorRadius",bottomInsulatorRadius);
  Control.addVariable(keyName+"BottomInsulatorHeight",bottomInsulatorHeight);
  Control.addVariable(keyName+"TopInsulatorRadius",topInsulatorRadius);
  Control.addVariable(keyName+"TopInsulatorHeight",topInsulatorHeight);
  Control.addVariable(keyName+"ModeratorMat",modMat);
  Control.addVariable(keyName+"TungstenMat",wMat);
  Control.addVariable(keyName+"HeWallMat",heWallMat);
  Control.addVariable(keyName+"HeMat",heMat);
  Control.addVariable(keyName+"RubberMat",rubberMat);
  Control.addVariable(keyName+"Dummy",dummy);

  return;

}


}  // namespace setVariable
