/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DREAM.h
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
#ifndef essSystem_FREIA_h
#define essSystem_FREIA_h

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
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class ChopperHousing;
  class ChopperUnit;
}

namespace essSystem
{  
  class GuideItem;
  class FreiaHut;
  class DetectorTank;

  /*!
    \class FREIA
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief FREIA beamline constructor for the ESS
  */
  
class FREIA : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> freiaAxis;

  /// Elliptic focus in monolith [m5]
  std::shared_ptr<beamlineSystem::GuideLine> BendA;

  /// Pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::GuideLine> BendB;

  /// Pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Bender int monolith
  std::shared_ptr<beamlineSystem::GuideLine> BendC;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;

  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> WFMDisk;

  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  
 public:
  
  FREIA(const std::string&);
  FREIA(const FREIA&);
  FREIA& operator=(const FREIA&);
  ~FREIA();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
