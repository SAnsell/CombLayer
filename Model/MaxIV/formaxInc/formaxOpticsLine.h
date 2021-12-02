/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/formaxOpticsLine.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef xraySystem_formaxOpticsLine_h
#define xraySystem_formaxOpticsLine_h

namespace insertSystem
{
  class insertPlate;
}

namespace tdcSystem
{
  class YagScreen;
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
  class JawValveCube;
  class JawFlange;
  class DiffPumpXIADP03;
}

namespace xraySystem
{
  class CylGateValve;
  class BremColl;
  class BremBlock;
  class BeamPair;
  class BremMonoColl;
  class DCMTank;
  class FlangeMount;
  class GaugeTube;
  class Mirror;
  class MLMono;
  class MonoBlockXstals;
  class ShutterUnit;
  class SquareFMask;
  class IonGauge;
  class TriggerTube;
  class HPJaws;
  class BremTube;
  class ViewScreenTube;
  class MonoShutter;
  class RoundMonoShutter;
  class PipeShield;
  
    
  /*!
    \class formaxOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2019
    \brief Constructor for the formax optics components
  */

class formaxOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// Items pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;
  /// construction space for main object
  attachSystem::BlockZone buildZone;
  int innerMat;                         ///< inner material if used

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vacuum trigger system
  std::shared_ptr<xraySystem::TriggerTube> triggerPipe;
  /// first ion pump+gate
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;
  /// Addaptor to connect from pump into point to diffuser
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Brem for collimator
  std::shared_ptr<xraySystem::SquareFMask> whiteCollA;
  /// Ion Gauge
  std::shared_ptr<xraySystem::IonGauge> bremHolderA;
  /// Brem collimator
  std::shared_ptr<xraySystem::BremBlock> bremCollA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// mini-Brem Collimator
  std::shared_ptr<constructSystem::VacuumPipe> bremPipeB;

  /// Diagnostic unit 3:
  std::shared_ptr<constructSystem::PortTube> diagBoxA;
  /// Jaws for the slit tube (x/z pair)
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> jaws;

  /// mini pipe out of diag box
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// gate valve
  std::shared_ptr<xraySystem::CylGateValve> gateTubeB;
  /// bellow to to mono
  std::shared_ptr<constructSystem::Bellows> bellowC;

  // PART 4: [MML]
  /// MirrorMonoBox 
  std::shared_ptr<constructSystem::VacuumBox> MLMVessel;
  /// Mirror-Mono system
  std::shared_ptr<xraySystem::MLMono> MLM;

  // PART 5: [connection pipe]

  /// bellow from MLM
  std::shared_ptr<constructSystem::Bellows> bellowD;

  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// gate valve
  std::shared_ptr<xraySystem::CylGateValve> gateTubeC;
  /// bellow to DCM
  std::shared_ptr<constructSystem::Bellows> bellowE;

  /// H-res mono vessel
  std::shared_ptr<xraySystem::DCMTank> monoVessel;
  /// Crystal pair
  std::shared_ptr<xraySystem::MonoBlockXstals> mbXstals;

    /// bellow to diag
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// pipe to gate valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  /// gate valve
  std::shared_ptr<xraySystem::CylGateValve> gateTubeD;

  /// Diag unit two contains Brem block
  std::shared_ptr<xraySystem::BremTube> bremTubeA;
  /// Brem collimator
  std::shared_ptr<xraySystem::BremBlock> bremCollB;
  /// Jaws
  std::shared_ptr<xraySystem::HPJaws> hpJawsA;

  /// Mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxA;
  /// Mirror front
  std::shared_ptr<xraySystem::Mirror> mirrorFrontA;
  /// Mirror back
  std::shared_ptr<xraySystem::Mirror> mirrorBackA;

  /// bellow from Mirror to Diag3
  std::shared_ptr<constructSystem::Bellows> bellowG;
  /// gate valve infront of diag3
  std::shared_ptr<xraySystem::CylGateValve> gateTubeE;
  /// view screen
  std::shared_ptr<xraySystem::ViewScreenTube> viewTube;
  /// yag screen in view screen tube
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;

  /// Diag unit two contains Brem block
  std::shared_ptr<constructSystem::PipeTube> bremTubeB; 
  /// Brem collimator
  std::shared_ptr<xraySystem::BremBlock> bremCollC;
 /// Jaws
  std::shared_ptr<xraySystem::HPJaws> hpJawsB;

  /// bellow out of diag 3 [close]
  std::shared_ptr<constructSystem::Bellows> bellowH;
  /// pipe to exit bellow of diag 3
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;
  /// bellow out of diag 3 [long/final]
  std::shared_ptr<constructSystem::Bellows> bellowI;

  /// gate valve infront of diag3
  std::shared_ptr<xraySystem::CylGateValve> gateTubeF;
  /// view screen
  std::shared_ptr<xraySystem::ViewScreenTube> viewTubeB;
  /// bellow out of diag 3 [long/final]
  std::shared_ptr<constructSystem::Bellows> bellowJ;
  /// adaptor plate from CF63->CF40
  std::shared_ptr<constructSystem::VacuumPipe> monoAdaptorA;
  /// The main mono shutter
  std::shared_ptr<xraySystem::RoundMonoShutter> monoShutter;
  /// adaptor plate from CF63->CF40
  std::shared_ptr<constructSystem::VacuumPipe> monoAdaptorB;
  /// Last gate valve:
  std::shared_ptr<xraySystem::CylGateValve> gateTubeG;
  /// last pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeF;
  
  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructMirrorMono(Simulation&,const attachSystem::FixedComp&,
			   const std::string&);
  
  void constructHDCM(Simulation&,const attachSystem::FixedComp&,
		     const std::string&);

  void constructDiag2(Simulation&,const attachSystem::FixedComp&,
		      const std::string&);

  void constructDiag3(Simulation&,const attachSystem::FixedComp&,
		      const std::string&);

  void constructDiag4(Simulation&,const attachSystem::FixedComp&,
		      const std::string&);

  void constructMonoShutter
  (Simulation&,const attachSystem::FixedComp&,const std::string&);
 
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  formaxOpticsLine(const std::string&);
  formaxOpticsLine(const formaxOpticsLine&);
  formaxOpticsLine& operator=(const formaxOpticsLine&);
  ~formaxOpticsLine();

  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
