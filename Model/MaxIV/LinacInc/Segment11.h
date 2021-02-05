/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment11.h
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
#ifndef tdcSystem_Segment11_h
#define tdcSystem_Segment11_h

namespace tdcSystem
{
  class LocalShielding;

  /*!
    \class Segment11
    \version 1.1
    \author S. Ansell
    \date Feb 2021
    \brief 11th segment
  */

class Segment11 :
  public TDCsegment
{
 private:

  /// first bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// Beam postion monitor
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;
  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;

  /// Quad
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// ion pump [rotated]
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;
  /// Yag screen for pump tube
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;

  /// exit pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;

  /// corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment11(const std::string&);
  Segment11(const Segment11&);
  Segment11& operator=(const Segment11&);
  ~Segment11();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
