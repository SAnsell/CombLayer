/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMCrystal.h
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
  public attachSystem::ExternalCut,
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

  double baseWidth;            ///< full width
  double baseLength;           ///< full length
  double baseDepth;            ///< Depth from mirror base surface
  double baseFrontHeight;      ///< Front height of lip
  double baseBackHeight;       ///< Back height of lip
  double baseInnerWidth;       ///< Space for crystal
  double baseInnerBeamFaceLen; ///< length of step (beam side) in Y
  double baseInnerOutFaceLen;  ///< length of step (far side) in Y
  double baseBackSlotLen;      ///< length cut out at back
  double baseOutSlotLen;       ///< Slot on out face sie

  
  int mirrorMat;             ///< XStal material
  int baseMat;               ///< Base material
  int voidMat;               ///< Void / vacuum material
  
  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMCrystal(const std::string&);
  MLMCrystal(const MLMCrystal&) =default;
  MLMCrystal& operator=(const MLMCrystal&) =default;
  ~MLMCrystal() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
