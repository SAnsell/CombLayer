/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/ConnectZone.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
  class GateValveCube;
  class JawValveCube;
    
}

namespace xraySystem
{
  class FlangeMount;
  class LeadBox;
  class Mirror;
  class OpticsHutch;

    
  /*!
    \class ConnectZone
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class ConnectZone :
  public attachSystem::CopiedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap      
{
 private:

  attachSystem::InnerZone buildZone;
  
  /// First bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;

  std::shared_ptr<xraySystem::LeadBox> boxA;            ///< Lead box protecting bellow
  std::shared_ptr<constructSystem::LeadPipe> pipeA;    ///< Pipe from first bellow
  std::shared_ptr<constructSystem::PortTube> ionPumpA; ///< Ion pump port
  std::shared_ptr<xraySystem::LeadBox> pumpBoxA;       ///< Ion pump lead box

  std::shared_ptr<constructSystem::LeadPipe> pipeB;     ///< Pipe from ion pump
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< Second bellow
  std::shared_ptr<xraySystem::LeadBox> boxB;            ///<  box protecting bellow

  std::shared_ptr<constructSystem::LeadPipe> pipeC;     
  std::shared_ptr<constructSystem::PortTube> ionPumpB;
  std::shared_ptr<xraySystem::LeadBox> pumpBoxB;      ///< Ion pump lead box
  
  std::shared_ptr<constructSystem::LeadPipe> pipeD;  ///< join pipe
  std::shared_ptr<constructSystem::Bellows> bellowC; ///< third bellow
  std::shared_ptr<xraySystem::LeadBox> boxC;         ///< box protecting bellow

  /// REGISTERED Shared_ptr : constructed not owned:
  std::shared_ptr<constructSystem::LeadPipe> JPipe;
  
  double outerRadius;          ///< radius of contained void
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&,const attachSystem::FixedComp&,
		    const long int);
  void createLinks();
  
 public:
  
  ConnectZone(const std::string&);
  ConnectZone(const ConnectZone&);
  ConnectZone& operator=(const ConnectZone&);
  ~ConnectZone();

  /// Register pipe
  void registerJoinPipe(const std::shared_ptr<constructSystem::LeadPipe>& JP)
    { JPipe=JP; }
  
  void insertFirstRegion(Simulation&,const int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
