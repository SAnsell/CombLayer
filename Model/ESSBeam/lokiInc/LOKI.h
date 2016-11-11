/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/LOKI.h
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
#ifndef essSystem_LOKI_h
#define essSystem_LOKI_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class ChopperUnit;
  class insertPlate;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class PipeCollimator;
}

namespace essSystem
{  
  class CompBInsert;
  class GuideItem;
  class LokiHut;
  class VacTank;

  /*!
    \class LOKI
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief LOKI beamline constructor for the ESS
  */
  
class LOKI : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;
  
  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> lokiAxis;

  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> BendA;
  /// Vacuum pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Shutter surround First pipe
  std::shared_ptr<constructSystem::insertPlate> ShutterA;
  /// Bender in gamma shield
  std::shared_ptr<beamlineSystem::GuideLine> BendB;


  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDiskA;

  /// Vacuum pipe to first chopper
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Guide to first chopper
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for second chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperAExtra;
  /// two single disks
  std::shared_ptr<constructSystem::DiskChopper> SDiskAEFirst;
  std::shared_ptr<constructSystem::DiskChopper> SDiskAESecond;

  /// Vacuum pipe from first chopper to second
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Guide from first to second chopper
  std::shared_ptr<beamlineSystem::GuideLine> BendD;

  /// Vac box for second chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDiskB;

  /// Vacuum pipe to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Guide to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;

  /// Vac box for third chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperC;
  /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDiskC;

  /// Vacuum pipe to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Guide to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;


  /// Vacuum ouside of the bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeFExtra;
  /// Guide outside bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusFExtra;

  /// Bunker insert
  //std::shared_ptr<essSystem::BunkerInsert> BInsert; // old and simple
  std::shared_ptr<essSystem::CompBInsert> BInsert;  
  /// Guide inside bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusFBunker;

   /// Chopper pit for first outer bunker chopper
  std::shared_ptr<constructSystem::ChopperPit> PitD;
    /// Vac box for fifth chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperD;
    /// Single disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDiskD;
//hole in choppet pit D to insert guide
  std::shared_ptr<constructSystem::HoleShape> CutD;

  //hole in choppet pit D to insert guide
  std::shared_ptr<constructSystem::HoleShape> CutG;
  // Piece after wall
  std::shared_ptr<constructSystem::LineShield> ShieldG;
    /// Vacuum pipe after bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeG;
  /// Guide after bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusG;
  /// Collimator block
  std::shared_ptr<constructSystem::PipeCollimator> CollG;
  /// Aperture after second collimator
  std::shared_ptr<constructSystem::Aperture> AppA;

   //Second collimator block, shielding around it, etc
  std::shared_ptr<constructSystem::LineShield> ShieldH;
  /// Vacuum pipe after bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeH;
  /// Guide after bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusH;
  //collimator block
  std::shared_ptr<constructSystem::PipeCollimator> CollH;
//aperture after second collimator
  std::shared_ptr<constructSystem::Aperture> AppB;
  

  /// Cave
  std::shared_ptr<LokiHut> Cave;
  /// Guide entering the cave
  std::shared_ptr<beamlineSystem::GuideLine> FocusK;
  /// Detector tank
  std::shared_ptr<VacTank> Tank;


  /// Cutting grid at bunker wall
  std::shared_ptr<constructSystem::RotaryCollimator> GridA;
  /// Rotary collimator [first to wall]
  //std::shared_ptr<constructSystem::PipeCollimator> CollG;
  /// Guide in CollA
  std::shared_ptr<beamlineSystem::GuideLine> FocusCA0;
  /// Guide in CollA
  std::shared_ptr<beamlineSystem::GuideLine> FocusCA1;
  /// Guide in CollA
  std::shared_ptr<beamlineSystem::GuideLine> FocusCA2;

  /// Space in wall for grid cutter
  std::shared_ptr<constructSystem::insertPlate> CBoxB;
  /// Cutting grid at bunker wall
  std::shared_ptr<constructSystem::RotaryCollimator> GridB;

  /// Rotary collimator [first to wall]
  std::shared_ptr<constructSystem::RotaryCollimator> CollB;

  /// Cutting grid at bunker wall
  std::shared_ptr<constructSystem::RotaryCollimator> GridC;

  /// Rotary collimator [first to wall]
  std::shared_ptr<constructSystem::RotaryCollimator> CollC;

  /// Final Cutting grid 
  std::shared_ptr<constructSystem::RotaryCollimator> GridD;
  
  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  
 public:
  
  LOKI(const std::string&);
  LOKI(const LOKI&);
  LOKI& operator=(const LOKI&);
  ~LOKI();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
