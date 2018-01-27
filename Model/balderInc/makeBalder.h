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
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class GateValve;
    
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
  class OpticsHutch;
  class MonoVessel;
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

  /// Real Ion pump (KF40) 10cm vertioal
  std::shared_ptr<constructSystem::CrossPipe> ionPA;

  /// Trigger Unit (pipe):
  std::shared_ptr<constructSystem::CrossPipe> triggerPipe;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::Bellows> pipeA;


  /// Filter box
  std::shared_ptr<constructSystem::VacuumBox> filterBox;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;

  /// CF40 gate valve
  std::shared_ptr<constructSystem::GateValve> gateA;

  /// Vertical mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBox;

  /// Straight value cross piece (ion pump)
  std::shared_ptr<constructSystem::GateValve> gateB;

  /// Joining Bellows from mirror box
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;

  /// Large drift chamber
  std::shared_ptr<constructSystem::VacuumPipe> driftA;

  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftB;

  /// Mono Vessel
  std::shared_ptr<xraySystem::MonoVessel> monoV;
  
  /// Huge Bellows to Mono
  std::shared_ptr<constructSystem::VacuumPipe> monoBellowA;
  /// Huge Bellows from Mono
  std::shared_ptr<constructSystem::VacuumPipe> monoBellowB;
  
  /// Gate valve after mono [large]
  std::shared_ptr<constructSystem::GateValve> gateC;


  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftC;


  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;

  /// Joining Bellows (pipe small):
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;

  /// Straight value cross piece (ion pump)
  std::shared_ptr<constructSystem::CrossPipe> ionPumpC;
  
  /// FocusBox
  std::shared_ptr<constructSystem::VacuumBox> focusBox;

  /// Straight value cross piece (ion pump)
  std::shared_ptr<constructSystem::CrossPipe> ionPumpD;

  /// Joining Bellows (pipe small):
  std::shared_ptr<constructSystem::VacuumPipe> pipeF;

  /// Large drift chamber post mirrorB
  std::shared_ptr<constructSystem::VacuumPipe> driftD;

 public:
  
  makeBalder();
  makeBalder(const makeBalder&);
  makeBalder& operator=(const makeBalder&);
  ~makeBalder();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
