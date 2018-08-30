/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/maxpeemOpticsBeamline.h
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
#ifndef xraySystem_maxpeemOpticsBeamline_h
#define xraySystem_maxpeemOpticsBeamline_h

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
  class GrateMonoBox;
  class MonoCrystals;
  class FlangeMount;
  class Mirror;
  class PipeShield;
  class JawValve;
  class TwinPipe;
    
  /*!
    \class maxpeemOpticsBeamline
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class maxpeemOpticsBeamline :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Bellows to ionPump
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Real Ion pump (KF40) 24.4cm vertical
  std::shared_ptr<constructSystem::CrossPipe> ionPA;
  /// Gate block
  std::shared_ptr<constructSystem::PipeTube> gateTubeA;
  /// Bellow to first connect line
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// Pipe to some stuff
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// Flor-port ???
  std::shared_ptr<constructSystem::PipeTube> florTubeA;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// Pipe to some stuff
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// collimator-port ?
  std::shared_ptr<constructSystem::PipeTube> pumpTubeA;
  /// Front port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeA;
  /// M1 - Mirror box
  std::shared_ptr<constructSystem::PipeTube> M1Tube;
  /// back port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeB;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValve> gateA;
  /// Large Pipe to slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// Small Pipe to slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  /// Small Pipe to slit section
  std::shared_ptr<constructSystem::PipeTube> slitTube;
  /// Small Pipe to gate-valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValve> gateB;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Pipe exiting slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeF;
  /// Mono Box
  std::shared_ptr<xraySystem::GrateMonoBox> monoB;
  /// Pipe exiting slit section
  std::shared_ptr<constructSystem::VacuumPipe> pipeG;
  /// Gate valve
  std::shared_ptr<constructSystem::GateValve> gateC;
  /// Bellow to to super section
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// Small Pipe to slits on before M3
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// Slits [second pair]
  std::shared_ptr<constructSystem::JawValve> slitsB;
  /// Second pumping port
  std::shared_ptr<constructSystem::PipeTube> pumpTubeB;
  /// Front port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeC;
  /// M3 - Mirror box
  std::shared_ptr<constructSystem::PipeTube> M3Tube;
  /// back port of mirror box
  std::shared_ptr<constructSystem::OffsetFlangePipe> offPipeD;
  /// Splitter
  std::shared_ptr<xraySystem::TwinPipe> splitter;


  MonteCarlo::Object* masterCellA;
  MonteCarlo::Object* masterCellB;
  
  void constructMasterCell(Simulation&,const HeadRule&);
  int constructDivideCell(Simulation&,const bool,
			   const attachSystem::FixedComp&,
			   const long int,
			   const attachSystem::FixedComp&,
			   const long int);
  
  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  const HeadRule&,HeadRule&,
			  const attachSystem::FixedComp&,
			  const long int);

  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  HeadRule&,
			  const attachSystem::FixedComp&,
			  const long int);

  int createDoubleVoidUnit(Simulation&,
			   HeadRule&,
			   const attachSystem::FixedComp&,
			   const long int);
  
  void refrontMasterCell(MonteCarlo::Object*,
			 const attachSystem::FixedComp&,
			 const long int) const;
  
  void refrontMasterCell(MonteCarlo::Object*,MonteCarlo::Object*,
			 const attachSystem::FixedComp&,
			 const long int) const;
  
  void insertFlanges(Simulation&,const constructSystem::PipeTube&);
  

  void buildM3Mirror(Simulation&,HeadRule&,MonteCarlo::Object*,
		     const attachSystem::FixedComp&,const long int);
  void buildMono(Simulation&,HeadRule&,MonteCarlo::Object*,
		 const attachSystem::FixedComp&,const long int);
  void buildSlitPackage(Simulation&,HeadRule&,MonteCarlo::Object*,
		       const attachSystem::FixedComp&,const long int);
  void buildSplitter(Simulation&,HeadRule&,HeadRule&,
		     const attachSystem::FixedComp&,const long int);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  maxpeemOpticsBeamline(const std::string&);
  maxpeemOpticsBeamline(const maxpeemOpticsBeamline&);
  maxpeemOpticsBeamline& operator=(const maxpeemOpticsBeamline&);
  ~maxpeemOpticsBeamline();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
