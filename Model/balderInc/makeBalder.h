/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/makeBalder.h
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
#ifndef xraySystem_makeBalder_h
#define xraySystem_makeBalder_h

namespace constructSystem
{
  class SupplyPipe;
  class VacuumPipe;
  class Motor;
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  class CrossPipe;
  class OpticsHutch;  
  /*!
    \class makeBalder
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class makeBalder
{
 private:

  /// Optics hutch
  std::shared_ptr<OpticsHutch> opticsHut;

  /// Trigger Unit (pipe):
  std::shared_ptr<CrossPipe> triggerPipe;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  
 public:
  
  makeBalder();
  makeBalder(const makeBalder&);
  makeBalder& operator=(const makeBalder&);
  ~makeBalder();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
