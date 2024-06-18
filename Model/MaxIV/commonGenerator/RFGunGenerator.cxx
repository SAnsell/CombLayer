/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/RFGunGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "RFGunGenerator.h"

namespace setVariable
{

RFGunGenerator::RFGunGenerator() :
  length(13.3), // measured,
  frontTubeLength(5.0), // measured,
  frontTubeRadius(5.1), // same as Extension,
  frontTubeFlangeLength(2.0),
  frontTubeFlangeRadius(7.7),
  frontTubePipeThick(0.2), // same as Extension,
  backTubeRadius(3.2), // same as pipeA,
  backTubeFlangeLength(1.2),
  backTubeFlangeRadius(5.7),
  backTubePipeThick(0.2),
  cavityRadius(4.25), // measured
  cavityLength(3.1), // measured
  cavityOffset(2.0), // measured (TODO: why not cavityLength*0.6?)
  irisThick(1.9), // measured
  irisRadius(1.5), // measured
  irisStretch(0.1),
  wallThick(1.0),
  nFrameFacets(8),
  frameWidth(13.0),
  frontPreFlangeThick(0.5),
  frontPreFlangeRadius(4.0),
  frontFlangeThick(1.9), // measured (bigger cylinder)
  frontFlangeRadius(4.8), // measured,
  insertWidth(6.7),
  insertLength(3.2),
  insertDepth(2.0),
  insertWallThick(0.5),
  insertCut(1.8),
  mainMat("Void"),
  wallMat("Copper"), // TODO,
  frontTubePipeMat("Aluminium"), // TODO
  backTubePipeMat("Aluminium") // TODO
  /*!
    Constructor and defaults
  */
{}

RFGunGenerator::~RFGunGenerator()
 /*!
   Destructor
 */
{}

void
RFGunGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("RFGunGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FrontTubeLength",frontTubeLength);
  Control.addVariable(keyName+"FrontTubeRadius",frontTubeRadius);
  Control.addVariable(keyName+"FrontTubeFlangeLength",frontTubeFlangeLength);
  Control.addVariable(keyName+"FrontTubeFlangeRadius",frontTubeFlangeRadius);
  Control.addVariable(keyName+"FrontTubePipeThick",frontTubePipeThick);
  Control.addVariable(keyName+"BackTubeRadius",backTubeRadius);
  Control.addVariable(keyName+"BackTubeFlangeLength",backTubeFlangeLength);
  Control.addVariable(keyName+"BackTubeFlangeRadius",backTubeFlangeRadius);
  Control.addVariable(keyName+"BackTubePipeThick",backTubePipeThick);
  Control.addVariable(keyName+"CavityRadius",cavityRadius);
  Control.addVariable(keyName+"CavityLength",cavityLength);
  Control.addVariable(keyName+"CavityOffset",cavityOffset);
  Control.addVariable(keyName+"IrisThick",irisThick);
  Control.addVariable(keyName+"IrisRadius",irisRadius);
  Control.addVariable(keyName+"IrisStretch",irisStretch);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"NFrameFacets",nFrameFacets);
  Control.addVariable(keyName+"FrameWidth",frameWidth);
  Control.addVariable(keyName+"FrontPreFlangeThick",frontPreFlangeThick);
  Control.addVariable(keyName+"FrontPreFlangeRadius",frontPreFlangeRadius);
  Control.addVariable(keyName+"FrontFlangeThick",frontFlangeThick);
  Control.addVariable(keyName+"FrontFlangeRadius",frontFlangeRadius);
  Control.addVariable(keyName+"InsertWidth",insertWidth);
  Control.addVariable(keyName+"InsertLength",insertLength);
  Control.addVariable(keyName+"InsertDepth",insertDepth);
  Control.addVariable(keyName+"InsertWallThick",insertWallThick);
  Control.addVariable(keyName+"InsertCut",insertCut);
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"FrontTubePipeMat",frontTubePipeMat);
  Control.addVariable(keyName+"BackTubePipeMat",backTubePipeMat);

  return;

}


}  // namespace setVariable
