/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/LOKI.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_LOKI_h
#define essSystem_LOKI_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class TwinComp;
  class CellMap;
}
namespace insertSystem
{
  class insertPlate;
}

namespace constructSystem
{  
  class ChopperPit;
  class SingleChopper;
  class Jaws;
  class DiskChopper;
  class PipeCollimator;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
}

namespace essSystem
{  
  class GuideItem;
  class LokiHut;
  class VacTank;

  /*!
    \class LOKI
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief LOKI beamline constructor for the ESS
  */
  
class LOKI : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int startPoint;
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;
  
  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> lokiAxis;

  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> BendA;
  /// Shutter around pipe in gamma block
  std::shared_ptr<insertSystem::insertPlate> ShutterA;
  /// Vacuum pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Bender in gamma shield
  std::shared_ptr<beamlineSystem::GuideLine> BendB;

  /// Vacuum pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBLink;
  /// Bender in gamma shield
  std::shared_ptr<beamlineSystem::GuideLine> BendBLink;


  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDiskA;

  /// Vacuum pipe to first chopper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Guide to first chopper
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Bunker insert [specialized]
  std::shared_ptr<essSystem::CompBInsert> BInsert;
  /// Vac pipe in wall (if used)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Gamma 
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// Shield for ChopperPit merged with bunker wall
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Exit of pit A
  std::shared_ptr<constructSystem::HoleShape> PitACut;
  /// vac box for first chopper out of bunker
  std::shared_ptr<constructSystem::SingleChopper> ChopperOutA;
  /// double disks for first chopper out of bunker
  std::shared_ptr<constructSystem::DiskChopper> DDiskOutA;

  /// First shielding after bunker wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;

  /// Vac pipe in OutPitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Tapered guide in OutPitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;
  /// Aperture after first guide unit in shielding
  std::shared_ptr<constructSystem::Aperture> AppA;
  /// Collimator block
  std::shared_ptr<constructSystem::PipeCollimator> CollA;

  /// Second collimator block, shielding around it, etc
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Second outer vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Guide after first appater
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;
  /// collimator blockB
  std::shared_ptr<constructSystem::PipeCollimator> CollB;
  //aperture after second collimator
  std::shared_ptr<constructSystem::Aperture> AppB;

  /// Cave
  std::shared_ptr<LokiHut> Cave;
  /// cave entrace vacuum pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Guide into cave
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;

  /// Guide to sample
  std::shared_ptr<beamlineSystem::GuideLine> CaveGuide;
  /// Vacuum tank
  std::shared_ptr<VacTank> VTank;

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);

  void registerObjects();
  
 public:
  
  LOKI(const std::string&);
  LOKI(const LOKI&);
  LOKI& operator=(const LOKI&);
  ~LOKI();

  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
