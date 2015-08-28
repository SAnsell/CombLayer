/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/NMX.h
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
    \class NMX
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief NMX beamline constructor for the ESS
  */
  
class NMX
{
 private:

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedComp> nmxAxis;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> GuideA;
  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> BendA;
  
  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  NMX();
  NMX(const NMX&);
  NMX& operator=(const NMX&);
  ~NMX();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
