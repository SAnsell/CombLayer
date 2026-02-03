/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/BladeBPMGenerator.cxx
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

#include "BladeBPMGenerator.h"

// [1] S3-4-1AB01568.pdf
// [2] S3-5-2AJ00851.pdf

namespace setVariable
{

BladeBPMGenerator::BladeBPMGenerator() :
  length(26.0), // [1]
  chamberLength(17.0), // [1]
  chamberRadius(4.8), // CAD measured
  chamberWallThick(0.3), // CAD measured
  chamberFlangeRadius(7.6), // CAD measured
  chamberFlangeLength(2.0), // CAD measured,
  insertFlangeRadius(5.715), // [1] (DN63 = 4.5 inch  = 11.43 cm diameter),
  insertFlangeLength(1.7), // CAD measured,
  insertInnerRadius(1.0), // [2]
  insertOuterRadius(3.4), // [2],
  insertPreOuterRadius(2.75), // [2]
  insertLength(16.35), // [2]
  chamberFlangeMat("Stainless304L"), // [1]: ICF152 => SS304L
  chamberWallMat("SteelUnknownGrade"),
  insertMat("Copper"), // [2] C1020
  insertFlangeMat("Stainless304L"), // [2]: SUS304L
  voidMat("Void"),
  airMat("Void")
  /*!
    Constructor and defaults
  */
{}

BladeBPMGenerator::~BladeBPMGenerator()
 /*!
   Destructor
 */
{}

void
BladeBPMGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("BladeBPMGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"ChamberLength",chamberLength);
  Control.addVariable(keyName+"ChamberRadius",chamberRadius);
  Control.addVariable(keyName+"ChamberWallThick",chamberWallThick);
  Control.addVariable(keyName+"ChamberFlangeRadius",chamberFlangeRadius);
  Control.addVariable(keyName+"ChamberFlangeLength",chamberFlangeLength);
  Control.addVariable(keyName+"InsertFlangeRadius",insertFlangeRadius);
  Control.addVariable(keyName+"InsertFlangeLength",insertFlangeLength);
  Control.addVariable(keyName+"InsertInnerRadius",insertInnerRadius);
  Control.addVariable(keyName+"InsertOuterRadius",insertOuterRadius);
  Control.addVariable(keyName+"InsertPreOuterRadius",insertPreOuterRadius);
  Control.addVariable(keyName+"InsertLength",insertLength);
  Control.addVariable(keyName+"ChamberFlangeMat",chamberFlangeMat);
  Control.addVariable(keyName+"ChamberWallMat",chamberWallMat);
  Control.addVariable(keyName+"InsertMat",insertMat);
  Control.addVariable(keyName+"InsertFlangeMat",insertFlangeMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"AirMat",airMat);

  return;

}


}  // namespace setVariable
