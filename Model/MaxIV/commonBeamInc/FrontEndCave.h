/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/FrontEndCave.h
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
#ifndef xraySystem_FrontEndCave_h
#define xraySystem_FrontEndCave_h

class Simulation;

namespace xraySystem
{
  class Maze;
  class RingDoor;
  
/*!
  \class FrontEndCave
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Place for straight section

  Built around the central beam axis
*/

class FrontEndCave :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double length;                  ///< Main length [exclude cut]
  double outerGap;                ///< Gap to outer wall
  double ringGap;                 ///< Gap to ring wall

  double frontWallThick;          ///< Outer wall thick
  double outerWallThick;          ///< Outer wall thick
  double ringWallThick;           ///< Ring wall thick

  double innerRingWidth;           ///< Extra void for fluka 
    
  
  double floorDepth;              ///< floor distance from centre
  double roofHeight;              ///< roof distance from centre
  double floorThick;              ///< floor thickness
  double roofThick;               ///< roof thickness


  double segmentLength;            ///< Angle off front wall
  double segmentAngle;            ///< Angle off front wall

  double frontHoleRadius;         ///< end hole
  
  int frontWallMat;          ///< front wall material
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material

  bool mazeActive;           ///< Flag if maze active
  std::shared_ptr<xraySystem::Maze> mazePtr;  ///< inner maze
  bool doorActive;           ///< Flag if door active
  std::shared_ptr<xraySystem::RingDoor> doorPtr;  ///< Outer door
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createMaze(Simulation&);
  void createDoor(Simulation&);
  
 public:

  FrontEndCave(const std::string&);
  FrontEndCave(const FrontEndCave&);
  FrontEndCave& operator=(const FrontEndCave&);
  virtual ~FrontEndCave();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
