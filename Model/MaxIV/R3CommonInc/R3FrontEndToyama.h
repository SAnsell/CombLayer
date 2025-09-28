/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3CommonInc/R3FrontEndToyama.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
#ifndef xraySystem_R3FrontEndToyama_h
#define xraySystem_R3FrontEndToyama_h

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
  class BremBlock;
  class HeatAbsorberToyama;
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

  /*!
    \class balderFrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class R3FrontEndToyama :
  public R3FrontEnd
{
protected:
  std::shared_ptr<constructSystem::Bellows> bellowPreMSM;
  std::shared_ptr<xraySystem::MovableSafetyMask> msm;
  std::shared_ptr<constructSystem::Bellows> bellowPostMSM;
  std::shared_ptr<constructSystem::VacuumPipe> msmExitPipe;
  std::shared_ptr<constructSystem::Bellows> bellowPreHA;
  std::shared_ptr<xraySystem::HeatAbsorberToyama> ha;
  std::shared_ptr<constructSystem::Bellows> bellowPostHA;
  std::shared_ptr<constructSystem::Bellows> bellowDA;
  std::shared_ptr<xraySystem::CylGateValve> gateTubePreMM1; // TODO ask what this component is. Maybe vacuum valve.

  // Bremsstrahlung collimator
  std::shared_ptr<xraySystem::BremBlock> bremColl;
  std::shared_ptr<constructSystem::VacuumPipe> bremCollPipe;

  std::shared_ptr<xraySystem::ProximityShielding> proxiShieldA; // proximity shielding
  std::shared_ptr<constructSystem::VacuumPipe> proxiShieldAPipe; // pipe inside proxiShieldA
  std::shared_ptr<xraySystem::ProximityShielding> proxiShieldB; // proximity shielding
  std::shared_ptr<constructSystem::VacuumPipe> proxiShieldBPipe; // pipe inside proxiShieldA

  virtual const attachSystem::FixedComp&
  buildUndulator(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) =0;

  /// Null op for extra components after build
  virtual void buildExtras(Simulation&) {}

  void buildHeatTable(Simulation&);
  void buildApertureTable(Simulation&,
			  const attachSystem::FixedComp&,const long int);
  void buildShutterTable(Simulation&);
  void buildMSM(Simulation&,
		const attachSystem::FixedComp&,
		const std::string&);


  void populate(const FuncDataBase&) override;
  void createSurfaces();
  virtual void createLinks()=0;

  void buildObjects(Simulation&);

 public:

  R3FrontEndToyama(const std::string&);
  R3FrontEndToyama(const R3FrontEndToyama&);
  R3FrontEndToyama& operator=(const R3FrontEndToyama&);
  ~R3FrontEndToyama() override;

  /// remove FM3
  void deactivateFM3() { collFM3Active=0; }
  /// set stop point
  void setStopPoint(const std::string& S) { stopPoint=S; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

  std::shared_ptr<constructSystem::VacuumPipe> getProxiShieldBPipe() const {return proxiShieldBPipe;}
  std::shared_ptr<xraySystem::ProximityShielding> getProxiShieldB() const {return proxiShieldB;}
};

}

#endif
