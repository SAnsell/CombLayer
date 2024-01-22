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

namespace xraySystem
{
  class Solenoid;
}

namespace constructSystem
{
  class Bellows;
  class VacuumPipe;
  class GTFGateValve;
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

  std::shared_ptr<IonPumpGammaVacuum> ionPump;
  std::shared_ptr<constructSystem::VacuumPipe> extension; // drawing 202964
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<xraySystem::Solenoid> solenoid; // drawing 204282
  std::shared_ptr<constructSystem::GTFGateValve> gate; // Gate Valve 48236-CE24, drawing 200564
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  std::shared_ptr<constructSystem::Bellows> bellowA;

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

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
