/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/IMPConstructor.h
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
#ifndef physicsSystem_IMPConstructor_h
#define physicsSystem_IMPConstructor_h

///\file 

class Simulation;
class SimMCNP;
namespace Geometry
{
  class Plane;
}

namespace mainSystem
{
  class inputParam;
}

namespace physicsSystem
{
  class PhysicsCards;
/*!
  \class IMPConstructor
  \brief Controls importance of cells
  \author S. Ansell
  \version 1.0
  \date April 2016
*/

class IMPConstructor 
{
 private:

  void writeHelp(std::ostream&) const;
    
 public:

  IMPConstructor() {}
  IMPConstructor(const IMPConstructor&) {}
  IMPConstructor& operator=(const IMPConstructor&) { return *this; }
  ~IMPConstructor() {}  ///< Destructor

  void processUnit(Simulation&,
		   const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
