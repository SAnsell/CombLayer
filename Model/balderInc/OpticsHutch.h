/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/OpticsHutch.h
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
#ifndef xraySystem_OpticsHutch_h
#define xraySystem_OpticsHutch_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class OpticsHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief OpticsHutch unit  

  Built around the central beam axis
*/

class OpticsHutch :
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
  double pbWallThick;           ///< Thickness of lead in walls
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerThick;            ///< Outer wall/roof skin
  double floorThick;            ///< Floor thickness

  double holeXStep;            ///< Hole XStep
  double holeZStep;            ///< Hole ZStep  
  double holeRadius;           ///< Hole radius
  
  int skinMat;                ///< Fe layer material for walls
  int pbMat;                  ///< pb layer material for walls 
  int floorMat;               ///< Floor layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  OpticsHutch(const std::string&);
  OpticsHutch(const OpticsHutch&);
  OpticsHutch& operator=(const OpticsHutch&);
  virtual ~OpticsHutch();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
