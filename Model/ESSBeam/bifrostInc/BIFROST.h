/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BIFROST.h
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
#ifndef essSystem_BIFROST_h
#define essSystem_BIFROST_h

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
  class Aperture;
  class SingleChopper;
  class ChopperPit;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumPipe;
  class VacuumWindow;


}

namespace essSystem
{
  class CompBInsert;
  class GuideItem;
  class DetectorTank;
  class BifrostHut;

  /*!
    \class BIFROST
    \version 1.0
    \author S. Ansell
    \date July 2016
    \brief BIFROST beamline constructor for the ESS
  */
  
class BIFROST : public attachSystem::CopiedComp
{
 private:

  const size_t nGuideSection;      ///< Guide section in rect unit
  const size_t nSndSection;        ///< Guide section in snd-rec unit
  const size_t nEllSection;        ///< Guide section in final elliptic unit
  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> bifrostAxis;

  /// Elliptic focus in bulkshield [m3]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe in the gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppA;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  
  /// Pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Elliptic guide leaving first chopper
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// 10.5m FOC 
  std::shared_ptr<constructSystem::SingleChopper> ChopperB;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskB;

  /// Pipe from first chopper [6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Straight guide leaving second
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Second pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Straight guide in second length
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// 20.5m FOC-2
  std::shared_ptr<constructSystem::SingleChopper> ChopperC;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskC;

  /// Pipe from first chopper [4m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// 4m elliptic leaving FOC2
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppB;

  /// Bunker insert
  std::shared_ptr<essSystem::CompBInsert> BInsert;
  /// Pipe in bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;  
  /// First vacuum pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// First guide leaving bunker
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

  /// First vacuum pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// First guide leaving bunker
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;

  /// Last ellispe vacuum pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Last ellipse section
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;

  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,8> RecPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,8> RecFocus;

  /// Chopper pit
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACutFront;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> OutACutBack;
  /// First out of bunker chopper 
  std::shared_ptr<constructSystem::SingleChopper> ChopperOutA;
  /// single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskOutA;
  
  /// Second shield wall : chopperOutA to Cave
  std::shared_ptr<constructSystem::LineShield> ShieldB;  

  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,8> SndPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,7> SndFocus;

  /// Vector of rectangular vac-pipe:
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,4> EllPipe;
  /// Vector of rectangular units:
  std::array<std::shared_ptr<beamlineSystem::GuideLine>,4> EllFocus;

  /// single disk chopper
  std::shared_ptr<BifrostHut> Cave;
  /// Enterance hole to cave
  std::shared_ptr<constructSystem::HoleShape> CaveCut;

  /// Vacuum pipe in the front of the cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCave;
    
 public:
  
  BIFROST(const std::string&);
  BIFROST(const BIFROST&);
  BIFROST& operator=(const BIFROST&);
  ~BIFROST();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
