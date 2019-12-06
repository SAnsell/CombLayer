/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxConnectLine.h
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
#ifndef xraySystem_danmaxConnectLine_h
#define xraySystem_danmaxConnectLine_h

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
  class SqrShield;
  /*!
    \class danmaxConnectLine
    \version 1.0
    \author S. Ansell
    \date Decmeber 2019
    \brief Connection vacuum system between optics and expt
  */

class danmaxConnectLine :
  public attachSystem::FixedOffsetUnit,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap      
{
 private:

  /// build unit for divisions
  attachSystem::InnerZone buildZone;

  /// Main system
  std::shared_ptr<xraySystem::SqrShield> connectShield;
  /// First bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// Pipe from first bellow
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  std::shared_ptr<constructSystem::PortTube> ionPumpA; ///< Ion pump port

  /// Pipe from ion pump
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; 
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< Second bellow

  std::shared_ptr<constructSystem::VacuumPipe> pipeC;  ///< join pipe
  
  std::shared_ptr<constructSystem::PortTube> ionPumpB;
  
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;  ///< join pipe
  std::shared_ptr<constructSystem::Bellows> bellowC; ///< third bellow

  /// REGISTERED Shared_ptr : constructed not owned:
  std::shared_ptr<constructSystem::VacuumPipe> JPipe;
  
  void populate(const FuncDataBase&);
  void buildObjects(Simulation&,const attachSystem::FixedComp&,
		    const std::string&);
  void createLinks();
  
 public:
  
  danmaxConnectLine(const std::string&);
  danmaxConnectLine(const danmaxConnectLine&);
  danmaxConnectLine& operator=(const danmaxConnectLine&);
  virtual ~danmaxConnectLine();

  /// Register pipe
  void registerJoinPipe(const std::shared_ptr<constructSystem::VacuumPipe>& JP)
    { JPipe=JP; }
  
  void insertFirstRegion(Simulation&,const int);
  void construct(Simulation&,const attachSystem::FixedComp&,
		 const std::string&);

};

}

#endif
