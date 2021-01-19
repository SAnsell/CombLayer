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
#ifndef xraySystem_formaxOpticsLineX_h
#define xraySystem_formaxOpticsLineX_h

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
  class SquareFMask;
  class BremMonoColl;
  class FlangeMount;
  class GaugeTube;
  class Mirror;
  class MonoCrystals;
  class MonoBox;
  class MonoShutter;
  class PipeShield;
  class ShutterUnit;
  class IonGauge;
  class TriggerTube;
    
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
  /// Diag Box C :: Jaw units
  std::array<std::shared_ptr<constructSystem::JawFlange>,2> jawCompA;


  
  /*
  /// Filter tube
  std::shared_ptr<constructSystem::PortTube> filterBoxA;
  /// Filter stick [only one blade type -- fix]
  std::shared_ptr<xraySystem::FlangeMount> filterStick;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateB;
  /// bellows after gateA ->view
  std::shared_ptr<constructSystem::Bellows> bellowB;
  
  /// diamond screen(?)
  std::shared_ptr<constructSystem::PipeTube> screenPipeA;
  /// View/something(?)/Ion pump
  std::shared_ptr<constructSystem::PipeTube> screenPipeB;
  /// Addaptor to connect from pumpint point to diffuser
  std::shared_ptr<constructSystem::VacuumPipe> adaptorPlateA;
  /// Diffusion pump
  std::shared_ptr<constructSystem::DiffPumpXIADP03> diffPumpA;
  
  /// Primary jaw (Box)
  std::shared_ptr<constructSystem::VacuumBox> primeJawBox;
  /// Bellow to gate on mono
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateC;
  /// Mono box
  std::shared_ptr<xraySystem::MonoBox> monoBox;
  /// Mono Xstal 
  std::shared_ptr<xraySystem::MonoCrystals> monoXtal;
  // Gate to isolate mono
  std::shared_ptr<constructSystem::GateValveCube> gateD;
  /// Bellow to diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Diagnostic unit 1:
  std::shared_ptr<constructSystem::PortTube> diagBoxA;
  /// Mono-Collimator for collimator
  std::shared_ptr<xraySystem::BremMonoColl> bremMonoCollA;
  /// Bellow from diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowE;
  // Gate for first mirror
  std::shared_ptr<constructSystem::GateValveCube> gateE;

  /// Mirror box 
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxA;
  /// Mirror front
  std::shared_ptr<xraySystem::Mirror> mirrorFrontA;
  /// Mirror back
  std::shared_ptr<xraySystem::Mirror> mirrorBackA;
  // Gate fro first mirror
  std::shared_ptr<constructSystem::GateValveCube> gateF;
  /// Bellow to diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// Diagnostic unit 2:
  std::shared_ptr<constructSystem::PortTube> diagBoxB;
  /// Diag Box B :: Jaw units
  std::array<std::shared_ptr<constructSystem::JawFlange>,2> jawCompB;

  /// Bellow to mirror B
  std::shared_ptr<constructSystem::Bellows> bellowG;
  // Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateG;
  /// Mirror box B
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxB;
  /// Mirror Front B
  std::shared_ptr<xraySystem::Mirror> mirrorFrontB;
  /// Mirror Back B
  std::shared_ptr<xraySystem::Mirror> mirrorBackB;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateH;
  /// Bellow to mirror B
  std::shared_ptr<constructSystem::Bellows> bellowH;


  /// Bellow to end station
  std::shared_ptr<constructSystem::Bellows> bellowI;

  /// Gate valve before mono shutter
  std::shared_ptr<constructSystem::GateValveCube> gateI;

  /// Shutter pipe
  std::shared_ptr<xraySystem::MonoShutter> monoShutter;
    
  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> bellowJ;

  /// Last gate valve:
  std::shared_ptr<constructSystem::GateValveCube> gateJ;
  */
  
  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

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
