/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxConnectLine.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::FrontBackCut,
    public attachSystem::CellMap      
{
 private:

  /// build unit for divisions
  attachSystem::BlockZone buildZone;

  /// Main system
  std::shared_ptr<xraySystem::SqrShield> connectShield;

  /// Pipe out
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;    
  std::shared_ptr<constructSystem::Bellows> bellowA;    ///< Bellow to pump
  std::shared_ptr<constructSystem::VacuumPipe> flangeA;  ///< adaptor flange

  std::shared_ptr<constructSystem::PipeTube> ionPumpA; ///< Ion pump port
  std::shared_ptr<constructSystem::VacuumPipe> flangeB;  ///< adaptor flange
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< Second bellow
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   // out pipe

  /// REGISTERED Shared_ptr : constructed not owned:
  std::shared_ptr<constructSystem::VacuumPipe> JPipe;

  void buildObjects(Simulation&,const attachSystem::FixedComp&,
		    const long int);
  
 public:
  
  danmaxConnectLine(const std::string&);
  danmaxConnectLine(const danmaxConnectLine&);
  danmaxConnectLine& operator=(const danmaxConnectLine&);
  virtual ~danmaxConnectLine();

  /// Register pipe
  void registerJoinPipe(const std::shared_ptr<constructSystem::VacuumPipe>& JP)
    { JPipe=JP; }
  
  void insertFirstRegion(Simulation&,const int);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
