/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trexInc/TREX.h
 *
 * Copyright (c) 2004-2022 by Tsitohaina Randriamalala/Stuart Ansell
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
#ifndef essSystem_TREX_h
#define essSystem_TREX_h

class Simulation;


namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace essConstruct
{
  class SingleChopper;
  class DiskChopper;
}

namespace beamlineSystem
{
  class BenderUnit;
  class PlateUnit;
}

namespace constuctSystem
{
  class Jaws;
  class ChopperPit;
  class ChopperHousing;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class PipeCollimator;
  class LineShield;
  class HoleShape;
}

namespace essSystem
{
  class CompBInsert;
  class GuideItem;
  class TrexHut;
  
  /*!
    \class TREX
    \version 1.0
    \author T. Randriamalala
    \date October 2016
    \brief TREX beamline constructor for the ESS
  */



class TREX : public attachSystem::CopiedComp
{
 private:

  static constexpr size_t nC=7;     // number of C shield units
  static constexpr size_t nF=8;     // number of focus units
  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis
  std::shared_ptr<attachSystem::FixedRotateUnit> trexAxis;
  
  /// Monolith guideline
  std::shared_ptr<beamlineSystem::PlateUnit> FocusMono;

  /// Bridge guide in the light shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeBridge;
  std::shared_ptr<beamlineSystem::PlateUnit> FocusBridge;

  /// First Bender inside Bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeInA;
  std::shared_ptr<beamlineSystem::BenderUnit> BendInA;

  std::shared_ptr<constructSystem::VacuumPipe> VPipeInB;
  std::shared_ptr<beamlineSystem::BenderUnit> BendInB;

  std::shared_ptr<constructSystem::PipeCollimator> CollimA;
  std::shared_ptr<constructSystem::PipeCollimator> CollimB;
  std::shared_ptr<constructSystem::PipeCollimator> CollimC;

  /// !! Heavy Shutter !!
  std::shared_ptr<constructSystem::VacuumPipe> VPipeInC;
  std::shared_ptr<beamlineSystem::BenderUnit> BendInC;
  
  /// Bunker Insert
  std::shared_ptr<essSystem::CompBInsert> BInsertA;
  std::shared_ptr<beamlineSystem::BenderUnit> FocusWallA;
  std::shared_ptr<essSystem::CompBInsert> BInsertB;
  std::shared_ptr<beamlineSystem::BenderUnit> FocusWallB;

  /// Structure Up to the First Chopper Pit
  std::shared_ptr<constructSystem::ChopperPit> PitA;
  std::shared_ptr<constructSystem::HoleShape> PitACutFront;
  std::shared_ptr<constructSystem::HoleShape> PitACutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperA;
  std::shared_ptr<essConstruct::DiskChopper> DiskA;
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  std::shared_ptr<beamlineSystem::BenderUnit> BendOutA;

  /// Structure Up to the Second Chopper Pit
  std::shared_ptr<constructSystem::ChopperPit> PitB;
  std::shared_ptr<constructSystem::HoleShape> PitBCutFront;
  std::shared_ptr<constructSystem::HoleShape> PitBCutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperB;
  std::shared_ptr<essConstruct::DiskChopper> DiskB;
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  std::shared_ptr<beamlineSystem::BenderUnit> BendOutB;

  /// Structur up to the T0 chopper position
  std::shared_ptr<constructSystem::LineShield> ShieldC;
  std::shared_ptr<constructSystem::ChopperPit> PitC;
  std::shared_ptr<constructSystem::HoleShape> PitCCutFront;
  std::shared_ptr<constructSystem::HoleShape> PitCCutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperC;
  std::shared_ptr<essConstruct::DiskChopper> DiskC;


  std::array<std::shared_ptr<constructSystem::VacuumPipe>,nC> VPipeOutCs;
  std::array<std::shared_ptr<beamlineSystem::BenderUnit>,nC> BendOutCs;

  /// Last part of the curve part
  std::shared_ptr<constructSystem::LineShield> ShieldD;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  std::shared_ptr<beamlineSystem::BenderUnit> BendOutD;

  /// First Straight Beamline up to the next ChopperPit
  std::shared_ptr<constructSystem::ChopperPit> PitE;
  std::shared_ptr<constructSystem::HoleShape> PitECutFront;
  std::shared_ptr<constructSystem::HoleShape> PitECutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperE;
  std::shared_ptr<essConstruct::DiskChopper> DiskE1;
  std::shared_ptr<essConstruct::DiskChopper> DiskE2;
  std::shared_ptr<constructSystem::LineShield> ShieldE;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutE;
  std::shared_ptr<beamlineSystem::PlateUnit> GuideOutE;

  std::shared_ptr<constructSystem::LineShield> ShieldF;
  std::array<std::shared_ptr<constructSystem::VacuumPipe>,8> VPipeOutFs;
  std::array<std::shared_ptr<beamlineSystem::PlateUnit>,8> GuideOutFs;

  std::shared_ptr<TrexHut> Cave;
  std::shared_ptr<constructSystem::HoleShape> CaveFrontCut;
 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutG;
  std::shared_ptr<beamlineSystem::PlateUnit> GuideOutG;

  std::shared_ptr<essConstruct::SingleChopper> ChopperG;
  std::shared_ptr<essConstruct::DiskChopper> DiskG;
  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutH;
  std::shared_ptr<beamlineSystem::PlateUnit> GuideOutH;

  std::shared_ptr<essConstruct::SingleChopper> ChopperH;
  std::shared_ptr<essConstruct::DiskChopper> DiskH1;
  std::shared_ptr<essConstruct::DiskChopper> DiskH2;

  std::shared_ptr<beamlineSystem::PlateUnit> GuideOutI;
 
  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
                        const long int,const int);
  void buildBunkerWallUnits(Simulation&,const Bunker&,
			    const attachSystem::FixedComp&,
			    const long int,const int);
  
  
 public:
  
  TREX(const std::string&);
  TREX(const TREX&);
  TREX& operator=(const TREX&);
  ~TREX() override;
  
  void build(Simulation&,const GuideItem&,const Bunker&,const int);
  
};

}

#endif

