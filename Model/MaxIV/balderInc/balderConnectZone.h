/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/balderConnectZone.h
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
#ifndef xraySystem_balderConnectZone_h
#define xraySystem_balderConnectZone_h

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
  class LeadPipe;
    
}

namespace xraySystem
{
  class FlangeMount;
  class LeadBox;
  class PipeShield;
  class Mirror;
  class OpticsHutch;
    
  /*!
    \class balderConnectZone
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class balderConnectZone :
  public attachSystem::CopiedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap      
{
 private:

  /// Items pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;

  attachSystem::BlockZone buildZone;    ///< main block divider
  int innerMat;                         ///< inner material if used
  
  /// First bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;

  std::shared_ptr<xraySystem::LeadBox> boxA;           ///< Lead box protecting bellow
  std::shared_ptr<constructSystem::LeadPipe> pipeA;    ///< Pipe from first bellow
  std::shared_ptr<constructSystem::PipeTube> ionPumpA; ///< Ion pump port
  std::shared_ptr<xraySystem::LeadBox> pumpBoxA;       ///< Ion pump lead box

  std::shared_ptr<xraySystem::PipeShield> pumpBoxAFShield;  ///< shield 
  
  std::shared_ptr<constructSystem::LeadPipe> pipeB;     ///< Pipe from ion pump
  std::shared_ptr<constructSystem::Bellows> bellowB;    ///< Second bellow
  std::shared_ptr<xraySystem::LeadBox> boxB;            ///<  box protecting bellow

  std::shared_ptr<constructSystem::LeadPipe> pipeC;     
  std::shared_ptr<constructSystem::PipeTube> ionPumpB;
  std::shared_ptr<xraySystem::LeadBox> pumpBoxB;      ///< Ion pump lead box
  
  std::shared_ptr<constructSystem::LeadPipe> pipeD;  ///< join pipe
  std::shared_ptr<constructSystem::Bellows> bellowC; ///< third bellow
  std::shared_ptr<xraySystem::LeadBox> boxC;         ///< box protecting bellow

  /// REGISTERED Shared_ptr : constructed not owned:
  std::shared_ptr<constructSystem::LeadPipe> JPipe;
  
  double outerRadius;          ///< radius of contained void

			
  template<typename MidTYPE>
  void makeBox(Simulation&,
	       const constructSystem::LeadPipe&,const long int,
	       LeadBox&,MidTYPE&,constructSystem::LeadPipe&);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&,const attachSystem::FixedComp&,
		    const long int);
  void createLinks();
  
 public:
  
  balderConnectZone(const std::string&);
  balderConnectZone(const balderConnectZone&);
  balderConnectZone& operator=(const balderConnectZone&);
  ~balderConnectZone();

  /// external registration
  void registerJoinPipe(std::shared_ptr<constructSystem::LeadPipe> JP)
    { JPipe=JP; }
 
  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
