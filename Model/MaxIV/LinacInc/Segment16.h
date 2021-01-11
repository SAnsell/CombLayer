/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment16.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef tdcSystem_Segment16_h
#define tdcSystem_Segment16_h

namespace xraySystem
{
  class CorrectorMag;
  class IonPumpTube;
}

namespace tdcSystem
{
  class StriplineBPM;
  class LQuadH;
  /*!
    \class Segment16
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 16
  */

class Segment16 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellows 304L
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;            ///< #2 BPM
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;      ///< #3
  std::shared_ptr<tdcSystem::LQuadH> quad;                  ///< #4
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;      ///< #5

  /// #6 - horizontal corrector magnet
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA; 
  
  /// #6 - vertical corrector magnet
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;      
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< #1 Bellows 304L  
  std::shared_ptr<xraySystem::IonPumpTube> ionPump;     ///< #7
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;      ///< #9

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment16(const std::string&);
  Segment16(const Segment16&);
  Segment16& operator=(const Segment16&);
  ~Segment16();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
