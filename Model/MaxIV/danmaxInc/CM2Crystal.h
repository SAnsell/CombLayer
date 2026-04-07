/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CM2Crystal.h
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
#ifndef xraySystem_CM2Crystal_h
#define xraySystem_CM2Crystal_h

class Simulation;

namespace xraySystem
{
/*!
  \class CM2Crystal
  \author U. Friman-Gayer
  \version 1.0
  \date April 2026
  \brief Crystal in CM2 for catching and realigning the beam from CM1.

  The purpose of the CM2 crystal is to redirect the Bragg-reflected beam from the CM1
  splitter crystal to a direction that is parallel to the original optical axis and
  constitutes the SINCRYS branch.
  This class has been developed in close analogy to the CM1BeamSplitter class. The
  reader is referred to that class for more details.

  This class constructs the central piece inside the CM2 chamber: a copper holder on
  which the crystal is mounted. Like the splitter crystal in CM1, the CM2 crystal has
  a nontrivial 3D rotation to prevent specular reflection into the SINCRYS branch.
  CM2 crystal provides a special mode for specular reflection along the beam path for
  testing purposes.

  As in CM1BeamSplitter, it is assumed that the beam travels in the positive Y
  direction. In all modes, the element is built such that the Origin is at the center
  of the crystal on its outer surface and that the crystal surface is (almost) the
  X-Z plane.

  This class uses the same convention for angles and coordinate axes as
  CM1BeamSplitter, but note that the coordinate systems for CM1 and CM2 in Ref. [1]
  are rotated against each other.

  This class supports two positioning modes. For compatibility with CM1BeamSplitter,
  these are called 2 and 3, because the SINCRYS branch is only active in these two 
  modes. Note again that the origin is always at the same position. The two modes only
  differ by a 3D rotation.

  Mode 2: Bragg-reflection mode. The crystal is tilted to prevent specular reflection
  into the SINCRYS branch.

  Mode 3: Specular-reflection mode. The holder is oriented such that the crystal
  surface is in the X-Z plane, i.e. the beam direction is perpendicular to the crystal
  surface. This means that the pitch, roll, and yaw rotation of the crystal have been
  compensated. By changing the yaw (ZAngle) of the holder, the specular-reflection
  angle in the X-Y plane can be set in a straighforward way.

  References:
  [1] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
  [2] CAD model CM2_chamber_2026-03-04.STEP /mxn/groups/rad/Beamlines/DanMAX/Simulations/CM2_chamber_2026-03-04.STEP

  Version history:
  1.0   - 2026-04-07
    - Compatible with CM1BeamSplitter v1.
*/
class CM2Crystal :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  // Holder and outer-surface dimensions

  double height;
  double length;
  double width;

  // Upper part of the holder

  double topHeight;
  double topHeightAboveOpticalAxis;
  double topWidth;

  // Crystal

  double crystalHeight;
  double crystalPitch;
  double crystalPitChannelDepth;
  double crystalPitChannelWidth;
  double crystalPitDepth;
  double crystalPitHeight;
  double crystalPitWidth;
  double crystalRoll;
  double crystalThick;
  double crystalWidth;
  double crystalYaw;

  // Materials

  int holderMaterial;
  int crystalMaterial;

  /// Crystal mode.
  /// Possible values are 2 or 3 for compatibility with CM1BeamSplitter.
  /// If mode is set to any other integer value, mode 2 will be used.
  int mode;

 protected:
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CM2Crystal(const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};

}

#endif
 
