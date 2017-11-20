/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/MAGIC.h
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
#ifndef essSystem_MAGIC_h
#define essSystem_MAGIC_h

namespace attachSystem
{
  class FixedComp;
  class CellMap;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace constructSystem
{  
  class Aperture;
  class SingleChopper;
  class ChopperPit;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class MultiChannel;
  class RotaryCollimator;
  class VacuumPipe;
  class VacuumWindow;


}

namespace essSystem
{
  class CompBInsert;
  class GuideItem;
  class DetectorTank;
  class MagicHut;

  /*!
    \class MAGIC
    \version 1.0
    \author S. Ansell
    \date July 2016
    \brief MAGIC beamline constructor for the ESS
  */
  
class MAGIC : public attachSystem::CopiedComp
{
 private:
  
  /// Start at [0: Complete / 1:Bunker / 2:BunkerWall / 3:Cave ]
  int startPoint;
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> magicAxis;

  /// Elliptic focus in bulkshield [m3]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  /// Pipe in the gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;

  /// Pipe for multichannel bender
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// 6.2m 200xchannel si-bender [0.9deg 
  std::shared_ptr<beamlineSystem::GuideLine> BendC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> PSCDisk;

  /// Pipe in the gamma shield [6.5m to 13m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Elliptic focus from 6.5m
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  /// Pipe in the gamma shield [13m to 19m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Elliptic focus from 6.5m
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;
  /// Pipe in the gamma shield [19m to wall]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Elliptic focus from 6.5m
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Bunker insert pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

    /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;

  /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldC;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;

    /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldD;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutD;

    /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldE;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutE;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutE;

    /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldF;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutF;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutF;
  /// Polarizer Pit
  std::shared_ptr<constructSystem::ChopperPit> PolarizerPit;
  /// Holder for the multichannel guide
  std::shared_ptr<beamlineSystem::GuideLine> MCGuideA;
  /// Mutlichannel device
  std::shared_ptr<constructSystem::MultiChannel> MCInsertA;

  /// Holder for the multichannel guide
  std::shared_ptr<beamlineSystem::GuideLine> MCGuideB;
  /// Mutlichannel device
  std::shared_ptr<constructSystem::MultiChannel> MCInsertB;

  /// Shield 
  std::shared_ptr<constructSystem::LineShield> ShieldG;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutG;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutG;

  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppA;

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildPolarizer(Simulation&,const attachSystem::FixedComp&,
		      const long int,const attachSystem::FixedComp&,
		      const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);
  
  
 public:
  
  MAGIC(const std::string&);
  MAGIC(const MAGIC&);
  MAGIC& operator=(const MAGIC&);
  ~MAGIC();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,const Bunker&,const int);

};

}

#endif
