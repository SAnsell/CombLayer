/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/MIRACLES.h
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
#ifndef essSystem_MIRACLES_h
#define essSystem_MIRACLES_h

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
  class Aperture;
  class BeamShutter;
  class SingleChopper;
  class ChopperPit;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class TwinChopper;
  class VacuumPipe;
  class VacuumWindow;


}

namespace essSystem
{
  class BInsert;
  class GuideItem;
  class DetectorTank;

  /*!
    \class MIRACLES
    \version 1.0
    \author S. Ansell
    \date July 2016
    \brief MIRACLES beamline constructor for the ESS
  */
  
class MIRACLES : public attachSystem::CopiedComp
{
 private:

  const size_t nGuideSection;      ///< Guide section in rect unit
  const size_t nSndSection;        ///< Guide section in snd-rec unit
  const size_t nEllSection;        ///< Guide section in final elliptic unit
  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> miraclesAxis;

  /// Elliptic focus in bulkshield [m3]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe in the gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Pipe in the gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Guide from 6m to 7m
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;
  /// Tungsten apperature after gamma focus
  std::shared_ptr<constructSystem::Aperture> AppA;

  /// Twin first chopper pair
  std::shared_ptr<constructSystem::TwinChopper> TwinB;
  /// Top twin disk
  std::shared_ptr<constructSystem::DiskChopper> BDiskTop;
  /// Lower twin disk
  std::shared_ptr<constructSystem::DiskChopper> BDiskLow;

  /// Pipe from Chopper A  to B
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Elliptic guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Twin second chopper pair
  std::shared_ptr<constructSystem::TwinChopper> TwinC;
  /// Top twin disk (2)
  std::shared_ptr<constructSystem::DiskChopper> CDiskTop;
  /// Lower twin disk (2)
  std::shared_ptr<constructSystem::DiskChopper> CDiskLow;

  /// Pipe from Chopper C to D
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Guide from 7.5 to 11.25 m
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// First single chopper pair
  std::shared_ptr<constructSystem::SingleChopper> ChopE;
  /// Top twin disk
  std::shared_ptr<constructSystem::DiskChopper> EDisk;
  
  /// Shutter
  std::shared_ptr<constructSystem::BeamShutter> ShutterA;

  /// Pipe from second single chopper 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Guide between single choppers
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Pipe around bender [rectangular?]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Bender in bunker
  std::shared_ptr<beamlineSystem::GuideLine> BendG;

  
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Pipe in bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;
  /// Vacuum pipe in the bunker wall if not provided by BInsert
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCave;

  /// Outer Shielding
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe around bender [rectangular?]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Bender in first shielding
  std::shared_ptr<beamlineSystem::GuideLine> BendOutA;

  /// Pipe around bender [rectangular?]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Bender in first shielding
  std::shared_ptr<beamlineSystem::GuideLine> BendOutB;

  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);

 public:
  
  MIRACLES(const std::string&);
  MIRACLES(const MIRACLES&);
  MIRACLES& operator=(const MIRACLES&);
  ~MIRACLES();

  void buildIsolated(Simulation&,const int);
    
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
