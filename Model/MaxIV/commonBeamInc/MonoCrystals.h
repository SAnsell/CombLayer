/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoCrystals.h
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
#ifndef xraySystem_MonoCrystals_h
#define xraySystem_MonoCrystals_h

class Simulation;

namespace xraySystem
{

/*!
  \class MonoCrystals
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Paired Mono-crystal constant exit gap
*/

class MonoCrystals :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double gap;              ///< Gap thickness
  double theta;            ///< Theta angle

  double phiA;            ///< phi angle of first 
  double phiB;            ///< pho angle of second
  
  double widthA;            ///< Radius of from centre
  double thickA;            ///< Radius of detector
  double lengthA;           ///< Outer wall thickness
  
  double widthB;            ///< Radius of from centre
  double thickB;            ///< Radius of detector
  double lengthB;           ///< Outer wall thickness

  double baseAGap;          ///< Base thickness
  double baseALength;       ///< Base length
  double baseAThick;        ///< Base thickness
  double baseAExtra;        ///< Extra width/lenght of base

  double baseBGap;          ///< Base thickness
  double baseBLength;       ///< Base length
  double baseBThick;        ///< Base thickness
  double baseBExtra;        ///< Extra width/lenght of base

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

  MonoCrystals(const std::string&);
  MonoCrystals(const MonoCrystals&);
  MonoCrystals& operator=(const MonoCrystals&);
  virtual ~MonoCrystals();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
