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

  /// string for pre-insertion into mastercell:0
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
  /// first ion pump
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;

  /// Addaptor to connect from pumpint point to diffuser
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Vacuum pipe for collimator
  std::shared_ptr<xraySystem::SquareFMask> bremCollA;
  /// Ion Gauge
  std::shared_ptr<xraySystem::IonGauge> ionGaugeA;
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
  /// Jaws
  std::shared_ptr<xraySystem::HPJaws> hpJawsA;

  /// Mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxA;
  /// Mirror front
  std::shared_ptr<xraySystem::Mirror> mirrorFrontA;
  /// Mirror back
  std::shared_ptr<xraySystem::Mirror> mirrorBackA;

  /// bellow from Mirror to Diag3
  std::shared_ptr<constructSystem::Bellows> bellowF;

  /// Last gate valve:
  std::shared_ptr<constructSystem::GateValveCube> gateJ;

  
  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructMirrorMono(Simulation&,const attachSystem::FixedComp&,
			   const std::string&);
  
  void constructHDCM(Simulation&,const attachSystem::FixedComp&,
		     const std::string&);

  void constructDiag2(Simulation&,const attachSystem::FixedComp&,
		      const std::string&);

  int constructMonoShutter
    (Simulation&,const attachSystem::FixedComp&,const long int);

  
  int constructDiag
    (Simulation&,
     constructSystem::PortTube&,
     std::array<std::shared_ptr<constructSystem::JawFlange>,2>&,
     const attachSystem::FixedComp&,
     const long int);

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
  void setInnerMat(const int M) { innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
