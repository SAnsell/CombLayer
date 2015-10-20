/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VOR.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_VOR_h
#define essSystem_VOR_h

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
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  class DHut;
  class DetectorTank;

  /*!
    \class VOR
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief VOR beamline constructor for the ESS
  */
  
class VOR
{
 private:

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedComp> vorAxis;

  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper Housing
  std::shared_ptr<constructSystem::ChopperHousing> DDiskHouse;
  /// Pipe between chopper 1 and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic forcus in bulkshield [m2.5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Elliptic forcus in bulkshield [m2.5] 
  std::shared_ptr<beamlineSystem::GuideLine> FocusBExtra;

   /// Chopper pit for first outer bunker chopper
  std::shared_ptr<constructSystem::ChopperPit> PitA;
  /// Guide from Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitAFront;
  /// Guide from Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitABack;
  /// Guide from Chopper to exterior
  std::shared_ptr<constructSystem::DiskChopper> ChopperA;

  /// Elliptic focus in bulkshield [m2.5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Chopper pit for first outer bunker chopper
  std::shared_ptr<constructSystem::ChopperPit> PitB;
  /// Guide from Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitBFront;
  /// Guide from Chopper to exterior
  std::shared_ptr<beamlineSystem::GuideLine> GuidePitBBack;
  /// Guide from Chopper to exterior
  std::shared_ptr<constructSystem::DiskChopper> ChopperB;

  /// Elliptic focus in bulkshield [m2.5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;

  /// Elliptic focus in bulkshield [m2.5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;


  /// Cave
  std::shared_ptr<DHut> Cave;
  /// Guide entering the cave
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;
  
  /// Detector tank
  std::shared_ptr<DetectorTank> Tank;

  /// Sample
  std::shared_ptr<instrumentSystem::CylSample> Sample;

  
  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  VOR();
  VOR(const VOR&);
  VOR& operator=(const VOR&);
  ~VOR();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
