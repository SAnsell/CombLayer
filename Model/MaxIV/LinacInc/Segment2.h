/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment2.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_Segment2_h
#define tdcSystem_Segment2_h

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
}

namespace xraySystem
{
  class CylGateValve;
}


namespace tdcSystem
{
  class LQuadF;
  class CorrectorMag;
  class BPM;
  class EArrivalMon;
  class YagUnit;
  class YagScreen;
  class LQuadF;
  
  /*!
    \class Segment2
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TDC from the linac
  */

class Segment2 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< start pipe
  /// quad
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// Beam position monitor
  std::shared_ptr<tdcSystem::BPM> bpmA;   

  /// Bellows from BPM
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// Long connecter pipe to gate valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; 
  
  std::shared_ptr<tdcSystem::LQuadF> QuadB;    /// quad B 

  /// Gate valve holder
  std::shared_ptr<xraySystem::CylGateValve> gateTube;   

  /// gateValve - Beam Arriva lMonitor
  std::shared_ptr<constructSystem::VacuumPipe> pipeC; 
  
  /// Beam arrival monitor 
  std::shared_ptr<tdcSystem::EArrivalMon> beamArrivalMon;

  /// gateValve - Beam Arriva lMonitor
  std::shared_ptr<constructSystem::VacuumPipe> pipeD; 

  /// Bellows
  std::shared_ptr<constructSystem::Bellows> bellowB;

  /// Beam pos mon 2
  std::shared_ptr<tdcSystem::BPM> bpmB;

  // quad pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeE; 
  std::shared_ptr<tdcSystem::LQuadF> QuadC;    /// quad C
  std::shared_ptr<tdcSystem::LQuadF> QuadD;    /// quad D
  std::shared_ptr<tdcSystem::LQuadF> QuadE;    /// quad E 

  /// yag station
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;
  /// yag screen
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment2(const std::string&);
  Segment2(const Segment2&);
  Segment2& operator=(const Segment2&);
  ~Segment2();

  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
