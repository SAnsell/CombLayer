/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/GTFLine.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef maxiv_GTFLine_h
#define maxiv_GTFLine_h

namespace tdcSystem
{
  class YagUnit;
  class YagScreen;
}


namespace xraySystem
{
  class RFGun;
  class Solenoid;
  class CurrentTransformer;
  class SlitsMask;
}

namespace constructSystem
{
  class Bellows;
  class VacuumPipe;
  class GTFGateValve;
  class PipeTube;
  class FlangePlate;
}

namespace MAXIV
{

  /*!
    \class GTFLine
    \version 1.0
    \author K. Batkov
    \date September 2023
    \brief Constructor for the Gun Test Facility line components
  */

  class IonPumpGammaVacuum;

class GTFLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// construction space for main object
  attachSystem::BlockZone buildZone;
  int innerMat;                         ///< inner material if used

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  std::shared_ptr<IonPumpGammaVacuum> ionPumpA;
  std::shared_ptr<constructSystem::VacuumPipe> extensionA; // drawing 202964
  std::shared_ptr<xraySystem::RFGun> gun;
  std::shared_ptr<constructSystem::VacuumPipe> pipeBelowGun;
  std::shared_ptr<constructSystem::Bellows> bellowBelowGun;
  std::shared_ptr<IonPumpGammaVacuum> pumpBelowGun;
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<xraySystem::Solenoid> solenoid; // emittance-compensating solenoid by Radiabeam, drawing 204282
  std::shared_ptr<constructSystem::GTFGateValve> gate; // Gate Valve 48236-CE24, drawing 200564
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  std::shared_ptr<constructSystem::Bellows> bellowA;
  std::shared_ptr<xraySystem::CurrentTransformer> mon;
  std::shared_ptr<constructSystem::PipeTube> laserChamber; // laser and viewing chamber
  std::shared_ptr<constructSystem::FlangePlate> laserChamberBackPlate; // laser chamber back plate
  std::shared_ptr<constructSystem::VacuumPipe> extensionB;
  std::shared_ptr<IonPumpGammaVacuum> ionPumpB;
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitA;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenA;
  std::shared_ptr<constructSystem::Bellows> bellowB;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitB;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenB;
  std::shared_ptr<constructSystem::Bellows> bellowC;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitC;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenC;
  std::shared_ptr<constructSystem::Bellows> bellowD;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitD;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenD;
  std::shared_ptr<xraySystem::SlitsMask> slits;

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructYAG(Simulation& System, attachSystem::BlockZone& buildZone,
		    const attachSystem::FixedComp& linkUnit,
		    const std::string& sideName,
		    tdcSystem::YagUnit& yag,
		    tdcSystem::YagScreen& screen);
  void constructSlits(Simulation& System, attachSystem::BlockZone& buildZone,
		    const attachSystem::FixedComp& linkUnit,
		    const std::string& sideName,
		    tdcSystem::YagUnit& yag,
		    xraySystem::SlitsMask& slits);
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  GTFLine(const std::string&);
  GTFLine(const GTFLine&);
  GTFLine& operator=(const GTFLine&);
  ~GTFLine();

  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
