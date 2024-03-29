/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/PancakeModerator.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell / Konstantin Batkov
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
#ifndef essSystem_PancakeModerator_h
#define essSystem_PancakeModerator_h

class Simulation;

namespace essSystem
{
  class DiskPreMod;
  class EdgeWater;

/*!
  \class PancakeModerator
  \author Konstantin Batkov
  \version 2.0
  \date June 2017
  \brief Pancake moderator
*/

class PancakeModerator :
  public EssModBase
{
 private:

  std::shared_ptr<DiskPreMod> MidH2;        ///< Hydrogen disc in the middle
  std::shared_ptr<EdgeWater> LeftWater;    ///< Left reflector part
  std::shared_ptr<EdgeWater> RightWater;    ///< Right reflector part

  double totalHeight;                     ///< Total height
  double outerRadius;                     ///< Main outer radius

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,
			const attachSystem::FixedComp&,
			const long int) override;

  void createExternal();
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PancakeModerator(const std::string&);
  PancakeModerator(const PancakeModerator&);
  PancakeModerator& operator=(const PancakeModerator&);
  PancakeModerator* clone() const override;
  ~PancakeModerator() override;

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;

  /// Accessor to radius
  void setRadiusX(const double R) { outerRadius=R; }

  const attachSystem::FixedComp&
    getComponent(const std::string&) const override;

  HeadRule getLeftExclude() const;
  HeadRule getRightExclude() const;
  HeadRule getLeftFarExclude() const;
  HeadRule getRightFarExclude() const;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif

