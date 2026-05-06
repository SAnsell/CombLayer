/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/CM1BeamSplitter.h
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
  \version 2.1.0
  \date April 2026
  \brief Beam Splitter and High-Pass Filter in CM1

  The CM1 beam splitter allows to switch between two operation modes in the
  DanMAX/SINCRYS beamline.
  In the first mode ("DanMAX mode"), the beam passes straight through a
  high-pass-filter crystal into the DanMAX branch.
  In the second mode ("DanMAX/SINCRYS mode"), the high-pass filter is replaced by a
  beam-splitter crystal that allows part of the beam to pass straight through into the
  DanMAX branch. Another part is monochromatized and diverted into the SINCRYS branch
  via Bragg scattering. The angle of the beam-splitter crystal can be varied to select
  different energies.

  This class constructs the central piece inside the CM1 chamber: a copper holder on
  which the high-pass- and beam-splitter crystals are mounted. By varying the position
  and orientation of this holder, the different operation modes of DanMAX/SINCRYS can
  be chosen. The crystals are mounted on the upstream entries of two parallel holes
  that are drilled into the holder. The holes are not aligned with the holder's axes,
  but at a slight yaw angle (see below about angle definition) of about 8 degrees.
  While the high-pass filter's surface is orthogonal to the beam and the
  transmission-hole axis, the splitter crystal has a nontrivial 3D rotation. This is
  probably to avoid specular reflection. However, CM1BeamSplitter provides a special
  mode for specular reflection along the beam path for testing purposes.

  In total, CM1BeamSplitter provides 5 default positions/orientations ("modes") for
  different use cases. In all of them, it is assumed that the beam travels in the
  positive Y direction, and that the SINCRYS branch is on the negative X side of the
  incoming beam. This class uses the same convention for the pitch, roll, and yaw
  angles as [1], but with the axes adapted to the CombLayer convention.

  Rotation Name | Rotation Axis in [1] | Rotation Axis in CombLayer
  -----------------------------------------------------------------
  Pitch         |         X            |             X
  Roll          |         Z            |             Y
  Yaw           |         Y            |             Z

  An overview of the modes is given below.

  Mode 0: Positions the holder such that its axes are aligned with the coordinate axes.
  The origin is at the center of the front surface, which corresponds neither to the
  DanMAX, nor the DanMAX/SINCRYS mode.

  Mode 1: DanMAX mode. The beam passes centrally through the high-pass-filter crystal
  and the corresponding hole in the holder. The origin is at the center of the
  high-pass-filter crystal on its upstream side, i.e. where the beam hits the crystal.

  Mode 2: DanMAX/SINCRYS Bragg-reflection mode. The beam passes centrally through the
  corresponding hole in the holder. The origin is on the upstream side of the
  beam-splitter crystal where the beam hits the crystal. In order to achieve the
  desired orientation of the hole, only the yaw angle is modified, and the entire
  holder is translated. Since the beam-splitter crystal is mounted with a nonzero
  pitch, roll, and yaw w.r.t. the holder, this means that specularly reflected
  particles will not travel along the beamline by design.

  Mode 3: DanMAX/SINCRYS specular-reflection mode. The origin is at the same point as
  in Mode 2, but the holder is oriented such that the beam axis is in the
  splitter-crystal plane and the -X axis is the normal axis of the splitter-crystal
  plane. This means that the pitch, roll, and yaw rotation of the splitter crystal have
  been compensated. By changing the yaw (ZAngle) of the holder, the specular-reflection
  angle in the X-Y plane can be set in a straighforward way.

  Mode 4: Blocking mode. The holder is oriented and positioned to approximate the
  worst-case scenario where the beam is unintentionally scattered in CM1 to create
  large radiation-dose rates on its outside. This mode is intended for studies of
  accident/misalignment scenarios at the beamline. Finding the exact worst-case 
  orientation -which is also dependent on the beam properties and the position of the
  observer- is nontrivial. To approximate the worst case, an orientation is chosen
  where a large amount of material is in the way of the beam and any secondary
  radiation. Starting from the position of Mode 2, the holder is shifted in the 
  negative X direction such that the optical axis is halfway between the two holes.

  References:
  [1] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
  [2] CAD model CM1_chamber_2026-03-04.STEP /mxn/groups/rad/Beamlines/DanMAX/Simulations/CM1_chamber_2026-03-04.STEP

  Version history:
  2.1.0 - 2026-04-29
    - Add Mode 4.
  2.0.1 - 2026-04-27
    - Fix cell boundaries in bottom part.
  2.0   - 2026-04-10
    - Orientation determined by CombLayer variables.
  1.0.1 - 2026-04-02
    - Set correct number of link points.
  1.0   - 2026-04-02
*/
class CM1BeamSplitter :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  // Holder and outer-surface dimensions

  /// Angle between the sides of the holder and the transmission holes.
  double bodyAngle;
  double length;
  double height;
  double width;

  // Lower part of the holder

  /// Width of the small flat part on the front surface of the holder.
  double bottomChamferWidth;
  double bottomDepth; ///< Distance from the bottom to the transmission hole centers.
  double bottomHeight;
  double bottomRoundingRadius;
  double bottomWidth;

  // Filter crystal

  double filterCrystalPitDepth;
  double filterCrystalPitSideLength;
  double filterCrystalSideLength;
  double filterCrystalThick;

  // Filter hole (i.e. beam path in DanMAX mode)

  double filterHoleDownstreamRadius;
  double filterHoleOffset;
  double filterHoleUpstreamLength;
  double filterHoleUpstreamRadius;

  // Splitter crystal

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

  // Splitter crystal hole (i.e. beam path in DanMAX/SINCRYS mode)

  double splitterHoleCenterLength;
  double splitterHoleCenterRadius;
  double splitterHoleDownstreamRadius;
  double splitterHoleToFilterHole;
  double splitterHoleUpstreamLength;
  double splitterHoleUpstreamRadius;

  // Overhang on the -X side of the upper part of the holder.

  double topOverhangThick;
  double topOverhangWidth;

  // Materials

  int holderMaterial;
  int filterCrystalMaterial;
  int splitterCrystalMaterial;

  double SINCRYSAngle;
  double SINCRYSCenterAngle;

  /// Beam-splitter mode.
  /// Possible values are 0,1,2, or 3.
  /// If mode is set to any other integer value, mode 3 will be used.
  int mode;

 protected:
  void updateInternalReferenceVectors(
    Geometry::Vec3D&,Geometry::Vec3D&,Geometry::Vec3D&,Geometry::Vec3D&,
    Geometry::Vec3D&);
  void calculateSplitterCrystalNormalVector(Geometry::Vec3D&);
  void calculateSplitterCrystalOrigin(const Geometry::Vec3D&,const Geometry::Vec3D&,
    const Geometry::Vec3D&,Geometry::Vec3D&,const Geometry::Vec3D&);
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
 
