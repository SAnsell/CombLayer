/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/maxpeemOpticsLine.h
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
#ifndef xraySystem_maxpeemOpticsLine_h
#define xraySystem_maxpeemOpticsLine_h

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
  class BlockStand;
  class CrossWayTube;
  class GrateMonoBox;
  class GratingMono;
  class JawValveCube;
  class Mirror;
  class OpticsHutch;
  class PipeShield;
  class TriggerTube;
  class TwinPipe;

    
  /*!
    \class maxpeemOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class maxpeemOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  /// Items pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;

  /// System for building a divided inner
  attachSystem::BlockZone buildZone;  
  attachSystem::BlockZone IZLeft;       ///< Left section after divider
  attachSystem::BlockZone IZRight;      ///< Right section after divider
  int innerMat;                         ///< inner material if used
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Bellows to ionPump
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// vacuum trigger system
  std::shared_ptr<xraySystem::TriggerTube> triggerPipe;
  /// first ion pump+gate
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;
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

  double outerLeft;           ///< Radius for inner void
  double outerRight;           ///< Radius for inner void
  

  
  void insertFlanges(Simulation&,const constructSystem::PipeTube&);
  

  void buildM1Mirror
  (Simulation&,const attachSystem::FixedComp&,const std::string&);
  void buildM3Mirror
  (Simulation&, const attachSystem::FixedComp&,const std::string&);
  void buildMono
  (Simulation&,const attachSystem::FixedComp&,const std::string&);
  void buildSlitPackage
  (Simulation&,const attachSystem::FixedComp&,const std::string&);
  void buildSplitter
  (Simulation&,const attachSystem::FixedComp&,const std::string&);
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  maxpeemOpticsLine(const std::string&);
  maxpeemOpticsLine(const maxpeemOpticsLine&);
  maxpeemOpticsLine& operator=(const maxpeemOpticsLine&);
  ~maxpeemOpticsLine();

  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  void buildExtras(Simulation&,const attachSystem::CellMap&);
  
  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
