/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMRadialSupport.h
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
#ifndef xraySystem_MLMRadialSupport_h
#define xraySystem_MLMRadialSupport_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMRadialSupport
  \author S. Ansell
   \version 1.0
  \date October 2019
  \brief Radial support component

  This class assumes that the origin is in the vertical face
  of the front of the mirror. The centre strut is lined up with
  the mirror for stability.
*/

class MLMRadialSupport :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double length;               ///< Total length

  double topGap;               ///< Top plate drop down
  double topThick;             ///< Top plate thickness
  double topBeamWidth;         ///< Top plate width
  double topOutWidth;          ///< Top plate width

  double plateThick;           ///< Thickness of plate
  double plateLength;          ///< Length of plates  (gap is length - 2*pLen)
  double plateBeam;            ///< Extra extention under beam
  double plateXStep;           ///< Step +x away from beam of main plate

  double sideWidth;            ///< Width of straight block
  double sideBlock;            ///< Length of outside block
  double sideLift;             ///< extra straight vertical
  double sideFullWidth;        ///< Total length [for screw steps]
  double sideBaseWidth;        ///< Length between inner blocks
  double sideOutWidth;         ///< Length between outer blocks

  double bladeDrop;            ///< Distance blades down 
  double bladeThick;           ///< blade thickness
  double bladeHeight;          ///< blade height from base plate
  double bladeTopGap;          ///< blade separation at top
  double bladeBaseGap;         ///< blade separation at base 

  double baseThick;            ///< Full base thickness
   
  int plateMat;                ///< top plate material
  int baseMat;                 ///< main material
  int voidMat;                 ///< Void / vacuum material
  
  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMRadialSupport(const std::string&);
  MLMRadialSupport(const MLMRadialSupport&);
  MLMRadialSupport& operator=(const MLMRadialSupport&);
  virtual ~MLMRadialSupport();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
