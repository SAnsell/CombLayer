/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment47.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef tdcSystem_Segment47_h
#define tdcSystem_Segment47_h

namespace attachSystem
{
  class WrapperCell;
}
namespace xraySystem
{
  class CylGateValve;
  class CrossWayTube;
}

namespace tdcSystem
{
  class LocalShielding;
  class SPFCameraShield;

  /*!
    \class Segment47
    \version 1.2
    \author K. Batkov
    \date Feb 2021
    \brief SPF segment 47
  */

class Segment47 :
  public TDCsegment
{
 private:

  std::unique_ptr<attachSystem::BlockZone> IZThin;       ///< Extra limited zone
  const InjectionHall* IHall;      ///< Storage for injection hall if used.

  std::shared_ptr<constructSystem::VacuumPipe> pipeA; ///< #1
  std::shared_ptr<tdcSystem::PrismaChamber> prismaChamberA; ///< #2 and #3
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberA; ///< #4
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; ///< #5
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberB; ///< #4
  std::shared_ptr<constructSystem::VacuumPipe> pipeC; ///< #5
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberC; ///< #4
  std::shared_ptr<constructSystem::VacuumPipe> pipeD; ///< #6
  std::shared_ptr<xraySystem::CylGateValve> gateA;    ///< #7
  std::shared_ptr<constructSystem::Bellows> bellowA;  ///< #8
  std::shared_ptr<constructSystem::VacuumPipe> pipeE; ///< #6

  /// local shielding perp to beam line
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; 
  std::shared_ptr<tdcSystem::SPFCameraShield> shieldB; ///< Mirror camera shield

  int back45; ///< back surface of segment 45
  int roof46; ///< roof surface of segment 46

  void createSplitInnerZone();

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment47(const std::string&);
  Segment47(const Segment47&);
  Segment47& operator=(const Segment47&);
  ~Segment47();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
