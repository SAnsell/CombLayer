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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "BaseMap.h"
#include "CellMap.h"
#include "Code.h"
#include "ContainedComp.h"
#include "FileReport.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "MaterialSupport.h"
#include "ModelSupport.h"
#include "MovableMask.h"
#include "NameStack.h"
#include "OutputLog.h"
#include "RegMethod.h"
#include "Simulation.h"
#include "SurfMap.h"
#include "Vec3D.h"
#include "generateSurf.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "surfRegister.h"
#include "varList.h"

namespace xraySystem {

MovableMask::MovableMask(const std::string &Key)
    : attachSystem::FixedRotate(Key), attachSystem::ContainedComp(),
      attachSystem::CellMap(), attachSystem::SurfMap()
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

  bodyMaterial = ModelSupport::EvalMat<int>(Control, keyName + "BodyMaterial");
  flangeMaterial =
      ModelSupport::EvalMat<int>(Control, keyName + "FlangeMaterial");
  voidMaterial = ModelSupport::EvalMat<int>(Control, keyName + "VoidMaterial");

  return;
}

void MovableMask::createSurfaces() {

  ModelSupport::buildPlane(SMap, buildIndex + 1, Origin - Y * length / 2.0, Y);
  ModelSupport::buildPlane(SMap, buildIndex + 11,
                           Origin - Y * (length / 2.0 - flangeLength), Y);
  ModelSupport::buildPlane(SMap, buildIndex + 21, Origin - Y * bodyLength / 2.0,
                           Y);
  ModelSupport::buildPlane(SMap, buildIndex + 2, Origin + Y * length / 2.0, Y);
  ModelSupport::buildPlane(SMap, buildIndex + 12,
                           Origin + Y * (length / 2.0 - flangeLength), Y);
  ModelSupport::buildPlane(SMap, buildIndex + 22, Origin + Y * bodyLength / 2.0,
                           Y);

  ModelSupport::buildPlane(SMap, buildIndex + 3, Origin - X * bodyWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 13, Origin - X * holeWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 4, Origin + X * bodyWidth / 2.0,
                           X);
  ModelSupport::buildPlane(SMap, buildIndex + 14, Origin + X * holeWidth / 2.0,
                           X);

  ModelSupport::buildPlane(SMap, buildIndex + 5, Origin - Z * bodyHeight / 2.0,
                           Z);
  ModelSupport::buildPlane(
      SMap, buildIndex + 15,
      Origin - Z * (bodyHeight / 2.0 - holeHeight / 2.0 - holeOffset), Z);
  ModelSupport::buildPlane(SMap, buildIndex + 6, Origin + Z * bodyHeight / 2.0,
                           Z);
  ModelSupport::buildPlane(
      SMap, buildIndex + 16,
      Origin + Z * (bodyHeight / 2.0 - holeHeight / 2.0 + holeOffset), Z);

  ModelSupport::buildCylinder(SMap, buildIndex + 7, Origin, Y, flangeRadius);
  ModelSupport::buildCylinder(SMap, buildIndex + 17, Origin, Y,
                              flangeInnerRadius + flangeWallThick);
  ModelSupport::buildCylinder(SMap, buildIndex + 27, Origin, Y,
                              flangeInnerRadius);

  return;
}

void MovableMask::createObjects(Simulation &System) {
  HeadRule HR;

  makeCell("FrontFlange", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "1 -11 -7 27"));
  makeCell("FrontFlangeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "1 -21 -27"));
  makeCell("FrontPipe", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "11 -21 -17 27"));
  makeCell("FrontPipeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "11 -21 -7 17"));

  makeCell("Body", System, cellIndex++, bodyMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex,
                                     "21 -22 3 -4 5 -6 (-13:14:-15:16)"));
  makeCell("BodyInnerVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 13 -14 15 -16"));
  makeCell(
      "BodyOuterVoid", System, cellIndex++, voidMaterial, 0.0,
      ModelSupport::getHeadRule(SMap, buildIndex, "21 -22 (-3:4:-5:6) -7"));

  makeCell("BackFlange", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "-2 12 -7 27"));
  makeCell("BackFlangeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "-2 22 -27"));
  makeCell("BackPipe", System, cellIndex++, flangeMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "22 -12 -17 27"));
  makeCell("BackPipeVoid", System, cellIndex++, voidMaterial, 0.0,
           ModelSupport::getHeadRule(SMap, buildIndex, "22 -12 -7 17"));

  addOuterSurf(ModelSupport::getHeadRule(SMap, buildIndex, "1 -2 -7"));
}

void MovableMask::createLinks()
/*!
  Construct the links for the system
*/
{
  ELog::RegMethod RegA("MovableMask", "createLinks");

  FixedComp::setConnect("front", Origin - Y * (length / 2.0), -Y);
  FixedComp::setLinkSurf("front", -SMap.realSurf(buildIndex + 1));
  FixedComp::setConnect("back", Origin + Y * (length / 2.0), Y);
  FixedComp::setLinkSurf("back", SMap.realSurf(buildIndex + 2));

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
