/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimaxInc/softimaxOpticsLine.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef xraySystem_softimaxOpticsLine_h
#define xraySystem_softimaxOpticsLine_h

namespace insertSystem
{
  class insertPlate;
}

namespace constructSystem
{
  class PortTube;
  class BiPortTube;
  class JawValveTube;
  class OffsetFlangePipe;
}

namespace xraySystem
{
  class BlockStand;
  class BremColl;
  class Mirror;
  class BeamPair;
  class TankMonoVessel;
  class GratingUnit;
  class FlangeMount;
  class TwinPipe;
  class SqrCollimator;
  class BremOpticsColl;
  class PipeShield;

  /*!
    \class softimaxOpticsLine
    \version 1.0
    \author K. Batkov
    \date September 2019
    \brief Optics line for the SoftiMAX beam line
  */

class softimaxOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// construction space for main object
  attachSystem::InnerZone buildZone;

  /// string for pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedComp> preInsert;
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  ////// Trigger unit
  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vauucm trigger system
  std::shared_ptr<constructSystem::PipeTube> triggerPipe;
  /// Cylindrical gate valve
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Gate block [item]
  std::shared_ptr<xraySystem::FlangeMount> gateTubeAItem;

  /// bellows after ion pump to filter
  std::shared_ptr<constructSystem::Bellows> bellowA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<constructSystem::PipeTube> pumpM1;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// M1 - Mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> M1TubeFront;
  std::shared_ptr<constructSystem::PipeTube> M1Tube;
  std::shared_ptr<constructSystem::OffsetFlangePipe> M1TubeBack;
  /// M1 - Mirror
  std::shared_ptr<xraySystem::Mirror> M1Mirror;
  std::shared_ptr<xraySystem::BlockStand> M1Stand;
  std::shared_ptr<constructSystem::Bellows> bellowC;
  // Pumping tube for the ion pump
  std::shared_ptr<constructSystem::PipeTube> pumpTubeA;
  /// Tungsten bremsstrahlung collimator
  std::shared_ptr<xraySystem::BremOpticsColl> bremCollA;
  /// gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateB;
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Pipe for slit section
  std::shared_ptr<constructSystem::PortTube> slitTube;
  /// Jaws for the slit tube (x/z pair)
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> jaws;
  /// Monochromator
  std::shared_ptr<xraySystem::TankMonoVessel> monoVessel;
  /// Grating
  std::shared_ptr<xraySystem::GratingUnit> grating;
  std::shared_ptr<xraySystem::FlangeMount> zeroOrderBlock;
  // Gate to isolate mono
  std::shared_ptr<constructSystem::GateValveCube> gateC;
  /// Bellow from gate C
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// first vacuum chamber
  std::shared_ptr<constructSystem::PipeTube> pumpTubeB;
  // Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateD;
  // vaccum pipe
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeA;
  std::shared_ptr<constructSystem::Bellows> bellowF;
  std::shared_ptr<constructSystem::JawValveTube> slitsA;
  std::shared_ptr<constructSystem::PipeTube> pumpTubeM3;
  std::shared_ptr<xraySystem::FlangeMount> pumpTubeM3Baffle;
  // Bellow to mirror M3
  std::shared_ptr<constructSystem::Bellows> bellowG;
  /// M3 Mirror
  std::shared_ptr<constructSystem::VacuumPipe> M3Front;
  std::shared_ptr<constructSystem::PipeTube> M3Tube;
  std::shared_ptr<xraySystem::Mirror> M3Mirror;
  std::shared_ptr<xraySystem::BlockStand> M3Stand;
  std::shared_ptr<constructSystem::VacuumPipe> M3Back;
  std::shared_ptr<constructSystem::Bellows> bellowH;
  // Gate valve for M3
  std::shared_ptr<constructSystem::GateValveCube> gateE;
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeB; // flange converter
  // M3 Middle pump
  std::shared_ptr<constructSystem::PipeTube> pumpTubeC;
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// Vacuum piece
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeC;
  std::shared_ptr<constructSystem::GateValveCube> gateF;
  std::shared_ptr<constructSystem::Bellows> bellowJ;
  /// M3 STXM
  std::shared_ptr<constructSystem::VacuumPipe> M3STXMFront;
  std::shared_ptr<constructSystem::PipeTube>   M3STXMTube;
  /// Splitter
  std::shared_ptr<xraySystem::TwinPipe> splitter;
  std::shared_ptr<constructSystem::Bellows> bellowAA;
  std::shared_ptr<constructSystem::Bellows> bellowBA;
  std::shared_ptr<constructSystem::BiPortTube> M3Pump;

  // Left branch STXM
  std::shared_ptr<constructSystem::Bellows> bellowAB;
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeAA;
  /// box for collimator
  std::shared_ptr<xraySystem::BremOpticsColl> bremCollAA;
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeAB;

  // Right branch CXI
  std::shared_ptr<constructSystem::Bellows> bellowBB;
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeBA;
  /// box for collimator
  std::shared_ptr<xraySystem::BremOpticsColl> bremCollBA;
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeBB;

  /// Shield between pipes
  std::shared_ptr<xraySystem::PipeShield> screenA;
  /// Extran shield to protect outer wall
  std::shared_ptr<insertSystem::insertPlate> lineScreen;
  

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle


  int constructMonoShutter
    (Simulation&,MonteCarlo::Object**,
     const attachSystem::FixedComp&,const long int);

  int constructDiag
    (Simulation&,
     MonteCarlo::Object**,
     constructSystem::PortTube&,
     std::array<std::shared_ptr<constructSystem::JawFlange>,2>&,
     const attachSystem::FixedComp&,
     const long int);

  void buildM1Mirror(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const std::string&);
  void buildM3Mirror(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const std::string&);
  void buildM3STXMMirror(Simulation&,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,const std::string&);
  void constructSlitTube(Simulation&,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,const std::string&);
  void buildMono(Simulation&,MonteCarlo::Object*,
		 const attachSystem::FixedComp&,const long int);
  void buildSplitter(Simulation&,MonteCarlo::Object*,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  softimaxOpticsLine(const std::string&);
  softimaxOpticsLine(const softimaxOpticsLine&);
  softimaxOpticsLine& operator=(const softimaxOpticsLine&);
  ~softimaxOpticsLine();

  void buildOutGoingPipes(Simulation&,const int,const int,
			  const std::vector<int>&);


  void setPreInsert
  (const std::shared_ptr<attachSystem::ContainedComp>& A) { preInsert=A; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
