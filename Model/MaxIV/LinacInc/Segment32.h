/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment32.h
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
#ifndef tdcSystem_Segment32_h
#define tdcSystem_Segment32_h

namespace tdcSystem
{
  class DipoleDIBMag;
  /*!
    \class Segment32
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 32
  */

class Segment32 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;  ///< Pipe through dmA
  std::shared_ptr<tdcSystem::DipoleDIBMag> dmA;        ///< First dipole magnet
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;  ///< Pipe between first two dipole magnets
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;  ///< Pipe through dmB
  std::shared_ptr<tdcSystem::DipoleDIBMag> dmB;        ///< Second dipole magnet
  std::shared_ptr<constructSystem::Bellows> bellow;    ///< Bellow

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment32(const std::string&);
  Segment32(const Segment32&);
  Segment32& operator=(const Segment32&);
  ~Segment32();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
