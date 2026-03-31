/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CM1BeamSplitter.h
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
#ifndef xraySystem_CM1BeamSplitter_h
#define xraySystem_CM1BeamSplitter_h

class Simulation;

namespace xraySystem
{
/*!
  \class CM1BeamSplitter
  \author U. Friman-Gayer
  \version 0.1
  \date March 2026
  \brief Beam Splitter and High-Pass Filter in CM1

  References:
  [1] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
  [2] CAD model CM1_chamber_2026-03-04.STEP /mxn/groups/rad/Beamlines/DanMAX/Simulations/CM1_chamber_2026-03-04.STEP
*/
class CM1BeamSplitter :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  double bodyAngle;
  double length;
  double height;
  double width;

  double bottomChamferWidth;
  double bottomDepth;
  double bottomHeight;
  double bottomRoundingRadius;
  double bottomWidth;

  double filterCrystalPitDepth;
  double filterCrystalPitSideLength;
  double filterCrystalSideLength;
  double filterCrystalThick;

  double filterHoleDownstreamRadius;
  double filterHoleOffset;
  double filterHoleUpstreamLength;
  double filterHoleUpstreamRadius;

  double splitterCrystalHeight;
  double splitterCrystalPitch;
  double splitterCrystalPitDepth;
  double splitterCrystalPitHeight;
  double splitterCrystalPitToBack;
  double splitterCrystalPitWidth;
  double splitterCrystalRoll;
  double splitterCrystalThick;
  double splitterCrystalWidth;
  double splitterCrystalYaw;

  double splitterHoleCenterLength;
  double splitterHoleCenterRadius;
  double splitterHoleDownstreamRadius;
  double splitterHoleToFilterHole;
  double splitterHoleUpstreamLength;
  double splitterHoleUpstreamRadius;

  double topOverhangThick;
  double topOverhangWidth;

  int holderMaterial;
  int filterCrystalMaterial;
  int splitterCrystalMaterial;

  int mode;

 protected:
  void calculateInternalAxes(
    Geometry::Vec3D&,Geometry::Vec3D&,Geometry::Vec3D&,Geometry::Vec3D&,
    Geometry::Vec3D&);
  void calculateSplitterCrystalNormalVector(Geometry::Vec3D&);
  void calculateSplitterCrystalOrigin(const Geometry::Vec3D&,const Geometry::Vec3D&,
    const Geometry::Vec3D&,Geometry::Vec3D&);
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CM1BeamSplitter(const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};

}

#endif
 
