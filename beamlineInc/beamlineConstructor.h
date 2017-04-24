/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/beamlineConstructor.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef beamlineSystem_beamlineConstructor_h
#define beamlineSystem_beamlineConstructor_h

namespace essSystem
{
  class Bunker;
}

/*!
  \namespace beamlineSystem
  \brief General stuff to build a beamline 
  \version 1.0
  \date April 2014
  \author S. Ansell
*/

namespace beamlineSystem
{

  /*!
    \class beamlineConstructor
    \version 1.0
    \author S. Ansell
    \date April 2014
    \brief Make function for beamline

    Builds a beamline from bulk Shield to termination
  */
  
class beamlineConstructor
{
 private:

 public:
  
  beamlineConstructor() {}   ///< constructo
  beamlineConstructor(const beamlineConstructor&) {}  ///< null assignement
  /// null assignement
  beamlineConstructor& operator=(const beamlineConstructor&) 
    { return *this; }
  virtual ~beamlineConstructor() {}   ///< Destructor
  
  virtual void build(Simulation&,const essSystem::Bunker&) =0;

};

}

#endif
