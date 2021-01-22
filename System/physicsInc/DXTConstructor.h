/********************************************************************* 
  Comb-Layer : MCNP(X) Input builder
 
 * File:   physicsInc/DXTConstructor.h
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
#ifndef physicsSystem_DXTConstructor_h
#define physicsSystem_DXTConstructor_h

class objectGroups;

namespace attachSystem
{
  class FixedComp;
}

namespace physicsSystem
{
  class PhysicsCards;
  class DXTControl;
  
/*!
  \class DXTConstructor
  \version 1.0
  \author S. Ansell
  \date May 2015
  \brief Processes and method of producing DXT card input
*/

class DXTConstructor 
{
 private:

  
  void writeHelp(std::ostream&) const;
    
 public:

  DXTConstructor();
  DXTConstructor(const DXTConstructor&);
  DXTConstructor& operator=(const DXTConstructor&);
  ~DXTConstructor() {}  ///< Destructor

  void processUnit(SimMCNP&,const mainSystem::inputParam&,
		   const size_t);
  void processDD(SimMCNP&,const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
