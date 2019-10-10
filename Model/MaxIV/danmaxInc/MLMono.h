/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/Mirror.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_MLMono_h
#define xraySystem_MLMono_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMono
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Mirror Mono [horizontal]
*/

class MLMono :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double gap;               ///< Gap thickness

  double theta;             ///< Theta angle [mirror -xray 2T]

  double widthA;            ///< Width of block across beam
  double heightA;           ///< Depth into beam
  double lengthA;           ///< Length along beam
  
  double widthB;            ///< Width of block across beam
  double heightB;           ///< Depth into beam
  double lengthB;           ///< Length along beam

  double supportAGap;       ///< Base under gap below mirror
  double supportASide;      ///< Side thickness
  double supportABase;      ///< Base thickness

  int mirrAMat;             ///< XStal material
  int mirrBMat;             ///< XStal material
  int baseAMat;              ///< Base material
  int baseBMat;              ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMono(const std::string&);
  MLMono(const MLMono&);
  MLMono& operator=(const MLMono&);
  virtual ~MLMono();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
