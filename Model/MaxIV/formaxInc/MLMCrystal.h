/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMCrystal.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_MLMCrystal_h
#define xraySystem_MLMCrystal_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMCrystal
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class MLMCrystal :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double width;            ///< Width of block across beam
  double height;           ///< Depth into beam
  double length;           ///< Length along beam
  

  // Mirror slot on top of mirror block
  double topSlotXStep;         ///< Slot on mirror block on top surf
  double topSlotWidth;         ///< Slot width
  double topSlotLength;        ///< Slot length
  double topSlotDepth;        ///< Slot length

  int mirrorMat;             ///< XStal material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMCrystal(const std::string&);
  MLMCrystal(const MLMCrystal&);
  MLMCrystal& operator=(const MLMCrystal&);
  virtual ~MLMCrystal();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 