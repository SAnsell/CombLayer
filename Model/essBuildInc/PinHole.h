/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PinHole.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_PinHole_h
#define essSystem_PinHole_h

class Simulation;

namespace constructSystem
{
  class RotaryCollimator;
}

namespace essSystem
{
  
/*!
  \class PinHole
  \version 1.0
  \author S. Ansell
  \date June 2015
  \brief Container class for multi-collimator system
*/

class PinHole : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,public attachSystem::CellMap
{
 private:

  const int pinIndex;             ///< SurfIndex offset
  int cellIndex;                  ///< cell index

  /// First collimator
  std::shared_ptr<constructSystem::RotaryCollimator> CollA;
  /// Second collimator
  std::shared_ptr<constructSystem::RotaryCollimator> CollB;

  double radius;           ///< Excloseing radius
  double length;           ///< Enclosing length

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
    
 public:

  PinHole(const std::string&);
  PinHole(const PinHole&);
  PinHole& operator=(const PinHole&);
  virtual ~PinHole();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
