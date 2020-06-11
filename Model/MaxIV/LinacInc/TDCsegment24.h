/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment24.h
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
#ifndef tdcSystem_TDCsegment24_h
#define tdcSystem_TDCsegment24_h

namespace tdcSystem
{
  /*!
    \class TDCsegment24
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 43
  */

class TDCsegment24 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #1 VC- Flanges 304L- Tube-316L
  std::shared_ptr<constructSystem::PipeTube> ionPump;     ///< #3 Ion Pump 75l cf63
  std::shared_ptr<constructSystem::Bellows> bellow;       ///< #4 Bellows -304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;     ///< #5 VC- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::CorrectorMag> cMagH;         ///< #6 Horizontal corrector magnet
  std::shared_ptr<tdcSystem::CorrectorMag> cMagV;         ///< #6 Vertical corrector magnet
  std::shared_ptr<tdcSystem::BPM> bpm;                    ///< #7 BPM - 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;     ///< #8 VC Ø10x1- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::LQuadH> quad;                ///< #9 type G

  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment24(const std::string&);
  TDCsegment24(const TDCsegment24&);
  TDCsegment24& operator=(const TDCsegment24&);
  ~TDCsegment24();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
