/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/OpticsBeamline.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_OpticsBeamline_h
#define xraySystem_OpticsBeamline_h

namespace insertSystem
{
  class insertPlate;
}

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class portItem;
  class PortTube;
  class GateValve;
  class JawValve;
}

namespace xraySystem
{
  class OpticsHutch;
  class MonoVessel;
  class MonoCrystals;
  class FlangeMount;
  class Mirror;
  class PipeShield;
  class ShutterUnit;
    
  /*!
    \class OpticsBeamline
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class OpticsBeamline :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  attachSystem::InnerZone buildZone;  
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Real Ion pump (KF40) 10cm vertioal
  std::shared_ptr<constructSystem::VacuumPipe> pipeInit;

  /// Real Ion pump (KF40) 10cm vertioal
  std::shared_ptr<constructSystem::CrossPipe> ionPA;

  /// Trigger Unit (pipe):
  std::shared_ptr<constructSystem::CrossPipe> triggerPipe;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::Bellows> pipeA;

  /// Filter box
  std::shared_ptr<constructSystem::PortTube> filterBox;

  /// Filter box
  
  std::array<std::shared_ptr<xraySystem::FlangeMount>,4> filters;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::Bellows> pipeB;

  /// CF40 gate valve
  std::shared_ptr<constructSystem::GateValve> gateA;

  /// Vertical mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBox;

  /// Vertical mirror
  std::shared_ptr<xraySystem::Mirror> mirror;

  /// Straight value cross piece (ion pump)
  std::shared_ptr<constructSystem::GateValve> gateB;

  /// Joining Bellows from mirror box
  std::shared_ptr<constructSystem:: Bellows> pipeC;

  /// Large drift chamber
  std::shared_ptr<constructSystem::VacuumPipe> driftA;

  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftB;

  /// Mono Vessel
  std::shared_ptr<xraySystem::MonoVessel> monoV;
  /// Mono crystal
  std::shared_ptr<xraySystem::MonoCrystals> monoXtal;
  
  /// Huge Bellows to Mono
  std::shared_ptr<constructSystem::Bellows> monoBellowA;
  /// Huge Bellows from Mono
  std::shared_ptr<constructSystem::Bellows> monoBellowB;
  
  /// Gate valve after mono [large]
  std::shared_ptr<constructSystem::GateValve> gateC;

  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftC;

  /// Beam stop
  std::shared_ptr<insertSystem::insertPlate> beamStop;

  /// Slits [first pair]
  std::shared_ptr<constructSystem::JawValve> slitsA;

  /// Tungsten shield pipe
  std::shared_ptr<constructSystem::PortTube> shieldPipe;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> pipeD;

  /// Gate valve after mono [small]
  std::shared_ptr<constructSystem::GateValve> gateD;

  /// Vertical mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxB;

  /// Vertical mirror
  std::shared_ptr<xraySystem::Mirror> mirrorB;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> pipeE;

  /// Slits [first pair]
  std::shared_ptr<constructSystem::JawValve> slitsB;

  /// Pipe for diamond filter
  std::shared_ptr<constructSystem::PortTube> viewPipe;

  /// ViewPipe mounts
  std::array<std::shared_ptr<xraySystem::FlangeMount>,1> viewMount;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> pipeF;

  /// Shutter pipe
  std::shared_ptr<constructSystem::PortTube> shutterPipe;

  /// shutter to stop beam
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterA;

  /// shutter to stop beam [second for redundency
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterB;
    
  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> pipeG;

  /// Last gate valve:
  std::shared_ptr<constructSystem::GateValve> gateE;

  /// Last gate valve:
  std::array<std::shared_ptr<xraySystem::PipeShield>,4> neutShield;

  double outerLeft;    /// Radius for cut rectangle
  double outerRight;   /// Radius for cut rectangle
  double outerTop;     /// Radius for cut rectangle
  

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  OpticsBeamline(const std::string&);
  OpticsBeamline(const OpticsBeamline&);
  OpticsBeamline& operator=(const OpticsBeamline&);
  ~OpticsBeamline();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
