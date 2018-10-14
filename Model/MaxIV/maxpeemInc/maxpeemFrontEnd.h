/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/maxpeemFrontEnd.h
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
#ifndef xraySystem_maxpeemFrontEnd_h
#define xraySystem_maxpeemFrontEnd_h

namespace insertSystem
{
  class insertCylinder;
}

namespace constructSystem
{
  class Bellows;
  class CrossPipe;
  class GateValve;
  class OffsetFlangePipe;
  class portItem;
  class PipeTube;
  class PortTube;
  class SupplyPipe;
  class VacuumBox;
  class VacuumPipe; 
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
  class BremBlock;
  class BeamMount;
  class FlangeMount;
  class HeatDump;
  class LCollimator;
  class SquareFMask;
  class UTubePipe;
  class Undulator;

    
  /*!
    \class maxpeemFrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class maxpeemFrontEnd :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap  
{
 private:
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Wiggler in vacuum box
  std::shared_ptr<xraySystem::UTubePipe> undulatorPipe;
  /// Wiggler in vacuum box
  std::shared_ptr<xraySystem::Undulator> undulator;
  /// dipole connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> dipolePipe;
  /// electron cut cell
  std::shared_ptr<insertSystem::insertCylinder> eCutDisk;
  /// bellow infront of collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// FixedMask 1
  std::shared_ptr<xraySystem::SquareFMask> collA;
  /// bellow after FixedMask
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// Real Ion pump (KF40) 26cm vertioal
  std::shared_ptr<constructSystem::CrossPipe> ionPA;
  /// bellow after Ion Pump
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// Pipe to heat dump
  std::shared_ptr<constructSystem::VacuumPipe> heatPipe;
  /// Heat dump container
  std::shared_ptr<constructSystem::PipeTube> heatBox;
  /// Flange for heat shield
  std::shared_ptr<xraySystem::FlangeMount> heatTopFlange;
  /// Heat dump container
  std::shared_ptr<xraySystem::HeatDump> heatDump;
  /// bellow after HeatShield
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Gate box
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Real Ion pump (KF40) 26cm vertioal
  std::shared_ptr<constructSystem::CrossPipe> ionPB;
  /// Pipe to third optic table
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// bellows for third table
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// Variable Apperature pipe
  std::shared_ptr<constructSystem::VacuumPipe> aperturePipe;
  /// L collimator
  std::shared_ptr<xraySystem::LCollimator> moveCollA;
  /// bellows for third table
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// Real Ion pump (KF40) 26cm vertioal
  std::shared_ptr<constructSystem::CrossPipe> ionPC;

  /// bellows for second movable aperature
  std::shared_ptr<constructSystem::Bellows> bellowG;
  /// Variable Apperature pipe B
  std::shared_ptr<constructSystem::VacuumPipe> aperturePipeB;
  /// L collimator
  std::shared_ptr<xraySystem::LCollimator> moveCollB;
  /// bellows for exit of moveable aperatures
  std::shared_ptr<constructSystem::Bellows> bellowH;
  /// Exit of movables
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;

  /// Exit of movables
  std::shared_ptr<constructSystem::GateValve> gateA;
  /// bellows for florescence system
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// florescence screen tube
  std::shared_ptr<constructSystem::PipeTube> florTubeA;
  /// bellows for florescence system
  std::shared_ptr<constructSystem::Bellows> bellowJ;
  /// Gate box B
  std::shared_ptr<constructSystem::PipeTube> gateTubeB;
  /// Front port connection for shutterbox
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeA;
  /// Main shutters
  std::shared_ptr<constructSystem::PipeTube> shutterBox;
  /// Shutters
  std::array<std::shared_ptr<xraySystem::BeamMount>,2> shutters;
  /// Back port connection for shutterbox
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeB;
  /// Brem-block
  std::shared_ptr<xraySystem::BremBlock> bremBlock;
  /// Front port connection for shutterbox
  std::shared_ptr<constructSystem::Bellows> bellowK;

  double outerRadius;
   
  int createOuterVoidUnit(Simulation&,MonteCarlo::Object&,
			  const attachSystem::FixedComp&,
			  const long int);
 
  MonteCarlo::Object& constructMasterCell(Simulation&);
  void refrontMasterCell(MonteCarlo::Object&,
			 const attachSystem::FixedComp&,
			 const long int) const;

  void insertFlanges(Simulation&,const constructSystem::PipeTube&);
  void buildHeatTable(Simulation&,MonteCarlo::Object&);
  void buildApertureTable(Simulation&,MonteCarlo::Object&);
  void buildShutterTable(Simulation&,MonteCarlo::Object&);  

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  maxpeemFrontEnd(const std::string&);
  maxpeemFrontEnd(const maxpeemFrontEnd&);
  maxpeemFrontEnd& operator=(const maxpeemFrontEnd&);
  ~maxpeemFrontEnd();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
