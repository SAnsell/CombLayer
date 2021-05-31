/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment45.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_Segment45_h
#define tdcSystem_Segment45_h

namespace constructSystem
{
  class VacuumPipe;
  class FlangePlate;
}

namespace tdcSystem
{
  class CeramicGap;
  class InjectionHall;
  class EBeamStop;

  /*!
    \class Segment45
    \version 1.1
    \author K. Batkov
    \date Feb 2021
    \brief SPF segment 45 + Main beam dump
  */

class Segment45 :
  public TDCsegment
{
 private:

  const InjectionHall* IHall;      ///< Storage for injection hall if used.

  double cutRadius;        ///< Cut radius

  std::shared_ptr<tdcSystem::CeramicGap> ceramic;     // #1 ceramic gap
  std::shared_ptr<constructSystem::VacuumPipe> pipeA; // #2
  std::shared_ptr<tdcSystem::YagUnitBig> yagUnit;     // #3
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;    // #3
  std::shared_ptr<constructSystem::FlangePlate> adaptor; // adaptor plate
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; // #5

  /// Main beam dump
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  std::shared_ptr<tdcSystem::EBeamStop> beamStop;

  virtual void setFrontSurfs(const std::vector<HeadRule>&) override;

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

  void constructHole(Simulation&);

 public:

  Segment45(const std::string&);
  Segment45(const Segment45&);
  Segment45& operator=(const Segment45&);
  virtual ~Segment45();

  void writePoints() const override;
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
