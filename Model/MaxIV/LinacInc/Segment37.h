/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment37.h
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
#ifndef tdcSystem_Segment37_h
#define tdcSystem_Segment37_h

namespace tdcSystem
{

  /*!
    \class Segment37
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 37
  */

class Segment37 :
  public TDCsegment
{
 private:

  std::shared_ptr<tdcSystem::CeramicGap> ceramicA;   // #1 ceramic gap
  std::shared_ptr<tdcSystem::EBeamStop> beamStop;    // #2 electron beam stop
  std::shared_ptr<tdcSystem::CeramicGap> ceramicB;   // #1 ceramic gap
  std::shared_ptr<constructSystem::VacuumPipe> pipe; // #3

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment37(const std::string&);
  Segment37(const Segment37&);
  Segment37& operator=(const Segment37&);
  ~Segment37();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
