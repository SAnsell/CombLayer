/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/R1Ring.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

namespace xraySystem
{
  class Maze;
  class RingDoor;
  
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
  public attachSystem::CellMap
{
 private:

  double hexRadius;          ///< Inner hex radius
  double hexWallThick;          ///< Inner hex wall thickness

  double height;                  ///< Main height
  double depth;                   ///< Main depth
  double floorThick;              ///< Floor depth
  double roofThick;               ///< Roof thickness

  /// number of points in track
  size_t NPoints;
  /// points that create the inner wall
  std::vector<Geometry::Vec3D> voidTrack;
  /// points that create the outer wall
  std::vector<Geometry::Vec3D> outerTrack;
  
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  R1Ring(const std::string&);
  R1Ring(const R1Ring&);
  R1Ring& operator=(const R1Ring&);
  virtual ~R1Ring();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
