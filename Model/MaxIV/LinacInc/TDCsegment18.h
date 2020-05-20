/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment18.h
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
#ifndef tdcSystem_TDCsegment18_h
#define tdcSystem_TDCsegment18_h

namespace tdcSystem
{
  /*!
    \class TDCsegment18
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 18
  */

class TDCsegment18 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<tdcSystem::BPM> bpm;                     ///< #2 BPM
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #3
  std::shared_ptr<tdcSystem::LQuad> quad;                  ///< #4
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #5
  std::shared_ptr<tdcSystem::CorrectorMag> cMagH;          ///< #6 - horizontal corrector magnet
  std::shared_ptr<tdcSystem::CorrectorMag> cMagV;          ///< #6 - vertical corrector magnet
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellows 304L
  std::shared_ptr<constructSystem::PipeTube> ionPump;      ///< #7
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;      ///< #9

  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment18(const std::string&);
  TDCsegment18(const TDCsegment18&);
  TDCsegment18& operator=(const TDCsegment18&);
  ~TDCsegment18();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
