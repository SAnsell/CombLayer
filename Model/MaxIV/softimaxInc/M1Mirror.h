/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/M1Mirror.h
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
#ifndef xraySystem_M1Mirror_h
#define xraySystem_M1Mirror_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1Mirror
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1Mirror :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double theta;            ///< Theta angle
  double phi;              ///< phi angle

  double radius;           ///< Radius of surface cylinder
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection
  double length;           ///< length along beam
  
  double baseTop;          ///< Base distance above mirror
  double baseDepth;        ///< Base distance below mirror 
  double baseOutWidth;     ///< Extra width of base 
  double baseGap;          ///< Base under gap below mirror

  int mirrMat;             ///< XStal material
  int baseMat;             ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Mirror(const std::string&);
  M1Mirror(const M1Mirror&);
  M1Mirror& operator=(const M1Mirror&);
  virtual ~M1Mirror();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
