/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3CommonInc/R3FrontEndToyamaDanMAX.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell / Konstantin Batkov
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
#ifndef xraySystem_R3FrontEndToyamaDanMAX_h
#define xraySystem_R3FrontEndToyamaDanMAX_h

namespace insertSystem
{
  class insertPlate;
  class insertCylinder;
}

namespace constructSystem
{
  class Bellows;
  class CornerPipe;
  class CrossPipe;
  class GateValveCube;
  class OffsetFlangePipe;
  class portItem;
  class PipeTube;
  class PortTube;
  class SupplyPipe;
  class VacuumBox;
  class VacuumPipe;
  class FlangePlate;
}

namespace xraySystem
{
  class BremBlock;
  class HeatAbsorberR3Toyama;
  class ProximityShielding;
  class CylGateValve;
  class HeatDump;
  class LCollimator;
  class SqrCollimator;
  class SquareFMask;
  class BeamMount;
  class PreDipole;
  class MagnetM1;
  class MagnetU1;

  class PreBendPipe;
  class EPSeparator;
  class EPCombine;
  class R3ChokeChamber;
  class R3ChokeInsert;
  class BladeBPMToyama;
  class FixedMaskHybrid;

  /*!
    \class R3FrontEndToyamaDanMAX
    \version 1.0
    \author K. Batkov
    \date January 2026
    \brief DanMAX-like Toyama frontend (TODO later: use appropriate name and inherit from general Toyama front-end R3FrontEndToyama (it's currently the TomoWISE front-end))
  */

class R3FrontEndToyamaDanMAX :
  public R3FrontEnd
{
protected:
  std::shared_ptr<xraySystem::FixedMaskHybrid> fm1;
  std::shared_ptr<constructSystem::VacuumPipe> beamPipe1;
  std::shared_ptr<constructSystem::PipeTube> beamPipe2;
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateAA;
  std::shared_ptr<xraySystem::BladeBPMToyama> xbpm1;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateXBPM2;
  std::shared_ptr<xraySystem::BladeBPMToyama> xbpm2;
  std::shared_ptr<constructSystem::VacuumPipe> pipePump2;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateA;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateB;
  std::shared_ptr<constructSystem::Bellows> bellowPreMSM;
  std::shared_ptr<xraySystem::MovableSafetyMask> msm;
  std::shared_ptr<constructSystem::Bellows> bellowPostMSM;
  std::shared_ptr<constructSystem::VacuumPipe> msmExitPipe;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateC;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateD;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateE;
  std::shared_ptr<constructSystem::Bellows> bellowPreHA;
  std::shared_ptr<xraySystem::HeatAbsorberR3Toyama> ha;
  std::shared_ptr<constructSystem::Bellows> bellowPostHA;
  std::shared_ptr<constructSystem::CrossPipe> ionPump3;
  std::shared_ptr<xraySystem::CylGateValve> valve2;
  std::shared_ptr<constructSystem::Bellows> bellowDA;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateF;
  std::shared_ptr<constructSystem::FlangePlate> flangePlateG;

  // Bremsstrahlung collimator
  std::shared_ptr<xraySystem::BremBlock> bremColl;
  std::shared_ptr<constructSystem::VacuumPipe> bremCollPipe;

  std::shared_ptr<xraySystem::ProximityShielding> proxiShieldA; // proximity shielding
  std::shared_ptr<constructSystem::VacuumPipe> proxiShieldAPipe; // pipe inside proxiShieldA
  std::shared_ptr<xraySystem::ProximityShielding> proxiShieldB; // proximity shielding
  std::shared_ptr<constructSystem::VacuumPipe> proxiShieldBPipe; // pipe inside proxiShieldA

  bool msmActive; /// Movable Safety Mask active flag

  /// point to stop [normal none]
  // std::string stopPoint;

  /// Inner buildzone
  //  attachSystem::BlockZone buildZone;

  // /// Shared point to use for last component:
  // std::shared_ptr<attachSystem::FixedComp> lastComp;

  // /// transfer pipe from undulator/wiggler
  // std::shared_ptr<constructSystem::VacuumPipe> transPipe;

  // /// First magnetic block out of undulator
  // std::shared_ptr<xraySystem::MagnetM1> magBlockM1;

  // /// Second magnetic block out of undulator
  // std::shared_ptr<xraySystem::MagnetU1> magBlockU1;


  // std::shared_ptr<xraySystem::EPSeparator> epSeparator;

  // /// Electron/photon separator to choke 1
  // std::shared_ptr<xraySystem::R3ChokeChamber> chokeChamber;

  // /// Electron/photon separator to choke 1
  // std::shared_ptr<xraySystem::R3ChokeInsert> chokeInsert;

  // /// dipole connection pipe
  // std::shared_ptr<constructSystem::CornerPipe> dipolePipe;
  // /// electron transfer pipe M1->U1
  // std::shared_ptr<constructSystem::VacuumPipe> eTransPipe;
  // /// electron cut cell
  // std::shared_ptr<insertSystem::insertCylinder> eCutDisk;
  // /// electron cut cell [with magnetic field]
  // std::shared_ptr<insertSystem::insertCylinder> eCutMagDisk;
  // /// bellow infront of collimator
  // std::shared_ptr<constructSystem::Bellows> bellowA;
  // /// collimator A
  // std::shared_ptr<xraySystem::SquareFMask> collA;
  // /// bellow after collimator
  // std::shared_ptr<constructSystem::Bellows> bellowB;
  // /// Mask1:2 connection pipe
  // std::shared_ptr<constructSystem::VacuumPipe> collABPipe;
  // /// bellow after collimator
  // std::shared_ptr<constructSystem::Bellows> bellowC;
  // /// collimator B
  // std::shared_ptr<xraySystem::SquareFMask> collB;
  // /// collimator C
  // std::shared_ptr<xraySystem::SquareFMask> collC;
  // /// Pipe from collimator B to heat dump
  // std::shared_ptr<constructSystem::VacuumPipe> collExitPipe;

  // /// head dump port
  // std::shared_ptr<constructSystem::PipeTube> heatBox;
  // /// Heat dump container
  // std::shared_ptr<xraySystem::HeatDump> heatDump;
  // /// bellow after HeatShield
  // std::shared_ptr<constructSystem::Bellows> bellowD;
  // /// Real Ion pump (KF40) 26cm vertioal
  // std::shared_ptr<constructSystem::CrossPipe> ionPB;
  // /// Pipe to third optic table
  // std::shared_ptr<constructSystem::VacuumPipe> pipeB;


  //   /// bellows for third table
  // /// Variable Apperature pipe
  // std::shared_ptr<constructSystem::VacuumPipe> aperturePipeA;
  // /// L collimator
  // std::shared_ptr<xraySystem::LCollimator> moveCollA;
  // /// bellows for third table
  // std::shared_ptr<constructSystem::Bellows> bellowF;
  // /// Real Ion pump (KF40) 26cm vertioal
  // std::shared_ptr<constructSystem::CrossPipe> ionPC;
  // /// bellows for second movable aperature
  // std::shared_ptr<constructSystem::Bellows> bellowG;
  // /// Variable Apperature pipe B
  // std::shared_ptr<constructSystem::VacuumPipe> aperturePipeB;
  // /// L collimator
  // std::shared_ptr<xraySystem::LCollimator> moveCollB;
  // /// bellows for exit of moveable aperatures
  // std::shared_ptr<constructSystem::Bellows> bellowH;
  // /// Exit of movables
  // std::shared_ptr<constructSystem::VacuumPipe> pipeC;


  // /// Exit of movables [?]
  // std::shared_ptr<constructSystem::GateValveCube> gateA;
  // /// bellows for florescence system
  // std::shared_ptr<constructSystem::Bellows> bellowI;
  // /// florescence screen tube
  // std::shared_ptr<constructSystem::PipeTube> florTubeA;
  // /// bellows for florescence system
  // std::shared_ptr<constructSystem::Bellows> bellowJ;
  // /// Gate box B
  // std::shared_ptr<xraySystem::CylGateValve> gateTubeB;
  // /// Front port connection for shutterbox
  // std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeA;
  // /// Main shutters
  // std::shared_ptr<constructSystem::PipeTube> shutterBox;
  // /// Shutters
  // std::array<std::shared_ptr<xraySystem::BeamMount>,2> shutters;
  // /// Back port connection for shutterbox
  // std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeB;

  // bool collFM3Active;   ///< Coll C active
  // double outerRadius;   ///< radius of tube for divisions
  // double frontOffset;   ///< Distance to move start point from origin

  // void insertFlanges(Simulation&,const constructSystem::PipeTube&,
  // 		     const size_t);
  virtual const attachSystem::FixedComp&
    buildUndulator(Simulation&,const attachSystem::FixedComp&,const long int) = 0;

  /// Null op for extra components after build
  virtual void buildExtras(Simulation&) {}

  void buildSupport5(Simulation&,const attachSystem::FixedComp&,const std::string&);
  void buildSupport6(Simulation&,
			  const attachSystem::FixedComp&,const long int);
  void buildSupport7(Simulation&);
  void buildMSM(Simulation&,
		attachSystem::FixedComp&,
		const std::string&);


  void populate(const FuncDataBase&) override;
  void createSurfaces();
  virtual void createLinks()=0;

  void buildObjects(Simulation&);

 public:

  R3FrontEndToyamaDanMAX(const std::string&);
  R3FrontEndToyamaDanMAX(const R3FrontEndToyamaDanMAX&);
  R3FrontEndToyamaDanMAX& operator=(const R3FrontEndToyamaDanMAX&);
  ~R3FrontEndToyamaDanMAX() override;

  /// set stop point
  void setStopPoint(const std::string& S) { stopPoint=S; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

  std::shared_ptr<constructSystem::VacuumPipe> getProxiShieldBPipe() const {return proxiShieldBPipe;}
  std::shared_ptr<xraySystem::ProximityShielding> getProxiShieldB() const {return proxiShieldB;}
};

}

#endif
