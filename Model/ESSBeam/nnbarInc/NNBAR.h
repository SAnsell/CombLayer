/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/NNBAR.h
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
#ifndef essSystem_NNBAR_h
#define essSystem_NNBAR_h

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
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class SingleChopper;
  class HoleShape;
  class TubeDetBox;  
}

namespace essSystem
{  
  class GuideItem;
  class DetectorChamber;
  class DetectorTank;

  /*!
    \class NNBAR
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief NNBAR beamline constructor for the ESS
  */
  
class NNBAR : public attachSystem::CopiedComp
{
 private:

  /// Start at [0:Complete / 1:Cave]
  int startPoint;  
  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> nnbarAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  /// Vac pipe  [Rectangle]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Tapered guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Vac pipe  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Tapered guide from 6m to 10m
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;
  /// Vac pipe  
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Tapered guide from 19m to 30m
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;

  /// First Shield wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;  
  /// Vac pipe (out of bunker)
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Tapered guide from 30m to 50
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;

  /// Drift tube
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  
  /// Cave
  std::shared_ptr<DetectorChamber> Cave;
  /// Cave enterance hole
  std::shared_ptr<constructSystem::HoleShape> CaveCut;


  void buildBunkerUnits(Simulation&,const attachSystem::FixedComp&,
			const long int,const int);
  void buildOutGuide(Simulation&,const attachSystem::FixedComp&,
		     const long int,const int);
  void buildHut(Simulation&,const attachSystem::FixedComp&,
		const long int,const int);
  void buildDetectorArray(Simulation&,const attachSystem::FixedComp&,
			  const long int,const int);
  
 public:
  
  NNBAR(const std::string&);
  NNBAR(const NNBAR&);
  NNBAR& operator=(const NNBAR&);
  ~NNBAR();
  
  void buildIsolated(Simulation&,const int);
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
