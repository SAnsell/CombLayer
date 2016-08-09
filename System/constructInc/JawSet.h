/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawSet.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_JawSet_h
#define essSystem_JawSet_h

class Simulation;

namespace constructSystem
{
  class RotaryCollimator;
  class Jaws;
  
/*!
  \class JawSet
  \version 1.0
  \author S. Ansell
  \date June 2016
  \brief Container class for multi-Jaw system
*/

class JawSet : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int jawsetIndex;             ///< SurfIndex offset
  int cellIndex;                  ///< cell index

  /// JAW X Axis
  std::shared_ptr<constructSystem::Jaws> JawX;
  /// JAW ZX (+45deg) [if present]
  std::shared_ptr<constructSystem::Jaws> JawXZ;


  double radius;           ///< Excloseing radius
  double length;           ///< Enclosing length

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
    
 public:

  JawSet(const std::string&);
  JawSet(const JawSet&);
  JawSet& operator=(const JawSet&);
  virtual ~JawSet();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
