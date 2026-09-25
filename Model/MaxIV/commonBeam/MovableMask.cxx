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
      attachSystem::SurfMap()
/*!
  Default constructor
  \param Key :: Key name for variables
*/
{}

void MovableMask::populate(const FuncDataBase &Control) {
  ELog::RegMethod RegA("MovableMask", "populate");

  FixedRotate::populate(Control);

  length = Control.EvalVar<double>(keyName + "Length");

  bodyHeight = Control.EvalVar<double>(keyName + "BodyHeight");
  bodyLength = Control.EvalVar<double>(keyName + "BodyLength");
  bodyWidth = Control.EvalVar<double>(keyName + "BodyWidth");

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

  slitHeight = Control.EvalVar<double>(keyName + "SlitHeight");
  slitInnerSurfaceAngle =
      Control.EvalVar<double>(keyName + "SlitInnerSurfaceAngle");
  slitThickness = Control.EvalVar<double>(keyName + "SlitThickness");

  bodyMaterial = ModelSupport::EvalMat<int>(Control, keyName + "BodyMaterial");
  flangeMaterial =
      ModelSupport::EvalMat<int>(Control, keyName + "FlangeMaterial");
  slitMaterial = ModelSupport::EvalMat<int>(Control, keyName + "SlitMaterial");
  voidMaterial = ModelSupport::EvalMat<int>(Control, keyName + "VoidMaterial");

  return;
}

void MovableMask::createSurfaces() {

  if (!isActive("front")) {
    ModelSupport::buildPlane(SMap, buildIndex + 1, Origin, Y);
    setFront(SMap.realSurf(buildIndex + 1));
  }
  if (!isActive("back")) {
    ModelSupport::buildPlane(SMap, buildIndex + 2, Origin + Y * length, Y);
    setBack(-SMap.realSurf(buildIndex + 2));
  }

  ModelSupport::buildPlane(SMap, buildIndex + 11, Origin + Y * (flangeLength),
                           Y);
  ModelSupport::buildPlane(SMap, buildIndex + 21,
                           Origin + Y * (length - bodyLength) / 2.0, Y);
  ModelSupport::buildPlane(SMap, buildIndex + 12,
                           Origin + Y * (length - flangeLength), Y);
  ModelSupport::buildPlane(SMap, buildIndex + 22,
                           Origin + Y * (length + bodyLength) / 2.0, Y);
  ModelSupport::buildPlane(
      SMap, buildIndex + 32,
      Origin + Y * ((length + bodyLength) / 2.0 + slitThickness), Y);

  ModelSupport::buildPlane(SMap, buildIndex + 3, Origin - X * bodyWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 13, Origin - X * holeWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 4, Origin + X * bodyWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 14, Origin + X * holeWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 24,
                           Origin + X * (-holeWidth / 2.0 + maskBottomMaxWidth),
                           X);

  ModelSupport::buildPlane(SMap, buildIndex + 5, Origin - Z * bodyHeight / 2.0,
                           Z);
  ModelSupport::buildPlane(SMap, buildIndex + 15,
                           Origin - Z * (holeHeight / 2.0 - holeOffset -
                                         maskLeftMaxHeight + slitHeight),
                           Z);
  ModelSupport::buildPlane(SMap, buildIndex + 25,
                           Origin - Z * (holeHeight / 2.0 - holeOffset), Z);

  const double slitInnerSurfaceAngleRad = slitInnerSurfaceAngle * M_PI / 180.0;
  Geometry::Vec3D slitBottomSurfaceNormal = Z;
  slitBottomSurfaceNormal.rotate(X, -slitInnerSurfaceAngleRad);
  ModelSupport::buildPlane(
      SMap, buildIndex + 35,
      Origin + Y * ((length + bodyLength) / 2.0) +
          Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight),
      slitBottomSurfaceNormal);
  ModelSupport::buildPlane(
      SMap, buildIndex + 45,
      Origin - Z * (holeHeight / 2.0 - holeOffset - maskLeftMaxHeight), Z);

  ModelSupport::buildPlane(SMap, buildIndex + 6, Origin + Z * bodyHeight / 2.0,
                           Z);
  ModelSupport::buildPlane(SMap, buildIndex + 16,
                           Origin + Z * (holeHeight / 2.0 + holeOffset), Z);

  ModelSupport::buildCylinder(SMap, buildIndex + 7, Origin, Y, flangeRadius);
  ModelSupport::buildCylinder(SMap, buildIndex + 17, Origin, Y,
                              flangeInnerRadius + flangeWallThick);
  ModelSupport::buildCylinder(SMap, buildIndex + 27, Origin, Y,
                              flangeInnerRadius);

  ModelSupport::buildPlane(SMap, buildIndex + 23,
                           Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth),
                           X);
  Geometry::Vec3D slitLeftSurfaceNormal = X;
  slitLeftSurfaceNormal.rotate(Z, slitInnerSurfaceAngleRad);
  ModelSupport::buildPlane(SMap, buildIndex + 33,
                           Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
                               Y * ((length + bodyLength) / 2.0),
                           slitLeftSurfaceNormal);

  const Geometry::Vec3D maskLeftSlope = Y * bodyLength + Z * maskLeftMaxHeight;
  Geometry::Vec3D maskLeftSlopeNormal = maskLeftSlope;
  maskLeftSlopeNormal.rotate(X, M_PI_2);
  ModelSupport::buildPlane(
      SMap, buildIndex + 55,
      Origin + Y * (length + bodyLength) / 2.0 +
          Z * (maskLeftMaxHeight - (holeHeight / 2.0 - holeOffset)),
      maskLeftSlopeNormal);

  const Geometry::Vec3D focalPoint =
      Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth) -
      Y * (length + bodyLength) / 2.0 - Z * (holeHeight / 2.0 - holeOffset) +
      maskLeftSlope * maskFocalPoint;
  const Geometry::Vec3D maskLeftDownstreamTopRight =
      Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskLeftMaxHeight);
  const double maskLeftDownstreamInnerPlaneAngleRad =
      maskDownstreamInnerPlaneAngle * M_PI / 180.0;
  Geometry::Vec3D maskLeftDownstreamBottomRight =
      Origin -
      X * (holeWidth / 2.0 - maskLeftMaxWidth +
           (maskLeftMaxHeight - maskBottomMaxHeight) *
               tan(maskLeftDownstreamInnerPlaneAngleRad)) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight);
  ModelSupport::buildPlane(SMap, buildIndex + 43, focalPoint,
                           (maskLeftDownstreamBottomRight - focalPoint) *
                               (maskLeftDownstreamTopRight - focalPoint));

  Geometry::Vec3D maskBottomDownstreamTopLeft =
      Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight -
           tan(maskLeftDownstreamInnerPlaneAngleRad) *
               (maskBottomMaxWidth - maskLeftMaxWidth));
  ModelSupport::buildPlane(SMap, buildIndex + 65, focalPoint,
                           (maskBottomDownstreamTopLeft - focalPoint) *
                               (maskLeftDownstreamBottomRight - focalPoint));

  Geometry::Vec3D maskBottomDownstreamTopRight =
      Origin - X * (holeWidth / 2.0 - maskBottomMaxWidth) +
      Y * (length + bodyLength) / 2.0 +
      Z * (-holeHeight / 2.0 + holeOffset + maskBottomMaxHeight);
  Geometry::Vec3D maskBottomDownstreamBottomRight =
      Origin - X * (holeWidth / 2.0 - maskBottomMaxWidth) +
      Y * (length + bodyLength) / 2.0 + Z * (-holeHeight / 2.0 + holeOffset);

  ModelSupport::buildPlane(SMap, buildIndex + 75, focalPoint,
                           (maskBottomDownstreamTopRight - focalPoint) *
                               (maskBottomDownstreamTopLeft - focalPoint));

  Geometry::Vec3D maskLeftUpstreamBottomRight =
      Origin - X * (holeWidth / 2.0 - maskLeftMaxWidth) -
      Y * (length + bodyLength) / 2.0 + Z * (-holeHeight / 2.0 + holeOffset);

  ModelSupport::buildPlane(
      SMap, buildIndex + 85, focalPoint,
      -(maskBottomDownstreamTopRight - focalPoint) *
          (maskLeftUpstreamBottomRight - maskBottomDownstreamTopRight));
  ModelSupport::buildPlane(
      SMap, buildIndex + 95, maskLeftUpstreamBottomRight,
      -(maskBottomDownstreamTopRight - maskLeftUpstreamBottomRight) *
          (maskBottomDownstreamBottomRight - maskLeftUpstreamBottomRight));
}

void MovableMask::createObjects(Simulation &System) {
  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");

  makeCell("FrontFlange", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "-11 -7 27") * front);
  makeCell("FrontFlangeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "-21 -27") * front);
  makeCell("FrontPipe", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "11 -21 -17 27"));
  makeCell("FrontPipeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "11 -21 -7 17"));

  makeCell("Body", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 3 -4 5 -6 (-13:14:-25:16)"));
  makeCell("BodyLeftInnerVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 13 -23 55 -16"));
  makeCell("BodyRightInnerVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 23 -14 25 -16 (75:85:95)"));
  makeCell(
      "BodyOuterVoid", System, cellIndex++, voidMaterial, 0.0,
      ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 (-3:4:-5:6) -7"));

  makeCell("BackFlange", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "12 -7 27") * back);
  makeCell("BackFlangeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "32 -27") * back);
  makeCell("BackPipe", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "22 -12 -17 27"));
  makeCell("BackPipeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "22 -12 -7 17"));

  makeCell("MaskLeft", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 13 -23 (-43:-65) 25 -55"));
  makeCell("MaskLeftVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 -23 43 -55 65"));
  makeCell(
      "MaskBottom", System, cellIndex++, bodyMaterial, 0.0,
      ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 23 25 -75 -85 -95"));

  makeCell("Slit", System, cellIndex++, slitMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "22 -32 13 -24 15 -45 (-33:-35)"));
  makeCell("SlitVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "22 -32 -27 (-13:-15:24:45)"));
  makeCell("SlitVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "22 -32 33 -24 35 -45"));

  addOuterSurf(ModelSupport::getHeadRule(SMap, buildIndex, "-7") * front *
               back);
}

void MovableMask::createLinks()
/*!
  Construct the links for the system
*/
{
  ELog::RegMethod RegA("MovableMask", "createLinks");

  FrontBackCut::createLinks(*this, Origin, Y);

  return;
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
  return;
}

} // NAMESPACE xraySystem
