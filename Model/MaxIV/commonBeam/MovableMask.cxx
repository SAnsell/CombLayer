/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/MovableMask.cxx
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
#include <cmath>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "MovableMask.h"

namespace xraySystem {

MovableMask::MovableMask(const std::string &Key)
    : attachSystem::FixedRotate(Key), attachSystem::ContainedComp(),
      attachSystem::FrontBackCut(), attachSystem::CellMap(),
      attachSystem::SurfMap() {}

void MovableMask::populate(const FuncDataBase &Control) {
  ELog::RegMethod RegA("MovableMask", "populate");

  FixedRotate::populate(Control);

  length = Control.EvalVar<double>(keyName + "Length");

  bodyHeight = Control.EvalVar<double>(keyName + "BodyHeight");
  bodyLength = Control.EvalVar<double>(keyName + "BodyLength");
  bodyWidth = Control.EvalVar<double>(keyName + "BodyWidth");

  useConnector = Control.EvalVar<int>(keyName + "UseConnector");
  connectorInnerEdgeRadius =
      Control.EvalVar<double>(keyName + "ConnectorInnerEdgeRadius");
  connectorInnerHeight =
      Control.EvalVar<double>(keyName + "ConnectorInnerHeight");
  connectorInnerWidth =
      Control.EvalVar<double>(keyName + "ConnectorInnerWidth");
  connectorWallThickness =
      Control.EvalVar<double>(keyName + "ConnectorWallThickness");

  flangeInnerRadius = Control.EvalVar<double>(keyName + "FlangeInnerRadius");
  flangeLength = Control.EvalVar<double>(keyName + "FlangeLength");
  flangeRadius = Control.EvalVar<double>(keyName + "FlangeRadius");
  flangeWallThick = Control.EvalVar<double>(keyName + "FlangeWallThick");

  holeHeight = Control.EvalVar<double>(keyName + "HoleHeight");
  holeOffset = Control.EvalVar<double>(keyName + "HoleOffset");
  holeWidth = Control.EvalVar<double>(keyName + "HoleWidth");

  maskLeftMaxHeight = Control.EvalVar<double>(keyName + "MaskLeftMaxHeight");
  maskLeftMaxWidth = Control.EvalVar<double>(keyName + "MaskLeftMaxWidth");
  maskBottomMaxHeight =
      Control.EvalVar<double>(keyName + "MaskBottomMaxHeight");
  maskBottomMaxWidth = Control.EvalVar<double>(keyName + "MaskBottomMaxWidth");
  maskFocalPoint = Control.EvalVar<double>(keyName + "MaskFocalPoint");
  maskDownstreamInnerPlaneAngle =
      Control.EvalVar<double>(keyName + "MaskDownstreamInnerPlaneAngle");

  positionX = Control.EvalVar<double>(keyName + "PositionX");
  positionZ = Control.EvalVar<double>(keyName + "PositionZ");

  slitInnerOffset = Control.EvalVar<double>(keyName + "SlitInnerOffset");
  slitInnerSurfaceAngle =
      Control.EvalVar<double>(keyName + "SlitInnerSurfaceAngle");
  slitThickness = Control.EvalVar<double>(keyName + "SlitThickness");

  bodyMaterial = ModelSupport::EvalMat<int>(Control, keyName + "BodyMaterial");
  flangeMaterial =
      ModelSupport::EvalMat<int>(Control, keyName + "FlangeMaterial");
  slitMaterial = ModelSupport::EvalMat<int>(Control, keyName + "SlitMaterial");
  voidMaterial = ModelSupport::EvalMat<int>(Control, keyName + "VoidMaterial");
}

void MovableMask::createSurfaces() {

  const Geometry::Vec3D center = Origin - X * positionX - Z * positionZ;

  if (!isActive("front")) {
    ModelSupport::buildPlane(SMap, buildIndex + 1, center, Y);
    setFront(SMap.realSurf(buildIndex + 1));
  }
  if (!isActive("back")) {
    ModelSupport::buildPlane(SMap, buildIndex + 2, center + Y * length, Y);
    setBack(-SMap.realSurf(buildIndex + 2));
  }

  const double effectiveFlangeLength =
      useConnector ? flangeLength : (length - bodyLength) / 2.0;

  ModelSupport::buildPlane(SMap, buildIndex + 11,
                           center + Y * (effectiveFlangeLength), Y);
  ModelSupport::buildPlane(SMap, buildIndex + 21,
                           center + Y * (length - bodyLength) / 2.0, Y);
  ModelSupport::buildPlane(SMap, buildIndex + 12,
                           center + Y * (length - effectiveFlangeLength), Y);
  ModelSupport::buildPlane(SMap, buildIndex + 22,
                           center + Y * (length + bodyLength) / 2.0, Y);
  ModelSupport::buildPlane(
      SMap, buildIndex + 32,
      center + Y * ((length + bodyLength) / 2.0 + slitThickness), Y);

  int sign;
  double bodyScale, connectorScale, holeScale;
  Geometry::Vec3D normalVector;
  for (int i = 3; i <= 6; ++i) {
    sign = i % 2 == 1 ? -1 : 1;
    if (i < 5) {
      normalVector = X;
      bodyScale = bodyWidth;
      connectorScale = connectorInnerWidth;
      holeScale = holeWidth;
    } else {
      normalVector = Z;
      bodyScale = bodyHeight;
      connectorScale = connectorInnerHeight;
      holeScale = holeHeight;
    }

    ModelSupport::buildPlane(SMap, buildIndex + i,
                             center + normalVector * sign * bodyScale / 2.0,
                             normalVector);
    ModelSupport::buildPlane(
        SMap, buildIndex + 10 + i,
        center + normalVector * sign *
                     (connectorScale / 2.0 + connectorWallThickness),
        normalVector);
    ModelSupport::buildPlane(
        SMap, buildIndex + 20 + i,
        center + normalVector * sign * connectorScale / 2.0, normalVector);
    ModelSupport::buildPlane(
        SMap, buildIndex + 30 + i,
        center + normalVector * sign *
                     (connectorScale / 2.0 - connectorInnerEdgeRadius),
        normalVector);
    ModelSupport::buildPlane(SMap, buildIndex + 40 + i,
                             center + normalVector * sign * holeScale / 2.0,
                             normalVector);
  }

  ModelSupport::buildPlane(SMap, buildIndex + 54,
                           center + X * (-holeWidth / 2.0 + maskBottomMaxWidth),
                           X);

  ModelSupport::buildPlane(SMap, buildIndex + 55,
                           center - Z * (holeHeight / 2.0 - holeOffset), Z);

  const double slitInnerSurfaceAngleRad = slitInnerSurfaceAngle * M_PI / 180.0;
  Geometry::Vec3D slitBottomSurfaceNormal = Z;
  slitBottomSurfaceNormal.rotate(X, -slitInnerSurfaceAngleRad);
  ModelSupport::buildPlane(SMap, buildIndex + 65,
                           center + Y * ((length + bodyLength) / 2.0) +
                               Z * (-holeHeight / 2.0 + holeOffset +
                                    maskBottomMaxHeight + slitInnerOffset),
                           slitBottomSurfaceNormal);
  ModelSupport::buildPlane(
      SMap, buildIndex + 75,
      center - Z * (holeHeight / 2.0 - holeOffset - maskLeftMaxHeight), Z);

  ModelSupport::buildCylinder(SMap, buildIndex + 7, center, Y, flangeRadius);
  ModelSupport::buildCylinder(SMap, buildIndex + 27, center, Y,
                              flangeInnerRadius);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 37,
      center - X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) -
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 47,
      center - X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) +
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 57,
      center + X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) -
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 67,
      center + X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) +
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 137,
      center - X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) -
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius + connectorWallThickness);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 147,
      center - X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) +
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius + connectorWallThickness);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 157,
      center + X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) -
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius + connectorWallThickness);
  ModelSupport::buildCylinder(
      SMap, buildIndex + 167,
      center + X * (connectorInnerWidth / 2.0 - connectorInnerEdgeRadius) +
          Z * (connectorInnerHeight / 2.0 - connectorInnerEdgeRadius),
      Y, connectorInnerEdgeRadius + connectorWallThickness);

  ModelSupport::buildPlane(SMap, buildIndex + 53,
                           center - X * (holeWidth / 2.0 - maskLeftMaxWidth),
                           X);
  Geometry::Vec3D slitLeftSurfaceNormal = X;
  slitLeftSurfaceNormal.rotate(Z, slitInnerSurfaceAngleRad);
  ModelSupport::buildPlane(
      SMap, buildIndex + 63,
      center - X * (holeWidth / 2.0 - maskLeftMaxWidth - slitInnerOffset) +
          Y * ((length + bodyLength) / 2.0),
      slitLeftSurfaceNormal);

  const Geometry::Vec3D maskLeftSlope = Y * bodyLength + Z * maskLeftMaxHeight;
  Geometry::Vec3D maskLeftSlopeNormal = maskLeftSlope;
  maskLeftSlopeNormal.rotate(X, M_PI_2);
  ModelSupport::buildPlane(
      SMap, buildIndex + 85,
      center + Y * (length + bodyLength) / 2.0 +
          Z * (maskLeftMaxHeight - (holeHeight / 2.0 - holeOffset)),
      maskLeftSlopeNormal);

  const Geometry::Vec3D maskLeftUpstreamBottomRight =
      center - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
      Y * (length - bodyLength) / 2.0 + Z * (-holeHeight / 2.0 + holeOffset);
  const Geometry::Vec3D focalPoint =
      maskLeftUpstreamBottomRight + maskLeftSlope * maskFocalPoint;

  const Geometry::Vec3D maskLeftDownstreamTopRight =
      center - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskLeftMaxHeight);

  const double maskLeftDownstreamInnerPlaneAngleRad =
      maskDownstreamInnerPlaneAngle * M_PI / 180.0;
  const Geometry::Vec3D maskLeftDownstreamBottomRight =
      center -
      X * (holeWidth / 2.0 - maskLeftMaxWidth +
           (maskLeftMaxHeight - maskBottomMaxHeight) *
               tan(maskLeftDownstreamInnerPlaneAngleRad)) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight);
  ModelSupport::buildPlane(SMap, buildIndex + 73, focalPoint,
                           (maskLeftDownstreamBottomRight - focalPoint) *
                               (maskLeftDownstreamTopRight - focalPoint));

  const Geometry::Vec3D maskBottomDownstreamTopLeft =
      center - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight -
           tan(maskLeftDownstreamInnerPlaneAngleRad) *
               (maskBottomMaxWidth - maskLeftMaxWidth));
  ModelSupport::buildPlane(SMap, buildIndex + 95, focalPoint,
                           (maskBottomDownstreamTopLeft - focalPoint) *
                               (maskLeftDownstreamBottomRight - focalPoint));

  const Geometry::Vec3D maskBottomDownstreamTopRight =
      center - X * (holeWidth / 2.0 - maskBottomMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight);
  const Geometry::Vec3D maskBottomDownstreamBottomRight =
      center - X * (holeWidth / 2.0 - maskBottomMaxWidth) +
      Y * (length + bodyLength) / 2.0 + Z * (-holeHeight / 2.0 + holeOffset);

  ModelSupport::buildPlane(SMap, buildIndex + 105, focalPoint,
                           (maskBottomDownstreamTopRight - focalPoint) *
                               (maskBottomDownstreamTopLeft - focalPoint));

  ModelSupport::buildPlane(
      SMap, buildIndex + 115, focalPoint,
      -(maskBottomDownstreamTopRight - focalPoint) *
          (maskLeftUpstreamBottomRight - maskBottomDownstreamTopRight));
  ModelSupport::buildPlane(
      SMap, buildIndex + 125, maskLeftUpstreamBottomRight,
      -(maskBottomDownstreamTopRight - maskLeftUpstreamBottomRight) *
          (maskBottomDownstreamBottomRight - maskLeftUpstreamBottomRight));
}

int cornerIndex(const int &i, const int &j, const int offset = 0) {
  int index = 37;
  if (i == 3) {
    if (j == 6) {
      index = 47;
    }
  } else if (j == 5) {
    index = 57;
  } else {
    index = 67;
  }
  return index + offset;
}

void MovableMask::createRoundedRectanglePipe(
    Simulation &System, const std::string name, const HeadRule &front,
    const HeadRule &back, const HeadRule &outer, const HeadRule &exclude,
    const bool buildOuterVoid) {
  int sign_i, sign_j;
  for (int i = 3; i <= 4; ++i) {
    sign_i = i % 2 == 0 ? 1 : -1;
    for (int j = 5; j <= 6; ++j) {
      sign_j = j % 2 == 0 ? 1 : -1;
      if (buildOuterVoid) {
        makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
                 ModelSupport::getHeadRule(
                     SMap, buildIndex,
                     std::to_string(cornerIndex(i, j)) + " " +
                         std::to_string(-cornerIndex(i, j, 100)) + " " +
                         std::to_string(sign_i * (30 + i)) + " " +
                         std::to_string(sign_j * (30 + j))) *
                     front * back);
        makeCell(name, System, cellIndex++, voidMaterial, 0.0,
                 ModelSupport::getHeadRule(
                     SMap, buildIndex,
                     std::to_string(cornerIndex(i, j, 100)) + " " +
                         std::to_string(sign_i * (30 + i)) + " " +
                         std::to_string(sign_j * (30 + j))) *
                     front * back * outer);
      } else {
        makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
                 ModelSupport::getHeadRule(
                     SMap, buildIndex,
                     std::to_string(cornerIndex(i, j)) + " " +
                         std::to_string(sign_i * (30 + i)) + " " +
                         std::to_string(sign_j * (30 + j))) *
                     front * back * outer);
      }
      makeCell(name + "Void", System, cellIndex++, voidMaterial, 0.0,
               ModelSupport::getHeadRule(
                   SMap, buildIndex,
                   std::to_string(-cornerIndex(i, j)) + " " +
                       std::to_string(sign_i * (30 + i)) + " " +
                       std::to_string(sign_j * (30 + j))) *
                   front * back);
    }
    if (buildOuterVoid) {
      makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
               ModelSupport::getHeadRule(
                   SMap, buildIndex,
                   std::to_string(-sign_i * (10 + i)) + " " +
                       std::to_string(sign_i * (20 + i)) + " 35 -36") *
                   front * back);
      makeCell(name, System, cellIndex++, voidMaterial, 0.0,
               ModelSupport::getHeadRule(SMap, buildIndex,
                                         std::to_string(sign_i * (10 + i)) +
                                             " 35 -36") *
                   front * back * outer);
      makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
               ModelSupport::getHeadRule(
                   SMap, buildIndex,
                   std::to_string(-sign_i * (10 + i + 2)) + " " +
                       std::to_string(sign_i * (20 + i + 2)) + " 33 -34") *
                   front * back);
      makeCell(name, System, cellIndex++, voidMaterial, 0.0,
               ModelSupport::getHeadRule(SMap, buildIndex,
                                         std::to_string(sign_i * (10 + i + 2)) +
                                             " 33 -34") *
                   front * back * outer);
    }
    makeCell(name + "Void", System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 std::to_string(-sign_i * (20 + i)) + " " +
                     std::to_string(sign_i * (30 + i)) + " 35 -36") *
                 front * back * exclude);
    makeCell(name + "Void", System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 std::to_string(-sign_i * (20 + i + 2)) + " " +
                     std::to_string(sign_i * (30 + i + 2)) + " 33 -34") *
                 front * back * exclude);
  }
  if (!buildOuterVoid) {
    makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
             ModelSupport::getHeadRule(SMap, buildIndex, "(-23:24) 35 -36") *
                 front * back * outer);
    makeCell(name, System, cellIndex++, flangeMaterial, 0.0,
             ModelSupport::getHeadRule(SMap, buildIndex, "(-25:26) 33 -34") *
                 front * back * outer);
  }
  makeCell(name + "Void", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "33 -34 35 -36") *
               front * back * exclude);
}

void MovableMask::createObjects(Simulation &System) {
  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");

  const std::string frontFlangeBack = useConnector ? "-11" : "-21";
  const std::string backFlangeFront = useConnector ? "12" : "22";

  createRoundedRectanglePipe(
      System, "FrontFlange", front,
      ModelSupport::getHeadRule(SMap, buildIndex, frontFlangeBack),
      ModelSupport::getHeadRule(SMap, buildIndex, "-7"), HeadRule(), false);
  if (useConnector) {
    createRoundedRectanglePipe(
        System, "FrontConnector",
        ModelSupport::getHeadRule(SMap, buildIndex, "11"),
        ModelSupport::getHeadRule(SMap, buildIndex, "-21"),
        ModelSupport::getHeadRule(SMap, buildIndex, "-7"), HeadRule(), true);
  }
  const HeadRule slitHR = ModelSupport::getHeadRule(
      SMap, buildIndex, "22 -32 43 -54 45 -75 (-63:-65)");
  if (useConnector) {
    createRoundedRectanglePipe(
        System, "BackConnector",
        ModelSupport::getHeadRule(SMap, buildIndex, "22"),
        ModelSupport::getHeadRule(SMap, buildIndex, "-12"),
        ModelSupport::getHeadRule(SMap, buildIndex, "-7"), slitHR.complement(),
        true);
  }
  createRoundedRectanglePipe(
      System, "BackFlange",
      ModelSupport::getHeadRule(SMap, buildIndex, backFlangeFront), back,
      ModelSupport::getHeadRule(SMap, buildIndex, "-7"),
      useConnector ? HeadRule() : slitHR.complement(), false);

  makeCell("Body", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 3 -4 5 -6 (-43:44:-55:46)"));
  makeCell("BodyLeftInnerVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 43 -53 85 -46"));
  makeCell("BodyRightInnerVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 53 -44 55 -46 (105:115:125)"));
  makeCell(
      "BodyOuterVoid", System, cellIndex++, voidMaterial, 0.0,
      ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 (-3:4:-5:6) -7"));

  makeCell("MaskLeft", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 43 -53 (-73:-95) 55 -85"));
  makeCell("MaskLeftVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 -53 73 -85 95"));
  makeCell("MaskBottom", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 53 55 -105 -115 -125"));

  makeCell("Slit", System, cellIndex++, slitMaterial, 0.0, slitHR);

  addOuterSurf(ModelSupport::getHeadRule(SMap, buildIndex, "-7") * front *
               back);
}

void MovableMask::createLinks() {
  ELog::RegMethod RegA("MovableMask", "createLinks");

  FrontBackCut::createLinks(*this, Origin, Y);
}

void MovableMask::createAll(Simulation &System,
                            const attachSystem::FixedComp &FC,
                            const long int sideIndex)
/*!
  Generic function to create everything
  \param System :: Simulation
  \param FC :: Fixed component to set axis etc
  \param sideIndex :: position of linkpoint
*/
{
  ELog::RegMethod RegA("MovableMask", "createAll");

  populate(System.getDataBase());
  createUnitVector(FC, sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
}

} // NAMESPACE xraySystem
