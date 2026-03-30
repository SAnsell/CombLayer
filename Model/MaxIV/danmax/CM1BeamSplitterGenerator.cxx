/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/CM1BeamSplitterGenerator.cxx
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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

#include "CM1BeamSplitterGenerator.h"

namespace setVariable
{
CM1BeamSplitterGenerator
::CM1BeamSplitterGenerator():
  // Dimensions
  // Measured in the CAD model [2] if not indicated otherwise.
  bodyAngle(8.04),
  width(2.75),
  bottomChamferWidth(0.15),
  bottomWidth(1.0),
  filterHoleOffset(0.293),
  splitterCrystalPitch(4.16060316),
  splitterCrystalRoll(-0.02926297),
  splitterCrystalYaw(0.80559476),
  splitterHoleToFilterHole(0.656),
  topOverhangWidth(0.6),
  mode(0)
  /*!
    Constructor and defaults
  */
{}

void CM1BeamSplitterGenerator
::generate(FuncDataBase& Control,const std::string& keyName)
  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("CM1BeamSplitterGenerator","generate");

  Control.addVariable(keyName+"BodyAngle",bodyAngle);
  const double bodyAngleRad = bodyAngle*M_PI/180.0;
  Control.addVariable(keyName+"Width",width);

  Control.addVariable(keyName+"BottomChamferWidth",bottomChamferWidth);
  Control.addVariable(keyName+"BottomWidth",bottomWidth);

  Control.addVariable(keyName+"FilterHoleOffset",filterHoleOffset);

  Control.addVariable(keyName+"SplitterCrystalPitch",splitterCrystalPitch);
  Control.addVariable(keyName+"SplitterCrystalRoll",splitterCrystalRoll);
  Control.addVariable(keyName+"SplitterCrystalYaw",splitterCrystalYaw);

  Control.addVariable(keyName+"SplitterHoleToFilterHole",splitterHoleToFilterHole);

  Control.addVariable(keyName+"TopOverhangWidth",topOverhangWidth);
  Control.addVariable(keyName+"Mode",mode);

  if(mode != 0){
    const double frontCornerX = -width/2.0+topOverhangWidth+bottomWidth-bottomChamferWidth;
    if(mode == 1){
        Control.addVariable(keyName+"XStep",filterHoleOffset-0.5*cos(bodyAngleRad)*bottomChamferWidth);
        Control.addVariable(keyName+"YStep",-sin(bodyAngleRad)*frontCornerX);
        Control.addVariable(keyName+"ZAngle",bodyAngle);
    }
    else if(mode == 2){
        Control.addVariable(keyName+"XStep",
          filterHoleOffset-0.5*cos(bodyAngleRad)*(bottomChamferWidth)+splitterHoleToFilterHole);
        Control.addVariable(keyName+"YStep",-sin(bodyAngleRad)*frontCornerX);
        Control.addVariable(keyName+"ZAngle",bodyAngle);
    }
  }
}

}  // NAMESPACE setVariable
