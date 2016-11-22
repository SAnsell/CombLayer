/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/ODIN.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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

namespace constructSystem
{


  class ChopperPit;
  class ChopperUnit;
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
  std::shared_ptr<attachSystem::FixedOffset> odinAxis;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Quad chopper housing
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Quad blades
  std::shared_ptr<constructSystem::DiskChopper> QDisk;

  /// Pipe after quad chopper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Basic tapered guid
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Quad chopper housing
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// T0 chopper [9-9.5m]
  std::shared_ptr<constructSystem::DiskChopper> T0Disk;

  /// Pipe after T0 [4m section]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Second 4m section
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// Second 4m section
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Second 4m section
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Basic tapered guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;
  

    /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// Chopper at 27.6m 
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACut;
  /// 27.9m FOC
  std::shared_ptr<constructSystem::ChopperUnit> ChopperOutA;
  /// Singe disk chopper (FOC5)
  std::shared_ptr<constructSystem::DiskChopper> FOC5Disk;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;  
  /// Pipe from 15.0 to 20m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

  /// Chopper at 37.9m
  std::shared_ptr<constructSystem::ChopperPit> OutPitB;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutBCutFront;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutBCutBack;

  /// 37.9m FOC
  std::shared_ptr<constructSystem::ChopperUnit> ChopperOutB;
  /// Singe disk chopper (FOC6)
  std::shared_ptr<constructSystem::DiskChopper> FOC6Disk;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldB;  
  /// Pipe from 15.0 to 22m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;

  /// Pipe from 22.0 to 30m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;

  /// Pipe from 30.0 to Cave 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutD;

  /// Pipe into cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutE;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutE;

  
  /// Main cave
  std::shared_ptr<essSystem::Hut> Cave;
  /// Enterance hole
  std::shared_ptr<constructSystem::HoleShape> CaveCut;

  /// Beamguide in cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCaveA;
  /// Guide in cave
  std::shared_ptr<beamlineSystem::GuideLine> FocusCaveA;

  /// Collimator
  std::shared_ptr<constructSystem::PinHole> PinA;

  /// BeamStop
  std::shared_ptr<RentrantBS> BeamStop;

  void setBeamAxis(const attachSystem::FixedGroup&,const bool);
  
 public:
  
  ODIN(const std::string&);
  ODIN(const ODIN&);
  ODIN& operator=(const ODIN&);
  ~ODIN();
  
  void build(Simulation&,const attachSystem::FixedGroup&,
	     const Bunker&,const int);

};

}

#endif
