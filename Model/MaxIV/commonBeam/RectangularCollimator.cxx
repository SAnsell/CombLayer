/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/RectangularCollimator.cxx
 *
 * Copyright (c) 2004-2026 by S. Ansell and U. Friman-Gayer
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

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "RectangularCollimator.h"

namespace xraySystem {
template <std::size_t N>
RectangularCollimator<N>::RectangularCollimator(const std::string &Key)
    : attachSystem::ContainedComp(), attachSystem::FixedRotate(Key),
      attachSystem::CellMap(), attachSystem::SurfMap() {}

template <std::size_t N>
RectangularCollimator<N>::RectangularCollimator(const RectangularCollimator &A)
    : attachSystem::ContainedComp(A), attachSystem::FixedRotate(A),
      attachSystem::FrontBackCut(A), attachSystem::CellMap(A),
      attachSystem::SurfMap(A) {}

template <std::size_t N>
RectangularCollimator<N> &
RectangularCollimator<N>::operator=(const RectangularCollimator<N> &A) {
  if (this != &A) {
    attachSystem::ContainedComp::operator=(A);
    attachSystem::FixedRotate::operator=(A);
    attachSystem::CellMap::operator=(A);
    attachSystem::SurfMap::operator=(A);
  }
  return *this;
}

template <std::size_t N>
int RectangularCollimator<N>::planeIndex(const unsigned int n_segment,
                                         const unsigned int side) const {
  if (n_segment == N - 1 && side == 1) {
    return 2;
  }
  if (side > 1) {
    return (n_segment + 1) * 10 + side;
  }
  return n_segment * 10 + side;
}

template <std::size_t N>
void RectangularCollimator<N>::buildInnerPlane(const unsigned int n_segment,
                                               const unsigned int side) {
  bool isOddSide = side % 2;
  double angle;
  Geometry::Vec3D normalVector, rotationAxis;
  Geometry::Vec3D planeOrigin = Y * apertureY[n_segment];
  if (side < 5) {
    normalVector = X;
    rotationAxis = -Z;
    angle = isOddSide ? tan((apertureX[n_segment + 1].first -
                             apertureX[n_segment].first) /
                            (apertureY[n_segment + 1] - apertureY[n_segment]))
                      : tan((apertureX[n_segment + 1].second -
                             apertureX[n_segment].second) /
                            (apertureY[n_segment + 1] - apertureY[n_segment]));
    planeOrigin += isOddSide ? X * apertureX[n_segment].first
                             : X * apertureX[n_segment].second;
  } else {
    normalVector = Z;
    rotationAxis = X;
    angle = isOddSide ? tan((apertureZ[n_segment + 1].first -
                             apertureZ[n_segment].first) /
                            (apertureY[n_segment + 1] - apertureY[n_segment]))
                      : tan((apertureZ[n_segment + 1].second -
                             apertureZ[n_segment].second) /
                            (apertureY[n_segment + 1] - apertureY[n_segment]));
    planeOrigin += isOddSide ? Z * apertureZ[n_segment].first
                             : Z * apertureZ[n_segment].second;
  }
  normalVector.rotate(rotationAxis, angle);
  ModelSupport::buildPlane(SMap, buildIndex + planeIndex(n_segment, side),
                           Origin + planeOrigin, normalVector);
}

template <std::size_t N>
void RectangularCollimator<N>::populate(const FuncDataBase &Control) {
  FixedRotate::populate(Control);

  apertureY[0] = 0.0;
  for (size_t n = 0; n < N; ++n) {
    apertureX[n].first =
        Control.EvalVar<double>(keyName + "ApertureXMin" + std::to_string(n));
    apertureX[n].second =
        Control.EvalVar<double>(keyName + "ApertureXMax" + std::to_string(n));
    if (n > 0) {
      apertureY[n] =
          Control.EvalVar<double>(keyName + "ApertureY" + std::to_string(n));
    }
    apertureZ[n].first =
        Control.EvalVar<double>(keyName + "ApertureZMin" + std::to_string(n));
    apertureZ[n].second =
        Control.EvalVar<double>(keyName + "ApertureZMax" + std::to_string(n));
  }

  height = Control.EvalVar<double>(keyName + "Height");
  width = Control.EvalVar<double>(keyName + "Width");

  material = ModelSupport::EvalMat<int>(Control, keyName + "Material");
  voidMaterial = ModelSupport::EvalDefMat(Control, keyName + "VoidMaterial", 0);
}

template <std::size_t N> void RectangularCollimator<N>::createSurfaces() {

  if (!isActive("front")) {
    ModelSupport::buildPlane(SMap, buildIndex + 1, Origin, Y);
    setFront(SMap.realSurf(buildIndex + 1));
  }
  if (!isActive("back")) {
    ModelSupport::buildPlane(SMap, buildIndex + 2,
                             Origin + Y * apertureY[N - 1], Y);
    setBack(-SMap.realSurf(buildIndex + 2));
  }

  ModelSupport::buildPlane(SMap, buildIndex + 3, Origin - X * width / 2.0, X);
  ModelSupport::buildPlane(SMap, buildIndex + 4, Origin + X * width / 2.0, X);
  ModelSupport::buildPlane(SMap, buildIndex + 5, Origin - Z * height / 2.0, Z);
  ModelSupport::buildPlane(SMap, buildIndex + 6, Origin + Z * height / 2.0, Z);

  for (unsigned int n = 1; n < N - 1; ++n) {
    ModelSupport::buildPlane(SMap, buildIndex + planeIndex(n, 1),
                             Origin + Y * apertureY[n], Y);
  }
  for (unsigned int n = 0; n < N - 1; ++n) {
    for (unsigned int side = 3; side < 7; ++side) {
      buildInnerPlane(n, side);
    }
  }
}

template <std::size_t N>
void RectangularCollimator<N>::createObjects(Simulation &System) {
  if (N == 2) {
    makeCell("Segment0", System, cellIndex++, material, 0.0,
             ModelSupport::getHeadRule(SMap, buildIndex, "1 -2 3 -4 5 -6"));
  } else {
    HeadRule hole, segmentLimits;
    for (unsigned int n = 0; n < N - 1; ++n) {
      segmentLimits =
          ModelSupport::getHeadRule(SMap, buildIndex,
                                    std::to_string(planeIndex(n, 1)) + " -" +
                                        std::to_string(planeIndex(n + 1, 1)));
      hole = ModelSupport::getHeadRule(
          SMap, buildIndex,
          std::to_string(planeIndex(n, 3)) + " -" +
              std::to_string(planeIndex(n, 4)) + " " +
              std::to_string(planeIndex(n, 5)) + " -" +
              std::to_string(planeIndex(n, 6)));
      makeCell("Segment" + std::to_string(n), System, cellIndex++, material,
               0.0,
               ModelSupport::getHeadRule(SMap, buildIndex, "3 -4 5 -6") *
                   segmentLimits * hole.complement());
      makeCell("SegmentHole" + std::to_string(n), System, cellIndex++,
               voidMaterial, 0.0, segmentLimits * hole);
    }
  }

  addOuterSurf(ModelSupport::getHeadRule(SMap, buildIndex, "1 -2 3 -4 5 -6"));
}

template <std::size_t N> void RectangularCollimator<N>::createLinks() {
  FrontBackCut::createLinks(*this, Origin, Y);
}

template <std::size_t N>
void RectangularCollimator<N>::createAll(Simulation &System,
                                         const attachSystem::FixedComp &FC,
                                         const long int sideIndex) {
  populate(System.getDataBase());
  createUnitVector(FC, sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
}

template class RectangularCollimator<2>;
template class RectangularCollimator<3>;
template class RectangularCollimator<4>;

} // namespace xraySystem
