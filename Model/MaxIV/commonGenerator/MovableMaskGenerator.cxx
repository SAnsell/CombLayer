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
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <sstream>
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
    :                   // All dimensions from [1] if not indicated otherwise.
      length(30.0),     // A-A
      bodyHeight(7.2),  // X VIEW (S=1/1)
      bodyLength(24.4), // A-A
      bodyWidth(7.2),   // X VIEW (S=1/1)
      flangeInnerRadius(CF63::innerRadius), // Side View
      flangeLength(CF63::flangeLength),     // Side View
      flangeRadius(CF63::flangeRadius),     // Side View
      flangeWallThick(CF63::wallThick),     // Side View
      holeHeight(3.0),                      // X VIEW (S=1/1)
      holeOffset(
          0.45), // X VIEW (S=1/1), difference between hole height and depth
      holeWidth(3.12),        // X VIEW (S=1/1)
      maskLeftMaxHeight(2.1), // X VIEW (S=1/1), 2 x 10.5 mm, assuming that the
                              // part is symmetric about the central axis
      maskLeftMaxWidth(1.3),  // X VIEW (S=1/1)
      maskBottomMaxHeight(0.79), // X VIEW (S=1/1)
      maskBottomMaxWidth(
          2.8), // X VIEW, 15.6 mm (half hole width) + 5.0 mm (beam offset)
                // + 2.0 mm (half beam width) + 5.4 mm
      maskFocalPoint(0.376),              // [5]
      maskDownstreamInnerPlaneAngle(9.0), // [5]
      positionX(0.0),                     // Nominal zero position
      positionZ(0.0),                     // Nominal zero position
      slitHeight(2.75),                   // [5]
      slitInnerSurfaceAngle(10.0),        // [5]
      slitThickness(0.5),                 // [5]
      bodyMaterial("Aluminium"),          // A-A, TODO: Should be GLIDCOP AL-15
      flangeMaterial("Stainless304L"),    // A-A
      slitMaterial(
          "Tantalum"), // Back View. The part is designated as "Tantalumslit" in
                       // the drawing, therefore it was assumed that it is pure
                       // tantalum. Usually, the material specifications in
                       // the Toyama drawings are more detailed.
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

  Control.addVariable(keyName + "MaskLeftMaxHeight", maskLeftMaxHeight);
  Control.addVariable(keyName + "MaskLeftMaxWidth", maskLeftMaxWidth);
  Control.addVariable(keyName + "MaskBottomMaxHeight", maskBottomMaxHeight);
  Control.addVariable(keyName + "MaskBottomMaxWidth", maskBottomMaxWidth);
  Control.addVariable(keyName + "MaskFocalPoint", maskFocalPoint);
  Control.addVariable(keyName + "MaskDownstreamInnerPlaneAngle",
                      maskDownstreamInnerPlaneAngle);

  Control.addVariable(keyName + "PositionX", positionX);
  Control.addVariable(keyName + "PositionZ", positionZ);

  Control.addVariable(keyName + "SlitHeight", slitHeight);
  Control.addVariable(keyName + "SlitThickness", slitThickness);
  Control.addVariable(keyName + "SlitInnerSurfaceAngle", slitInnerSurfaceAngle);

  Control.addVariable(keyName + "FlangeMaterial", flangeMaterial);
  Control.addVariable(keyName + "BodyMaterial", bodyMaterial);
  Control.addVariable(keyName + "VoidMaterial", voidMaterial);
  Control.addVariable(keyName + "SlitMaterial", slitMaterial);
}

void MovableMaskGenerator::setAperture(const double posX, const double posZ) {
  if (fabs(posX) > 0.5 || fabs(posZ) > 0.5) {
    ELog::EM << "MovableMask: Value outside the nominal configuration space "
                "(|positionX|, |positionZ| < 0.5 cm) set."
             << ELog::endDiag;
  }
  positionX = posX;
  positionZ = posZ;
}

} // NAMESPACE setVariable
