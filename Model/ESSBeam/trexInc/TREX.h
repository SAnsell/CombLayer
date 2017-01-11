/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trexInc/TREX.h
 *
 * Copyright (c) 2004-2016 by Tsitohaina Randriamalala/Stuart Ansell
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

namespace constuctSystem
{
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class ChopperHousing;
  class ChopperUnit;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class PipeCollimator;
  class LineShield;
}

namespace essSystem
{
    
  class GuideItem;
  
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

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis
  std::shared_ptr<attachSystem::FixedOffset> trexAxis;
  
  /// Monolith guideline
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  /// Gamma Shield Vacuum
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Gamma Shield Guide
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  /// Bender Vacuum
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// First bender
  std::shared_ptr<beamlineSystem::GuideLine> BendC;

  /// Bunker Insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
      
  void setBeamAxis(const FuncDataBase&,const GuideItem&,
		   const bool);
    
 public:
  
  TREX(const std::string&);
  TREX(const TREX&);
  TREX& operator=(const TREX&);
  ~TREX();
  
  void build(Simulation&,const GuideItem&,const Bunker&,const int);
  
};

}

#endif

