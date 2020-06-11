/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment23.h
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
#ifndef tdcSystem_TDCsegment23_h
#define tdcSystem_TDCsegment23_h

namespace tdcSystem
{
  /*!
    \class TDCsegment23
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 23
  */

class TDCsegment23 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #2 VC Ø10x1- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::LQuadH> quad;                 ///< #3 Quadrupole type G (H)
  std::shared_ptr<tdcSystem::BPM> bpm;                     ///< #4 BPM - 304L
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #5 VC- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::CorrectorMag> cMagH;          ///< #6 Horizontal corrector magnet
  std::shared_ptr<tdcSystem::CorrectorMag> cMagV;          ///< #6 Vertical corrector magnet
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;         ///< #7 Yag screen - 304L
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;             ///< #8 Yag screen unit (ionPump)
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;      ///< #9 VC- Flanges 304L- Tube-316L
  std::shared_ptr<xraySystem::CylGateValve> gate;     ///< #10 Gate valve 48132 CE44
  std::shared_ptr<constructSystem::Bellows> bellowC;       ///< #1 Bellows 304L

  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment23(const std::string&);
  TDCsegment23(const TDCsegment23&);
  TDCsegment23& operator=(const TDCsegment23&);
  ~TDCsegment23();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
