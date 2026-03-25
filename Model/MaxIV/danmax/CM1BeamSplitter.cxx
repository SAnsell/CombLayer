/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CM1BeamSplitter.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Vec3D.h"
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
#include "FixedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CM1BeamSplitter.h"

namespace xraySystem
{

CM1BeamSplitter::CM1BeamSplitter(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

void
CM1BeamSplitter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CM1BeamSplitter","populate");

  FixedRotate::populate(Control);

  // Dimensions
  // Measured in the CAD model [2] if not indicated otherwise.
  bodyAngle=Control.EvalDefVar<double>(keyName+"BodyAngle", 8.04)*M_PI/180.0;
  height=Control.EvalDefVar<double>(keyName+"Height", 2.55);
  length=Control.EvalDefVar<double>(keyName+"Length", 4.38);
  width=Control.EvalDefVar<double>(keyName+"Width", 2.75);

  bottomChamferWidth=Control.EvalDefVar<double>(keyName+"BottomChamferWidth", 0.15);
  bottomDepth=Control.EvalDefVar<double>(keyName+"BottomDepth", 0.9);
  bottomHeight=Control.EvalDefVar<double>(keyName+"BottomHeight", 1.6);
  bottomRoundingRadius=Control.EvalDefVar<double>(keyName+"BottomRoundingRadius", 0.5);
  bottomWidth=Control.EvalDefVar<double>(keyName+"BottomWidth", 1.0);

  filterCrystalPitDepth=Control.EvalDefVar<double>(
    keyName+"FilterCrytalPitDepth", 0.075);
  filterCrystalPitSideLength=Control.EvalDefVar<double>(
    keyName+"FilterCrytalPitSideLength", 0.64);
  filterCrystalSideLength=Control.EvalDefVar<double>(
    keyName+"FilterCrytalSideLength", 0.6); // [1]
  filterCrystalThick=Control.EvalDefVar<double>(
    keyName+"FilterCrystalThick", 0.06); // [1]

  filterHoleDownstreamRadius=Control.EvalDefVar<double>(
    keyName+"FilterHoleDownstreamRadius", 0.2);
  filterHoleOffset=Control.EvalDefVar<double>(keyName+"FilterHoleOffset", 0.293);
  filterHoleUpstreamLength=Control.EvalDefVar<double>(
    keyName+"FilterHoleUpstreamLength", 0.65);
  filterHoleUpstreamRadius=Control.EvalDefVar<double>(
    keyName+"FilterHoleUpstreamRadius", 0.15);

  splitterHoleCenterLength=Control.EvalDefVar<double>(
    keyName+"SplitterHoleCenterLength", 1.0);
  splitterHoleCenterRadius=Control.EvalDefVar<double>(
    keyName+"SplitterHoleCenterRadius", 0.15);
  splitterHoleDownstreamRadius=Control.EvalDefVar<double>(
    keyName+"SplitterHoleDownstreamRadius", 0.2);
  splitterHoleToFilterHole=Control.EvalDefVar<double>(
    keyName+"SplitterHoleToFilterHole", 0.656);
  splitterHoleUpstreamLength=Control.EvalDefVar<double>(
    keyName+"SplitterHoleUpstreamLength", 2.0);
  splitterHoleUpstreamRadius=Control.EvalDefVar<double>(
    keyName+"SplitterHoleUpstreamRadius", 0.09);

  topOverhangThick=Control.EvalDefVar<double>(keyName+"TopOverhangThick", 0.5);
  topOverhangWidth=Control.EvalDefVar<double>(keyName+"TopOverhangWidth", 0.6);

  // Materials
  filterCrystalMaterial=ModelSupport::EvalDefMat(
    Control,keyName+"FilterCrystalMaterial","Diamond"); // "CVD Diamond" [1]
  holderMaterial=ModelSupport::EvalDefMat(
    Control,keyName+"HolderMaterial","Copper"); // [1]
  splitterCrystalMaterial=ModelSupport::EvalDefMat(
    Control,keyName+"SplitterCrystalMaterial","Diamond");

  return;
}


void
CM1BeamSplitter::createSurfaces()
{
  ELog::RegMethod RegA("CM1BeamSplitter","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,-Y);
      setBack(SMap.realSurf(buildIndex+2));
    }

  Geometry::Vec3D holeX = X;
  holeX.rotate(Z,-bodyAngle);
  Geometry::Vec3D holeDir = Y;
  holeDir.rotate(Z,-bodyAngle);

  const Geometry::Vec3D frontCorner = (
    Origin+X*(topOverhangWidth+bottomWidth-width/2.0-bottomChamferWidth));
  ModelSupport::buildPlane(SMap,buildIndex+11,
    frontCorner,holeDir);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+11,holeDir,
    filterCrystalPitDepth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+11,holeDir,
    filterCrystalPitDepth-filterCrystalThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+41,buildIndex+11,holeDir,
    filterHoleUpstreamLength);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+51,buildIndex+11,holeDir,
    splitterHoleUpstreamLength);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+61,buildIndex+51,holeDir,
    splitterHoleCenterLength);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
//   ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*filterHoleOffset,holeX);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+4,X,
    topOverhangWidth-width);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+23,buildIndex+4,X,
    topOverhangWidth+bottomWidth-width-bottomChamferWidth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,X,
    topOverhangWidth+bottomWidth-width);
  const Geometry::Vec3D filterHoleOrigin = frontCorner-holeX*filterHoleOffset;
  ModelSupport::buildPlane(SMap,buildIndex+33,
    filterHoleOrigin-holeX*filterCrystalPitSideLength/2.0,holeX);
  ModelSupport::buildPlane(SMap,buildIndex+34,
    filterHoleOrigin+holeX*filterCrystalPitSideLength/2.0,holeX);
  ModelSupport::buildPlane(SMap,buildIndex+43,
    filterHoleOrigin-holeX*filterCrystalSideLength/2.0,holeX);
  ModelSupport::buildPlane(SMap,buildIndex+44,
    filterHoleOrigin+holeX*filterCrystalSideLength/2.0,holeX);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*bottomDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height-bottomDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
    Origin+Z*(height-bottomDepth-topOverhangThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(bottomHeight-bottomDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+35,
    Origin-Z*filterCrystalPitSideLength/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
    Origin+Z*filterCrystalPitSideLength/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*filterCrystalSideLength/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*filterCrystalSideLength/2.0,Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,filterHoleOrigin,holeDir,
    filterHoleDownstreamRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,filterHoleOrigin,holeDir,
    filterHoleUpstreamRadius);

  const Geometry::Vec3D splitterHoleOrigin = (
    filterHoleOrigin-holeX*splitterHoleToFilterHole);
  ModelSupport::buildCylinder(SMap,buildIndex+27,splitterHoleOrigin,holeDir,
    splitterHoleDownstreamRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,splitterHoleOrigin,holeDir,
    splitterHoleCenterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+47,splitterHoleOrigin,holeDir,
    splitterHoleUpstreamRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+57,
    X*(-width/2.0+topOverhangWidth+bottomWidth-bottomRoundingRadius)
    +Z*(bottomRoundingRadius-bottomDepth),Y,bottomRoundingRadius);

}

void
CM1BeamSplitter::createObjects(Simulation& System)
  /*!
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CM1BeamSplitter","createObjects");

  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");

  addOuterSurf(
    front*back
    *ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6")
  );

  makeCell("Top",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"13 -4 -6 26"));
  makeCell("TopOverhang",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 -13 -6 16"));

  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,"-21 34 -14 5 -26"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    back*ModelSupport::getHeadRule(SMap,buildIndex,"21 23 -14 5 -26 7"));
  makeCell("FilterCrystalPit",System,cellIndex++,0,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,
        "11 -21 33 -34 35 -36 (-31:-43:44:-45:46)"));
  makeCell("FilterCrystal",System,cellIndex++,filterCrystalMaterial,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -21 43 -44 45 -46"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,
        "11 -21 13 -34 5 -26 47 (-33:34:-35:36)"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -41 13 -23 5 -26 17 47"));
  makeCell("FilterCrystalHoleUpstream",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -41 -17"));
  makeCell("FilterCrystalHoleUpstream",System,cellIndex++,0,0.0,
    back*ModelSupport::getHeadRule(SMap,buildIndex,"41 -14 -7"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"41 -51 13 -23 5 -26 7 47"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"51 -61 13 -23 5 -26 7 37"));
  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    back*ModelSupport::getHeadRule(SMap,buildIndex,"61 13 -23 5 -26 7 27"));

  makeCell("FrontVoid",System,cellIndex++,0,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,"-11 13 5 -26"));
  makeCell("BottomRightVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"14 -4 5 -26"));
  makeCell("LeftVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 -13 5 -16"));
}

void
CM1BeamSplitter::createLinks()
{
  ELog::RegMethod RControl("CM1BeamSplitter","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
CM1BeamSplitter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("CM1BeamSplitter","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
