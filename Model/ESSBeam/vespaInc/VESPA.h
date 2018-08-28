/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vespaInc/VESPA.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef essSystem_VESPA_h
#define essSystem_VESPA_h

namespace attachSystem
{
  class FixedComp;
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
  class TriangleShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class SingleChopper;
  class TwinChopper;
  class HoleShape;
  class CrystalMount;
  class TubeDetBox;
  class TwinChopperFlat;
}

namespace essSystem
{
  class LightShutter;
  class HeavyShutter;
  class HorseCollar;
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
  
  bool HorseCollar_exist = false;
  
  /// Start at [0:Complete / 1:Cave]
  int startPoint;
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;
  
  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> vespaAxis;
  
  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  
  /// Light Shutter
  std::shared_ptr<essSystem::LightShutter> LShutter;
  
  /// Vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeLS;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusLS;
  
  /// Vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  
  /// Common vacuum box: first element
  std::shared_ptr<constructSystem::VacuumPipe> JPipeAIn;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;
  
  /// PulseShapingChopper-A 
  std::shared_ptr<constructSystem::TwinChopperFlat> TwinChopperA;
  /// PSC-A -- top disk
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskTopA;
  /// PSC-A -- lower disk
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskBottomA;
  
  /// Common vacuum box: between Chopper A and Chopper B
  std::shared_ptr<constructSystem::VacuumPipe> JPipeAB;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  
  /// PulseShapingChopper-B 
  std::shared_ptr<constructSystem::TwinChopperFlat> TwinChopperB;
  /// PSC-B -- top disk 
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskTopB;
  /// PSC-B -- lower disk
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskBottomB;
  
  /// Common vacuum box: between Chopper B and Chopper C
  std::shared_ptr<constructSystem::VacuumPipe> JPipeBC;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;
  
  /// PulseShapingChopper-C 
  std::shared_ptr<constructSystem::TwinChopperFlat> TwinChopperC;
  /// PSC-C -- top disk 
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskTopC;
  /// PSC-C -- lower disk
  std::shared_ptr<constructSystem::DiskChopper> PSCDiskBottomC;
  
  /// Common vacuum box: last section
  std::shared_ptr<constructSystem::VacuumPipe> JPipeCOut;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  
  /// Vac pipe between Chopper C and Chopper FO
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;
  
  /// Chopper FO
  std::shared_ptr<constructSystem::SingleChopper> ChopperFOC;
  /// Chopper FO Disk
  std::shared_ptr<constructSystem::DiskChopper> FOCDisk;
  
  /// Vac pipe between Chopper FO and Heavy Shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeH;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusH;
  
  /// Horse Collar
  std::shared_ptr<essSystem::HorseCollar> HCollar;
  
  /// Heavy Shutter
  std::shared_ptr<essSystem::HeavyShutter> HShutter;

  /// Vac pipe inside Heavy Shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeHS;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusHS;
  
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Vac pipe in wall (if used)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;
  
  /// Chopper T0 Pit
  std::shared_ptr<constructSystem::ChopperPit> ChopperT0Pit;
  /// Chopper T0
  std::shared_ptr<constructSystem::SingleChopper> ChopperT0;
  /// Chopper T0 Disk
  std::shared_ptr<constructSystem::DiskChopper> T0Disk;
  /// Collimator hole 
  std::shared_ptr<constructSystem::HoleShape> T0ExitPort;
  
  /// First triangular shielding from Chopper FOC to Chopper sFOC
  std::shared_ptr<constructSystem::TriangleShield> ShieldA;
  /// Vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeI;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusI;
  
  /// Chopper sFOC Pit
  std::shared_ptr<constructSystem::ChopperPit> ChoppersFOCPit;
  /// Chopper sFOC
  std::shared_ptr<constructSystem::SingleChopper> ChoppersFOC;
  /// Chopper sFOC Disk
  std::shared_ptr<constructSystem::DiskChopper> SFOCDisk;
    
  /// Second triangular shielding from Chopper sFOC to the array of Tr. Shields
  std::shared_ptr<constructSystem::TriangleShield> ShieldB;
  /// Vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeJ;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusJ;
  
  /// Shield from Chopper sFOC Pit to ShielC
  std::vector<std::shared_ptr<constructSystem::TriangleShield>> ShieldArray;
  /// Vac pipe array
  std::vector<std::shared_ptr<constructSystem::VacuumPipe>> VPipeArray;
  /// Guide array
  std::vector<std::shared_ptr<beamlineSystem::GuideLine>> FocusArray;
  
  /// Shield from ShieldArray to hutch
  std::shared_ptr<constructSystem::LineShield> ShieldC;
  /// Vac pipe
  std::shared_ptr<constructSystem::VacuumPipe> VPipeK;
  /// Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusK;
  
  /// Vespa Hutch
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
  
  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,const long int,const int);
  void buildDetectorArray(Simulation&,const attachSystem::FixedComp&,const long int,const int);
  
 public:
  
  VESPA(const std::string&);
  VESPA(const VESPA&);
  VESPA& operator=(const VESPA&);
  ~VESPA();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,const Bunker&,const int);
  
};

}

#endif
