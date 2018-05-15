/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VOR.h
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
#ifndef essSystem_VOR_h
#define essSystem_VOR_h

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
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class SingleChopper;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  class DHut;
  class DetectorTank;

  /*!
    \class VOR
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief VOR beamline constructor for the ESS
  */
  
class VOR : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Mono Wall / 2:Cave ]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> vorAxis;

  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

   /// Pipe between in gamma sheild
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  /// Pipe from gamma shield to first chopper [at 9.5m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Elliptic guide from 6m to 9.5m
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;


  /// Pipe to bunker Insert
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Elliptic guide to bunker inset
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Pipe in bunker Insert
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Elliptic forcus in bulkshield [m2.5] 
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;


  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperOutA;
  /// FOC disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDisk;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> FOCExitPort;

  
  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;  

  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitB;
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperOutB;
  /// FOC disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskB;
  /// Collimator hole [front]
  std::shared_ptr<constructSystem::HoleShape> FOCEntryPortB;
  /// Collimator hole [exit]
  std::shared_ptr<constructSystem::HoleShape> FOCExitPortB;

    /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;  
  
  /// Cave
  std::shared_ptr<DHut> Cave;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> CavePort;
  /// Detector tank
  std::shared_ptr<DetectorTank> Tank;

  /// Sample
  std::shared_ptr<instrumentSystem::CylSample> Sample;

  
  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);
  void buildDetectorArray(Simulation&,const attachSystem::FixedComp&,
			  const long int,const int);

 public:
  
  VOR(const std::string&);
  VOR(const VOR&);
  VOR& operator=(const VOR&);
  ~VOR();

  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
