/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/HEIMDAL.h
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
#ifndef essSystem_HEIMDAL_h
#define essSystem_HEIMDAL_h

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
  class Cryostat;   
  class DiskChopper;
  class Jaws;
  class JawSet;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class SingleChopper;
  class Motor;
  class TwinChopper;
  class HoleShape;
  class CrystalMount;
  class TubeDetBox;  
}

namespace essSystem
{  
  class GuideItem;
  class HeimdalHut;
  class DetectorTank;

  /*!
    \class HEIMDAL
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief HEIMDAL beamline constructor for the ESS
  */
  
class HEIMDAL : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Cave]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> heimdalAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusTA;
  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusCA;

  /// Vac pipe in gamma shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusTB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusCB;

  /// 6.0 - 6.5m Vac piper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Cold guide from 6.0 to 6.5metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusTC;
  /// Thermal guide from 6.0 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusCC;

  /// First single chopper pair
  std::shared_ptr<constructSystem::SingleChopper> TChopA;
  /// Top twin disk
  std::shared_ptr<constructSystem::DiskChopper> ADiskOne;
  /// Lower twin disk
  std::shared_ptr<constructSystem::DiskChopper> ADiskTwo;

  /// 6.5m Vac pipe for Thermal
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTD;
  /// Themal guide from 6.5 to 8m
  std::shared_ptr<beamlineSystem::GuideLine> FocusTD;

  /// 6.5m to 12m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCD;
  /// Cold guide from 6.5 to 8m
  std::shared_ptr<beamlineSystem::GuideLine> FocusCD;
  /// Cold bender from 8m to 20m [too long?]
  std::shared_ptr<beamlineSystem::GuideLine> BendCD;

  /// Second thermal chopper [single]
  std::shared_ptr<constructSystem::SingleChopper> TChopB;
  /// Second Thermal disk
  std::shared_ptr<constructSystem::DiskChopper> BDisk;

  /// 8mm Vac pipe for Thermal
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTE;
  /// Themal guide from 8m to 20m [too long?]
  std::shared_ptr<beamlineSystem::GuideLine> FocusTE;

  /// Thermal T0 Chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperT0;
  /// Thermal T0 Chopper blades
  std::shared_ptr<constructSystem::DiskChopper> T0Disk;
  /// Thermal T0 Chopper motor
  std::shared_ptr<constructSystem::Motor> T0Motor;

  /// Vac pipe for Thermal to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTF;
  /// Themal guide from 20m to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusTF;


  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const attachSystem::FixedComp&,
			const long int,const int);
  
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);
  void buildDetectorArray(Simulation&,const attachSystem::FixedComp&,
			  const long int,const int);
  
 public:
  
  HEIMDAL(const std::string&);
  HEIMDAL(const HEIMDAL&);
  HEIMDAL& operator=(const HEIMDAL&);
  ~HEIMDAL();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
