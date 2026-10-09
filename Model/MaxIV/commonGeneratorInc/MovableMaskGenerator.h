/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/MovableMaskGenerator.h
 *
 * Copyright (c) 2026 by S. Ansell and U. Friman-Gayer
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
#ifndef setVariable_MovableMaskGenerator_h
#define setVariable_MovableMaskGenerator_h

class FuncDataBase;

namespace setVariable {

/*!
  \class MovableMaskGenerator
  \version 2.0
  \author U. Friman-Gayer
  \date October 2026
  \brief MovableMaskGenerator for variables

  Version history:
  2.0   - 2026-10-09
        - Switch between different movable-mask models by via a template
          parameter.
        - Set aperture position within user-defined (instead of hard-coded)
          limits.
        - Compatibility with MovableMask v2.0.
  1.1.1 - 2026-10-08
          Fix default body material.
  1.1.0 - 2026-09-28
          Introduce SlitInnerOffset parameter for compatibility with MovableMask
          v1.1.
  1.0   - 2026-09-25
*/

class MovableMaskGenerator {
private:
  double length;

  double bodyHeight;
  double bodyLength;
  double bodyWidth;

  int useConnector;
  double connectorInnerEdgeRadius;
  double connectorInnerHeight;
  double connectorInnerWidth;
  double connectorWallThickness;

  double flangeInnerRadius;
  double flangeLength;
  double flangeRadius;
  double flangeWallThick;

  double holeHeight;
  double holeOffsetHorizontal;
  double holeOffsetVertical;
  double holeWidth;

  double maskLeftMaxHeight;
  double maskLeftMaxWidth;
  double maskBottomMaxHeight;
  double maskBottomMaxWidth;
  double maskDownstreamInnerPlaneAngle;

  double positionX;
  double positionXMax;
  double positionXMin;
  double positionZ;
  double positionZMax;
  double positionZMin;

  double slitInnerOffset;
  double slitInnerSurfaceAngle;
  double slitThickness;

  std::string bodyMaterial;
  std::string flangeMaterial;
  std::string slitMaterial;
  std::string voidMaterial;

public:
  MovableMaskGenerator();
  ~MovableMaskGenerator() = default;

  template <typename Dimensions> void setDimensions();
  void setNominalZero() {
    positionX = 0.0;
    positionZ = 0.0;
  }
  void setFullyOpen() {
    positionX = positionXMax;
    positionZ = positionZMax;
  }
  void setFullyClose() {
    positionX = positionXMin;
    positionZ = positionZMin;
  }
  void setAperture(const double posX, const double posZ);
  void generate(FuncDataBase &, const std::string &) const;
};

} // namespace setVariable

#endif
