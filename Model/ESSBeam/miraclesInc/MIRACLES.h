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
  class ChopperUnit;
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
  class CompBInsert;
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

  /// Twin first chopper pair
  std::shared_ptr<constructSystem::ChopperUnit> ChopE;
  /// Top twin disk
  std::shared_ptr<constructSystem::DiskChopper> EDisk;

  /// Bunker insert
  std::shared_ptr<essSystem::CompBInsert> BInsert;
  /// Pipe in bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeWall;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;


  /// Vacuum pipe in the front of the cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCave;
  
  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
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
