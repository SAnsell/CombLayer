/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment31.h
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
#ifndef tdcSystem_Segment31_h
#define tdcSystem_Segment31_h

namespace tdcSystem
{
  /*!
    \class Segment31
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 31
  */

class Segment31 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;    ///< #1 Bellows – 304L
  std::shared_ptr<constructSystem::BlankTube> ionPumpA; ///< #3
  std::shared_ptr<xraySystem::CylGateValve> gate;       ///< #4 Gate valve 48132 CE44
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< #1 Bellows – 304L
  std::shared_ptr<tdcSystem::BPM> bpm;                  ///< #5 BPM – 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< #3 VC- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::LQuadF> quad;              ///< #7 Quadropole magnet type F
  std::shared_ptr<constructSystem::Bellows> bellowC;    ///< #1 Bellows – 304L
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   ///< #8 VC- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::CorrectorMag> cMagH;       ///< #9 Corrector magnet type D
  std::shared_ptr<constructSystem::BlankTube> ionPumpB; ///< #3
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;   ///< #8 VC- Flanges 304L- Tube-316L
  std::shared_ptr<constructSystem::Bellows> bellowD;    ///< #1 Bellows – 304L

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment31(const std::string&);
  Segment31(const Segment31&);
  Segment31& operator=(const Segment31&);
  ~Segment31();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
