/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Mirror.h
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
#ifndef xraySystem_Mirror_h
#define xraySystem_Mirror_h

class Simulation;

namespace xraySystem
{

/*!
  \class Mirror
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Paired Mono-crystal constant exit gap
*/

class Mirror :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const int mirrIndex;     ///< Index of surface offset
  int cellIndex;           ///< Cell index

  double theta;            ///< Theta angle
  double phi;              ///< phi angle

  double radius;           ///< Radius of surface cylinder
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection
  double length;           ///< length along beam

  double baseThick;        ///< Base thickness
  double baseExtra;        ///< Extra width/lenght of base

  int mirrMat;             ///< XStal material
  int baseMat;             ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Mirror(const std::string&);
  Mirror(const Mirror&);
  Mirror& operator=(const Mirror&);
  virtual ~Mirror();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
