/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/SideShield.h
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
#ifndef xraySystem_SideShield_h
#define xraySystem_SideShield_h

class Simulation;

namespace xraySystem
{
  /*!
    \class SideShield
    \version 1.0
    \author S. Ansell
    \date March 2020
    \brief Builds an extra shielding slab into the side wall
  
    This class needs and externalCut of Wall that is used to construct
    the normal basis set. 
  */
  
class SideShield :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  const std::string baseName;       ///< base name
  
  double depth;                     ///< depth into wall
  double height;                    ///< height
  double length;                    ///< full length
  double outStep;                   ///< outStep into void
  
  int mat;                          ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  SideShield(const std::string&);
  SideShield(const std::string&,const std::string&);
  SideShield(const SideShield&);
  SideShield& operator=(const SideShield&);
  virtual ~SideShield() {}  ///< Destructor

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
