/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment21.h
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
#ifndef tdcSystem_Segment21_h
#define tdcSystem_Segment21_h

namespace tdcSystem
{
  /*!
    \class Segment21
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 21
  */

class Segment21 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;            ///< #2 BPM
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #3
  std::shared_ptr<tdcSystem::LQuadH> quad;                 ///< #4 Quadrupole type G (H)
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;             ///< #5 Yag screen unit
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;         ///< yag screen
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #5
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;          ///< #6 - horizontal corrector magnet
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;          ///< #6 - vertical corrector magnet
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellows 304L

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment21(const std::string&);
  Segment21(const Segment21&);
  Segment21& operator=(const Segment21&);
  ~Segment21();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
