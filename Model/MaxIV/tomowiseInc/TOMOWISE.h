/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   tomowiseInc/TOMOWISE.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_TOMOWISE_h
#define xraySystem_TOMOWISE_h

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
  class ExperimentalHutch;
  class ExptBeamline;
  class tomowiseDetectorTube;
  class tomowiseExptLine;
  class tomowiseOpticsLine;
  class R3FrontEndCave;
  class ConnectZone;
  class tomowiseFrontEnd;
  class PipeShield;
  class WallLead;

  /*!
    \class TOMOWISE
    \version 1.0
    \author K. Batkov
    \date February 2015
    \brief TomoWISE beamline
  */

class TOMOWISE : public R3Beamline
{
 private:

  /// the components in the front end
  std::shared_ptr<tomowiseFrontEnd> frontBeam;

  /// lead in beam wall
  std::shared_ptr<WallLead> wallLead;

  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipe;

  /// Optics hutch
  std::shared_ptr<OpticsHutch> opticsHut;

  /// Beamline
  std::shared_ptr<tomowiseOpticsLine> opticsBeam;

  /// Expt hutch A
  std::shared_ptr<ExperimentalHutch> exptHut;

  /// Pipe joining frontend to Expt hutch
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeB;

  /// Shield at exit of optics hutch
  std::shared_ptr<xraySystem::PipeShield> pShield;

  /// Beamline expt
  std::shared_ptr<tomowiseExptLine> exptBeam;

  /// Expt hutch B
  std::shared_ptr<ExperimentalHutch> exptHutB;

  /// Pipe joining Expt Hutch to Expt Hutch B
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeC;

 public:

  TOMOWISE(const std::string&);
  TOMOWISE(const TOMOWISE&);
  TOMOWISE& operator=(const TOMOWISE&);
  ~TOMOWISE() override;


  void build(Simulation&,const attachSystem::FixedComp&,
	     const long int) override;

};

}

#endif
