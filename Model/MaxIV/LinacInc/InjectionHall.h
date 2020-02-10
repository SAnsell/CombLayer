/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxivBuildInc/InjectionHall.h
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
#ifndef xraySystem_InjectionHall_h
#define xraySystem_InjectionHall_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class InjectionHall
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Main 1.5GeV ring building and inner void  
*/

class InjectionHall :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double mainLength;             ///< total length
  
  double linearRCutLength;       ///< Length to right out step
  double linearLTurnLength;      ///< Length to left angle out (inner)
  double spfAngleLength;         ///< Length to left off angle
  double spfAngle;               ///< SPF hall diagonal wall

  double rightWallStep;          ///< Extra out step on right
  
  double linearWidth;            ///< Wall - Wall width

  double floorDepth;             ///< Depth (floor to under roof)
  double roofHeight;             ///< Height (floor to under roof)

  double wallThick;             ///< Wall thickness
  double roofThick;             ///< roof thickness
  double floorThick;            ///< floor thickness

  double boundaryWidth;           ///< Width after walls
  double boundaryHeight;          ///< Height after roof
  
    
  int voidMat;               ///< void material
  int wallMat;               ///< Wall material
  int roofMat;               ///< Roof material
  int floorMat;              ///< Floor material

  void createFloor(Simulation&);
    
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  InjectionHall(const std::string&);
  InjectionHall(const InjectionHall&);
  InjectionHall& operator=(const InjectionHall&);
  virtual ~InjectionHall();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
