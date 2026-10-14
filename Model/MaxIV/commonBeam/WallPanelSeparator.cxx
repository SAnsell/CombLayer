/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/WallPanelSeparator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "ExternalCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "WallPanelSeparator.h"

namespace xraySystem {

WallPanelSeparator::WallPanelSeparator(const std::string &Key)
    : attachSystem::ContainedComp(), attachSystem::FixedRotate(Key),
      attachSystem::CellMap(), attachSystem::SurfMap(),
      attachSystem::ExternalCut() {}

void WallPanelSeparator::populate(const FuncDataBase &Control) {
  ELog::RegMethod RegA("WallPanelSeparator", "populate");

  FixedRotate::populate(Control);

  channelGapToPlug = Control.EvalVar<double>(keyName + "ChannelGapToPlug");
  channelGapToWall = Control.EvalVar<double>(keyName + "ChannelGapToWall");
  channelHeight = Control.EvalVar<double>(keyName + "ChannelHeight");
  channelThickness = Control.EvalVar<double>(keyName + "ChannelThickness");
  channelWidth = Control.EvalVar<double>(keyName + "ChannelWidth");

  height = Control.EvalVar<double>(keyName + "Height");

  plugGapInside = Control.EvalVar<double>(keyName + "PlugGapInside");
  plugGapOutside = Control.EvalVar<double>(keyName + "PlugGapOutside");
  plugOffsetInside = Control.EvalVar<double>(keyName + "PlugOffsetInside");
  plugThicknessInside =
      Control.EvalVar<double>(keyName + "PlugThicknessInside");
  plugThicknessOutside =
      Control.EvalVar<double>(keyName + "PlugThicknessOutside");
  plugWidthInside = Control.EvalVar<double>(keyName + "PlugWidthInside");
  plugWidthOutside = Control.EvalVar<double>(keyName + "PlugWidthOutside");

  channelMaterial =
      ModelSupport::EvalMat<int>(Control, keyName + "ChannelMaterial");
  plugMaterial = ModelSupport::EvalMat<int>(Control, keyName + "PlugMaterial");
  voidMaterial = ModelSupport::EvalMat<int>(Control, keyName + "VoidMaterial");
}

void WallPanelSeparator::createSurfaces() {
  ExternalCut::makeShiftedSurf(SMap, "innerWall", buildIndex + 1, Y,
                               -plugWidthInside);
  ExternalCut::makeShiftedSurf(SMap, "innerWall", buildIndex + 101, Y,
                               -channelGapToWall);
  ExternalCut::makeShiftedSurf(SMap, "innerWall", buildIndex + 111, Y,
                               -(channelGapToWall + channelThickness));
  ExternalCut::makeShiftedSurf(
      SMap, "innerWall", buildIndex + 121, Y,
      -(channelGapToWall + channelWidth - channelThickness));
  ExternalCut::makeShiftedSurf(SMap, "innerWall", buildIndex + 131, Y,
                               -(channelGapToWall + channelWidth));

  ExternalCut::makeShiftedSurf(SMap, "outerWall", buildIndex + 2, Y,
                               plugGapOutside);
  ExternalCut::makeShiftedSurf(SMap, "outerWall", buildIndex + 12, Y,
                               plugGapOutside + plugThicknessOutside);

  double sign = 1.0;
  for (int side = 3; side <= 4; ++side) {
    sign = side == 3 ? -1.0 : 1.0;
    ModelSupport::buildPlane(SMap, buildIndex + side,
                             Origin + X * sign * (plugThicknessInside / 2.0),
                             X);
    ModelSupport::buildPlane(SMap, buildIndex + side + 10,
                             Origin +
                                 X * sign *
                                     (plugThicknessInside / 2.0 +
                                      plugGapInside + sign * plugOffsetInside),
                             X);
    ModelSupport::buildPlane(SMap, buildIndex + side + 20,
                             Origin + X * sign *
                                          (plugThicknessInside / 2.0 +
                                           channelGapToPlug +
                                           sign * plugOffsetInside),
                             X);
    ModelSupport::buildPlane(
        SMap, buildIndex + side + 30,
        Origin + X * sign *
                     (plugThicknessInside / 2.0 + channelGapToPlug +
                      sign * plugOffsetInside + channelThickness),
        X);
    ModelSupport::buildPlane(SMap, buildIndex + side + 40,
                             Origin + X * sign * (plugWidthOutside / 2.0), X);
    ModelSupport::buildPlane(
        SMap, buildIndex + side + 50,
        Origin + X * sign *
                     (plugThicknessInside / 2.0 + plugGapInside +
                      sign * plugOffsetInside + channelHeight),
        X);
  }

  roofHR = getRule("roof");
  if (roofHR.isEmpty()) {
    ModelSupport::buildPlane(SMap, buildIndex + 5, Origin - Z * (height / 2.0),
                             Z);
    ModelSupport::buildPlane(SMap, buildIndex + 6, Origin + Z * (height / 2.0),
                             Z);
    roofHR = ModelSupport::getHeadRule(SMap, buildIndex, "-6");
  } else {
    ExternalCut::makeShiftedSurf(SMap, "roof", buildIndex + 5, Z, -height);
  }
}

void WallPanelSeparator::createObjects(Simulation &System) {
  const HeadRule outerHR = getRule("outerWall");
  const HeadRule innerHR = getRule("innerWall");

  makeCell("PlugInside", System, cellIndex++, plugMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "1 -2 3 -4 5") * roofHR);
  makeCell("PlugOutside", System, cellIndex++, plugMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "2 -12 43 -44 5") *
               roofHR);

  std::string sideName = "Left";
  int sign = 1;
  for (int side = 3; side <= 4; ++side) {
    sideName = side == 3 ? "Left" : "Right";
    sign = side == 3 ? -1 : 1;
    makeCell("PlugGapWall" + sideName, System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-2 " + std::to_string(sign * side) + " " +
                     std::to_string(-sign * (side + 10)) + " 5") *
                 roofHR * innerHR.complement());
    makeCell("PlugGapInside" + sideName, System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "101 " + std::to_string(sign * side) + " " +
                     std::to_string(-sign * (side + 50)) + " 5") *
                 roofHR * innerHR);
    makeCell("PlugGapInside" + sideName, System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-101 131 " + std::to_string(sign * side) + " " +
                     std::to_string(-sign * (side + 20)) + " 5") *
                 roofHR * innerHR);
    makeCell("PlugGapInside" + sideName, System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "1 -131 " + std::to_string(sign * side) + " " +
                     std::to_string(-sign * (side + 50)) + " 5") *
                 roofHR * innerHR);
    makeCell("PlugGapOutside" + sideName, System, cellIndex++, voidMaterial,
             0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-2 " + std::to_string(sign * (side + 10)) + " " +
                     std::to_string(-sign * (side + 40)) + " 5") *
                 roofHR * outerHR);

    makeCell("Channel" + sideName, System, cellIndex++, channelMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-101 131 " + std::to_string(sign * (side + 20)) + " " +
                     std::to_string(-sign * (side + 30)) + " 5") *
                 roofHR);
    makeCell("Channel" + sideName, System, cellIndex++, channelMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-101 111 " + std::to_string(sign * (side + 30)) + " " +
                     std::to_string(-sign * (side + 50)) + " 5") *
                 roofHR);
    makeCell("Channel" + sideName, System, cellIndex++, channelMaterial, 0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-121 131 " + std::to_string(sign * (side + 30)) + " " +
                     std::to_string(-sign * (side + 50)) + " 5") *
                 roofHR);
    makeCell("Channel" + sideName + "Void", System, cellIndex++, voidMaterial,
             0.0,
             ModelSupport::getHeadRule(
                 SMap, buildIndex,
                 "-111 121 " + std::to_string(sign * (side + 30)) + " " +
                     std::to_string(-sign * (side + 50)) + " 5") *
                 roofHR);
  }

  HeadRule backSurface;
  if (channelGapToWall + channelWidth > plugWidthInside) {
    backSurface = ModelSupport::getHeadRule(SMap, buildIndex, "131");
    makeCell("PlugInsideVoid", System, cellIndex++, voidMaterial, 0.0,
             ModelSupport::getHeadRule(SMap, buildIndex, "131 -1 3 -4 5") *
                 roofHR);
  } else {
    backSurface = ModelSupport::getHeadRule(SMap, buildIndex, "1");
  }

  addOuterSurf(ModelSupport::getHeadRule(SMap, buildIndex, "53 -54 5") *
                   roofHR * innerHR * backSurface +
               ModelSupport::getHeadRule(SMap, buildIndex, "13 -14 5") *
                   roofHR * innerHR.complement() * outerHR.complement() +
               ModelSupport::getHeadRule(SMap, buildIndex, "-12 43 -44 5") *
                   roofHR * outerHR);
}

void WallPanelSeparator::createLinks() {}

void WallPanelSeparator::createAll(Simulation &System,
                                   const attachSystem::FixedComp &FC,
                                   const long int sideIndex) {
  ELog::RegMethod RegA("WallPanelSeparator", "createAll");

  populate(System.getDataBase());
  createUnitVector(FC, sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
}

} // NAMESPACE xraySystem
