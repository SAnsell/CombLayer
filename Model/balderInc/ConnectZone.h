/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/ConnectZone.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_ConnectZone_h
#define xraySystem_ConnectZone_h

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class portItem;
  class PortTube;
  class GateValve;
  class JawValve;
    
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  class OpticsHutch;
  class MonoVessel;
  class MonoCrystals;
  class FlangeMount;
  class Mirror;
    
  /*!
    \class ConnectZone
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class ConnectZone :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  /// First bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;
  std::shared_ptr<constructSystem::LeadPipe> pipeA;
  std::shared_ptr<constructSystem::PortTube> ionPumpA;
  std::shared_ptr<constructSystem::LeadPipe> pipeB;
  std::shared_ptr<constructSystem::Bellows> bellowB;
  std::shared_ptr<constructSystem::LeadPipe> pipeC;
  std::shared_ptr<constructSystem::PortTube> ionPumpB;
  std::shared_ptr<constructSystem::LeadPipe> pipeD;
  std::shared_ptr<constructSystem::Bellows> bellowC;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void buildObjects(Simulation&,const attachSystem::FixedComp&,
		    const long int);

  void createLinks();
  
 public:
  
  ConnectZone(const std::string&);
  ConnectZone(const ConnectZone&);
  ConnectZone& operator=(const ConnectZone&);
  ~ConnectZone();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
