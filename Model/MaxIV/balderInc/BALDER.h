/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/BALDER.h
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
#ifndef xraySystem_BALDER_h
#define xraySystem_BALDER_h

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
  class GateValve;
  class JawValve;
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
  class BeamMount;
  class OpticsHutch;
  class ExperimentalHutch;
  class ExptBeamline;
  class OpticsBeamline;
  class FrontEndCave;
  class ConnectZone;
  class balderFrontEnd;
  class PipeShield;
  class WallLead;
  
  /*!
    \class BALDER
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class BALDER : public attachSystem::CopiedComp
{
 private:

  std::string startPoint;       ///< Start point
  std::string stopPoint;        ///< End point
  /// Front end cave volume
  std::shared_ptr<FrontEndCave> ringCaveA;
  // Joining front cave
  std::shared_ptr<FrontEndCave> ringCaveB;

  /// the components in the front end
  std::shared_ptr<balderFrontEnd> frontBeam;
  std::shared_ptr<WallLead> wallLead;            ///< lead in beam wall
  
  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipe;

  /// Optics hutch
  std::shared_ptr<OpticsHutch> opticsHut;
  
  /// Beamline
  std::shared_ptr<OpticsBeamline> opticsBeam;

  /// Pipe joining optics hut to outer 
  std::shared_ptr<constructSystem::LeadPipe> joinPipeB;


  /// Pipe shield on inner of optics hutch
  std::shared_ptr<xraySystem::PipeShield> pShield;

  /// Neutron shield on inner of optics hutch
  std::shared_ptr<xraySystem::PipeShield> nShield;

  /// Pipe shield on inner of optics hutch
  std::shared_ptr<xraySystem::PipeShield> outerShield;
  
  /// Connection between hutches
  std::shared_ptr<xraySystem::ConnectZone> connectZone;

  /// Pipe joining optics hut to outer 
  std::shared_ptr<constructSystem::LeadPipe> joinPipeC;

  /// Pipe joining optics hut to outer 
  std::shared_ptr<xraySystem::ExperimentalHutch> exptHut;

  /// Beamline for experimental hutch
  std::shared_ptr<ExptBeamline> exptBeam;

  
 public:
  
  BALDER(const std::string&);
  BALDER(const BALDER&);
  BALDER& operator=(const BALDER&);
  ~BALDER();

  void setStopPoint(const std::string& SP)  { stopPoint=SP; }
  void build(Simulation&,const attachSystem::FixedComp&,
	     const long int);

};

}

#endif
