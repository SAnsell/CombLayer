/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/beer/BEER.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef essSystem_BEER_h
#define essSystem_BEER_h

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

namespace constructSystem
{  
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class ChopperHousing;
  class ChopperPit;
  class HoleShape;
  class JawSet;
}

namespace essSystem
{  
  class GuideItem;
  class BeerHut;
  class DetectorTank;

  /*!
    \class BEER
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief BEER beamline constructor for the ESS
  */
  
class BEER :
    public attachSystem::CopiedComp,
    public attachSystem::ExternalCut
    
{
 private:

  /// Start at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedRotateUnit> beerAxis;

  /// Elliptic focus in monolith [m5]
  std::shared_ptr<beamlineSystem::BenderUnit> BendA;

  /// Pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::BenderUnit> BendB;

  /// Pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::BenderUnit> BendC;

  /// Vac box for first chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperA;
  /// Double disk chopper
  std::shared_ptr<essConstruct::DiskChopper> DDisk;

  /// Vac box for first chopper
  std::shared_ptr<essConstruct::SingleChopper> ChopperB;
  /// Double disk chopper
  std::shared_ptr<essConstruct::DiskChopper> WFMDisk;

  /// Pipe between chopper pairs
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::BenderUnit> BendD;

  /// 8.5m FOC 
  std::shared_ptr<essConstruct::SingleChopper> ChopperC;
  /// Double disk chopper
  std::shared_ptr<essConstruct::DiskChopper> FOCDiskC;

  /// Pipe between chopper pairs
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::BenderUnit> BendE;

  /// 10m 
  std::shared_ptr<essConstruct::SingleChopper> ChopperD;
  /// Double disk chopper [wbc2]
  std::shared_ptr<essConstruct::DiskChopper> WBC2Disk;

  /// Pipe from 10.0 to 11.1m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Bender in 10-11m pipe
  std::shared_ptr<beamlineSystem::BenderUnit> BendF;

  /// 11.1m FOC 
  std::shared_ptr<essConstruct::SingleChopper> ChopperE;
  /// Double disk chopper
  std::shared_ptr<essConstruct::DiskChopper> FOC2Disk;

  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Vac pipe in wall (if used)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;

  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::PlateUnit> FocusWall;

  /// Chopper at 15m [double pit]
  std::shared_ptr<constructSystem::ChopperPit> OutPitA;
    /// 15m WBC3 
  std::shared_ptr<constructSystem::HoleShape> OutACut;
  /// 15m WBC3 
  std::shared_ptr<essConstruct::SingleChopper> ChopperOutA;
  /// Double disk chopper (WBC3)
  std::shared_ptr<essConstruct::DiskChopper> WBC3Disk;

  /// 15m FOC3 
  std::shared_ptr<essConstruct::SingleChopper> ChopperOutB;
  /// Double disk chopper (FOC3)
  std::shared_ptr<essConstruct::DiskChopper> FOC3Disk;

  /// Jaws at 20m
  std::shared_ptr<constructSystem::ChopperPit> JawPit;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;  
  /// Pipe from 15.0 to 20m 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Beamline from bunker to hutch
  std::shared_ptr<beamlineSystem::PlateUnit> FocusOutA;

  /// Beamline from bunker to hutch
  std::shared_ptr<constructSystem::JawSet> CaveJaw;

  void insertChoppersInRoof(Simulation&,const Bunker&);
  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);

 public:
  
  BEER(const std::string&);
  BEER(const BEER&);
  BEER& operator=(const BEER&);
  ~BEER() override;

  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
