/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VOR.h
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
  class ChopperUnit;
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
  
class VOR : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> vorAxis;

  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

   /// Pipe between in gamma sheild
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  /// Pipe from gamma shield to first chopper [at 9.5m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Elliptic guide from 6m to 9.5m
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;


  /// Pipe to bunker Insert
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Elliptic guide to bunker inset
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Elliptic forcus in bulkshield [m2.5] 
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;  

  /// Cave
  std::shared_ptr<DHut> Cave;
  
  /// Detector tank
  std::shared_ptr<DetectorTank> Tank;

  /// Sample
  std::shared_ptr<instrumentSystem::CylSample> Sample;

  
  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  
 public:
  
  VOR(const std::string&);
  VOR(const VOR&);
  VOR& operator=(const VOR&);
  ~VOR();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
