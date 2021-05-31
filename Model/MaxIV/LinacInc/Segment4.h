/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment4.h
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
#ifndef tdcSystem_Segment4_h
#define tdcSystem_Segment4_h

namespace tdcSystem
{
  class LSexupole;
  class LocalShielding;

  /*!
    \class Segment4
    \version 1.1
    \author S. Ansell
    \date Feb 2021
    \brief 4th L2SPF segment
  */

class Segment4 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;

  /// Beam position monitor
  std::shared_ptr<tdcSystem::StriplineBPM> bpmA;

  // first pipe in dipole
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;

  /// Quad begining QSQ
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// sexupole
  std::shared_ptr<tdcSystem::LSexupole> SexuA;

  /// Quad endng QSQ
  std::shared_ptr<tdcSystem::LQuadF> QuadB;

  /// yag station
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;

  /// yag screen
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;
  /// shielding of the YAG screen camera (the camera is not built)
  std::shared_ptr<tdcSystem::LocalShielding> shieldA;

  /// exit pipe for corrector mags
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// exit pipe for corrector mags
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;

  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;    ///< corrector mag

  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;   ///< corrector mag

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment4(const std::string&);
  Segment4(const Segment4&);
  Segment4& operator=(const Segment4&);
  ~Segment4();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
