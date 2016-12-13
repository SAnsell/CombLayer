/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/shortNMX/shortNMX.h
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
#ifndef essSystem_shortNMX_h
#define essSystem_shortNMX_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class RotaryCollimator;
  class VacuumBox;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  /*!
    \class shortNMX
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief shortNMX beamline constructor for the ESS
  */
  
class shortNMX : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> nmxAxis;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe in the gamma shield [5.5m to 6m]
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6 metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;


  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  // Piece after wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  
  void setBeamAxis(const FuncDataBase&,
		   const GuideItem&,const bool);
  
 public:
  
  shortNMX(const std::string&);
  shortNMX(const shortNMX&);
  shortNMX& operator=(const shortNMX&);
  ~shortNMX();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
