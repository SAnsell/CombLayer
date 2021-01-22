/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/FCLConstructor.h
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
#ifndef physicsSystem_FCLConstructor_h
#define physicsSystem_FCLConstructor_h



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
  /*!
    \class FCLConstructor
    \version 1.0
    \author S. Ansell
    \date January 2017
    \brief Builds the Forced Collision physics card
  */

class FCLConstructor 
{
 private:

  void writeHelp(std::ostream&) const;
    
 public:

  FCLConstructor() {}
  FCLConstructor(const FCLConstructor&) {}
  FCLConstructor& operator=(const FCLConstructor&) { return *this; }
  ~FCLConstructor() {}  ///< Destructor

  void processUnit(SimMCNP&,const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
