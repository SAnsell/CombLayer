/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment35.h
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
#ifndef tdcSystem_Segment35_h
#define tdcSystem_Segment35_h

namespace tdcSystem
{
  class YagUnitBig;
  class ButtonBPM;
  /*!
    \class Segment35
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief SPF segment 35
  */

class Segment35 :
  public TDCsegment
{
 private:

  /// yag station [big type]
  std::shared_ptr<tdcSystem::YagUnitBig> yagUnit;

  /// yag screen
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;

  // first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;

  /// Quadrupole magnet
  std::shared_ptr<tdcSystem::LQuadF> quadA;

  /// Button BPM
  std::shared_ptr<tdcSystem::ButtonBPM> bpm;

  // main magnetic pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;

  /// Quad endng QSQ
  std::shared_ptr<tdcSystem::LQuadF> quadB;

  /// corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;

  /// corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;

  /// mirror chamber
  std::shared_ptr<tdcSystem::CrossWayBlank> mirrorChamber;

  /// exit pipe + corrector mag
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;

  /// bellow
  std::shared_ptr<constructSystem::Bellows> bellow;

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment35(const std::string&);
  Segment35(const Segment35&);
  Segment35& operator=(const Segment35&);
  ~Segment35();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
