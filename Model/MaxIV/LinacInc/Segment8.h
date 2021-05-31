/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment8.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#ifndef tdcSystem_Segment8_h
#define tdcSystem_Segment8_h

namespace tdcSystem
{

  /*!
    \class Segment8
    \version 1.1
    \author S. Ansell / K. Batkov
    \date Feb 2021
    \brief 8th segment
  */

class Segment8 :
  public TDCsegment
{
 private:

  /// first bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Beam stop
  std::shared_ptr<tdcSystem::EBeamStop> beamStop;
  /// second bellow
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment8(const std::string&);
  Segment8(const Segment8&);
  Segment8& operator=(const Segment8&);
  ~Segment8();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
