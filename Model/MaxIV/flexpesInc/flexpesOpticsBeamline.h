/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flexpesInc/flexpesOpticsBeamline.h
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
#ifndef xraySystem_flexpesOpticsBeamline_h
#define xraySystem_flexpesOpticsBeamline_h

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
  class GateValveCube;
  class JawValveCube;
}

namespace xraySystem
{
  class BlockStand;
  class OpticsHutch;
  class GrateMonoBox;
  class GratingMono;
  class BeamMount;
  class Mirror;
  class PipeShield;
  class JawValveCube;
  class TwinPipe;
    
  /*!
    \class flexpesOpticsBeamline
    \version 1.0
    \author S. Ansell
    \date February 2019
    \brief General constructor for the xray system
  */

class flexpesOpticsBeamline :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// System for building a divided inner
  attachSystem::InnerZone buildZone;
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Bellows to ionPump
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Real Ion pump (KF40) 24.4cm vertical
  std::shared_ptr<constructSystem::CrossPipe> ionPA;
  /// Gate valve for ring
  std::shared_ptr<constructSystem::GateValveCube> gateRing;
  /// Gate block
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Bellow to first connect line
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// Pipe to some stuff
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// Flor-port ???
  std::shared_ptr<constructSystem::PipeTube> florTubeA;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// Pipe to some stuff
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// Extra screen to make stuff good
  std::shared_ptr<xraySystem::PipeShield> screenExtra;
  /// collimator-port ?
  std::shared_ptr<constructSystem::PipeTube> pumpTubeA;
  /// Front port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeA;
  /// M1 - Mirror box
  std::shared_ptr<constructSystem::PipeTube> M1Tube;
  /// M1 - Mirror
  std::shared_ptr<xraySystem::Mirror> M1Mirror;
  /// Pipe exiting slit section
  std::shared_ptr<xraySystem::BlockStand> M1Stand;
  /// back port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeB;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  /// Large Pipe to slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// Shield between pipes
  std::shared_ptr<xraySystem::PipeShield> screenA;
  /// Small Pipe to slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  /// Small Pipe to slit section
  std::shared_ptr<constructSystem::PipeTube> slitTube;
  /// Jaws for the slit tube (x/z pair)
  std::array<std::shared_ptr<xraySystem::BeamMount>,4> jaws;
  /// Small Pipe to gate-valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateB;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Pipe exiting slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeF;
  /// Mono Box
  std::shared_ptr<xraySystem::GrateMonoBox> monoB;
  /// Mono Crystals
  std::shared_ptr<xraySystem::GratingMono> monoXtal;
  /// Pipe exiting slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeG;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateC;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// Small Pipe to slits on before M3
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// Slits [second pair]
  std::shared_ptr<constructSystem::JawValveCube> slitsB;
  /// Second pumping port
  std::shared_ptr<constructSystem::PipeTube> pumpTubeB;
  /// Front port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeC;
  /// M3 - Mirror box
  std::shared_ptr<constructSystem::PipeTube> M3Tube;
  /// M3 - Mirror
  std::shared_ptr<xraySystem::Mirror> M3Mirror;
  /// back port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeD;
  /// Splitter
  std::shared_ptr<xraySystem::TwinPipe> splitter;
  /// Shield between pipes
  std::shared_ptr<xraySystem::PipeShield> screenB;

  // PIPE A :: LEFT SIDE
  // bellowA on left split
  std::shared_ptr<constructSystem::Bellows> bellowAA;
  /// Gate valve for left
  std::shared_ptr<constructSystem::GateValveCube> gateAA;
  /// Pumping tube
  std::shared_ptr<constructSystem::PortTube> pumpTubeAA;

  // PIPE B :: RIGHT SIDE
  std::shared_ptr<constructSystem::Bellows> bellowBA;
  /// Gate valve for right
  std::shared_ptr<constructSystem::GateValveCube> gateBA;
  /// Pumping tube
  std::shared_ptr<constructSystem::PortTube> pumpTubeBA;

  /// Pipe to exit
  std::shared_ptr<constructSystem::VacuumPipe> outPipeA;
  /// Pipe to exit
  std::shared_ptr<constructSystem::VacuumPipe> outPipeB;

  double outerRadius;           ///< Radius for inner void
  

  int constructDivideCell(Simulation&,const bool,
			   const attachSystem::FixedComp&,
			   const long int,
			   const attachSystem::FixedComp&,
			   const long int);

  int createDoubleVoidUnit(Simulation&,
			   HeadRule&,
			   const attachSystem::FixedComp&,
			   const long int);
  
  void insertFlanges(Simulation&,const constructSystem::PipeTube&);
  

  void buildM1Mirror(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);
  void buildM3Mirror(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);
  void buildMono(Simulation&,MonteCarlo::Object*,
		 const attachSystem::FixedComp&,const long int);
  void buildSlitPackage(Simulation&,MonteCarlo::Object*,
		       const attachSystem::FixedComp&,const long int);
  void buildSplitter(Simulation&,MonteCarlo::Object*,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  flexpesOpticsBeamline(const std::string&);
  flexpesOpticsBeamline(const flexpesOpticsBeamline&);
  flexpesOpticsBeamline& operator=(const flexpesOpticsBeamline&);
  ~flexpesOpticsBeamline();

  void buildOutGoingPipes(Simulation&,const int,const int,
			  const std::vector<int>&);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
