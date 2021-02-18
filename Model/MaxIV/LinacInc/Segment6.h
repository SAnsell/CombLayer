/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment6.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef tdcSystem_Segment6_h
#define tdcSystem_Segment6_h

namespace constructSystem
{
  class VacuumPipe;
}

namespace tdcSystem
{
  class Scrapper;
  class EBeamStop;
  class CeramicGap;
  class LocalShielding;

  /*!
    \class Segment6
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Second segment in the TDC from the linac
  */

class Segment6 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// seonc pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// third pipe (falange change)
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;

  /// beam scrapper
  std::shared_ptr<tdcSystem::Scrapper> scrapper;
  std::shared_ptr<tdcSystem::LocalShielding> shieldA;

  /// fourth pipe (flange change)
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  std::shared_ptr<tdcSystem::LocalShielding> shieldB;

  /// long bellow
  std::shared_ptr<tdcSystem::CeramicGap> ceramicA;

  /// Electron beam stop
  std::shared_ptr<tdcSystem::EBeamStop> beamStop;

  /// ceramicBellow [reversed]
  std::shared_ptr<tdcSystem::CeramicGap> ceramicB;

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment6(const std::string&);
  Segment6(const Segment6&);
  Segment6& operator=(const Segment6&);
  ~Segment6();

  std::shared_ptr<EBeamStop> getBeamStop() const { return beamStop; }

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
