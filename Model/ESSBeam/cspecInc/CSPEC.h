/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   cspec/CSPEC.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef essSystem_CSPEC_h
#define essSystem_CSPEC_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace beamlineSystem
{
  class BenderUnit;
  class PlateUnit;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace essConstruct
{
  class DiskChopper;
  class SingleChopper;
}

namespace constructSystem
{  
  class ChopperPit;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class ChopperHousing;

}

namespace essSystem
{  
  class GuideItem;
  class CspecHut;
  class DetectorTank;

  /*!
    \class CSPEC
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief CSPEC beamline constructor for the ESS
  */
  
class CSPEC : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedRotateUnit> cspecAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::PlateUnit> FocusA;

  // Vac pipe in gamma shield
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::PlateUnit> FocusB;

  /// Vac pipe to 18m
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Direct beamguide to 18m
  std::shared_ptr<beamlineSystem::PlateUnit> FocusC;

  /// First Chopper unit [BW1]
  std::shared_ptr<essConstruct::SingleChopper> ChopperA;
  /// First BW blade
  std::shared_ptr<essConstruct::DiskChopper> BWDiskA;

  /// Vac pipe to bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// S-Bender first section [in bunker]
  std::shared_ptr<beamlineSystem::BenderUnit> BendD;
  
 public:
  
  CSPEC(const std::string&);
  CSPEC(const CSPEC&);
  CSPEC& operator=(const CSPEC&);
  ~CSPEC() override;
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
