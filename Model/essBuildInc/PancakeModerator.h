/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/PancakeModerator.h
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov / Stuart Ansell
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
  \version 1.0
  \date March 2017
  \brief Pancake moderator
*/

class PancakeModerator :
  public EssModBase
{
 private:

  const int flyIndex;        ///< Index of surface offset
  int cellIndex;             ///< Cell index

  int bfType;                  ///< Type (BF1 or BF2)
  
  std::shared_ptr<DiskPreMod> MidH2;    ///< Water disc in the middle
  std::shared_ptr<EdgeWater> LeftWater;    ///< Left reflector part
  std::shared_ptr<EdgeWater> RightWater;    ///< Right reflector part

  double totalHeight;                     ///< Total height
  double outerRadius;                     ///< Main outer radius
  int    wallMat;                         ///< upper/bottom wall material
  double wallDepth;                       ///< upper wall thickness
  double wallHeight;                      ///< bottom wall thickness

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,
			const attachSystem::FixedComp&,
			const long int);
			

  void createExternal();
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  PancakeModerator(const std::string&);
  PancakeModerator(const PancakeModerator&);
  PancakeModerator& operator=(const PancakeModerator&);
  virtual PancakeModerator* clone() const;
  virtual ~PancakeModerator();
  
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual int getCommonSurf(const long int) const;

  /// Accessor to radius
  void setRadiusX(const double R) { outerRadius=R; }
  virtual const attachSystem::FixedComp&
    getComponent(const std::string&) const;

  std::string getSideRule() const;
  std::string getLeftRightWaterSideRule() const;
  Geometry::Vec3D getFocalPoint(const long int) const;
  std::vector<Geometry::Vec3D> getFocalPoints() const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
