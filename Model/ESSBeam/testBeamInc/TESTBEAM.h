/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/TESTBEAM.h
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
#ifndef essSystem_TESTBEAM_h
#define essSystem_TESTBEAM_h

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
  class Motor;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class SingleChopper;
  class TwinChopper;
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
    \class TESTBEAM
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief TESTBEAM beamline constructor for the ESS
  */
  
class TESTBEAM : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Cave]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> testAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Test item [Twin chopper]
  std::shared_ptr<constructSystem::TwinChopper> TwinA;

  /// Blade for Twin - chopper
  std::shared_ptr<constructSystem::DiskChopper> ADisk;
  /// Blade for Twin - chopper
  std::shared_ptr<constructSystem::DiskChopper> BDisk;

  /// Test item : T0 Chopper
  std::shared_ptr<constructSystem::SingleChopper> ChopperT0;
  /// Test item : T0 Chopper blades
  std::shared_ptr<constructSystem::DiskChopper> T0Disk;
  /// Test item : T0 Chopper motor
  std::shared_ptr<constructSystem::Motor> T0Motor;

  /// Test item [Crysotat]
  std::shared_ptr<constructSystem::Cryostat> CryoA;

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  
 public:
  
  TESTBEAM(const std::string&);
  TESTBEAM(const TESTBEAM&);
  TESTBEAM& operator=(const TESTBEAM&);
  ~TESTBEAM();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
