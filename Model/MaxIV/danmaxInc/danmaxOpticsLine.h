/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxOpticsLine.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef xraySystem_danmaxOpticsLine_h
#define xraySystem_danmaxOpticsLine_h

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
  class PipeTube;
  class GateValveCube;
  class GateValveCylinder;
  class JawValveCube;
  class JawFlange;

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
  class BeamPair;
  class BremBlock;
  class BremMonoColl;
  class FlangeMount;
  class Mirror;
  class MonoBlockXstals;
  class DCMTank;

  
  /*!
    \class danmaxOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class danmaxOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// construction space for main object
  attachSystem::InnerZone buildZone;

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vacuum trigger system
  std::shared_ptr<constructSystem::PipeTube> triggerPipe;
  /// first ion pump
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Gate block [item]
  std::shared_ptr<xraySystem::FlangeMount> gateTubeAItem;

  /// bellows from trigger outward
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Mid trigger -> collimator
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// bellows to collimator
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// Collimator zone
  std::shared_ptr<constructSystem::PipeTube> collTubeA;
  /// Brem Collimator
  std::shared_ptr<xraySystem::BremBlock> bremColl;
  /// Filter unit pipe
  std::shared_ptr<constructSystem::VacuumPipe> filterPipe;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValveCylinder> gateA;
  /// bellows to laue mono:
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// Pipe for laue mono
  std::shared_ptr<constructSystem::VacuumPipe> lauePipe;
  /// bellows from laue mono
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Pipe for slit section
  std::shared_ptr<constructSystem::PortTube> slitTube;
  /// Jaws for the slit tube (x/z pair)
  std::array<std::shared_ptr<xraySystem::BeamPair>,2> jaws;
  // Gate after slittube
  std::shared_ptr<constructSystem::GateValveCylinder> gateB;
  /// bellows to laue mono:
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// Main mono vessel
  std::shared_ptr<xraySystem::DCMTank> monoVessel;
  /// Grating
  std::shared_ptr<xraySystem::MonoBlockXstals> mbXstals;
  /// Gate value BL-V3
  std::shared_ptr<constructSystem::GateValveCylinder> gateC;
  /// Beam viewer 2
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// Beam viewer [item]
  std::shared_ptr<xraySystem::FlangeMount> viewTubeScreen;

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle


  void constructSlitTube(Simulation&,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,const std::string&);
  void constructMono(Simulation&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const std::string&);
  void constructViewScreen(Simulation&,MonteCarlo::Object*,
			   const attachSystem::FixedComp&,const std::string&);
  
  int constructMonoShutter
    (Simulation&,MonteCarlo::Object**,
     const attachSystem::FixedComp&,const long int);

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);

  void createLinks();
  
 public:
  
  danmaxOpticsLine(const std::string&);
  danmaxOpticsLine(const danmaxOpticsLine&);
  danmaxOpticsLine& operator=(const danmaxOpticsLine&);
  ~danmaxOpticsLine();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
