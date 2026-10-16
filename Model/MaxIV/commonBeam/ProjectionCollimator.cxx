/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/ProjectionCollimator.cxx
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

#include <array>
#include <fstream>
#include <iomanip>
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
#include "ProjectionCollimator.h"

namespace xraySystem {
template <std::size_t NPoints, std::size_t NSegments>
ProjectionCollimator<NPoints, NSegments>::ProjectionCollimator(
    const std::string &Key)
    : attachSystem::ContainedComp(), attachSystem::FixedRotate(Key),
      attachSystem::CellMap(), attachSystem::SurfMap() {}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::populate(
    const FuncDataBase &Control) {
  FixedRotate::populate(Control);

  apertureY[0] = 0.0;
  for (size_t nSection = 0; nSection < NSegments + 1; ++nSection) {
    for (size_t nPoint = 0; nPoint < NPoints; ++nPoint) {
      apertureX[nSection][nPoint] =
          Control.EvalVar<double>(keyName + "X" + std::to_string(nPoint) +
                                  "AtY" + std::to_string(nSection));
      if (nSection > 0) {
        apertureY[nSection] =
            Control.EvalVar<double>(keyName + "Y" + std::to_string(nSection));
      }
      apertureZ[nSection][nPoint] =
          Control.EvalVar<double>(keyName + "Z" + std::to_string(nPoint) +
                                  "AtY" + std::to_string(nSection));
    }
  }

  height = Control.EvalVar<double>(keyName + "Height");
  width = Control.EvalVar<double>(keyName + "Width");

  material = ModelSupport::EvalMat<int>(Control, keyName + "Material");
  voidMaterial = ModelSupport::EvalDefMat(Control, keyName + "VoidMaterial", 0);
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::initializePoints(
    std::array<std::array<Geometry::Vec3D, NPoints + 1>, NSegments + 1>
        &points) {
  for (unsigned int nSection = 0; nSection < NSegments + 1; ++nSection) {
    for (unsigned int nPoint = 0; nPoint < NPoints; ++nPoint) {
      points[nSection][nPoint] =
          Geometry::Vec3D(apertureX[nSection][nPoint], apertureY[nSection],
                          apertureZ[nSection][nPoint]);
    }
    points[nSection][NPoints] = points[nSection][0];
  }
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::findCOM(
    std::array<Geometry::Vec3D, NSegments> &com,
    const std::array<std::array<Geometry::Vec3D, NPoints + 1>, NSegments + 1>
        &points) {
  const double norm = 1.0 / (double)NPoints;
  for (size_t nSegment = 0; nSegment < NSegments; ++nSegment) {
    com[nSegment] = Geometry::Vec3D(0.0, 0.0, 0.0);
    for (size_t nPoint = 0; nPoint < NPoints; ++nPoint) {
      com[nSegment] += points[nSegment][nPoint];
    }
    com[nSegment] = com[nSegment] * norm;
  }
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::createSurfaces() {

  if (!isActive("front")) {
    ModelSupport::buildPlane(SMap, buildIndex + 1, Origin, Y);
    setFront(SMap.realSurf(buildIndex + 1));
  }
  if (!isActive("back")) {
    ModelSupport::buildPlane(SMap, buildIndex + 2,
                             Origin + Y * apertureY[NSegments], Y);
    setBack(-SMap.realSurf(buildIndex + 2));
  }

  std::array<std::array<Geometry::Vec3D, NPoints + 1>, NSegments + 1> points;
  initializePoints(points);
  std::array<Geometry::Vec3D, NSegments> com;
  findCOM(com, points);

  ModelSupport::buildPlane(SMap, buildIndex + 3, Origin - X * width / 2.0, X);
  ModelSupport::buildPlane(SMap, buildIndex + 4, Origin + X * width / 2.0, X);
  ModelSupport::buildPlane(SMap, buildIndex + 5, Origin - Z * height / 2.0, Z);
  ModelSupport::buildPlane(SMap, buildIndex + 6, Origin + Z * height / 2.0, Z);

  for (unsigned int nSection = 0; nSection < NSegments + 1; ++nSection) {
    if (nSection > 0 && nSection < NSegments) {
      ModelSupport::buildPlane(SMap, buildIndex + nSection * 10 + 1,
                               Origin + Y * apertureY[nSection], Y);
    }
    for (unsigned int nPoint = 0; nPoint < NPoints; ++nPoint) {
      ModelSupport::buildPlane(
          SMap, buildIndex + (int)(nSection * NPoints + nPoint + 1) * 10 + 3,
          Origin + points[nSection][nPoint],
          (points[nSection][nPoint + 1] - points[nSection][nPoint]) *
              (points[nSection + 1][nPoint] - points[nSection][nPoint]));
      ModelSupport::buildPlane(
          SMap, buildIndex + (int)(nSection * NPoints + nPoint + 1) * 10 + 5,
          Origin + points[nSection][nPoint],
          (com[nSection] - points[nSection][nPoint]) *
              (points[nSection + 1][nPoint] - points[nSection][nPoint]));
    }
  }
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::createObjects(
    Simulation &System) {

  std::string holestr;
  HeadRule holeSegment, segmentFront, segmentBack;
  const HeadRule body =
      ModelSupport::getHeadRule(SMap, buildIndex, "1 -2 3 -4 5 -6");
  for (unsigned int nSection = 0; nSection < NSegments; ++nSection) {
    holestr = "";
    segmentFront = ModelSupport::getHeadRule(SMap, buildIndex,
                                             std::to_string(nSection * 10 + 1));
    segmentBack =
        ModelSupport::getHeadRule(SMap, buildIndex,
                                  nSection == NSegments - 1
                                      ? "2"
                                      : std::to_string((nSection + 1) * 10 + 1))
            .complement();

    for (unsigned int nPoint = 0; nPoint < NPoints; ++nPoint) {
      holestr +=
          std::to_string((nSection * NPoints + nPoint + 1) * 10 + 3) + " ";
    }

    holeSegment = ModelSupport::getHeadRule(SMap, buildIndex, holestr);
    makeCell("Segment" + std::to_string(nSection) + "Hole", System, cellIndex++,
             voidMaterial, 0.0, segmentFront * segmentBack * holeSegment);
    makeCell("Segment" + std::to_string(nSection) + "Body", System, cellIndex++,
             material, 0.0,
             segmentFront * body * segmentBack * holeSegment.complement());
  }

  addOuterSurf(body);
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::createLinks() {
  FrontBackCut::createLinks(*this, Origin, Y);
}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimator<NPoints, NSegments>::createAll(
    Simulation &System, const attachSystem::FixedComp &FC,
    const long int sideIndex) {
  populate(System.getDataBase());
  createUnitVector(FC, sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
}

template class ProjectionCollimator<4, 2>;
template class ProjectionCollimator<8, 2>;
template class ProjectionCollimator<9, 2>;
template class ProjectionCollimator<12, 2>;

} // namespace xraySystem
