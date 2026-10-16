/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGenerator/ProjectionCollimatorGenerator.cxx
 *
 * Copyright (c) 2026 U. Friman-Gayer
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"

#include "ProjectionCollimatorGenerator.h"

namespace setVariable {

template <std::size_t NPoints, std::size_t NSegments>
ProjectionCollimatorGenerator<NPoints,
                              NSegments>::ProjectionCollimatorGenerator()
    : apertureX{}, apertureY{}, apertureZ{}, height(10.0), width(10.0),
      material("Copper"), voidMaterial("Void") {}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimatorGenerator<NPoints, NSegments>::generate(
    FuncDataBase &Control, const std::string &keyName) const {

  for (size_t nSection = 0; nSection < NSegments + 1; ++nSection) {
    for (size_t nPoint = 0; nPoint < NPoints; ++nPoint) {
      Control.addVariable(keyName + "X" + std::to_string(nPoint) + "AtY" +
                              std::to_string(nSection),
                          apertureX[nSection][nPoint]);
      if (nSection > 0) {
        Control.addVariable(keyName + "Y" + std::to_string(nSection),
                            apertureY[nSection]);
      }
      Control.addVariable(keyName + "Z" + std::to_string(nPoint) + "AtY" +
                              std::to_string(nSection),
                          apertureZ[nSection][nPoint]);
    }
  }

  Control.addVariable(keyName + "Height", height);
  Control.addVariable(keyName + "Width", width);

  Control.addVariable(keyName + "Material", material);
  Control.addVariable(keyName + "VoidMaterial", voidMaterial);
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimatorGenerator<NPoints, NSegments>::
    setRoundedRectangularAperture(
        const std::array<std::array<double, 2>, NSegments + 1> &aX,
        const std::array<double, NSegments + 1> &aY,
        const std::array<std::array<double, 2>, NSegments + 1> &aZ,
        const double radius) {
  if (NPoints < 4) {
    throw ColErr::SizeError<size_t>(
        NPoints, 4,
        "At least four points are required to draw a rectangular aperture.");
  }
  if (NPoints % 4 != 0) {
    ELog::EM << "Warning: Given number of points is not divisible by four. The "
                "resulting shape will not be symmetric."
             << ELog::endDiag;
  }

  apertureY = aY;

  std::array<size_t, 4> nPointsPerQuadrant;
  nPointsPerQuadrant.fill(NPoints / 4);
  for (size_t n = 0; n < NPoints % 4; ++n) {
    nPointsPerQuadrant[n]++;
  }

  size_t nPoint = 0;
  double angleStep, cosAngle, sinAngle, x(0.0), z(0.0);
  for (size_t nSection = 0; nSection < NSegments + 1; ++nSection) {
    nPoint = 0;
    for (size_t quadrant = 0; quadrant < 4; ++quadrant) {
      if (nPointsPerQuadrant[quadrant] == 1) {
        switch (quadrant) {
        case 0:
          apertureX[nSection][nPoint] = aX[nSection][1];
          apertureZ[nSection][nPoint] = aZ[nSection][1];
          break;
        case 1:
          apertureX[nSection][nPoint] = aX[nSection][0];
          apertureZ[nSection][nPoint] = aZ[nSection][1];
          break;
        case 2:
          apertureX[nSection][nPoint] = aX[nSection][0];
          apertureZ[nSection][nPoint] = aZ[nSection][0];
          break;
        case 3:
          apertureX[nSection][nPoint] = aX[nSection][1];
          apertureZ[nSection][nPoint] = aZ[nSection][0];
          break;
          ++nPoint;
        }
      } else {
        angleStep = M_PI_2 / (double)(nPointsPerQuadrant[quadrant] - 1);
        for (size_t nQuadrantPoint = 0;
             nQuadrantPoint < nPointsPerQuadrant[quadrant]; ++nQuadrantPoint) {
          cosAngle = cos(angleStep * (double)nQuadrantPoint /
                         (double)(nPointsPerQuadrant[quadrant] - 1));
          sinAngle = sin(angleStep * (double)nQuadrantPoint /
                         (double)(nPointsPerQuadrant[quadrant] - 1));
          switch (quadrant) {
          case 0:
            apertureX[nSection][nPoint] = aX[nSection][1] - radius * sinAngle;
            apertureZ[nSection][nPoint] = aZ[nSection][1] - radius * cosAngle;
            break;
          case 1:
            apertureX[nSection][nPoint] = aX[nSection][0] + radius * cosAngle;
            apertureZ[nSection][nPoint] = aZ[nSection][1] - radius * sinAngle;
            break;
          case 2:
            apertureX[nSection][nPoint] = aX[nSection][0] + radius * sinAngle;
            apertureZ[nSection][nPoint] = aZ[nSection][0] + radius * cosAngle;
            break;
          case 3:
            apertureX[nSection][nPoint] = aX[nSection][1] - radius * cosAngle;
            apertureZ[nSection][nPoint] = aZ[nSection][0] + radius * sinAngle;
            break;
          }
          ++nPoint;
        }
      }
    }
  }
}

template class ProjectionCollimatorGenerator<4, 2>;
template class ProjectionCollimatorGenerator<8, 2>;

} // NAMESPACE setVariable
