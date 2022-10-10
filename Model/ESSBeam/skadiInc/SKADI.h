/********************************************************************* 
  CombLayer : MCNP(X) put builder
 
 * File:   ESSBeam/skadic/SKADI.h
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
#ifndef essSystem_SKADI_h
#define essSystem_SKADI_h

class Simulation;


namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace constuctSystem
{
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class ChopperHousing;
  class SingleChopper;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class PipeCollimator;
  class LineShield;
  class HoleShape;
  class Aperture;
}

namespace essSystem
{
  class CompBsert;
  class GuideItem;
  class SkadiHut;
  
  /*!
    \class SKADI
    \version 1.0
    \author T. Randriamalala
    \date October 2016
    \brief SKADI beamline constructor for the ESS
  */



class SKADI : public attachSystem::CopiedComp
{
 private:

   /// Stop at [0:Complete / 1:Mono Wall / 2:ner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis
  std::shared_ptr<attachSystem::FixedRotateUnit> skadiAxis;
  
  /// Monolith guideline
  std::shared_ptr<beamlineSystem::GuideUnit> BendA;

  /// VacPipe in the light shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// bender guide in the light shutter
  std::shared_ptr<beamlineSystem::GuideUnit> BendB;

  /// Pipe A inside Bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Benders inside Bunker
  std::shared_ptr<beamlineSystem::GuideUnit> BendC;

  /// Vac-pipe for S-Bend inbetween section 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  ///  S-Bend inbetween section 
  std::shared_ptr<beamlineSystem::GuideUnit> BendD;

  /// Last vac-pipe in S-Bend
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Last bender in S-Bend
  std::shared_ptr<beamlineSystem::GuideUnit> BendE;

  /// Straight vac pipe for heavy shutter space
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  std::shared_ptr<beamlineSystem::GuideUnit> FocusF;

  /// Collamator directly after gamma shield
  std::shared_ptr<constructSystem::PipeCollimator> CollA;
  /// Collamator mid - S bend
  std::shared_ptr<constructSystem::PipeCollimator> CollB;
  /// Collamator before bunker wall
  std::shared_ptr<constructSystem::PipeCollimator> CollC;
  
  /// Bunker sert
  std::shared_ptr<essSystem::CompBInsert> BInsert;
  std::shared_ptr<beamlineSystem::GuideUnit> FocusWallA;
  std::shared_ptr<essSystem::CompBInsert> CInsert;
  std::shared_ptr<beamlineSystem::GuideUnit> FocusWallB;

  /// Structure Up to the First Chopper I  Pit
  std::shared_ptr<constructSystem::LineShield> ShieldA;

  // Instrument shutter
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  std::shared_ptr<beamlineSystem::GuideUnit> GuideOutA;

  std::shared_ptr<constructSystem::ChopperPit> PitA;
  std::shared_ptr<constructSystem::HoleShape> PitACutFront;
  std::shared_ptr<constructSystem::HoleShape> PitACutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperA;
  std::shared_ptr<essConstruct::Motor> ChopAMotor;
  std::shared_ptr<essConstruct::DiskChopper> DiskA;

  /// Structure Up to the First Chopper II  Pit

  std::shared_ptr<constructSystem::LineShield> ShieldB;
  std::shared_ptr<constructSystem::LineShield> ShieldB1;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  std::shared_ptr<beamlineSystem::GuideUnit> GuideOutB;

  std::shared_ptr<constructSystem::ChopperPit> PitB;
  std::shared_ptr<constructSystem::HoleShape> PitBCutFront;
  std::shared_ptr<constructSystem::HoleShape> PitBCutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperB;
  std::shared_ptr<essConstruct::DiskChopper> DiskB;

  /// Structure Up to the First Chopper III  Pit

  std::shared_ptr<constructSystem::LineShield> ShieldC;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutC;
  std::shared_ptr<beamlineSystem::GuideUnit> GuideOutC;

  std::shared_ptr<constructSystem::ChopperPit> PitC;
  std::shared_ptr<constructSystem::HoleShape> PitCCutFront;
  std::shared_ptr<constructSystem::HoleShape> PitCCutBack;
  std::shared_ptr<essConstruct::SingleChopper> ChopperC1;
  std::shared_ptr<essConstruct::DiskChopper> DiskC1;
  std::shared_ptr<essConstruct::SingleChopper> ChopperC2;
  std::shared_ptr<essConstruct::DiskChopper> DiskC2;

  std::shared_ptr<constructSystem::LineShield> ShieldD;
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutD;
  std::shared_ptr<beamlineSystem::GuideUnit> GuideOutD;
  
  std::shared_ptr<beamlineSystem::GuideUnit> GuideOutE;
  
  std::shared_ptr<SkadiHut> Cave;
  std::shared_ptr<constructSystem::HoleShape> CaveFrontCut;

  std::shared_ptr<constructSystem::LineShield> ShieldF; 

  void buildBunkerUnits(Simulation&,
			const attachSystem::FixedComp&,
			const long int,const int);

 public:
  
  SKADI(const std::string&);
  SKADI(const SKADI&);
  SKADI& operator=(const SKADI&);
  ~SKADI();
  
  void build(Simulation&,const GuideItem&,const Bunker&,const int);
  
};

}

#endif

