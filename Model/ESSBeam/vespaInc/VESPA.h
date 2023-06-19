/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   vespaInc/VESPA.h
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
#ifndef essSystem_VESPA_h
#define essSystem_VESPA_h

namespace attachSystem
{
  class FixedComp;
  class CellMap;
}

namespace essConstruct
{
  class DiskChopper;  
  class SingleChopper;
  class TwinChopper;
  class TwinChopperFlat;
}

namespace beamlineSystem
{
  class BenderUnit;
  class PlateUnit;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace constructSystem
{  
  class ChopperPit;
  class Cryostat;   
  class Jaws;
  class JawSet;
  class LineShield;
  class TriangleShield;
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
  class VespaHut;
  class VespaInner;
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

  bool cryoFlag;           ///< Insert cryo
  
  /// Start at [0:Complete / 1:Cave]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedRotateUnit> vespaAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::PlateUnit> FocusA;

  /// Vac pipe  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusB;

  /// Vac pipe  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusC;

  /// PulseShapingChopper-A 
  std::shared_ptr<essConstruct::TwinChopperFlat> TwinChopperA;
  /// PSC -- top disk
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskTopA;
  /// PSC -- lower disk
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskBottomA;

  /// Joining pipe between Choppers: A-B
  std::shared_ptr<constructSystem::VacuumPipe> JPipeAB;
  /// Guide in Join AB
  std::shared_ptr<beamlineSystem::PlateUnit> FocusD;
  
  /// PulseShapingChopper-B 
  std::shared_ptr<essConstruct::TwinChopperFlat> TwinChopperB;
  /// PSC -- top disk 
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskTopB;
  /// PSC -- lower disk
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskBottomB;

  /// Joining pipe between Choppers: B-C
  std::shared_ptr<constructSystem::VacuumPipe> JPipeBC;
  /// Guide in Join AB
  std::shared_ptr<beamlineSystem::PlateUnit> FocusE;

  /// PulseShapingChopper-C 
  std::shared_ptr<essConstruct::TwinChopperFlat> TwinChopperC;
  /// PSC -- top disk 
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskTopC;
  /// PSC -- lower disk
  std::shared_ptr<essConstruct::DiskChopper> PSCDiskBottomC;

  /// Joining pipe between Choppers: C to Out
  std::shared_ptr<constructSystem::VacuumPipe> JPipeCOut;
  /// Guide in Join AB
  std::shared_ptr<beamlineSystem::PlateUnit> FocusF;

  /// General Pipe between PSC-C and FO-chopper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Guide for VPipeG
  std::shared_ptr<beamlineSystem::PlateUnit> FocusG;

  /// FO-Chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperFOC;
  /// FO-Chopper Disk
  std::shared_ptr<essConstruct::DiskChopper> FOCDisk;

  /// General Pipe between FOC and Bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeH;
  /// Guide for VPipeH
  std::shared_ptr<beamlineSystem::PlateUnit> FocusH;


  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Vac pipe in wall (if used)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::PlateUnit> FocusWall;


  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitT0;
  /// Quad chopper housing 
  std::shared_ptr<essConstruct::SingleChopper> ChopperT0;
  /// T0 chopper [15.5m]
  std::shared_ptr<essConstruct::DiskChopper> T0Disk;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> T0ExitPort;
  
  /// Shield for Chopper Out-A
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
  /// First outer shield section
  std::shared_ptr<constructSystem::TriangleShield> ShieldA;
  /// First Vac pipe out of bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Tapered guide out of bunker
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutA;

  /// Vac box for First Out-of-bunker chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperOutA;

  /// Shield for Chopper Out-B
  std::shared_ptr<constructSystem::ChopperPit> OutPitB;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> PitBPortA;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> PitBPortB;


  /// Double disk chopper [Frame overlap chopper]
  std::shared_ptr<essConstruct::DiskChopper> FOCDiskB;
  /// Shield out of PitA
  std::shared_ptr<constructSystem::TriangleShield> ShieldB;
  /// Vac pipe out of PitA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Tapered guide out of PitA
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutB;

  /// Shield from PitA to PitC
  std::vector<std::shared_ptr<constructSystem::TriangleShield>> ShieldArray;
  /// Vac pipe out of PitB to PitC
  std::vector<std::shared_ptr<constructSystem::VacuumPipe>> VPipeArray;
  /// Segment from PitB to Pit C
  std::vector<std::shared_ptr<beamlineSystem::PlateUnit>> FocusArray;
  
  /// Vac box for first chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperOutB;
  /// Double disk chopper [Wavelength Frame multiplication]
  std::shared_ptr<essConstruct::DiskChopper> FOCDiskOutB;

  /// Shield out of PitB
  std::shared_ptr<constructSystem::LineShield> ShieldC;
  /// Vac pipe to hutch
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  /// Tapered guide to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutC;

  /// Vespa hut
  std::shared_ptr<VespaHut> Cave;
  /// Vespa Inner cave
  std::shared_ptr<VespaInner> VInner;
  /// Vespa Inner cave exit
  std::shared_ptr<constructSystem::HoleShape> VInnerExit;

  /// Jaws in cave
  std::shared_ptr<constructSystem::JawSet> VJaws;
  /// Sample
  std::shared_ptr<instrumentSystem::CylSample> Sample;
  /// Cryostat
  std::shared_ptr<constructSystem::Cryostat> Cryo;

  /// Array of crystals
  std::vector<std::shared_ptr<constructSystem::CrystalMount>> XStalArray;
  /// Array of detectors
  std::vector<std::shared_ptr<constructSystem::TubeDetBox>> ADetArray;

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
  ~VESPA() override;
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
