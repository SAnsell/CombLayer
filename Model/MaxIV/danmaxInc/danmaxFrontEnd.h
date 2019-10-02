/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxFrontEnd.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_danmaxFrontEnd_h
#define xraySystem_danmaxFrontEnd_h

namespace insertSystem
{
  class insertCylinder;
}

namespace constructSystem
{
  class Bellows;
  class CrossPipe;
  class GateValveCube;
  class OffsetFlangePipe;
  class portItem;
  class PipeTube;
  class PortTube;
  class SupplyPipe;
  class VacuumBox;
  class VacuumPipe; 
}

namespace xraySystem
{

  class HeatDump;
  class LCollimator;
  class SqrCollimator;
  class SquareFMask;
  class UTubePipe;
  class Undulator;
  class Wiggler;
    
  /*!
    \class danmaxFrontEnd
    \version 1.0
    \author S. Ansell
    \date September 2019
    \brief General constructor front end optics
  */

class danmaxFrontEnd :
  public R3FrontEnd
{
 private:

  /// Pipe in undulator
  std::shared_ptr<constructSystem::PortTube> undulatorTube;
  /// Undulator in vacuum box [needs updating]
  std::shared_ptr<xraySystem::Wiggler> undulator;

  virtual const attachSystem::FixedComp&
    buildUndulator(Simulation&,MonteCarlo::Object*,
		   const attachSystem::FixedComp&,const long int);
			      
  void createSurfaces();
  void buildObjects(Simulation&);
  virtual void createLinks();
  
 public:
  
  danmaxFrontEnd(const std::string&);
  danmaxFrontEnd(const danmaxFrontEnd&);
  danmaxFrontEnd& operator=(const danmaxFrontEnd&);
  virtual ~danmaxFrontEnd();

};

}

#endif
