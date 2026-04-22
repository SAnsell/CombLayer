/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/CM2Crystal.cxx
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
#include <sstream>
#include <cmath>
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
#include "Quaternion.h"
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
#include "CM2Crystal.h"

namespace xraySystem
{

CM2Crystal::CM2Crystal(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
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
CM2Crystal::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CM2Crystal","populate");

  FixedRotate::populate(Control);

  // Dimensions
  // Measured in the CAD model [2] if not indicated otherwise.
  height=Control.EvalDefVar<double>(keyName+"Height",3.7);
  length=Control.EvalDefVar<double>(keyName+"Length",2.4);
  width=Control.EvalDefVar<double>(keyName+"Width",4.0);

  topHeight=Control.EvalDefVar<double>(keyName+"TopWidth",2.5);
  topHeightAboveOpticalAxis=Control.EvalDefVar<double>(
    keyName+"TopWidthAboveOpticalAxis",1.4);
  topWidth=Control.EvalDefVar<double>(keyName+"Width",2.2);

  crystalHeight=Control.EvalDefVar<double>(keyName+"CrystalHeight",0.77);
  // The pitch, roll, and yaw angle and the pivot point have been determined in the
  // same way as described in CM1BeamSplitter. Here, the three points sampled 
  // from the corresponding CAD model [2] were:
  //
  // v0 = ( 0.0,   0.110,  0.00)
  // v1 = ( 0.0,   0.059, -0.81)
  // v2 = (-1.254, 0.090, -0.81)
  //
  // As in CM1BeamSplitter, the crystal shape is approximated as a rectangle.
  crystalPitch=
    Control.EvalDefVar<double>(keyName+"CrystalPitch",-0.04443803)*M_PI/180.0;
  crystalPitChannelDepth=Control.EvalDefVar<double>(
    keyName+"CrystalPitChannelDepth",0.065);
  crystalPitChannelWidth=Control.EvalDefVar<double>(
    keyName+"CrystalPitChannelWidth",0.3);
  crystalPitDepth=Control.EvalDefVar<double>(keyName+"CrystalPitDepth",0.1);
  crystalPitHeight=Control.EvalDefVar<double>(keyName+"CrystalPitHeight",0.81);
  crystalPitWidth=Control.EvalDefVar<double>(keyName+"CrystalPitWidth",1.44);
  crystalRoll=
    Control.EvalDefVar<double>(keyName+"CrystalRoll",-3.60165985)*M_PI/180.0;
  crystalThick=Control.EvalDefVar<double>(keyName+"CrystalThick",0.05);
  crystalWidth=Control.EvalDefVar<double>(keyName+"CrystalWidth",1.41);
  crystalYaw=Control.EvalDefVar<double>(keyName+"CrystalYaw",1.41331732)*M_PI/180.0;

  // Materials
  crystalMaterial=ModelSupport::EvalDefMat(
    Control,keyName+"CrystalMaterial","Diamond");
  holderMaterial=ModelSupport::EvalDefMat(
    Control,keyName+"HolderMaterial","Copper"); // [1]

  const std::string beamName = "DanMAX";
  CM1Y = Control.EvalVar<double>(beamName+"CM1Y");
  SINCRYSAngle = Control.EvalVar<double>(beamName+"SINCRYSAngle")*M_PI/180.0;
  SINCRYSBranchShift = Control.EvalVar<double>(beamName+"SINCRYSBranchShift");
  SINCRYSCenterAngle = Control.EvalVar<double>(beamName+"SINCRYSCenterAngle")
    *M_PI/180.0;

  mode=Control.EvalDefVar<int>(keyName+"Mode",2);

  xStep = SINCRYSBranchShift;
  yStep = (CM1Y+fabs(SINCRYSBranchShift)/tan(SINCRYSAngle));

  return;
}

void
CM2Crystal::createSurfaces()
{
  ELog::RegMethod RegA("CM2Crystal","createSurfaces");

  // By default, CM2Crystal is (in mode 2: almost) in X-Z plane, i.e. it would block
  // the SINCRYS branch. The yaw rotation by 90 degrees places it (almost) parallel
  // to the Y-Z plane.
  double zAngle = M_PI_2;
  if(mode == 2){
    // In mode 2, assume that the orientation of the CM1 splitter crystal corresponds
    // to Bragg scattering at the SINCRYS center angle. The actual relation between 
    // the crystal-surface angle and the crystal-lattice orientation is neither given 
    // in the main reference [32], nor is FLUKA capable of simulating Bragg scattering.
    // Therefore, this is more a cosmetic issue.
    zAngle -= (SINCRYSAngle-SINCRYSCenterAngle)/2.0;
  } else if (mode == 3){
    zAngle += SINCRYSAngle/2.0;
  }
  X.rotate(Z,zAngle);
  Y.rotate(Z,zAngle);

  Geometry::Vec3D crystalNormalVector = Y;
  crystalNormalVector.rotate(X,crystalRoll);
  crystalNormalVector.rotate(Y,crystalPitch);
  crystalNormalVector.rotate(Z,crystalYaw);

  if(mode == 3){
    const Geometry::Vec3D Y0 = Y;
    Geometry::Quaternion q = Geometry::Quaternion::calcQVRot(
        Y,crystalNormalVector,Y);
      X = q.rotate(X);
      Y = q.rotate(Y);
      Z = q.rotate(Z);

    crystalNormalVector = Y0;
  }

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
        Origin-Y*(crystalPitDepth-crystalThick),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
        Origin+Y*(length-(crystalPitDepth-crystalThick)),-Y);
      setBack(SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,Y,
    crystalPitChannelDepth);

  ModelSupport::buildPlane(SMap,buildIndex+21,
    Origin,crystalNormalVector);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+21,crystalNormalVector,
    crystalThick);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*topWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*crystalPitWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*crystalWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*topWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*crystalPitWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*crystalWidth/2.0,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,
    Origin-Z*(height-topHeightAboveOpticalAxis),Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,
    Origin+Z*(topHeightAboveOpticalAxis-topHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+25,
    Origin-Z*crystalPitHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+35,
    Origin-Z*crystalHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+45,
    Origin-Z*crystalPitChannelWidth/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
    Origin+Z*topHeightAboveOpticalAxis,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
    Origin+Z*crystalPitHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
    Origin+Z*crystalHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,
    Origin+Z*crystalPitChannelWidth/2.0,Z);
}

void
CM2Crystal::createObjects(Simulation& System)
  /*!
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CM2Crystal","createObjects");

  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");

  addOuterSurf(
    front*back
    *ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6")
  );

  makeCell("Bottom",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -15"));
  makeCell("TopLeft",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,
      "13 -23 15 -6 (11 : -45 : 46)"));
  makeCell("ChannelLeft",System,cellIndex++,0,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,"-11 13 -23 45 -46"));
  makeCell("TopCenter",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,
      "23 -24 15 -6 (-25 : 26 : 31)"));
  makeCell("TopRight",System,cellIndex++,holderMaterial,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,
      "24 -14 15 -6 (11 : -45 : 46)"));
  makeCell("ChannelRight",System,cellIndex++,0,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,"-11 -14 24 45 -46"));
  makeCell("TopLeftVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"3 -13 15 -6"));
  makeCell("TopRightVoid",System,cellIndex++,0,0.0,
    front*back*ModelSupport::getHeadRule(SMap,buildIndex,"14 -4 15 -6"));

  makeCell("CrystalPit",System,cellIndex++,0,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,
      "-31 23 -24 25 -26 (-21 : -33 : 34 : -35 : 36)"));
  makeCell("Crystal",System,cellIndex++,crystalMaterial,0.0,
    front*ModelSupport::getHeadRule(SMap,buildIndex,
      "21 -31 33 -34 35 -36"));
}

void
CM2Crystal::createLinks()
{
  ELog::RegMethod RControl("CM2Crystal","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
CM2Crystal::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("CM2Crystal","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
