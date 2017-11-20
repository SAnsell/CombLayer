/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/ESTIA.h
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
#ifndef essSystem_ESTIA_h
#define essSystem_ESTIA_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class CellMap;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace constructSystem
{  
  class ChopperPit;
  class ChopperHousing;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;

}

namespace essSystem
{  

  /*!
    \class ESTIA
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief ESTIA beamline constructor for the ESS
  */
class ESTIA : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> estiaAxis;

  /// mirror to end of monolith
  std::shared_ptr<beamlineSystem::GuideLine> FocusMono;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// mirror to end of monolith
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Vac box for 
  std::shared_ptr<constructSystem::VacuumBox> VacBoxA;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;


  static void
    buildChopperBlock(Simulation&,const Bunker&,
		      const attachSystem::FixedComp&,
		      const constructSystem::VacuumBox&,
		      constructSystem::VacuumBox&,
		      beamlineSystem::GuideLine&,
		      constructSystem::DiskChopper&,
		      constructSystem::ChopperHousing&,
		      constructSystem::VacuumPipe&);
  
 public:
  
  ESTIA(const std::string&);
  ESTIA(const ESTIA&);
  ESTIA& operator=(const ESTIA&);
  ~ESTIA();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
