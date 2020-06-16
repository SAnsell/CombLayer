/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment17.h
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
#ifndef tdcSystem_Segment17_h
#define tdcSystem_Segment17_h

namespace tdcSystem
{
  class YagScreen;
  /*!
    \class Segment17
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 17
  */

class Segment17 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;       ///< #1
  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #2
  std::shared_ptr<constructSystem::BlankTube> ionPump;       ///< #4 Ion pump 75l cf63
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;       ///< #5 - VC

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment17(const std::string&);
  Segment17(const Segment17&);
  Segment17& operator=(const Segment17&);
  ~Segment17();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
