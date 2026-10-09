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
#include "MovableMaskToyama.h"

namespace setVariable {

MovableMaskGenerator::MovableMaskGenerator()
    :                   // All dimensions from [1] if not indicated otherwise.
      length(30.0),     // A-A
      bodyHeight(7.2),  // X VIEW (S=1/1)
      bodyLength(24.4), // A-A
      bodyWidth(7.2),   // X VIEW (S=1/1)
      useConnector(true), connectorInnerEdgeRadius(0.5), // [5]
      connectorInnerHeight(5.2),                         // [5]
      connectorInnerWidth(4.3),                          // [5]
      connectorWallThickness(0.5),                       // [5]
      flangeInnerRadius(CF63::innerRadius),              // Side View
      flangeLength(CF63::flangeLength),                  // Side View
      flangeRadius(CF63::flangeRadius),                  // Side View
      flangeWallThick(CF63::wallThick),                  // Side View
      holeHeight(3.0),                                   // X VIEW (S=1/1)
      holeOffsetHorizontal(0.0),                         // X VIEW (S=1/1)
      holeOffsetVertical(
          0.45), // X VIEW (S=1/1), difference between hole height and depth
      holeWidth(3.12),        // X VIEW (S=1/1)
      maskLeftMaxHeight(2.1), // X VIEW (S=1/1), 2 x 10.5 mm, assuming that the
                              // part is symmetric about the central axis
      maskLeftMaxWidth(1.3),  // X VIEW (S=1/1)
      maskBottomMaxHeight(0.79), // X VIEW (S=1/1)
      maskBottomMaxWidth(
          2.8), // X VIEW, 15.6 mm (half hole width) + 5.0 mm (beam offset)
                // + 2.0 mm (half beam width) + 5.4 mm
      maskDownstreamInnerPlaneAngle(9.0), // [5]
      positionX(0.0),                     // Nominal zero position
      positionZ(0.0),                     // Nominal zero position
      slitInnerOffset(0.01), // X VIEW (S=1/1), "0.1/slit", see also the slit
                             // dimensions given in parentheses.
      slitInnerSurfaceAngle(10.0),     // [5]
      slitThickness(0.5),              // [5]
      bodyMaterial("Copper"),          // A-A, TODO: Should be GLIDCOP AL-15
      flangeMaterial("Stainless304L"), // A-A
      slitMaterial(
          "Tantalum"), // Back View. The part is designated as "Tantalumslit" in
                       // the drawing, therefore it was assumed that it is pure
                       // tantalum. Usually, the material specifications in
                       // the Toyama drawings are more detailed.
      voidMaterial("Void") {}

template <typename Dimensions> void MovableMaskGenerator::setDimensions() {
  length = Dimensions::length;

  bodyHeight = Dimensions::bodyHeight;
  bodyLength = Dimensions::bodyLength;
  bodyWidth = Dimensions::bodyWidth;

  useConnector = Dimensions::useConnector;

  connectorInnerEdgeRadius = Dimensions::connectorInnerEdgeRadius;
  connectorInnerHeight = Dimensions::connectorInnerHeight;
  connectorInnerWidth = Dimensions::connectorInnerWidth;

  holeHeight = Dimensions::holeHeight;
  holeOffsetHorizontal = Dimensions::holeOffsetHorizontal;
  holeOffsetVertical = Dimensions::holeOffsetVertical;
  holeWidth = Dimensions::holeWidth;

  maskLeftMaxHeight = Dimensions::maskLeftMaxHeight;
  maskLeftMaxWidth = Dimensions::maskLeftMaxWidth;
  maskBottomMaxHeight = Dimensions::maskBottomMaxHeight;
  maskBottomMaxWidth = Dimensions::maskBottomMaxWidth;
  maskDownstreamInnerPlaneAngle = Dimensions::maskDownstreamInnerPlaneAngle;

  slitInnerOffset = Dimensions::slitInnerOffset;
  slitInnerSurfaceAngle = Dimensions::slitInnerSurfaceAngle;
  slitThickness = Dimensions::slitThickness;

  bodyMaterial = Dimensions::bodyMaterial;
  flangeMaterial = Dimensions::flangeMaterial;
  slitMaterial = Dimensions::slitMaterial;
  voidMaterial = Dimensions::voidMaterial;
}

void MovableMaskGenerator::generate(FuncDataBase &Control,
                                    const std::string &keyName) const {

  Control.addVariable(keyName + "Length", length);

  Control.addVariable(keyName + "BodyHeight", bodyHeight);
  Control.addVariable(keyName + "BodyLength", bodyLength);
  Control.addVariable(keyName + "BodyWidth", bodyWidth);

  Control.addVariable(keyName + "UseConnector", useConnector);
  Control.addVariable(keyName + "ConnectorInnerEdgeRadius",
                      connectorInnerEdgeRadius);
  Control.addVariable(keyName + "ConnectorInnerHeight", connectorInnerHeight);
  Control.addVariable(keyName + "ConnectorInnerWidth", connectorInnerWidth);
  Control.addVariable(keyName + "ConnectorWallThickness",
                      connectorWallThickness);

  Control.addVariable(keyName + "FlangeInnerRadius", flangeInnerRadius);
  Control.addVariable(keyName + "FlangeLength", flangeLength);
  Control.addVariable(keyName + "FlangeRadius", flangeRadius);
  Control.addVariable(keyName + "FlangeWallThick", flangeWallThick);

  Control.addVariable(keyName + "HoleHeight", holeHeight);
  Control.addVariable(keyName + "HoleOffsetHorizontal", holeOffsetHorizontal);
  Control.addVariable(keyName + "HoleOffsetVertical", holeOffsetVertical);
  Control.addVariable(keyName + "HoleWidth", holeWidth);

  Control.addVariable(keyName + "MaskLeftMaxHeight", maskLeftMaxHeight);
  Control.addVariable(keyName + "MaskLeftMaxWidth", maskLeftMaxWidth);
  Control.addVariable(keyName + "MaskBottomMaxHeight", maskBottomMaxHeight);
  Control.addVariable(keyName + "MaskBottomMaxWidth", maskBottomMaxWidth);
  Control.addVariable(keyName + "MaskDownstreamInnerPlaneAngle",
                      maskDownstreamInnerPlaneAngle);

  Control.addVariable(keyName + "PositionX", positionX);
  Control.addVariable(keyName + "PositionZ", positionZ);

  Control.addVariable(keyName + "SlitInnerOffset", slitInnerOffset);
  Control.addVariable(keyName + "SlitInnerSurfaceAngle", slitInnerSurfaceAngle);
  Control.addVariable(keyName + "SlitThickness", slitThickness);

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

template void MovableMaskGenerator::setDimensions<MovableMaskToyamaR3B2>();
template void MovableMaskGenerator::setDimensions<MovableMaskToyamaR3B3>();
template void MovableMaskGenerator::setDimensions<MovableMaskToyamaR3B3B4>();
template void MovableMaskGenerator::setDimensions<MovableMaskToyamaR3B5>();

} // NAMESPACE setVariable
