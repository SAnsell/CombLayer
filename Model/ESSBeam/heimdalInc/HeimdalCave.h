/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdalInc/HeimdalCave.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef essSystem_HeimdalCave_h
#define essSystem_HeimdalCave_h

class Simulation;


namespace essSystem
{

/*!
  \class HeimdalCave
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief HeimdalCave system
*/

class HeimdalCave : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  double length;                  ///< main length in beam
  double height;                 ///< full width (no sub floor)
  double width;                  ///< inner width (void)
  double corridorWidth;          ///< width of corridor (void)
  double corridorLength;         ///< length of corridor (void)
  double corridorTLen;           ///< length of thick part of corridor

  double doorLength;             ///< length of door on corridor

  double roofGap;                ///< width of roof gap
  double beamWidth;              ///< beam width (front gap)
  double beamHeight;             ///< beam height (front gap)

  double midZStep;               ///< mid level
  double midThick;               ///< mid thickness
  double midHoleRadius;          ///< mid level Hole
  
  double mainThick;              ///< full height
  double subThick;               ///< full height

  int wallMat;
  int voidMat;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  HeimdalCave(const std::string&);
  HeimdalCave(const HeimdalCave&);
  HeimdalCave& operator=(const HeimdalCave&);
  ~HeimdalCave();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
