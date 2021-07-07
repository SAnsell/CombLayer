/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VoidUnit.h
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
#ifndef constructSystem_VoidUnit_h
#define constructSystem_VoidUnit_h

class Simulation;

namespace constructSystem
{

/*!
  \class VoidUnit
  \version 1.0
  \author S. Ansell
  \date Febrary 2021
  \brief VoidUnit 

  Designed as a virtual void [spacer for something]
*/

class VoidUnit :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:
  
  double height;                  ///< Height of the cut
  double width;                   ///< Width of the basic cut
  double length;                  ///< Length of basic cut
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  VoidUnit(const std::string&);
  VoidUnit(const VoidUnit&);
  VoidUnit& operator=(const VoidUnit&);
  ~VoidUnit();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
