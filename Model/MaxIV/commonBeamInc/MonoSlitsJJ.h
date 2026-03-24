/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoSlitsJJ.h
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#ifndef xraySystem_MonoSlitsJJ_h
#define xraySystem_MonoSlitsJJ_h

class Simulation;

namespace xraySystem
{
/*!
  \class MonoSlitsJJ
  \author U. Friman-Gayer
  \version 0.1
  \date March 2026
  \brief JJ X-Ray Individual Blade (IB)-C30-UHV Slit System

  The IB-C30-UHV slit system [1-3] consists of four blades that move along 
  "rails"/threads inside a cylindric main vessel. This model includes the main vessel,
  the blades and rails with their individual ports, and three auxiliary ports.
  The stepper motors on the blade ports and the two forked, electrical feedthrough 
  ports are not modeled. Furthermore, many structural components inside the main vessel
  that are not hit directly by the beam are not modeled. In particular, the copper 
  mounts of the blades are missing.

  At the SINCRYS beamline, the blades with names "M1"-"M4" are hit by the incoming 
  beam in the following order [4]:
  - M4 (x > 0, near motor, "right")
  - M3 (x < 0, far from motor, "left")
  - M1 (z < 0, far from motor, "bottom")
  - M2 (z > 0, near motor, "top")
  This is the order in which the blades are implemented here. Note that the IB-C30-UHV
  manual [3] shows the reverse order, where M1/M2 are hit first, and then M3/M4.

  The ports for M3 and M4 are on the right-hand side as seen from upstream.
  The ports for M1 and M2 are on the top. The auxiliary ports are on the left.

  References:
  [1] JJ X-RAY, IB-C30-UHV CAD Model,
      https://www.jjxray.dk/product/ib-c30-uhv/ (accessed on 2026-03-09)
  [2] JJ X-RAY, IB-C30-UHV Technical Drawing, for URL see [1]
  [3] JJ X-RAY, IB-C30-UHV Manual, for URL see [1]
  [4] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
*/
class MonoSlitsJJ :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  double length;

  double adapterThick;
  double adapterInnerRadius;

  double auxPortAngleStep;
  double auxPortCenterAngle;
  double auxPortFlangeLength;
  double auxPortFlangeRadius;
  double auxPortInnerRadius;
  double auxPortOpticalAxisOffset;
  double auxPortWallThick;

  double mainInnerRadius;
  double mainWallThick;

  double baseDepth;
  double baseThick;
  double baseWidth;

  double bladeAngle;
  double bladeLongEdge;
  double bladeM1Pos;
  double bladeM2Pos;
  double bladeM3Pos;
  double bladeM4Pos;
  double bladePortCenterDist;
  double bladePortDist;
  double bladePortFlangeLength;
  double bladePortFlangeRadius;
  double bladePortInnerRadius;
  double bladePortLength;
  double bladePortWallThick;
  double bladeShortEdge;
  double bladeThick;
  double bladeThreadLength;
  double bladeThreadRadius;
  double bladeToThreadDist;

  int bladeMat;
  int mainMat;

 protected:
  void populate(const FuncDataBase&) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  MonoSlitsJJ(const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};

}

#endif
 
