/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/NMX.h
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
#ifndef essSystem_NMX_h
#define essSystem_NMX_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class BeamShutter;
  class DiskChopper;
  class ChopperPit;
  class Jaws;
  class RotaryCollimator;
  class VacuumBox;
  class ChopperHousing;
  class PipeCollimator;
}

namespace essSystem
{  
  class GuideItem;
  /*!
    \class NMX
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief NMX beamline constructor for the ESS
  */
  
class NMX : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> nmxAxis;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  
  /// Pipe between gamma shield / 10m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Bender in first pipe [6-10m]
  std::shared_ptr<beamlineSystem::GuideLine> BendA;

  /// Pipe between 10m-14m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Bender in second pipe [10-14m]
  std::shared_ptr<beamlineSystem::GuideLine> BendB;
  
  /// Pipe between 14m-18m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Bender in second pipe [14-18m]
  std::shared_ptr<beamlineSystem::GuideLine> BendC;

  /// Pipe between 18m-22m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Bender in second pipe [18m-22m]
  std::shared_ptr<beamlineSystem::GuideLine> BendD;

  /// Pipe between 22m-BunkerWall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Bender in second pipe [22m-Wall]
  std::shared_ptr<beamlineSystem::GuideLine> BendE;

  /// Collimator A 
  std::shared_ptr<constructSystem::PipeCollimator> CollA;

  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// Main beam shutter
  std::shared_ptr<constructSystem::BeamShutter> MainShutter;

  /// Piece after wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;
    
 public:
  
  NMX(const std::string&);
  NMX(const NMX&);
  NMX& operator=(const NMX&);
  ~NMX();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
