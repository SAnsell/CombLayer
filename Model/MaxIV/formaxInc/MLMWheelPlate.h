/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMWheelPlate.h
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
#ifndef xraySystem_MLMWheelPlate_h
#define xraySystem_MLMWheelPlate_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMWheelPlate
  \author S. Ansell
   \version 1.0
  \date October 2019
  \brief Radial support component

  This class assumes that the origin is in the vertical face
  of the front of the mirror. The centre strut is lined up with
  the mirror for stability.
*/

class MLMWheelPlate :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  // top plate
  double length;               ///< Total length
  double width;                ///< Total width
  double thick;                ///< Total thickness (include ridge)
  double ridgeThick;           ///< Thickness of just the front/back ridges
  double ridgeLen;             ///< Length of ridge (beam direction)
  
  double outSlotWidth;         ///< Slot width 
  double outSlotLength;        ///< Out slot length

  double midSlotXStep;         ///< Displacement of slot
  double midSlotLength;        ///< mid slot length
  double midSlotWidth;         ///< Mid slot width

  double driveZClear;          ///< ZClearance
  double driveThick;           ///< Full thickness
  double driveWidth;           ///< Full Width
  double driveLength;          ///< Full Length
  double driveTopLen;          ///< Len of top section
  double driveBaseThick;       ///< Thickness at base
  double driveCutWidth;
  double driveCutLength;       ///< Length from +x side
  double driveCutRadius;       ///< Half radius of cut out
  double driveCutRadLen;       ///< length of cutou
  
  double baseYStep;            ///< Step for main base
  double baseThick;            ///< Total Thickness of base plae
  double baseWidth;
  double baseLength; 

  double baseCutDepth;         ///< Recess cut depth
  double baseCutFLen;          ///< Recess from front surf
  double baseCutBLen;          ///< Recess from back suf
  double baseCutWidth;         ///< Recess cut with
  double baseMidSlotHeight;    ///< Mid slot Height (from base)
  double baseMidSlotWidth;     ///< Mid slot cut out

  double baseSideSlotEdge;     ///< Side Slot edge thickness
  double baseSideSlotHeight;   ///< Side slot height (from base)
  double baseSideSlotWidth;    ///< Side slot height 
  
  int plateMat;                ///< top plate material
  int voidMat;                 ///< Void / vacuum material
  
  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMWheelPlate(const std::string&);
  MLMWheelPlate(const MLMWheelPlate&);
  MLMWheelPlate& operator=(const MLMWheelPlate&);
  virtual ~MLMWheelPlate();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
