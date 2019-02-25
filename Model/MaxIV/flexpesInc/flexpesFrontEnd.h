/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flexpesInc/flexpesFrontEnd.h
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
#ifndef xraySystem_flexpesFrontEnd_h
#define xraySystem_flexpesFrontEnd_h

namespace insertSystem
{
  class insertCylinder;
}

namespace constructSystem
{
  class Bellows;
  class CrossPipe;
  class GateValve;
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
  class BremBlock;
  class BeamMount;
  class FlangeMount;
  class HeatDump;
  class LCollimator;
  class SquareFMask;
  class UTubePipe;
  class Undulator;

    
  /*!
    \class flexpesFrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class flexpesFrontEnd :  public R1FrontEnd
{
 private:

  /// Pipe in undulator
  std::shared_ptr<xraySystem::UTubePipe> undulatorPipe;
  /// Undulator in vacuum box
  std::shared_ptr<xraySystem::Undulator> undulator;

  virtual const attachSystem::FixedComp&
    buildUndulator(Simulation&,MonteCarlo::Object*,
		   const attachSystem::FixedComp&,const long int);
			      
  virtual void createLinks();
  
 public:
  
  flexpesFrontEnd(const std::string&);
  flexpesFrontEnd(const flexpesFrontEnd&);
  flexpesFrontEnd& operator=(const flexpesFrontEnd&);
  virtual ~flexpesFrontEnd();

};

}

#endif
