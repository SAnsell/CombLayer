/*********************************************************************
 CombLayer : MCNP(X) Input builder

* File:   commonBeamInc/MovableMask.h
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
#ifndef xraySystem_MovableMask_h
#define xraySystem_MovableMask_h

class Simulation;

namespace xraySystem {
/*!
  \class MovableMask
  \version 0.1
  \author U. Friman-Gayer
  \date September 2026
  \brief Movable Mask with L-shaped cross section manufactured by TOYAMA

  When viewed from upstream or downstream, the movable mask appears as an
  L-shaped block that collimates the beam from two sides. However, the inner
  surfaces are angled to ensure a uniform heat load inside the component.

  The component supports independent left-right and up-down movement.
  Therefore, two of these movable masks, in sequence and rotated against each
  other, can be used to create a beam with a rectangular cross section with
  arbitrary dimensions and position within the given limits of the mechanics.

  The geometry is based on drawings provided for the DanMAX beamline at MAX IV
  [1-4] and a CAD model [5]. It is a simplified version of the 300-mm long main
  body [1,3] that includes the flanges. As indicated above, DanMAX uses two of
  these movable masks in the front end. When viewed from upstream, the first
  movable mask is oriented like the letter "L", i.e. it constrains the beam from
  the left and from the bottom. This corresponds to the default orientation of
  this class: Beam along the y axis, mask along the x (bottom) and z (left)
  axis. The second movable mask is rotated by 180 degrees about the beam axis
  compared to the first one, and it constrains the beam from the right and from
  the top. At the level of detail of this class, the two movable masks are
  identical.

  [1] TOYAMA, Movable Mask 1 for DanMAX, S6-4-1AG01042.pdf
  [2] TOYAMA, Movable Mask 1 main body for DanMAX, S6-5-1AG01044.pdf
  [3] TOYAMA, Movable Mask 2 for DanMAX, S6-8-1AG01043.pdf
  [4] TOYAMA, Movable Mask 2 main body for DanMAX, S6-9-1AG01045.pdf
  [5] CAD model of DanMAX/SINCRYS, J. Selberg, fall/winter 2025,
  /mxn/groups/rad/Beamlines/DanMAX/Simulations/FE_02.STEP
*/

class MovableMask : public attachSystem::FixedRotate,
                    public attachSystem::ContainedComp,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap {
private:
  double length;

  double bodyHeight;
  double bodyLength;
  double bodyWidth;

  double flangeInnerRadius;
  double flangeLength;
  double flangeRadius;
  double flangeWallThick;

  double holeHeight;
  double holeWidth;
  double holeOffset;

  double maskLeftMaxHeight;   // Maximum height of the left part of the mask,
                              // measured from the bottom of the hole.
  double maskLeftMaxWidth;    // Maximum width of the left part of the mask,
                              // measured from the left side of the hole.
  double maskBottomMaxHeight; // Maximum height of the bottom part of the mask,
                              // measured from the bottom of the hole.
  double maskBottomMaxWidth;  // Maximum width of the bottom part of the mask,
                              // measured from the left side of the hole.
  double maskFocalPoint;      // Distance (normalized to bodyLength, i.e.
                         // dimensionless value between 0.0 and 1.0) from the
                         // upstream edge of the body to the point where the
                         // slopes of the mask's surfaces change.
  double
      maskDownstreamInnerPlaneAngle; // Angle w.r.t. canonical axis in deg.
                                     // Upstream from the focal point, the inner
                                     // surfaces of the mask are parallel to the
                                     // x and z axes, respectively. Downstream
                                     // from this point, there is a transition
                                     // to an angled inner surface which
                                     // increases the inner cross section.

  int flangeMaterial;
  int voidMaterial;
  int bodyMaterial;

  void createSurfaces();
  void createObjects(Simulation &);
  void createLinks();

public:
  MovableMask(const std::string &);
  MovableMask &operator=(const MovableMask &);
  ~MovableMask() override {}

  void populate(const FuncDataBase &) override;

  using FixedComp::createAll;
  void createAll(Simulation &, const attachSystem::FixedComp &,
                 const long int) override;
};

} // namespace xraySystem

#endif
