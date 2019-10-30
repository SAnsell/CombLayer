/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimaxInc/softimaxOpticsLine.h
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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

namespace constructSystem
{
  class PortTube;
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

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  ////// Trigger unit
  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vauucm trigger system
  std::shared_ptr<constructSystem::CrossPipe> triggerPipe;
  /// first ion pump
  std::shared_ptr<constructSystem::CrossPipe> gaugeA;

  /// bellows after ion pump to filter
  std::shared_ptr<constructSystem::Bellows> bellowA;
  std::shared_ptr<constructSystem::PipeTube> pumpM1;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// M1 - Mirror box
  std::shared_ptr<constructSystem::PipeTube> M1Tube;
  /// M1 - Mirror
  std::shared_ptr<xraySystem::Mirror> M1Mirror;
  std::shared_ptr<xraySystem::BlockStand> M1Stand;
  std::shared_ptr<constructSystem::Bellows> bellowC;
  // Pumping tube for the ion pump
  std::shared_ptr<constructSystem::PipeTube> pumpTubeA;
  /// Tungsten bremsstrahlung collimator
  std::shared_ptr<xraySystem::BremColl> bremCollA;
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
  std::shared_ptr<constructSystem::PipeTube> pumpTubeM3;
  std::shared_ptr<xraySystem::FlangeMount> pumpTubeM3Baffle;
  // Bellow to mirror M3
  std::shared_ptr<constructSystem::Bellows> bellowG;
  /// M3 Mirror
  std::shared_ptr<constructSystem::PipeTube> M3Tube;
  std::shared_ptr<xraySystem::Mirror> M3Mirror;
  std::shared_ptr<xraySystem::BlockStand> M3Stand;
  std::shared_ptr<constructSystem::Bellows> bellowH;
  // Gate valve for M3
  std::shared_ptr<constructSystem::GateValveCube> gateE;
  // M3 Middle pump
  std::shared_ptr<constructSystem::PipeTube> pumpTubeC;
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// Vacuum piece
  std::shared_ptr<constructSystem::PipeTube> vacPiece;
  std::shared_ptr<constructSystem::GateValveCube> gateF;



  // /// Vacuum pipe for collimator
  // std::shared_ptr<xraySystem::BremColl> bremCollA;
  // /// Filter tube
  // std::shared_ptr<constructSystem::PortTube> filterBoxA;
  // /// Filter stick [only one blade type -- fix]
  // std::shared_ptr<xraySystem::FlangeMount> filterStick;
  // /// diamond screen(?)
  // std::shared_ptr<constructSystem::PipeTube> screenPipeA;
  // /// View/something(?)/Ion pump
  // std::shared_ptr<constructSystem::PipeTube> screenPipeB;
  // /// Addaptor to connect from pumpint point to diffuser
  // std::shared_ptr<constructSystem::VacuumPipe> adaptorPlateA;
  // /// Diffusion pump
  // std::shared_ptr<constructSystem::DiffPumpXIADP03> diffPumpA;

  // /// Primary jaw (Box)
  // std::shared_ptr<constructSystem::VacuumBox> primeJawBox;
  // /// Bellow to gate on mono
  // std::shared_ptr<constructSystem::Bellows> bellowC;
  // /// Mono box
  // std::shared_ptr<xraySystem::MonoBox> monoBox;
  // /// Mono Xstal
  // std::shared_ptr<xraySystem::MonoCrystals> monoXtal;
  // /// Bellow to diagnositics
  // std::shared_ptr<constructSystem::Bellows> bellowD;
  // /// Diagnostic unit 1:
  // std::shared_ptr<constructSystem::PortTube> diagBoxA;
  // /// Mono-Collimator for collimator
  // std::shared_ptr<xraySystem::BremMonoColl> bremMonoCollA;

  // /// Mirror box
  // std::shared_ptr<constructSystem::VacuumBox> mirrorBoxA;
  // /// Mirror front
  // std::shared_ptr<xraySystem::Mirror> mirrorFrontA;
  // /// Mirror back
  // std::shared_ptr<xraySystem::Mirror> mirrorBackA;
  // /// Bellow to diagnositics
  // std::shared_ptr<constructSystem::Bellows> bellowF;
  // /// Diagnostic unit 2:
  // std::shared_ptr<constructSystem::PortTube> diagBoxB;
  // /// Diag Box B :: Jaw units
  // std::array<std::shared_ptr<constructSystem::JawFlange>,2> jawCompB;

  // // Gate valve
  // std::shared_ptr<constructSystem::GateValveCube> gateG;
  // /// Mirror box B
  // std::shared_ptr<constructSystem::VacuumBox> mirrorBoxB;
  // /// Mirror Front B
  // std::shared_ptr<xraySystem::Mirror> mirrorFrontB;
  // /// Mirror Back B
  // std::shared_ptr<xraySystem::Mirror> mirrorBackB;
  // /// Gate valve
  // std::shared_ptr<constructSystem::GateValveCube> gateH;
  // /// Bellow to mirror B
  // std::shared_ptr<constructSystem::Bellows> bellowH;

  // /// Diagnostic unit 3:
  // std::shared_ptr<constructSystem::PortTube> diagBoxC;
  // /// Diag Box C :: Jaw units
  // std::array<std::shared_ptr<constructSystem::JawFlange>,2> jawCompC;

  // /// Gate valve before mono shutter
  // std::shared_ptr<constructSystem::GateValveCube> gateI;

  // /// Shutter pipe
  // std::shared_ptr<xraySystem::MonoShutter> monoShutter;

  // /// Joining Bellows (pipe large):
  // std::shared_ptr<constructSystem::Bellows> bellowJ;

  // /// Last gate valve:
  // std::shared_ptr<constructSystem::GateValveCube> gateJ;

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
		     const attachSystem::FixedComp&,const long int);
  void buildM3Mirror(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);
  void constructSlitTube(Simulation&,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,const std::string&);
  void buildMono(Simulation&,MonteCarlo::Object*,
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

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
