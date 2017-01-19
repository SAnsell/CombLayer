/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VESPA.h
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
#ifndef essSystem_VESPA_h
#define essSystem_VESPA_h

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
  class JawSet;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class ChopperHousing;
  class ChopperUnit;
  class HoleShape;
  class CrystalMount;
  class TubeDetBox;  
}

namespace essSystem
{  
  class GuideItem;
  class VespaHut;
  class DetectorTank;

  /*!
    \class VESPA
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief VESPA beamline constructor for the ESS
  */
  
class VESPA : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Cave]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> vespaAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Vac pipe  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<constructSystem::DiskChopper> WFMDiskA;
  
  /// Vac pipe 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Tapered guide from WFMA to WFMB
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<constructSystem::DiskChopper> WFMDiskB;

  /// Vac pipe in 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Rectangular guide from WFMB to WFMC
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperC;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<constructSystem::DiskChopper> WFMDiskC;

  /// Vac pipe in 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Rectangular guide from WFMB to WFMC
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperD;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskA;

  /// Vac pipe in 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Rectangular guide from WFMB to WFMC
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Vac pipe in wall (if used)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;


  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitT0;
  /// Quad chopper housing 
  std::shared_ptr<constructSystem::ChopperUnit> ChopperT0;
  /// T0 chopper [15.5m]
  std::shared_ptr<constructSystem::DiskChopper> T0Disk;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> T0ExitPort;


  
  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// First Vac pipe out of bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Tapered guide out of bunker
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

  /// Vac box for First Out-of-bunker chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperOutA;

  /// Shield for Chopper Out-B
  std::shared_ptr<constructSystem::ChopperPit> OutPitB;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> PitBPortA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> PitBPortB;


  /// Double disk chopper [Frame overlap chopper]
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskB;
  /// Shield out of PitA
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;

  /// Shield from PitA to PitC
  std::vector<std::shared_ptr<constructSystem::LineShield>> ShieldArray;
  /// Vac pipe out of PitB to PitC
  std::vector<std::shared_ptr<constructSystem::VacuumPipe>> VPipeArray;
  /// Segment from PitB to Pit C
  std::vector<std::shared_ptr<beamlineSystem::GuideLine>> FocusArray;
  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperOutB;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<constructSystem::DiskChopper> FOCDiskOutB;

  /// Shield out of PitB
  std::shared_ptr<constructSystem::LineShield> ShieldC;
  /// Vac pipe to hutch
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Tapered guide to hutch
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutC;

  /// Vespa hut
  std::shared_ptr<VespaHut> Cave;

  /// Jaws in cave
  std::shared_ptr<constructSystem::JawSet> VJaws;

  /// Sample
  std::shared_ptr<instrumentSystem::CylSample> Sample;

  /// Array of crystals and detectors
  std::vector<std::shared_ptr<constructSystem::CrystalMount>> XStalArray;
  std::vector<std::shared_ptr<constructSystem::TubeDetBox>> ADetArray;

  void setBeamAxis(const GuideItem&,const bool);

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);
  void buildDetectorArray(Simulation&,const attachSystem::FixedComp&,
			  const long int,const int);
  
 public:
  
  VESPA(const std::string&);
  VESPA(const VESPA&);
  VESPA& operator=(const VESPA&);
  ~VESPA();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
