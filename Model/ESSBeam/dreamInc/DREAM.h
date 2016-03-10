/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DREAM.h
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
#ifndef essSystem_DREAM_h
#define essSystem_DREAM_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace instrumentSystem
{
  class CylSample;
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
}

namespace essSystem
{  
  class GuideItem;
  class DreamHut;
  class DetectorTank;

  /*!
    \class DREAM
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief DREAM beamline constructor for the ESS
  */
  
class DREAM : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedComp> dreamAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  
  /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> SDisk;
  /// Single disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> SDiskHouse;


  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T0DiskA;
  /// T0 disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0DiskAHouse;

  /// Vac box for second part of T0
  std::shared_ptr<constructSystem::VacuumBox> VacBoxB;

  /// Pipe between chopper 1 and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  
  /// TO disk chopper [part B]  
  std::shared_ptr<constructSystem::DiskChopper> T0DiskB;  
  /// T0 disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0DiskBHouse;
  /// Elliptic guide between T0 fragments
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for second chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxC;
  /// Band Chopper
  std::shared_ptr<constructSystem::DiskChopper> BandADisk;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> BandAHouse;
  /// Pipe between T0 and Second chopper set
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;


  /// Vac box for second chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxD;
  /// Band Chopper 
  std::shared_ptr<constructSystem::DiskChopper> BandBDisk;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> BandBHouse;
  /// Pipe between T0 and Second chopper set
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  
  /// Vac box for Second T0  chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxE;
  /// T0 Chopper 
  std::shared_ptr<constructSystem::DiskChopper> T0DiskC;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0HouseC;
  /// Pipe between Pos3 and T0
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

    /// Vac box for Second T0  chopper set [Part B]
  std::shared_ptr<constructSystem::VacuumBox> VacBoxF;
  /// T0 Chopper 
  std::shared_ptr<constructSystem::DiskChopper> T0DiskD;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0HouseD;
  /// Pipe between Pos3 and T0
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;


  /// Pipe to bunker Wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeFinal;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusFinal;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;
  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;  

  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;  


  /// Cave 
  std::shared_ptr<DreamHut> Cave;


  static void
    buildChopperBlock(Simulation&,const Bunker&,
		      const attachSystem::FixedComp&,
		      const constructSystem::VacuumBox&,
		      constructSystem::VacuumBox&,
		      beamlineSystem::GuideLine&,
		      constructSystem::DiskChopper&,
		      constructSystem::ChopperHousing&,
		      constructSystem::VacuumPipe&);

  
  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  DREAM(const std::string&);
  DREAM(const DREAM&);
  DREAM& operator=(const DREAM&);
  ~DREAM();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
