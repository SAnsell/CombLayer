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
*/

namespace setVariable {
struct MovableMaskToyamaR3B3 {
  // Data from [1] and [5]
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
  static constexpr double holeOffset = 0.45;
  static constexpr double holeWidth = 3.12;

  static constexpr double maskLeftMaxHeight = 2.1;
  static constexpr double maskLeftMaxWidth = 1.3;
  static constexpr double maskBottomMaxHeight = 0.79;
  static constexpr double maskBottomMaxWidth = 2.8;
  static constexpr double maskFocalPoint = 0.376;
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0;

  static constexpr double slitHeight = 2.75;
  static constexpr double slitInnerOffset = 0.01;
  static constexpr double slitInnerSurfaceAngle = 10.0;
  static constexpr double slitThickness = 0.5;

  static constexpr std::string bodyMaterial = "Copper";
  static constexpr std::string flangeMaterial = "Stainless304L";
  static constexpr std::string slitMaterial = "Tantalum";
  static constexpr std::string voidMaterial = "Void";
};

struct MovableMaskToyamaR3B5 {
  // Data from [18] and [19]
  // With the available software, measurements in the CAD model [19] were not
  // possible. Therefore, some values are marked as "Estimate".
  static constexpr double length = 30.0;
  static constexpr double bodyHeight = 9.5;
  static constexpr double bodyLength = 26.0;
  static constexpr double bodyWidth = 7.6;

  static constexpr bool useConnector = false;

  static constexpr double connectorInnerEdgeRadius = 0.5;
  static constexpr double connectorInnerHeight = 3.95;
  static constexpr double connectorInnerWidth = 5.2;

  static constexpr double holeHeight = 2.0;
  static constexpr double holeOffset = 0.0;
  static constexpr double holeWidth = 3.85;

  static constexpr double maskLeftMaxHeight = 1.5;
  static constexpr double maskLeftMaxWidth = 0.74;
  static constexpr double maskBottomMaxHeight = 0.84;
  static constexpr double maskBottomMaxWidth = 3.59;
  static constexpr double maskFocalPoint = 0.5;                // Estimate
  static constexpr double maskDownstreamInnerPlaneAngle = 9.0; // Estimate

  static constexpr double slitHeight = 1.45; // Estimate
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