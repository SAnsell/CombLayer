/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Inc/R1FrontEnd.h
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
#ifndef xraySystem_R1FrontEnd_h
#define xraySystem_R1FrontEnd_h

namespace constructSystem
{
  class Bellows;
  class CrossPipe;
  class GateValveCube;
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
  class CylGateValve;
  class DipoleChamber;
  class FlangeMount;
  class HeatDump;
  class LCollimator;
  class MagnetBlock;
  class QuadUnit;
  class Quadrupole;
  class SquareFMask;
  class UTubePipe;
  class Undulator;

    
  /*!
    \class R1FrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class R1FrontEnd :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap  
{
 protected:   

  /// insert cells for magnet block
  std::set<int> magnetCells;
  
  /// point to stop [normal none]
  std::string stopPoint;          
  /// Inner buildzone
  attachSystem::BlockZone buildZone;
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;
  
  /// Gate unit
  std::shared_ptr<constructSystem::GateValveCube> elecGateA;

  /// Quad unit
  std::shared_ptr<xraySystem::MagnetBlock> magnetBlock;

  /// dipole connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> dipolePipe;
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
  /// Heat dump container
  std::shared_ptr<xraySystem::HeatDump> heatDump;
  /// bellow after HeatShield
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Gate box
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;
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
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  /// bellows for florescence system
  std::shared_ptr<constructSystem::Bellows> bellowI;
  /// florescence screen tube
  std::shared_ptr<constructSystem::PipeTube> florTubeA;
  /// bellows for florescence system
  std::shared_ptr<constructSystem::Bellows> bellowJ;
  /// Gate box B
  std::shared_ptr<xraySystem::CylGateValve> gateTubeB;
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

  double outerLeft;     ///< left size of tube for divisions
  double outerRight;    ///< right of tube for divisions
  double outerFront;    ///< front side offset if needed
  
  virtual const attachSystem::FixedComp&
  buildUndulator(Simulation&,
		   const attachSystem::FixedComp&,
		   const std::string&) =0;
  
  void buildHeatTable(Simulation&,
		      const attachSystem::FixedComp&,
		      const std::string&);
  void buildApertureTable(Simulation&,
			  const attachSystem::FixedComp&,
			  const std::string&);
  void buildShutterTable(Simulation&,
			 const attachSystem::FixedComp&,
			 const std::string&);

  void processEnd(Simulation&,
		  std::shared_ptr<attachSystem::FixedComp>);
  
  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces();
  virtual void buildObjects(Simulation&);
  virtual void createLinks() =0;
  
 public:
  
  R1FrontEnd(const std::string&);
  R1FrontEnd(const R1FrontEnd&);
  R1FrontEnd& operator=(const R1FrontEnd&);
  virtual ~R1FrontEnd();

  /// insert a magnet cells
  void addInsertMagnetCell(const int CN) { magnetCells.emplace(CN); }
  void setStopPoint(const std::string& S) { stopPoint=S; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
