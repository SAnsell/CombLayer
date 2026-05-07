/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   danmaxInc/danmaxOpticsLine.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell, Konstantin Batkov and Udo Friman-Gayer
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
#ifndef xraySystem_danmaxOpticsLine_h
#define xraySystem_danmaxOpticsLine_h

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
  class PipeTube;
  class GateValveCube;
  class GateValveCylinder;
  class JawValveCube;
  class JawValveTube;
  class JawFlange;
  class FlangePlate;
}

namespace xraySystem
{
  class OpticsHutch;
  class BeamPair;
  class BremBlock;
  class BremMonoColl;
  class FlangeMount;
  class Mirror;
  class MonoBlockXstals;
  class MonoShutterR3;
  class MonoSlitsJJ;
  class DCMTank;
  class MLMono;
  class MLMVessel;
  class MonoBox;
  class SmallAngleBellows;
  class TriggerTube;
  class TwinPipe;
  class WhiteBeamStop;
  class CardanBellowsSINCRYS;
  class CM1BeamSplitter;
  class CM2Crystal;
  class CM2Vessel;

/*!
  \class danmaxOpticsLine
  \brief DanMAX optics line
  \version 2.0
  \date February 2026
  \author S. Ansell, K. Batkov, U. Friman-Gayer
*/

class danmaxOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  /// Pointer to front end for use in absolute positioning.
  std::shared_ptr<attachSystem::FixedComp> frontEnd;
  /// Object that needs to be inserted into the first build zone of the optics line.
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;
  /// construction space for main object
  attachSystem::BlockZone buildZone;           ///< Common section before divider
  attachSystem::BlockZone buildZoneSinCrys;    ///< SinCrys section after divider
  attachSystem::BlockZone buildZoneDanMAX;     ///< DanMAX section after divider

  int innerMat;                         ///< inner material if used

  /// Shared points to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;
  std::shared_ptr<attachSystem::FixedComp> lastCompSINCRYS;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vacuum trigger system
  std::shared_ptr<xraySystem::TriggerTube> triggerPipe;
  /// First gate valve in Optics Hutch (common branch)
  std::shared_ptr<xraySystem::CylGateValve> valve4;
  /// bellows from trigger outward
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Bremsstrahlung Collimator 1 (tube and collimator)
  std::shared_ptr<constructSystem::PipeTube> bc1Tube;
  std::shared_ptr<xraySystem::BremBlock> bc1;
  /// High Pass Filter
  std::shared_ptr<constructSystem::VacuumPipe> highPassFilter;
  /// Second gate valve in Optics Hutch (common branch)
  std::shared_ptr<xraySystem::CylGateValve> valve5;
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<constructSystem::PipeTube> cm1;
  std::shared_ptr<xraySystem::CM1BeamSplitter> cm1BeamSplitter;
  // First gate valve in SINCRYS (S) branch
  std::shared_ptr<xraySystem::CylGateValve> valveS1;
  std::shared_ptr<constructSystem::FlangePlate> diamondWindow;
  std::shared_ptr<constructSystem::PipeTube> beamViewerS1;
  std::shared_ptr<xraySystem::FlangeMount> beamViewerS1Screen;
  std::shared_ptr<xraySystem::CardanBellowsSINCRYS> cardanBellowsUpstream;
  std::shared_ptr<constructSystem::Bellows> bellowBA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeSinCrys;
  std::shared_ptr<constructSystem::Bellows> linearlyGuidedBellowsUpstream;
  std::shared_ptr<xraySystem::CardanBellowsSINCRYS> cardanBellowsDownstream;
  std::shared_ptr<xraySystem::CM2Vessel> cm2;
  std::shared_ptr<xraySystem::CM2Crystal> cm2Crystal;
  std::shared_ptr<xraySystem::FlangeMount> beamViewerS2Screen;
  std::shared_ptr<constructSystem::GateValveCube> valveS2;
  std::shared_ptr<constructSystem::Bellows> cardanBellowsCM2;
  std::shared_ptr<constructSystem::Bellows> linearlyGuidedBellowsDownstream;
  std::shared_ptr<constructSystem::VacuumPipe> transportPipe2;
  std::shared_ptr<constructSystem::Bellows> cardanBellowsTransfocator;
  std::shared_ptr<xraySystem::MonoBox> transfocator;
  std::shared_ptr<constructSystem::VacuumPipe> transfocatorToSlitsPipe1;
  std::shared_ptr<constructSystem::Bellows> transfocatorToSlitsBellows;
  std::shared_ptr<constructSystem::VacuumPipe> transfocatorToSlitsPipe2;
  std::shared_ptr<xraySystem::MonoSlitsJJ> slitTubeS;
  std::shared_ptr<constructSystem::PipeTube> beamViewerS3;
  std::shared_ptr<xraySystem::FlangeMount> beamViewerS3Screen;
  std::shared_ptr<constructSystem::GateValveCube> valveS3;
  std::shared_ptr<constructSystem::Bellows> slitsToShutterBellows;
  std::shared_ptr<xraySystem::MonoShutterR3> monoShutterS;
  std::shared_ptr<constructSystem::Bellows> monoShutterSExitBellows;

  /// bellows to laue mono:
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// Pipe for laue mono
  std::shared_ptr<constructSystem::VacuumPipe> lauePipe;
  /// bellows from laue mono
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Pipe for slit section
  std::shared_ptr<constructSystem::PortTube> slitTube;
  /// Jaws for the slit tube (x/z pair)
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> jaws;
  // Gate after slittube
  std::shared_ptr<xraySystem::CylGateValve> valve6;
  /// bellows to laue mono:
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// hDCM vessel
  std::shared_ptr<xraySystem::DCMTank> hdcmVessel;
  /// Grating
  std::shared_ptr<xraySystem::MonoBlockXstals> mbXstals;
  /// Bellows between monochromator and Valve7
  std::shared_ptr<constructSystem::Bellows> bellowAfterHDCM;
  /// Gate value BL-V3
  std::shared_ptr<xraySystem::CylGateValve> valve7;
  /// Beam viewer 2
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// Beam viewer [item]
  std::shared_ptr<xraySystem::FlangeMount> viewTubeScreen;
  /// Gate value BL-V4
  std::shared_ptr<xraySystem::CylGateValve> valve8;
  /// bellows to second mono
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// MirrorMonoBox
  std::shared_ptr<constructSystem::VacuumBox> mlmVessel;
  /// Mirror-Mono system
  std::shared_ptr<xraySystem::MLMono> MLM;
  /// bellows from second mono
  std::shared_ptr<constructSystem::Bellows> bellowG;
  /// Gate value BL-V5
  std::shared_ptr<xraySystem::CylGateValve> valve9;
  // Connector from CF40 to larger pipe
  std::shared_ptr<constructSystem::VacuumPipe> bc2InPipe;
  // Short section that contains a port for the White Beam Stop
  std::shared_ptr<constructSystem::PipeTube> wbsSection;
  std::shared_ptr<xraySystem::WhiteBeamStop> wbs;
  /// BeamStopTube
  std::shared_ptr<constructSystem::PipeTube> BSCollTube;
  /// Bremsstrahlung collimator
  std::shared_ptr<xraySystem::BremBlock> bc2;
  // Connector from BS collimator tube to slit tube
  std::shared_ptr<constructSystem::VacuumPipe> bc2OutPipe;
  /// Slits after beamstop
  std::shared_ptr<constructSystem::PipeTube> monoSlitsTube;
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> monoSlits;
  /// Flange converter
  std::shared_ptr<constructSystem::FlangePlate> slitsAOut;
  /// Connector to next beam view
  std::shared_ptr<constructSystem::Bellows> bellowH;
  /// Beam viewer 2
  std::shared_ptr<constructSystem::PortTube> viewTubeB;
  /// Beam viewer [item]
  std::shared_ptr<xraySystem::FlangeMount> viewTubeBScreen;
  /// Connector to CRL
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// CRL with gate valves at front and back
  std::shared_ptr<constructSystem::GateValveCylinder> CRLGateIn;
  std::shared_ptr<xraySystem::MonoBox> lensBox;
  std::shared_ptr<constructSystem::GateValveCylinder> CRLGateOut;

  /// Connector to reverse beamstop
  std::shared_ptr<constructSystem::Bellows> bellowJ;

  std::shared_ptr<constructSystem::FlangePlate> revMonoSlitsIn;
  std::shared_ptr<constructSystem::PipeTube> revMonoSlitsTube;
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> revMonoSlits;
  std::shared_ptr<constructSystem::PipeTube> bc3Tube;
  std::shared_ptr<xraySystem::BremBlock> bc3;

  // /// Flange convert
  // std::shared_ptr<constructSystem::VacuumPipe> slitsBOut;

  /// bellows from second mono
  std::shared_ptr<constructSystem::Bellows> bellowK;
  /// The main mono shutter
  std::shared_ptr<xraySystem::MonoShutterR3> monoShutter;
  /// Bellows from Mono Shutter
  std::shared_ptr<constructSystem::Bellows> bellowL;


  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructBC1Tube(Simulation&,
			 const attachSystem::FixedComp&,const std::string&);

  void constructSlitTube(Simulation&,
			 const attachSystem::FixedComp&,const std::string&);

  void constructHDCM(Simulation&,
		     const attachSystem::FixedComp&,const std::string&);

  void constructMirrorMono(Simulation&,
			   const attachSystem::FixedComp&,const std::string&);

  void constructViewScreen(Simulation&,
			const attachSystem::FixedComp&,const std::string&);

  void constructViewScreenB(Simulation&,
			const attachSystem::FixedComp&,const std::string&);

  void constructBeamStopTube(Simulation&,
			     const attachSystem::FixedComp&,const std::string&);

  void constructBC3Tube(Simulation&,
				const attachSystem::FixedComp&,
				const std::string&);

  void constructMonoShutter(Simulation&,
			    const attachSystem::FixedComp&,
			    const std::string&);

  void buildSplitter(Simulation&,const attachSystem::FixedComp&,const std::string&);


  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void buildObjects(Simulation&);

  void createLinks();

 public:

  danmaxOpticsLine(const std::string&);
  danmaxOpticsLine(const danmaxOpticsLine&);
  danmaxOpticsLine& operator=(const danmaxOpticsLine&);
  ~danmaxOpticsLine() override;

  /// Assignment to inner void
  void setInnerMat(const int M) { innerMat=M; }
  void setFrontEnd(const std::shared_ptr<attachSystem::FixedComp>& FE){
    frontEnd = FE;
  }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  std::shared_ptr<attachSystem::FixedComp> getLastCompSINCRYS(){
    return lastCompSINCRYS;
  }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
