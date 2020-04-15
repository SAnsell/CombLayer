/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxOpticsLine.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
  class BeamPair;
  class BremBlock;
  class BremMonoColl;
  class FlangeMount;
  class Mirror;
  class MonoBlockXstals;
  class MonoShutter;
  class DCMTank;
  class MLMono;
  class MLMVessel;
  class MonoBox;

  
  /*!
    \class danmaxOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class danmaxOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// string for pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedComp> preInsert;
  /// construction space for main object
  attachSystem::InnerZone buildZone;
  int innerMat;                         ///< inner material if used

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vacuum trigger system
  std::shared_ptr<constructSystem::PipeTube> triggerPipe;
  /// first ion pump
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Gate block [item]
  std::shared_ptr<xraySystem::FlangeMount> gateTubeAItem;

  /// bellows from trigger outward
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Mid trigger -> collimator
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// bellows to collimator
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// Collimator zone
  std::shared_ptr<constructSystem::PipeTube> collTubeA;
  /// Brem Collimator
  std::shared_ptr<xraySystem::BremBlock> bremColl;
  /// Filter unit pipe
  std::shared_ptr<constructSystem::VacuumPipe> filterPipe;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCylinder> gateA;
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
  std::shared_ptr<constructSystem::GateValveCylinder> gateB;
  /// bellows to laue mono:
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// Main mono vessel
  std::shared_ptr<xraySystem::DCMTank> monoVessel;
  /// Grating
  std::shared_ptr<xraySystem::MonoBlockXstals> mbXstals;
  /// Gate value BL-V3
  std::shared_ptr<constructSystem::GateValveCylinder> gateC;
  /// Beam viewer 2
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// Beam viewer [item]
  std::shared_ptr<xraySystem::FlangeMount> viewTubeScreen;
  /// Gate value BL-V4
  std::shared_ptr<constructSystem::GateValveCylinder> gateD;
  /// bellows to second mono
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// MirrorMonoBox
  std::shared_ptr<constructSystem::VacuumBox> MLMVessel;
  /// Mirror-Mono system
  std::shared_ptr<xraySystem::MLMono> MLM;
  /// bellows from second mono
  std::shared_ptr<constructSystem::Bellows> bellowG;  
  /// Gate value BL-V5
  std::shared_ptr<constructSystem::GateValveCylinder> gateE;
  /// BeamStopTube
  std::shared_ptr<constructSystem::PipeTube> beamStopTube;
  /// BeamStopBlock
  std::shared_ptr<xraySystem::BremBlock> beamStop;
  /// Slits after beamstop
  std::shared_ptr<constructSystem::JawValveTube> slitsA;
  /// Flange converter
  std::shared_ptr<constructSystem::VacuumPipe> slitsAOut;
  /// Connector to next beam view
  std::shared_ptr<constructSystem::Bellows> bellowH;
  /// Beam viewer 2
  std::shared_ptr<constructSystem::PortTube> viewTubeB;
  /// Beam viewer [item]
  std::shared_ptr<xraySystem::FlangeMount> viewTubeBScreen;
  /// Connector to next beam view
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// Connector to next beam view
  std::shared_ptr<xraySystem::MonoBox> lensBox;

  /// gate valve from focus box
  std::shared_ptr<constructSystem::GateValveCylinder> gateF;
  /// Connector to review beamstop
  std::shared_ptr<constructSystem::Bellows> bellowJ;

  /// Flange convert
  std::shared_ptr<constructSystem::VacuumPipe> slitsBOut;
  
  /// BeamStopTube
  std::shared_ptr<constructSystem::PipeTube> revBeamStopTube;
  /// BeamStopBlock
  std::shared_ptr<xraySystem::BremBlock> revBeamStop;
  /// Slits after beamstop
  std::shared_ptr<constructSystem::JawValveTube> slitsB;
  /// bellows from second mono
  std::shared_ptr<constructSystem::Bellows> bellowK;  
  /// adaptor plate from CF63->CF40
  std::shared_ptr<constructSystem::VacuumPipe> monoAdaptorA;
  /// The main mono shutter
  std::shared_ptr<xraySystem::MonoShutter> monoShutter;
  /// adaptor plate from CF63->CF40
  std::shared_ptr<constructSystem::VacuumPipe> monoAdaptorB;
  /// bellows from second mono
  std::shared_ptr<constructSystem::GateValveCylinder> gateG;  

  

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle


  void constructSlitTube(Simulation&,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,const std::string&);

  void constructMono(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const std::string&);

  void constructMirrorMono(Simulation&,MonteCarlo::Object*,
			   const attachSystem::FixedComp&,const std::string&);

  void constructViewScreen(Simulation&,MonteCarlo::Object*,
			const attachSystem::FixedComp&,const std::string&);

  void constructViewScreenB(Simulation&,MonteCarlo::Object*,
			const attachSystem::FixedComp&,const std::string&);

  void constructBeamStopTube(Simulation&,MonteCarlo::Object*,
			     const attachSystem::FixedComp&,const std::string&);

  void constructRevBeamStopTube(Simulation&,MonteCarlo::Object*,
				const attachSystem::FixedComp&,
				const std::string&);

  void constructMonoShutter(Simulation&,MonteCarlo::Object*,
			    const attachSystem::FixedComp&,
			    const std::string&);

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);

  void createLinks();
  
 public:
  
  danmaxOpticsLine(const std::string&);
  danmaxOpticsLine(const danmaxOpticsLine&);
  danmaxOpticsLine& operator=(const danmaxOpticsLine&);
  ~danmaxOpticsLine();

  /// Assignment to inner void
  void setInnerMat(const int M) { innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedComp>& A) { preInsert=A; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
