/********************************************************************* 
  Comb-Layer : MCNP(X) Input builder
 
 * File:   physicsInc/ExtConstructor.h
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
#ifndef physicsSystem_ExtConstructor_h
#define physicsSystem_ExtConstructor_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace physicsSystem
{

/*!
  \class ExtConstructor
  \version 1.0
  \author S. Ansell
  \date May 2015
  \brief Processes and method of producing Ext card input

*/

class ExtConstructor 
{
 private:

  std::vector<int> cellNumbers;         ///< Cell number to apply to

  void procZone(std::vector<std::string>&);
  void writeHelp(std::ostream&) const;
    
 public:

  ExtConstructor();
  ExtConstructor(const ExtConstructor&);
  ExtConstructor& operator=(const ExtConstructor&);
  ~ExtConstructor() {}  ///< Destructor

  void processUnit(Simulation&,const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
