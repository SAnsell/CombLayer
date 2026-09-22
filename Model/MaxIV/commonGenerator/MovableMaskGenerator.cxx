/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/MovableMaskGenerator.cxx
 *
 * Copyright (c) 2026 S. Ansell and U. Friman-Gayer
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
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"

#include "MovableMaskGenerator.h"

namespace setVariable {

MovableMaskGenerator::MovableMaskGenerator()
    :                                       // All dimensions from [1]
      length(30.0),                         // A-A
      bodyHeight(7.2),                      // X VIEW (S=1/1)
      bodyLength(24.4),                     // A-A
      bodyWidth(7.2),                       // X VIEW (S=1/1)
      flangeInnerRadius(CF63::innerRadius), // Side View
      flangeLength(CF63::flangeLength),     // Side View
      flangeRadius(CF63::flangeRadius),     // Side View
      flangeWallThick(CF63::wallThick),     // Side View
      holeHeight(3.0),                      // X VIEW (S=1/1)
      holeOffset(
          0.45), // X VIEW (S=1/1), difference between hole height and depth
      holeWidth(3.12),                 // X VIEW (S=1/1)
      bodyMaterial("Aluminium"),       // A-A, TODO: Should be GLIDCOP AL-15
      flangeMaterial("Stainless304L"), // A-A
      voidMaterial("Void") {}

void MovableMaskGenerator::generate(FuncDataBase &Control,
                                    const std::string &keyName) const {

  Control.addVariable(keyName + "Length", length);

  Control.addVariable(keyName + "BodyHeight", bodyHeight);
  Control.addVariable(keyName + "BodyLength", bodyLength);
  Control.addVariable(keyName + "BodyWidth", bodyWidth);

  Control.addVariable(keyName + "FlangeInnerRadius", flangeInnerRadius);
  Control.addVariable(keyName + "FlangeLength", flangeLength);
  Control.addVariable(keyName + "FlangeRadius", flangeRadius);
  Control.addVariable(keyName + "FlangeWallThick", flangeWallThick);

  Control.addVariable(keyName + "HoleHeight", holeHeight);
  Control.addVariable(keyName + "HoleOffset", holeOffset);
  Control.addVariable(keyName + "HoleWidth", holeWidth);

  Control.addVariable(keyName + "FlangeMaterial", flangeMaterial);
  Control.addVariable(keyName + "BodyMaterial", bodyMaterial);
  Control.addVariable(keyName + "VoidMaterial", voidMaterial);
}

} // NAMESPACE setVariable
