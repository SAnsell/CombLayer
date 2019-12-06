/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/shortOdin/shortODIN.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef essSystem_shortODIN_h
#define essSystem_shortODIN_h

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
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class ChopperHousing;
  class PinHole;
}

namespace essSystem
{
  class Bunker;
  class BunkerInsert;
  class Hut;
  class RentrantBS;
  
  /*!
    \class shortODIN
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class shortODIN : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint; 
  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffsetUnit> odinAxis;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> BladeChopper;

  /// Vac box for T0 chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxB;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0House;
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;
  /// T0 chopper [9-9.5m]
  std::shared_ptr<constructSystem::DiskChopper> T0Chopper;
  /// Pipe to bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeFinal;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusFinal;

  
  /// Tapper Unit
  std::shared_ptr<beamlineSystem::GuideLine> GuideA;
  /// Tapper Unit
  std::shared_ptr<beamlineSystem::GuideLine> GuideB;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Guide in the Bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> GuideC;
  /// Guide after the Bunker to first chopper
  std::shared_ptr<beamlineSystem::GuideLine> GuideD;

  /// Chopper pit for first outer bunker chopper
  std::shared_ptr<constructSystem::ChopperPit> PitA;
  /// Guide to Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitAFront;
  /// Guide from Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitABack;
  /// Guide from Chopper to exterior
  std::shared_ptr<constructSystem::DiskChopper> ChopperA;

  /// Guide from Chopper A to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuideE;


  /// Chopper pit for second choppers:
  std::shared_ptr<constructSystem::ChopperPit> PitB;
  /// Guide from Chopper to exterior [target]
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitBFront;
  /// Guide from Chopper to exterior [Hutch side]
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitBBack;
  /// Guide from Chopper to exterior
  std::shared_ptr<constructSystem::DiskChopper> ChopperB;
  /// Guide from chopper B to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuideF;


  /// Chopper pit for third choppers:
  std::shared_ptr<constructSystem::ChopperPit> PitC;
  /// Guide from Chopper to exterior [target]
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitCFront;
  /// Guide from Chopper to exterior [Hutch side]
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitCBack;


  /// Guide from chopper C to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuideG;

  /// The Hutch
  std::shared_ptr<Hut> Cave;
  /// Guide in Hutch 
  std::shared_ptr<beamlineSystem::GuideLine> GuideH;
  
  // Collimator
  std::shared_ptr<constructSystem::PinHole> PinA;

  // BeamStop
  std::shared_ptr<RentrantBS> BeamStop;

  void setBeamAxis(const attachSystem::FixedGroup&,const bool);

  static void
    buildChopperBlock(Simulation&,const Bunker&,
		      const attachSystem::FixedComp&,
		      const constructSystem::VacuumBox&,
		      constructSystem::VacuumBox&,
		      beamlineSystem::GuideLine&,
		      constructSystem::DiskChopper&,
		      constructSystem::ChopperHousing&,
		      constructSystem::VacuumPipe&);

 public:
  
  shortODIN(const std::string&);
  shortODIN(const shortODIN&);
  shortODIN& operator=(const shortODIN&);
  ~shortODIN();
  
  void build(Simulation&,const attachSystem::FixedGroup&,
	     const Bunker&,const int);

};

}

#endif
