/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essLinacInc/makeLinac.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#ifndef essSystem_makeLinac_h
#define essSystem_makeLinac_h


namespace beamlineSystem
{
  class beamlineConstructor;
}

namespace constructSystem
{
  class ModBase;
  class SupplyPipe;

}

namespace moderatorSystem
{
  class BasicFlightLine;
  class FlightLine;
}

/*!
  \namespace essSystem
  \brief General ESS stuff
  \version 1.0
  \date January 2013
  \author S. Ansell
*/

namespace essSystem
{
  class Linac;

      
  /*!
    \class makeLinac
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class makeLinac
{
 private:
  
  std::shared_ptr<Linac> LinacTunnel;   ///< target object

 public:
  
  makeLinac();
  makeLinac(const makeLinac&);
  makeLinac& operator=(const makeLinac&);
  ~makeLinac();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
