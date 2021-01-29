/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment23.h
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
#ifndef tdcSystem_Segment23_h
#define tdcSystem_Segment23_h

namespace tdcSystem
{
  class StriplineBPM;
  /*!
    \class Segment23
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 23
  */

class Segment23 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #2 VC Ø10x1- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::LQuadH> quad;                 ///< #3 Quadrupole type G (H)
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;            ///< #4 BPM - 304L
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #5 VC- Flanges 304L- Tube-316L
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;          ///< #6 Horizontal corrector magnet
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;          ///< #6 Vertical corrector magnet
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;         ///< #7 Yag screen - 304L
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;             ///< #8 Yag screen unit (ionPump)
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;      ///< #9 VC- Flanges 304L- Tube-316L
  std::shared_ptr<xraySystem::CylGateValve> gate;     ///< #10 Gate valve 48132 CE44
  std::shared_ptr<constructSystem::Bellows> bellowC;       ///< #1 Bellows 304L

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment23(const std::string&);
  Segment23(const Segment23&);
  Segment23& operator=(const Segment23&);
  ~Segment23();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
