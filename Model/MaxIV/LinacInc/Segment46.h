/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment46.h
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
#ifndef tdcSystem_Segment46_h
#define tdcSystem_Segment46_h

namespace constructSystem
{
  class PortTube;
  class JawFlange;
}

namespace xraySystem
{
  class CylGateValve;
  class CrossWayTube;
}

namespace tdcSystem
{
  class CleaningMagnet;
  class PrismaChamber;
  class LocalShielding;

  /*!
    \class Segment46
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 46
  */

class Segment46 :
  public TDCsegment
{
 private:

  std::unique_ptr<attachSystem::BlockZone> IZThin;       ///< Extra limited zone

  std::shared_ptr<constructSystem::VacuumPipe> pipeA; // #1
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding
  std::shared_ptr<xraySystem::CylGateValve> gateA;    // #2x50
  std::shared_ptr<constructSystem::Bellows> bellowA;  // #3
  std::shared_ptr<tdcSystem::PrismaChamber> prismaChamber; ///< #4
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberA; ///< #6
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; // #7
  std::shared_ptr<tdcSystem::CleaningMagnet> cleaningMag; // #8
  std::shared_ptr<constructSystem::PortTube> slitTube; // #9
  std::array<std::shared_ptr<constructSystem::JawFlange>,2> jaws; // jaws in #9
  std::shared_ptr<constructSystem::Bellows> bellowB; // #10
  std::shared_ptr<xraySystem::CrossWayTube> mirrorChamberB; ///< #6
  std::shared_ptr<constructSystem::Bellows> bellowC; // #10
  std::shared_ptr<xraySystem::CylGateValve> gateB;    // #11
  std::shared_ptr<constructSystem::Bellows> bellowD; ///< Additional bellow

  void createSplitInnerZone(Simulation&);
  void buildObjects(Simulation&);
  void createLinks();

  void postBuild(Simulation&);

 public:

  Segment46(const std::string&);
  Segment46(const Segment46&);
  Segment46& operator=(const Segment46&);
  ~Segment46();

  virtual void insertPrevSegment(Simulation&,const TDCsegment*) const override;

  void writePoints() const override;
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
