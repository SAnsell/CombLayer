/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGeneratorInc/ProjectionCollimatorGenerator.h
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
#ifndef setVariable_ProjectionCollimatorGenerator_h
#define setVariable_ProjectionCollimatorGenerator_h

#include <array>

class FuncDataBase;

namespace setVariable {

/*!
  \class ProjectionCollimatorGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief ProjectionCollimatorGenerator for variables
*/

template <std::size_t NPoints, std::size_t NSegments>
class ProjectionCollimatorGenerator {
private:
  std::array<std::array<double, NPoints>, NSegments + 1> apertureX;
  std::array<double, NSegments + 1> apertureY;
  std::array<std::array<double, NPoints>, NSegments + 1> apertureZ;

  double height;
  double width;

  std::string material;
  std::string voidMaterial;

public:
  ProjectionCollimatorGenerator();
  ~ProjectionCollimatorGenerator() = default;

  void setAperture(
      const std::array<std::array<double, NPoints>, NSegments + 1> &aX,
      const std::array<double, NSegments + 1> &aY,
      const std::array<std::array<double, NPoints>, NSegments + 1> &aZ) {
    apertureX = aX;
    apertureY = aY;
    apertureZ = aZ;
  };

  void generate(FuncDataBase &, const std::string &) const;
};

} // namespace setVariable

#endif
