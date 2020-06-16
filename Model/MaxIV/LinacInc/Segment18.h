/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment18.h
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
#ifndef tdcSystem_Segment18_h
#define tdcSystem_Segment18_h

namespace tdcSystem
{
  /*!
    \class Segment18
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 18
  */

class Segment18 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::BlankTube> ionPump;     ///< #2, 3
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellows 304L
  std::shared_ptr<tdcSystem::BPM> bpm;                     ///< #4 BPM
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #5
  std::shared_ptr<tdcSystem::LQuadH> quad;                 ///< #6
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #7
  std::shared_ptr<tdcSystem::CorrectorMag> cMagH;          ///< #8 - horizontal corrector magnet
  std::shared_ptr<tdcSystem::CorrectorMag> cMagV;          ///< #8 - vertical corrector magnet

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment18(const std::string&);
  Segment18(const Segment18&);
  Segment18& operator=(const Segment18&);
  ~Segment18();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
