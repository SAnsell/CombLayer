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
  class VacuumWindow;
  class ChopperHousing;
  class ChopperUnit;
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

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> SDisk;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Guide between diskchoppers  / T0
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperC;
  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T0DiskA;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusT0Mid;
  /// TO disk chopper [part B]  
  std::shared_ptr<constructSystem::DiskChopper> T0DiskB;  
  

  /// Pipe between T0 chopper and BandA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Guide between diskchoppers  / T0
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperD;
  /// Band Chopper
  std::shared_ptr<constructSystem::DiskChopper> BandADisk;  


  /// Pipe between  BandA and Band B 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Guide between BandA and Band B
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// Vac box for second band chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperE;  
  /// Band Chopper 
  std::shared_ptr<constructSystem::DiskChopper> BandBDisk;  


    /// Pipe between  BandA and Band B 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Guide between BandA and Band B
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Vac box for second band chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperF;  

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperG;
  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T1DiskA;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusT1Mid;
  /// TO disk chopper [part B]  
  std::shared_ptr<constructSystem::DiskChopper> T1DiskB;  

  /// Pipe to Wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Focus unit to wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;


  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  
  /// Double disk chopper Housing
  //  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  

  /// Single disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> SDiskHouse;


  /// T0 disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0DiskAHouse;

  /// Vac box for second part of T0
  std::shared_ptr<constructSystem::VacuumBox> VacBoxB;
  
  /// T0 disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0DiskBHouse;

  /// Vac box for second chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxC;
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> BandAHouse;
  /// Pipe between T0 and Second chopper set
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;


  /// Vac box for second chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxD;
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> BandBHouse;

  
  /// Vac box for Second T0  chopper set
  std::shared_ptr<constructSystem::VacuumBox> VacBoxE;
  /// T0 Chopper 
  std::shared_ptr<constructSystem::DiskChopper> T0DiskC;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0HouseC;

    /// Vac box for Second T0  chopper set [Part B]
  std::shared_ptr<constructSystem::VacuumBox> VacBoxF;
  /// T0 Chopper 
  std::shared_ptr<constructSystem::DiskChopper> T0DiskD;  
  /// Band width chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> T0HouseD;


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
