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
  public attachSystem::CellMap
  
{
 private:
  
  const int wigIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double frontWallThick;          ///< front wall depth

  double length;                  ///< Main length
  double ringGap;                 ///< Gap to ring wall
  double ringRadius;              ///< roof thickness
  double ringThick;               ///< Gap to ring wall
  double outerGap;                ///< Gap to ring wall
  double outerThick;              ///< Gap to ring wall
  double floorDepth;              ///< floor distance from centre
  double roofHeight;              ///< roof distance from centre
  double floorThick;              ///< floor thickness
  double roofThick;               ///< roof thickness

  double frontHoleRadius;         ///< end hole
  
  int frontWallMat;          ///< front wall material
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

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
 
