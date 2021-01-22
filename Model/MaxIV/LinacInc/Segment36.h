/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment36.h
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
#ifndef tdcSystem_Segment36_h
#define tdcSystem_Segment36_h

namespace xraySystem
{
  class GaugeTube;
}

namespace tdcSystem
{
  class StriplineBPM;
  /*!
    \class Segment36
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief SPF segment 36
  */

class Segment36 :
  public TDCsegment
{
 private:
  std::shared_ptr<xraySystem::GaugeTube> gauge;   // #3
  std::shared_ptr<constructSystem::VacuumPipe> pipeA; // #4
  std::shared_ptr<tdcSystem::LQuadF> quadA;           // #5
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;     // #6
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;     // #6
  std::shared_ptr<tdcSystem::LQuadF> quadB;           // #5
  std::shared_ptr<tdcSystem::StriplineBPM> bpmA;      // #9 - stripline BPM
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; // #8
  std::shared_ptr<tdcSystem::EArrivalMon> beamArrivalMon; // #9
  std::shared_ptr<constructSystem::VacuumPipe> pipeC; // #10
  std::shared_ptr<constructSystem::VacuumPipe> pipeD; // #11
  std::shared_ptr<tdcSystem::StriplineBPM> bpmB;      // #12
  std::shared_ptr<xraySystem::CylGateValve> gate;     // #13
  std::shared_ptr<constructSystem::VacuumPipe> pipeE; // #11

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment36(const std::string&);
  Segment36(const Segment36&);
  Segment36& operator=(const Segment36&);
  ~Segment36();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
