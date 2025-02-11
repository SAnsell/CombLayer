/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   tomowiseInc/tomowiseFrontEnd.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_tomowiseFrontEnd_h
#define xraySystem_tomowiseFrontEnd_h

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


  /*!
    \class tomowiseFrontEnd
    \version 1.0
    \author K. Batkov
    \date February 2025
    \brief TomoWISE front end
  */

class tomowiseFrontEnd :
  public R3FrontEnd
{
 private:

  /// Pipe in undulator
  std::shared_ptr<xraySystem::UTubePipe> undulatorPipe;
  /// Undulator in vacuum box
  std::shared_ptr<xraySystem::Undulator> undulator;

  const attachSystem::FixedComp&
    buildUndulator(Simulation&,
		   const attachSystem::FixedComp&,const long int) override;

  void createLinks() override;

 public:

  tomowiseFrontEnd(const std::string&);
  tomowiseFrontEnd(const tomowiseFrontEnd&);
  tomowiseFrontEnd& operator=(const tomowiseFrontEnd&);
  ~tomowiseFrontEnd() override;

};

}

#endif
