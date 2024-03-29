/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment41.h
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
#ifndef tdcSystem_Segment41_h
#define tdcSystem_Segment41_h

namespace tdcSystem
{
  /*!
    \class Segment41
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 41
  */

class Segment41 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA; // #1
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;               // #2
  std::shared_ptr<xraySystem::CylGateValve> gate;    // #3
  std::shared_ptr<constructSystem::VacuumPipe> pipe; // #4
  std::shared_ptr<constructSystem::Bellows> bellowB; // #1

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment41(const std::string&);
  Segment41(const Segment41&);
  Segment41& operator=(const Segment41&);
  ~Segment41() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
