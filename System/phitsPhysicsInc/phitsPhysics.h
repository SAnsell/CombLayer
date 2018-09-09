/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsPhysicsInc/phitsPhysics.h
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
#ifndef phitsSystem_phitsPhysics_h
#define phitsSystem_phitsPhysics_h

namespace phitsSystem
{
  
/*!
  \class phitsPhysics
  \version 1.0
  \date March 2018
  \author S.Ansell
  \brief Processes the physics cards in the PHITS output
*/

class phitsPhysics 
{
 private:

 public:
   
  phitsPhysics();
  phitsPhysics(const phitsPhysics&);
  phitsPhysics& operator=(const phitsPhysics&);
  virtual ~phitsPhysics();


  void writePHITS(std::ostream&) const;
};

}

#endif
