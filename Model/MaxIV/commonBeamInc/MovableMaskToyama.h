/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/MovableMaskToyama.h
 *
 * Copyright (c) 2004-2026 by S. Ansell and Udo Friman-Gayer
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

/*!
  \class MovableMaskToyama
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief Dimensions of Movable Masks in Toyama Design

  See the documentation of the MovableMask and MovableMaskGenerator classes for
  more information.

  The models are identified by the location of their beamline (R1 or R3 ring at
  MAX IV) and batch numbers (B2 - B5, some models apply to multiple batches).
  For all of the models, it was found that Movable Mask 1 (upstream) is
  identical to Movable Mask 2 (downstream) (up to the orientation) within the
  precision of the MovableMask class.
*/

namespace setVariable {

struct MovableMaskToyamaR3B2 {
  /*
    Data from [6,7]. These are the least informative of the available 2D
    drawings, therefore some dimensions were assumed to be the same as in the
    short version of B3.

    Position Name | positionX (cm) | positionZ (cm) | Aperture (cm2)
    ----------------------------------------------------------------
    Fully open    |      0.8       |       0.8      |  2.0   x  2.0
    Nominal       |      0.0       |       0.0      |  1.2   x  1.2
    Fully close   |     -1.7       |      -1.7      | -0.5   x -0.5
  */
  static constexpr double length = 24.0;

  static constexpr double bodyHeight = 7.2;
  static constexpr double bodyLength = 18.4;
  static constexpr double bodyWidth = 7.2;

  static constexpr bool useConnector = true;

  static constexpr double connectorInnerEdgeRadius = 0.5;
  static constexpr double connectorInnerHeight = 5.2;
  static constexpr double connectorInnerWidth = 4.3;
  static constexpr double connectorWallThickness = 0.5;

  static constexpr double holeHeight = 5.1;
  static constexpr double holeOffsetHorizontal = 0.0;
  static constexpr double holeOffsetVertical = 0.0;
  static constexpr double holeWidth = 4.2;

  static constexpr double maskLeftMaxHeight =
      3.825; // Estimated, 3/4 of the hole height.
  static constexpr double maskLeftMaxWidth = 1.515;
  static constexpr double maskBottomMaxHeight = 1.965;
  static constexpr double maskBottomMaxWidth =
      3.36; // Estimated, 4/5 of the hole width.
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0;

  // All Min/Max positions estimated, since there is no information about this
  // in [6,7].
  static constexpr double positionXMax = 0.8;
  static constexpr double positionXMin = -1.7;
  static constexpr double positionZMax = 0.8;
  static constexpr double positionZMin = -1.7;

  static constexpr double slitInnerOffset =
      0.015; // Determined such that the default aperture in [6,7] results in a
             // "round" value of 1.2 cm x 1.2 cm.
  static constexpr double slitInnerSurfaceAngle = 10.0;
  static constexpr double slitThickness = 0.5;

  static constexpr std::string bodyMaterial = "Copper";
  static constexpr std::string flangeMaterial = "Stainless304L";
  static constexpr std::string slitMaterial = "Tantalum";
  static constexpr std::string voidMaterial = "Void";
};

struct MovableMaskToyamaR3B3 {
  /*
    Data from [9,11]. Since a 3D model of the short mask in B3 was not
    available, assumed that all missing dimensions are the same as in the long
    mask [5].

    Position Name | positionX (cm) | positionZ (cm) | Aperture (cm2)
    ----------------------------------------------------------------
    Fully open    |      0.5       |       0.5      |  0.75  x  0.75
    Nominal       |      0.0       |       0.0      |  0.25  x  0.25
    Fully close   |     -0.5       |      -0.5      | -0.25  x -0.25
  */

  static constexpr double length = 24.0;

  static constexpr double bodyHeight = 7.2;
  static constexpr double bodyLength = 18.4;
  static constexpr double bodyWidth = 7.2;

  static constexpr bool useConnector = true;

  static constexpr double connectorInnerEdgeRadius = 0.5;
  static constexpr double connectorInnerHeight = 5.2;
  static constexpr double connectorInnerWidth = 4.3;
  static constexpr double connectorWallThickness = 0.5;

  static constexpr double holeHeight = 3.0;
  static constexpr double holeOffsetHorizontal = 0.0;
  static constexpr double holeOffsetVertical = 0.45;
  static constexpr double holeWidth = 3.12;

  static constexpr double maskLeftMaxHeight = 2.1;
  static constexpr double maskLeftMaxWidth = 1.3;
  static constexpr double maskBottomMaxHeight = 0.79;
  static constexpr double maskBottomMaxWidth = 2.8;
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0;

  static constexpr double positionXMax = 0.5;
  static constexpr double positionXMin = -0.5;
  static constexpr double positionZMax = 0.5;
  static constexpr double positionZMin = -0.5;

  static constexpr double slitInnerOffset = 0.01;
  static constexpr double slitInnerSurfaceAngle = 10.0;
  static constexpr double slitThickness = 0.5;

  static constexpr std::string bodyMaterial = "Copper";
  static constexpr std::string flangeMaterial = "Stainless304L";
  static constexpr std::string slitMaterial = "Tantalum";
  static constexpr std::string voidMaterial = "Void";
};

struct MovableMaskToyamaR3B3B4 {
  /*
     Data from [2,4] and [5] for B3.
     Verified from Ref. [13,15] that the dimensions in the 2D drawings of B4 are
     identical to the long version of B3. Since a 3D model of B4 was not
     available, assumed that all missing dimensions are the same as in the long
     B3.

    Position Name | positionX (cm) | positionZ (cm) | Aperture (cm2)
    ----------------------------------------------------------------
    Fully open    |      0.5       |       0.5      |  0.75  x  0.75
    Nominal       |      0.0       |       0.0      |  0.25  x  0.25
    Fully close   |     -0.5       |      -0.5      | -0.25  x -0.25
  */

  static constexpr double length = 30.0;

  static constexpr double bodyHeight = 7.2;
  static constexpr double bodyLength = 24.4;
  static constexpr double bodyWidth = 7.2;

  static constexpr bool useConnector = true;

  static constexpr double connectorInnerEdgeRadius = 0.5;
  static constexpr double connectorInnerHeight = 5.2;
  static constexpr double connectorInnerWidth = 4.3;
  static constexpr double connectorWallThickness = 0.5;

  static constexpr double holeHeight = 3.0;
  static constexpr double holeOffsetHorizontal = 0.0;
  static constexpr double holeOffsetVertical = 0.45;
  static constexpr double holeWidth = 3.12;

  static constexpr double maskLeftMaxHeight = 2.1;
  static constexpr double maskLeftMaxWidth = 1.3;
  static constexpr double maskBottomMaxHeight = 0.79;
  static constexpr double maskBottomMaxWidth = 2.8;
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0;

  static constexpr double positionXMax = 0.5;
  static constexpr double positionXMin = -0.5;
  static constexpr double positionZMax = 0.5;
  static constexpr double positionZMin = -0.5;

  static constexpr double slitInnerOffset = 0.01;
  static constexpr double slitInnerSurfaceAngle = 10.0;
  static constexpr double slitThickness = 0.5;

  static constexpr std::string bodyMaterial = "Copper";
  static constexpr std::string flangeMaterial = "Stainless304L";
  static constexpr std::string slitMaterial = "Tantalum";
  static constexpr std::string voidMaterial = "Void";
};

struct MovableMaskToyamaR3B5 {
  /*
    Data from [18] and [19]
    With the available software, measurements in the CAD model [19] were not
    possible. Therefore, some values are marked as "Estimate".

    Position Name | positionX (cm) | positionZ (cm) | Aperture (cm2)
    ----------------------------------------------------------------
    Fully open    |      0.375     |       0.0      |  2.65  x  0.3
    Nominal       |      0.0       |       0.0      |  1.9   x  0.3
    Fully close   |     -1.5       |      -0.3      | -0.55  x -0.3
  */

  static constexpr double length = 30.0;

  static constexpr double bodyHeight = 9.5;
  static constexpr double bodyLength = 26.0;
  static constexpr double bodyWidth = 7.6;

  static constexpr bool useConnector = false;

  static constexpr double connectorInnerEdgeRadius = 0.5;
  static constexpr double connectorInnerHeight = 3.95;
  static constexpr double connectorInnerWidth = 5.2;

  static constexpr double holeHeight = 2.0;
  static constexpr double holeOffsetHorizontal = -0.225;
  static constexpr double holeOffsetVertical = 0.0;
  static constexpr double holeWidth = 3.85;

  static constexpr double maskLeftMaxHeight = 1.5;
  static constexpr double maskLeftMaxWidth = 0.74;
  static constexpr double maskBottomMaxHeight = 0.84;
  static constexpr double maskBottomMaxWidth = 3.6;
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0; // Estimate

  static constexpr double positionXMax = 0.375;
  static constexpr double positionXMin = -1.5;
  static constexpr double positionZMax = 0.375;
  static constexpr double positionZMin = -0.3;

  static constexpr double slitInnerOffset = 0.01;
  static constexpr double slitInnerSurfaceAngle =
      -1.0; // Estimate. Note the different sign.
  static constexpr double slitThickness =
      0.5; // In reality, the slit is not a single piece, but the "left" part is
           // attached at the downstream side of the "bottom" part. Assuming a
           // common thickness for both parts should be accurate within +- 10%.

  static constexpr std::string bodyMaterial =
      "Copper"; // Both "CuCrZr" and "CrCu" shown as body material in [18].
                // Using pure copper as approximation.
  static constexpr std::string flangeMaterial = "Stainless304L";
  static constexpr std::string slitMaterial = "Tantalum";
  static constexpr std::string voidMaterial = "Void";
};
} // namespace setVariable