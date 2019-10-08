/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoBlockXstals.h
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
#ifndef xraySystem_MonoBlockXstals_h
#define xraySystem_MonoBlockXstals_h

class Simulation;

namespace xraySystem
{

/*!
  \class MonoBlockXstals
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Paired Mono-crystal constant exit gap

  Built as if the crystals are horrizontal but can
  be rotated
*/

class MonoBlockXstals :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double gap;              ///< Gap thickness
  double theta;            ///< Theta angle [rot: Z]

  double phiA;              ///< phi angle of first [rot:Y]
  double phiB;              ///< phi angle of second [rot:Y]
  
  double widthA;            ///< Radius of from centre
  double heightA;           ///< Radius of detector
  double lengthA;           ///< Outer wall thickness
  
  double widthB;            ///< Radius of from centre
  double heightB;           ///< Radius of detector
  double lengthB;           ///< Outer wall thickness

  double baseALength;       ///< Base length
  double baseAHeight;       ///< Base thickness
  double baseAWidth;        ///< Edge aligned to crystal

  double topALength;        ///< Top length
  double topAHeight;        ///< Top thickness
  double topAWidth;         ///< Edge aligned to crystal

  double baseBLength;       ///< Base length
  double baseBHeight;       ///< Base thickness
  double baseBWidth;        ///< Edge aligned to crystal

  double topBLength;       ///< Top length
  double topBHeight;       ///< Top thickness
  double topBWidth;        ///< Edge aligned to crystal
  
  int xtalMat;             ///< XStal material
  int baseMat;             ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MonoBlockXstals(const std::string&);
  MonoBlockXstals(const MonoBlockXstals&);
  MonoBlockXstals& operator=(const MonoBlockXstals&);
  virtual ~MonoBlockXstals();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
