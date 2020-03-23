/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1CommmoInc/R1Ring.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef xraySystem_R1Ring_h
#define xraySystem_R1Ring_h

class Simulation;
namespace insertSystem
{
  class insertObject;
  class insertPlate;
}

namespace xraySystem
{
  class Maze;
  class RingDoor;
  class SideShield;
  
/*!
  \class R1Ring
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Main 1.5GeV ring building and inner void  
*/

class R1Ring :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double fullOuterRadius;         ///< full outer radius
  double hexRadius;               ///< Inner hex radius
  double hexWallThick;            ///< Inner hex wall thickness

  double height;                  ///< Main height
  double depth;                   ///< Main depth
  double floorThick;              ///< Floor depth
  double roofThick;               ///< Roof thickness
  double roofExtra;               ///< Roof Extra void above roof
  
  size_t NPoints;                 ///< number of points in track
  size_t concaveNPoints;          ///< number of concave points in track
  std::vector<size_t> concavePts;    ///< number of points in track
  /// points that create the inner wall
  std::vector<Geometry::Vec3D> voidTrack;
  /// points that create the outer wall
  std::vector<Geometry::Vec3D> outerTrack;
  
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material

  size_t doorActive      ;           ///< Flag/sector for door if modeled
  std::shared_ptr<xraySystem::RingDoor> doorPtr;  ///< Outer door

  /// free standing plate shields :: Wall ID / FreeShield
  std::map<size_t,std::shared_ptr<insertSystem::insertPlate>> plateShields;

  ///  Side shields :: Wall ID  / SideShield
  std::map<size_t,std::shared_ptr<SideShield>> sideShields;

  void createRoof(Simulation&);
  void createFloor(Simulation&);
  void createDoor(Simulation&);
  void createSideShields(Simulation&);
  void createFreeShields(Simulation&);
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:

  R1Ring(const std::string&);
  R1Ring(const R1Ring&);
  R1Ring& operator=(const R1Ring&);
  virtual ~R1Ring();

  /// accessor to size of concave points
  size_t nConcave() const { return concaveNPoints; }
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
