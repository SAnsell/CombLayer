/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/longLOKI.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_longLOKI_h
#define essSystem_longLOKI_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  class LokiHut;
  class VacTank;

  /*!
    \class longLOKI
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief longLOKI beamline constructor for the ESS
  */
  
class longLOKI
{
 private:

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffsetUnit> lokiAxis;

  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> BendA;
  /// Vac box for first two choppers
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Straight section to first chopper pit
  std::shared_ptr<beamlineSystem::GuideLine> GuideA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  /// Straight section between choppers
  std::shared_ptr<beamlineSystem::GuideLine> GuideInner;
  /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> SDisk;
  /// Single disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> SDiskHouse;
  /// Guide leaving single chopper 
  std::shared_ptr<beamlineSystem::GuideLine> GuideB;
  /// Bender in opposite direciton
  std::shared_ptr<beamlineSystem::GuideLine> BendB;
  /// Guide after bender to chopper pit B
  std::shared_ptr<beamlineSystem::GuideLine> GuideC;
  /// Vac box for second single box
  std::shared_ptr<constructSystem::VacuumBox> VacBoxB;
  /// Single disk chopper at 10m
  std::shared_ptr<constructSystem::DiskChopper> SingleDisk;
  /// 10m disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> SingleDiskHouse;
  /// Guide after 10m to 12m chopper pit
  std::shared_ptr<beamlineSystem::GuideLine> GuideD;
  /// Vac box for second single box
  std::shared_ptr<constructSystem::VacuumBox> VacBoxC;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> D12mDisk;
  /// Double disk 12m chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> D12mDiskHouse;
  /// Guide between single and double choppers
  std::shared_ptr<beamlineSystem::GuideLine> Guide12mInter;
  /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> S12mDisk;
  /// Single disk 12m chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> S12mDiskHouse;
  /// Guide leaving chopper pit 3 to collimator section
  std::shared_ptr<beamlineSystem::GuideLine> GuideE;

  /// Collimator unit selector A
  std::shared_ptr<constructSystem::RotaryCollimator> GridA;
  /// Collimator unit
  std::shared_ptr<constructSystem::RotaryCollimator> CollA;
  /// Collimator unit selector A
  std::shared_ptr<constructSystem::RotaryCollimator> GridB;
  /// Collimator unit
  std::shared_ptr<constructSystem::RotaryCollimator> CollB;
  /// Collimator unit selector A
  std::shared_ptr<constructSystem::RotaryCollimator> GridC;
  /// Collimator unit
  std::shared_ptr<constructSystem::RotaryCollimator> CollC;
  /// Collimator unit selector A
  std::shared_ptr<constructSystem::RotaryCollimator> GridD;

  /// Guide in collimator A
  std::shared_ptr<beamlineSystem::GuideLine> GuideCollA;
  /// Guide in collimator B
  std::shared_ptr<beamlineSystem::GuideLine> GuideCollB;
  /// Guide in collimator C
  std::shared_ptr<beamlineSystem::GuideLine> GuideCollC;

  /// Cave
  std::shared_ptr<LokiHut> Cave;
  /// Guide to sample
  std::shared_ptr<beamlineSystem::GuideLine> CaveGuide;
  /// Vacuum tank
  std::shared_ptr<VacTank> VTank;

  /// Vacuum Pipes
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  
  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  
 public:
  
  longLOKI();
  longLOKI(const longLOKI&);
  longLOKI& operator=(const longLOKI&);
  ~longLOKI();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
