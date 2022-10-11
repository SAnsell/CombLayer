/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/heimdal/HEIMDAL.h
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
#ifndef essSystem_HEIMDAL_h
#define essSystem_HEIMDAL_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace beamlineSystem
{
  class PlateUnit;
  class BenderUnit;
}

namespace instrumentSystem
{
  class CylSample;
}


namespace essConstruct
{
  class DiskChopper;  
  class SingleChopper;
  class TwinChopper;
  class TwinChopperFlat;
  class Motor;
}

namespace constructSystem
{  
  class Cryostat;   
  class Jaws;
  class JawSet;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
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
  std::shared_ptr<attachSystem::FixedRotateUnit> heimdalAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTA;
  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::PlateUnit> FocusCA;

  /// Vac pipe in gamma shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusCB;

  /// 6.0 - 6.5m Vac piper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Cold guide from 6.0 to 6.5metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTC;
  /// Thermal guide from 6.0 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusCC;

  /// First single chopper pair
  std::shared_ptr<essConstruct::SingleChopper> TChopA;
  /// Top twin disk
  std::shared_ptr<essConstruct::DiskChopper> ADiskOne;
  /// Lower twin disk
  std::shared_ptr<essConstruct::DiskChopper> ADiskTwo;

  /// 6.5m Vac pipe for Thermal
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTD;
  /// Themal guide from 6.5 to 8m
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTD;

  /// 6.5m to 12m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCD;
  /// Cold guide from 6.5 to 8m
  std::shared_ptr<beamlineSystem::PlateUnit> FocusCD;
  /// Cold bender from 8m to 20m [too long?]
  std::shared_ptr<beamlineSystem::BenderUnit> BendCD;

  /// Second thermal chopper [single]
  std::shared_ptr<essConstruct::SingleChopper> TChopB;
  /// Second Thermal disk
  std::shared_ptr<essConstruct::DiskChopper> BDisk;

  /// 8mm Vac pipe for Thermal
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTE;
  /// Themal guide from 8m to 20m [too long?]
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTE;

  /// Thermal T0 Chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperT0;
  /// Thermal T0 Chopper blades
  std::shared_ptr<essConstruct::DiskChopper> T0Disk;
  /// Thermal T0 Chopper motor
  std::shared_ptr<essConstruct::Motor> T0Motor;

  /// Vac pipe for Thermal to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeTF;
  /// Themal guide from 20m to bunker wall
  std::shared_ptr<beamlineSystem::PlateUnit> FocusTF;


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
