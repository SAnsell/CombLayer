/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsProcessInc/phitsImpConstructor.h
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
#ifndef physicsSystem_phitsImpConstructor_h
#define physicsSystem_phitsImpConstructor_h

///\file 

class Simulation;
namespace Geometry
{
  class Plane;
}

namespace mainSystem
{
  class inputParam;
}

namespace phitsSystem
{
  class phitsPhysics;
/*!
  \class phitsImpConstructor
  \brief Controls importance of cells in PHITS
  \author S. Ansell
  \version 1.0
  \date September 2018
*/

class phitsImpConstructor 
{
 private:

  /// Typedef for ENDL type in help
  typedef ELog::OutputLog<ELog::EReport>& (*ENDL)
    (ELog::OutputLog<ELog::EReport>&);

 
 public:

  /// null constructor
  phitsImpConstructor() {}
  /// null copy constructor
  phitsImpConstructor(const phitsImpConstructor&) {}
  /// null assignment operator
  phitsImpConstructor& operator=(const phitsImpConstructor&) { return *this; }
  ~phitsImpConstructor() {}  ///< Destructor

};

}

#endif
 
