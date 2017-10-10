/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DREAM.h
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
  class SingleChopper;
  class Aperture;
  class PipeCollimator;
}

namespace essSystem
{  
  class CompBInsert;
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
  std::shared_ptr<attachSystem::FixedOffset> dreamAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> SDisk;

  std::shared_ptr<constructSystem::PipeCollimator> CollimA;
  std::shared_ptr<constructSystem::PipeCollimator> CollimB;
  std::shared_ptr<constructSystem::PipeCollimator> CollimC;

  /// Empty Pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC0;

  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for BandPass chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperB;
  /// Band Chopper
  std::shared_ptr<constructSystem::DiskChopper> BandADisk;

  /// Pipe between T0 chopper and BandA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  
  /// Vac box for T0 chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperC;
  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T0DiskA;

  /// Pipe between  T0 and heavy shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE1;
  std::shared_ptr<beamlineSystem::GuideLine> FocusE1;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE2;
  std::shared_ptr<beamlineSystem::GuideLine> FocusE2;

    /// Heavy SHutter 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;

  /// Bunker insert
  std::shared_ptr<essSystem::CompBInsert> BInsertA;
  std::shared_ptr<essSystem::CompBInsert> BInsertB;
  std::shared_ptr<beamlineSystem::GuideLine> FocusWallA;

  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;  

  std::shared_ptr<constructSystem::LineShield> ShieldB;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;  

  std::shared_ptr<constructSystem::LineShield> ShieldC;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;  

  /// Cave 
  std::shared_ptr<DreamHut> Cave;

  /// Pipe entering cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCaveA;
  /// Guide in cave 
  std::shared_ptr<beamlineSystem::GuideLine> FocusCaveA;  
  std::shared_ptr<beamlineSystem::GuideLine> FocusCaveB;
 
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
