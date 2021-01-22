/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment38.h
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
#ifndef tdcSystem_Segment38_h
#define tdcSystem_Segment38_h

namespace xraySystem
{
  class IonPumpTube;
}

namespace tdcSystem
{
  /*!
    \class Segment38
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 38
  */

class Segment38 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;       ///< #1
  std::shared_ptr<xraySystem::IonPumpTube> ionPump;      ///< #2,3,4
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;       ///< #5
  std::shared_ptr<constructSystem::Bellows> bellowA;        ///< #6

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment38(const std::string&);
  Segment38(const Segment38&);
  Segment38& operator=(const Segment38&);
  ~Segment38();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
