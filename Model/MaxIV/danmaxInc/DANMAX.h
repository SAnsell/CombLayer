/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/DANMAX.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_DANMAX_h
#define xraySystem_DANMAX_h

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class LeadPipe;
  class VacuumBox;
  class portItem;
  class PortTube;
  class GateValveCube;
  class JawValveCube;
}

namespace xraySystem
{
  class BeamMount;
  class OpticsHutch;
  class danmaxOpticsHutch;
  class ExperimentalHutch;
  class balderExptLine;
  class R3FrontEndCave;
  class R3FrontEnd;
  class SqrShield;
  class balderFrontEnd;
  class PipeShield;
  class WallLead;

  class danmaxConnectLine;
  class danmaxOpticsLine;
  class danmaxFrontEnd;

  /*!
    \class DANMAX
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class DANMAX : public R3Beamline
{
 private:

  /// the components in the front end
  std::shared_ptr<danmaxFrontEnd> frontBeam;

  /// lead in beam wall
  std::shared_ptr<WallLead> wallLead;           
  
  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipe;

  /// Optics hutch
  std::shared_ptr<danmaxOpticsHutch> opticsHut;
  
  /// Beamline
  std::shared_ptr<danmaxOpticsLine> opticsBeam;

    /// Pipe joining optics hut to outer 
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeB;
  
  /// Pipe joining optics hut to outer 
  std::shared_ptr<xraySystem::danmaxConnectLine> connectUnit;

  /// Pipe joining connecting unit to expt.
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeC;

  /// Pipe joining optics hut to outer 
  std::shared_ptr<xraySystem::ExperimentalHutch> exptHut;

  /// Shielding block guilatine
  std::shared_ptr<xraySystem::PipeShield> pShield;

  /// Beamline for experimental hutch
  std::shared_ptr<balderExptLine> exptBeam;

  
 public:
  
  DANMAX(const std::string&);
  DANMAX(const DANMAX&);
  DANMAX& operator=(const DANMAX&);
  virtual ~DANMAX();

  virtual  void build(Simulation&,const attachSystem::FixedComp&,
		      const long int);

};

}

#endif
