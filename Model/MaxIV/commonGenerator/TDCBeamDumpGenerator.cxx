/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/TDCBeamDumpGenerator.cxx
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

#include "TDCBeamDumpGenerator.h"

namespace setVariable
{

TDCBeamDumpGenerator::TDCBeamDumpGenerator() :
  length(115.0),bulkWidthLeft(25.0),bulkWidthRight(55.0), // 35+20
  bulkHeight(30.0),bulkDepth(30), bulkThickBack(75.0),
  coreRadius(2.0),coreLength(5.0),preCoreLength(25.0),
  skinThick(5.0),
  frontPlateThick(5.0),
  carbonThick(0.5),
  coreMat("Lead"),
  bulkMat("Stainless304"),
  skinMat("B-Poly"),
  frontPlateMat("Stainless304"),
  carbonMat("Graphite")
  /*!
    Constructor and defaults
  */
{}

TDCBeamDumpGenerator::~TDCBeamDumpGenerator()
 /*!
   Destructor
 */
{}

void
TDCBeamDumpGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("TDCBeamDumpGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"BulkWidthLeft",bulkWidthLeft);
  Control.addVariable(keyName+"BulkWidthRight",bulkWidthRight);
  Control.addVariable(keyName+"BulkHeight",bulkHeight);
  Control.addVariable(keyName+"BulkDepth",bulkDepth);
  Control.addVariable(keyName+"BulkThickBack",bulkThickBack);
  Control.addVariable(keyName+"CoreRadius",coreRadius);
  Control.addVariable(keyName+"CoreLength",coreLength);
  Control.addVariable(keyName+"PreCoreLength",preCoreLength);
  Control.addVariable(keyName+"SkinThick",skinThick);
  Control.addVariable(keyName+"FrontPlateThick",frontPlateThick);
  Control.addVariable(keyName+"CarbonThick",carbonThick);
  Control.addVariable(keyName+"CoreMat",coreMat);
  Control.addVariable(keyName+"BulkMat",bulkMat);
  Control.addVariable(keyName+"SkinMat",skinMat);
  Control.addVariable(keyName+"FrontPlateMat",frontPlateMat);
  Control.addVariable(keyName+"CarbonMat",carbonMat);

  return;

}


}  // namespace setVariable
