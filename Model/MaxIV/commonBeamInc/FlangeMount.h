/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/FlangeMount.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_FlangeMount_h
#define xraySystem_FlangeMount_h

class Simulation;

namespace xraySystem
{

/*!
  \class FlangeMount
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief FlangeMount unit
*/

class FlangeMount :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double plateThick;            ///< Top plate thickness
  double plateRadius;           ///< plate radius

  double threadRadius;          ///< support thread
  double threadLength;          ///< Length of thread [full]

  int inBeam;                   ///< In beam
  double bladeXYAngle;          ///< Angle of blade
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeWidth;            ///< moving blade radius
  double bladeHeight;           ///< moving blade radius

  int threadMat;                ///< thread material
  int bladeMat;                 ///< blade material
  int plateMat;                 ///< plate material

  bool bladeActive;             ///< Blade made/not made
  int bladeCentreActive;        ///< Flag to use bladeCentre
  /// Norminal point to get centre from [over-writes threadLength]
  Geometry::Vec3D bladeCentre;

  // hole in the blade centre (e.g. if used as zero-order block)
  int holeActive; ///< Hole made/not made
  double holeWidth; ///< Hole width
  double holeHeight; ///< Hole height

  void calcThreadLength();

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FlangeMount(const std::string&);
  FlangeMount(const FlangeMount&);
  FlangeMount& operator=(const FlangeMount&);
  ~FlangeMount() override;

  void setBladeCentre(const attachSystem::FixedComp&,const long int);
  void setBladeCentre(const Geometry::Vec3D&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
