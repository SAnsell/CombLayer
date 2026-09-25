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
  \version 1.0
  \author U. Friman-Gayer
  \date September 2026
  \brief MovableMaskGenerator for variables

  Version history:
  1.0   - 2026-09-25
*/

class MovableMaskGenerator {
private:
  double length;

  double bodyHeight;
  double bodyLength;
  double bodyWidth;

  double flangeInnerRadius;
  double flangeLength;
  double flangeRadius;
  double flangeWallThick;

  double holeHeight;
  double holeOffset;
  double holeWidth;

  double maskLeftMaxHeight;
  double maskLeftMaxWidth;
  double maskBottomMaxHeight;
  double maskBottomMaxWidth;
  double maskFocalPoint;
  double maskDownstreamInnerPlaneAngle;

  double positionX;
  double positionZ;

  double slitHeight;
  double slitInnerSurfaceAngle;
  double slitThickness;

  std::string bodyMaterial;
  std::string flangeMaterial;
  std::string slitMaterial;
  std::string voidMaterial;

public:
  MovableMaskGenerator();
  ~MovableMaskGenerator() = default;

  void setNominalZero() {
    positionX = 0.0;
    positionZ = 0.0;
  }
  void setFullyOpen() {
    positionX = 0.5;
    positionZ = 0.5;
  }
  void setFullyClose() {
    positionX = -0.5;
    positionZ = -0.5;
  }
  void setAperture(const double posX, const double posZ);
  void generate(FuncDataBase &, const std::string &) const;
};

} // namespace setVariable

#endif
