/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/ODIN.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef essSystem_ODIN_h
#define essSystem_ODIN_h

namespace attachSystem
{
  class FixedComp;
  class FixedGroup;
  class TwinComp;
  class CellMap;
}

namespace essConstruct
{
    class SingleChopper;
}

namespace beamlineSystem
{
  class BenderUnit;
  class PlateUnit;
}


namespace constructSystem
{
  class ChopperPit;
  class SingleChopper;
  class DiskChopper;
  class HoleShape;
  class Jaws;
  class JawSet;
  class LineShield;
  class PinHole;
  class RotaryCollimator;
  class VacuumPipe;
}

namespace essSystem
{
  class Bunker;
  class BunkerInsert;
  class Hut;
  class RentrantBS;
  
  /*!
    \class ODIN
    \version 1.0
    \author S. Ansell
    \date May 2015
    \brief ODIN instrument builder
  */
  
class ODIN : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedRotateUnit> odinAxis;
  /// Elliptic guide in monolith
  std::shared_ptr<beamlineSystem::PlateUnit> FocusA;


  /// Pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusB;

  /// Pipe from gamma shield to first chopper:
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusC;

  /// Front chopper housing pair [movable]
  std::shared_ptr<essConstruct::SingleChopper> ChopperAA;
  /// Front  blades
  std::shared_ptr<essConstruct::DiskChopper> DiskAA;
  /// Back chopper housing pair [movable]
  std::shared_ptr<essConstruct::SingleChopper> ChopperAB;
  /// Back chopper bladed
  std::shared_ptr<essConstruct::DiskChopper> DiskAB;

  /// Pipe after T0 [4m section]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusD;

  /// T0 housing
  std::shared_ptr<essConstruct::SingleChopper> ChopperB;
  /// T0 chopper [8.150m]
  std::shared_ptr<essConstruct::DiskChopper> T0Disk;

  /// FOC1 housing
  std::shared_ptr<essConstruct::SingleChopper> ChopperFOC1;
  /// FOC1 chopper [8.50m]
  std::shared_ptr<essConstruct::DiskChopper> FOC1Disk;

  /// Section between FOC1 - FOC2
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusE;

  /// FOC2 housing
  std::shared_ptr<essConstruct::SingleChopper> ChopperFOC2;
  /// FOC2 chopper [11.84m]
  std::shared_ptr<essConstruct::DiskChopper> FOC2Disk;
  
  /// Pipe from FOC2 - FOC3
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusF;

  /// FOC3 housing [16.49m]
  std::shared_ptr<essConstruct::SingleChopper> ChopperFOC3;
  /// FOC3 chopper [16.49m]
  std::shared_ptr<essConstruct::DiskChopper> FOC3Disk;
  
  /// Second 4m section
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusG;
  
  /// FOC4 housing [22.97m]
  std::shared_ptr<essConstruct::SingleChopper> ChopperFOC4;
  /// FOC3 chopper [22.97m]
  std::shared_ptr<essConstruct::DiskChopper> FOC4Disk;

  /// Section of pipe to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeH;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::PlateUnit> FocusH;

  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::PlateUnit> FocusWall;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;  
  /// Pipe from 28.0 to 31.0m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutA;

  /// Chopper at 32.0m 
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACut;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutBCut;
  /// 32.0m FOC
  std::shared_ptr<essConstruct::SingleChopper> ChopOutFOC5;
  /// Singe disk chopper (FOC5)
  std::shared_ptr<essConstruct::DiskChopper> FOC5Disk;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldB;  
  /// Pipe from 15.0 to 22m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutB;

  /// Pipe from 22.0 to 30m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutC;

  /// Pipe from 30.0 to Cave 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutD;

  /// Pipe into cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutE;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutE;
  
  /// Main cave
  std::shared_ptr<essSystem::Hut> Cave;
  /// Enterance hole
  std::shared_ptr<constructSystem::HoleShape> CaveCut;
  /// Enterance hole
  std::shared_ptr<constructSystem::HoleShape> CaveMidCut;

  /// Beamguide in cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCaveA;
  /// Guide in cave
  std::shared_ptr<beamlineSystem::PlateUnit> FocusCaveA;

  /// Collimator
  std::shared_ptr<constructSystem::PinHole> PinA;

  /// BeamStop
  std::shared_ptr<RentrantBS> BeamStop;

  void setBeamAxis(const FuncDataBase&,
		   const attachSystem::FixedGroup&,const bool);

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildCave(Simulation&,const attachSystem::FixedComp&,
		 const long int,const int);
  
 public:
  
  ODIN(const std::string&);
  ODIN(const ODIN&);
  ODIN& operator=(const ODIN&);
  ~ODIN() override;
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
