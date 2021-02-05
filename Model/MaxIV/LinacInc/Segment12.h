/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment12.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef tdcSystem_Segment12_h
#define tdcSystem_Segment12_h

namespace constructSystem
{
}
namespace xraySystem
{
  class IonPumpTube;
}

namespace tdcSystem
{
  class LocalShielding;

  /*!
    \class Segment12
    \version 1.1
    \author S. Ansell
    \date Feb 2021
    \brief 12th segment
  */

class Segment12 :
  public TDCsegment
{
 private:

  /// entrance bellows
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatA;

  /// first  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;

  /// first pipe
  std::shared_ptr<tdcSystem::BeamDivider> beamA;
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding

  // LEFT DIVIDER:

  // bellow on left exit
  std::shared_ptr<constructSystem::Bellows> bellowLA;

  /// Ion pump
  std::shared_ptr<xraySystem::IonPumpTube> ionPumpLA;

  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeLA;

  // LAST Unit in left cell
  std::shared_ptr<constructSystem::Bellows> bellowLB;

  // RIGHT DIVIDER:

  /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatB;

  /// second  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleB;

    /// second  dipole
  std::shared_ptr<constructSystem::Bellows> bellowRB;


  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment12(const std::string&);
  Segment12(const Segment12&);
  Segment12& operator=(const Segment12&);
  ~Segment12();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
