/********************************************************************* 
  Comb-Layer : MCNP(X) Input builder
 
 * File:   physicsInc/ELPTConstructor.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef physicsSystem_ELPTConstructor_h
#define physicsSystem_ELPTConstructor_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace physicsSystem
{
  class PhysicsCards;
  class ExtControl;
  
/*!
  \class ELPTConstructor
  \version 1.0
  \author S. Ansell
  \date February 2016
  \brief Processes input to produce ELPT energy cut card

*/

class ELPTConstructor 
{
 private:

  ZoneUnit<double> ZUnits;
  
  void writeHelp(std::ostream&) const;
    
 public:

  ELPTConstructor();
  ELPTConstructor(const ELPTConstructor&);
  ELPTConstructor& operator=(const ELPTConstructor&);
  ~ELPTConstructor() {}  ///< Destructor

  void processUnit(SimMCNP&,
		   const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
