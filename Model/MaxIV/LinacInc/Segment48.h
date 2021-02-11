/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment48.h
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
#ifndef tdcSystem_Segment48_h
#define tdcSystem_Segment48_h

namespace xraySystem
{
  class CylGateValve;
  class CrossWayTube;
}

namespace tdcSystem
{
  class LocalShielding;
  /*!
    \class Segment48
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 48
  */

class Segment48 :
  public TDCsegment
{
 private:

  std::shared_ptr<tdcSystem::EBeamStop> beamStopA;    // #1
  std::shared_ptr<constructSystem::Bellows> bellowA;  // #2
  std::shared_ptr<tdcSystem::EBeamStop> beamStopB;    // #1
  std::shared_ptr<constructSystem::VacuumPipe> pipeA; // #1
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding perp to beam line
  std::shared_ptr<tdcSystem::LocalShielding> shieldB; // local shielding in the corner of shieldA
  std::shared_ptr<tdcSystem::LocalShielding> shieldC; // local shielding parallel to beam line
  std::shared_ptr<constructSystem::PortTube> slitTube; // #4
  std::array<std::shared_ptr<constructSystem::JawFlange>,2> jaws; // jaws in #4
  std::shared_ptr<constructSystem::Bellows> bellowB;  // #5
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberA; ///< #6
  std::shared_ptr<constructSystem::Bellows> bellowC;  // #5

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment48(const std::string&);
  Segment48(const Segment48&);
  Segment48& operator=(const Segment48&);
  ~Segment48();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
