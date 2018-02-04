/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/OpticsWall.h
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
#ifndef xraySystem_OpticsWall_h
#define xraySystem_OpticsWall_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class OpticsWall
  \version 1.0
  \author S. Ansell
  \date July 2016
  \brief OpticsWall unit  

  Built around the central beam axis
*/

class OpticsWall :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
  
{
 private:
  
  const int hutIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double depth;                 ///< void height 
  double height;                ///< void deptth
  double length;                ///< void out side width
  double ringWidth;             ///< void flat part to ring
  double ringWallLen;           ///< void flat length [before angle]
  double ringWallAngle;         ///< angle on ring side wall
  double outWidth;              ///< out side width

  double innerThick;            ///< Inner wall/roof skin
  double pbThick;               ///< Thickness of lead in walls
  double outerThick;            ///< Outer wall/roof skin
  double floorThick;            ///< Floor thickness
  
  int skinMat;                ///< Fe layer material for walls
  int pbMat;                  ///< pb layer material for walls 
  int floorMat;               ///< Floor layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  OpticsWall(const std::string&);
  OpticsWall(const OpticsWall&);
  OpticsWall& operator=(const OpticsWall&);
  virtual ~OpticsWall();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
