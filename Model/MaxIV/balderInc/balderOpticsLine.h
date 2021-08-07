/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/balderOpticsLine.h
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
#ifndef xraySystem_balderOpticsLine_h
#define xraySystem_balderOpticsLine_h

namespace insertSystem
{
  class insertPlate;
}

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
  class OpticsHutch;
  class MonoVessel;
  class MonoCrystals;
  class FlangeMount;
  class Mirror;
  class MonoShutter;
  class PipeShield;
  class ShutterUnit;
  class TriggerTube;
    
  /*!
    \class balderOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class balderOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// Items pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;

  /// construction space for main object
  attachSystem::BlockZone buildZone;  
  int innerMat;                         ///< inner material if used
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;
  /// Real Ion pump (KF40) 10cm vertioal
  std::shared_ptr<constructSystem::VacuumPipe> pipeInit;
  /// vacuum trigger system
  std::shared_ptr<xraySystem::TriggerTube> triggerPipe;
  /// first ion pump+gate
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;
  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// Filter box
  std::shared_ptr<constructSystem::PortTube> filterBox;

  /// Filter box
  
  std::array<std::shared_ptr<xraySystem::FlangeMount>,4> filters;

  /// Joining Bellows (pipe):
  std::shared_ptr<constructSystem::Bellows> bellowB;

  /// CF40 gate valve
  std::shared_ptr<constructSystem::GateValveCube> gateA;

  /// Vertical mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBox;

  /// Vertical mirror
  std::shared_ptr<xraySystem::Mirror> mirror;

  /// Straight value cross piece (ion pump)
  std::shared_ptr<constructSystem::GateValveCube> gateB;

  /// Joining Bellows from mirror box
  std::shared_ptr<constructSystem:: Bellows> bellowC;

  /// Large drift chamber
  std::shared_ptr<constructSystem::VacuumPipe> driftA;

  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftB;

  /// Mono Vessel
  std::shared_ptr<xraySystem::MonoVessel> monoV;
  /// Mono crystal
  std::shared_ptr<xraySystem::MonoCrystals> monoXtal;
  
  /// Huge Bellows to Mono
  std::shared_ptr<constructSystem::Bellows> monoBellowA;
  /// Huge Bellows from Mono
  std::shared_ptr<constructSystem::Bellows> monoBellowB;
  
  /// Gate valve after mono [large]
  std::shared_ptr<constructSystem::GateValveCube> gateC;

  /// Large drift chamber post mono
  std::shared_ptr<constructSystem::VacuumPipe> driftC;

  /// Beam stop
  std::shared_ptr<insertSystem::insertPlate> beamStop;

  /// Slits [first pair]
  std::shared_ptr<constructSystem::JawValveCube> slitsA;

  /// Tungsten shield pipe
  std::shared_ptr<constructSystem::PortTube> shieldPipe;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> bellowD;

  /// Gate valve after mono [small]
  std::shared_ptr<constructSystem::GateValveCube> gateD;

  /// Vertical mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxB;

  /// Vertical mirror
  std::shared_ptr<xraySystem::Mirror> mirrorB;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> bellowE;

  /// Slits [first pair]
  std::shared_ptr<constructSystem::JawValveCube> slitsB;

  /// Pipe for diamond filter
  std::shared_ptr<constructSystem::PortTube> viewPipe;

  /// ViewPipe mounts
  std::array<std::shared_ptr<xraySystem::FlangeMount>,1> viewMount;

  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> bellowF;

  /// Shutter pipe
  std::shared_ptr<xraySystem::MonoShutter> monoShutter;
    
  /// Joining Bellows (pipe large):
  std::shared_ptr<constructSystem::Bellows> bellowG;

  /// Last gate valve:
  std::shared_ptr<constructSystem::GateValveCube> gateE;

  /// Last gate valve:
  std::array<std::shared_ptr<xraySystem::PipeShield>,4> neutShield;

  double outerLeft;    /// Left  for cut rectangle
  double outerRight;   /// Right for cut rectangle
  double outerTop;     /// Top for cut rectangle  

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:

  balderOpticsLine(const std::string&);
  balderOpticsLine(const balderOpticsLine&);
  balderOpticsLine& operator=(const balderOpticsLine&);
  ~balderOpticsLine();

  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  using FixedComp::createAll;  // for (Sim,FixedComp,string)
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
