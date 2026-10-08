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
  \version 1.1.0
  \author U. Friman-Gayer
  \date September 2026
  \brief Movable Mask with L-shaped cross section manufactured by TOYAMA

  The model of the movable mask consists of an inner part with a rectangular
  bulk cross section and the aperture. On the upstream and downstream end of the
  inner part, there are flanges to connect to incoming/outgoing bellows, with an
  optional connecting piece (rectangular with rounded edges) between the inner
  part and the flanges.

  When viewed from upstream or downstream, the appears appears
  as an L-shaped block that collimates the beam from two sides. However, the
  inner surfaces are angled in a more complex way to ensure a uniform heat load
  inside the component and to reduce the probability of transmitting
  small-angle-scattered photons.

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

  The morphology of the movable masks at the Bloch [6-7], CoSAXS [8-11],
  FinEstBeAMS [6-7], FlexPES [6-7], ForMAX [12-15], MAXPEEM [6-7], MicroMAX
  [12-15], SoftiMAX [8-11], and SPECIES [6-7] beamlines at MAX IV is equivalent
  to DanMAX. They differ only in dimensions and materials. Furthermore, this
  class is flexible enough to generate a model of the movable masks at the
  TomoWISE [16-18] beamline.

  This class provides two variables (positionX, positionZ) for positioning the
  mask relative to its nominal zero position. This is the recommended method for
  adjusting the beam cross section. At the moment, the geometry in this class
  consists solely of the movable part of the movable mask, i.e. the same effect
  could be achieved by using the XStep and ZStep variables of the base class.
  The "nominal zero position" [2,4] is the default setting (positionX =
  positionZ = 0).
  In general, the parameter space for positionX and positionZ is independently
  limited to [-0.5, 0.5]. The following special configurations are defined
  [2,4]:

  Position Name | positionX (cm) | positionZ (cm) | Aperture (cm2)
  ----------------------------------------------------------------
  Fully open    |      0.5       |       0.5      |  0.75  x  0.75
  Nominal       |      0.0       |       0.0      |  0.25  x  0.25
  Fully close   |     -0.5       |      -0.5      | -0.25  x -0.25

  Note that the x/z offsets of the special configurations are defined as in
  [2,4].

  [1] TOYAMA, Movable Mask 1 for DanMAX, S6-4-1AG01042.pdf
  [2] TOYAMA, Movable Mask 1 main body for DanMAX, S6-5-1AG01044.pdf
  [3] TOYAMA, Movable Mask 2 for DanMAX, S6-8-1AG01043.pdf
  [4] TOYAMA, Movable Mask 2 main body for DanMAX, S6-9-1AG01045.pdf
  [5] CAD model of DanMAX/SINCRYS, J. Selberg, fall/winter 2025,
  /mxn/groups/rad/Beamlines/DanMAX/Simulations/FE_02.STEP
  [6] TOYAMA, Movable Aperture 1, 1AG00962.pdf
  [7] TOYAMA, Movable Aperture 2, 1AG00963.pdf
  [8] TOYAMA, Movable Mask 1 for CoSAXS and SoftiMAX, S6-2-1AG01033.pdf
  [9] TOYAMA, Movable Mask 1 main body for CoSAXS and SoftiMAX,
  S6-3-1AG01040.pdf [10] TOYAMA, Movable Mask 2 for CoSAXS and SoftiMAX,
  S6-6-1AG01034.pdf
  [11] TOYAMA, Movable Mask 2 main body for CoSAXS and SoftiMAX,
  S6-7-1AG01041.pdf
  [12] TOYAMA, Movable Mask 1 ForMAX & MicroMAX, 1AB-01670.pdf
  [13] TOYAMA, Movable Mask 1 main body, 1AG-01044.pdf
  [14] TOYAMA, Movable Mask 2 ForMAX & MicroMAX, 1AB-01671.pdf
  [15] TOYAMA, Movable Mask 2 main body, 1AG-01045.pdf
  [16] TOYAMA, TomoWISE FE Mobable [sic] Mask 1, 14_MM1_1AG01502_20260715.pdf
  [17] TOYAMA, TomoWISE FE Mobable [sic] Mask 2, 15_MM2_1AG01504_20260715.pdf
  [18] TOYAMA, Movable mask main body, 16_MM_2JG01416_20260715.pdf

  Version history:
  1.1.0 - 2026-09-28
          0.1-mm gap between main body and slit.
  1.0.1 - 2026-09-28
          Fix inner planes.
  1.0   - 2026-09-25
*/

class MovableMask : public attachSystem::FixedRotate,
                    public attachSystem::ContainedComp,
                    public attachSystem::FrontBackCut,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap {
private:
  double length;

  double bodyHeight;
  double bodyLength;
  double bodyWidth;

  bool useConnector; // True: Build a connector piece with a rectangular cross
                     // section with rounded edges between the main body and the
                     // flanges.
                     // False: Attach the flange directly to the main body. If
                     // necessary, adjust the flange length for the correct
                     // length of the entire component.
  double connectorInnerEdgeRadius;
  double connectorInnerHeight;
  double connectorInnerWidth;
  double connectorWallThickness;

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

  double positionX; // Horizontal positioning of the movable mask.
  double positionZ; // Vertical positioning of the movable mask.

  double slitHeight;      // Height of the slit.
  double slitInnerOffset; // Ultimately, the slit constrains the beam. This
                          // (positive) parameter is the difference -both in
                          // horizontal and vertical direction- between the
                          // smallest aperture of the main body and the slit.
  double
      slitInnerSurfaceAngle; // The edges of the slit that define the beam's
                             // cross section have a slope. The angle in degrees
                             // is the same for the left and bottom edge.
  double slitThickness;      // Thickness of the slit at the downstream end.

  int bodyMaterial;
  int flangeMaterial;
  int slitMaterial;
  int voidMaterial;

  void createSurfaces();
  void createRoundedRectanglePipe(Simulation &System, const std::string name,
                                  const HeadRule &front, const HeadRule &back,
                                  const HeadRule &outer,
                                  const HeadRule &exclude,
                                  const bool buildOuterVoid);
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
