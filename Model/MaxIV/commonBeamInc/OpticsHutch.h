/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/bladerOpticsHutch.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_OpticsHutch_h
#define xraySystem_OpticsHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
/*!
  \class OpticsHutch
  \version 1.0
  \author S. Ansell
  \date January 2021
  \brief OpticsHutch unit 

  Built around the central beam axis with out 
  side wall on the ring side.
*/

class OpticsHutch :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 protected:

  double height;                ///< void height
  double length;                ///< void out side width
  double outWidth;              ///< Width from beamline centre to outside
  
  // walls
  double innerThick;            ///< Inner wall/roof skin
  double pbWallThick;           ///< Thickness of lead in walls
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerThick;            ///< Outer wall/roof skin

  double innerOutVoid;          ///< Extension for inner left void space
  double outerOutVoid;          ///< Extension for outer left void space
  double backVoid;              ///< Extension for outer back void space

  std::vector<Geometry::Vec3D> holeOffset;  ///< hole offsets [y ignored]
  std::vector<double> holeRadius;           ///< hole radii

  int skinMat;                ///< Fe layer material for walls
  int pbMat;                  ///< pb layer material for walls
  int concreteMat;            ///< side wall (concrete filler)
  int voidMat;                ///< Void material

  /// Chicanes
  std::vector<std::shared_ptr<PortChicane>> PChicane;

  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&);
  void createLinks();
  void createChicane(Simulation&);

 public:

  OpticsHutch(const std::string&);
  OpticsHutch(const OpticsHutch&);
  OpticsHutch& operator=(const OpticsHutch&);
  virtual ~OpticsHutch();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
