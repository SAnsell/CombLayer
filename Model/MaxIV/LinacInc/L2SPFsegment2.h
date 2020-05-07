/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment2.h
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
#ifndef tdcSystem_L2SPFsegment2_h
#define tdcSystem_L2SPFsegment2_h

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
  class CylGateTube;
}


/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace tdcSystem
{
  class LQuad;
  class CorrectorMag;
  class BPM;
  /*!
    \class L2SPFsegment2
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TCD from the linac
  */

class L2SPFsegment2 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< start pipe
  /// quad
  std::shared_ptr<tdcSystem::LQuad> QuadA;

  /// Beam position monitor
  std::shared_ptr<tdcSystem::BPM> bpmA;   ///< Beam pos mo

  /// Bellows from BPM
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// Long connecter pipe to gate valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; 
  
  std::shared_ptr<tdcSystem::LQuad> QuadB;    /// quad B 

  /// Gate valve holder
  std::shared_ptr<xraySystem::CylGateTube> gateTube;   

  /// gateValve - Beam Arriva lMonitor
  std::shared_ptr<constructSystem::VacuumPipe> pipeC; 
  
  /// Beam arraival monitor \todo BUILD
  std::shared_ptr<constructSystem::Bellows> beamArrivalMon;

  /// gateValve - Beam Arriva lMonitor
  std::shared_ptr<constructSystem::VacuumPipe> pipeD; 

  /// Bellows
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// BPM \todo BUILD A BPM
  std::shared_ptr<constructSystem::VacuumPipe> bpmB; 

  /// gateValve - Beam Arriva lMonitor
  std::shared_ptr<constructSystem::VacuumPipe> pipeE; 
  std::shared_ptr<tdcSystem::LQuad> QuadC;    /// quad C
  std::shared_ptr<tdcSystem::LQuad> QuadD;    /// quad D
  std::shared_ptr<tdcSystem::LQuad> QuadE;    /// quad E 
  
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  L2SPFsegment2(const std::string&);
  L2SPFsegment2(const L2SPFsegment2&);
  L2SPFsegment2& operator=(const L2SPFsegment2&);
  ~L2SPFsegment2();

  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
