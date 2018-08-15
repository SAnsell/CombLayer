/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/maxpeemFrontEnd.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_maxpeemFrontEnd_h
#define xraySystem_maxpeemFrontEnd_h

namespace insertSystem
{
  class insertCylinder;
}

namespace constructSystem
{
  class Bellows;
  class CrossPipe;
  class GateValve;
  class portItem;
  class PipeTube;
  class PortTube;
  class SupplyPipe;
  class VacuumBox;
  class VacuumPipe; 
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  class FlangeMount;
  class HeatDump;
  class SqrCollimator;
  class Wiggler;

    
  /*!
    \class maxpeemFrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class maxpeemFrontEnd :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Wiggler in vacuum box
  std::shared_ptr<constructSystem::VacuumBox> wigglerBox;
  /// Wiggler in vacuum box
  std::shared_ptr<Wiggler> wiggler;
  /// dipole connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> dipolePipe;
  /// electron cut cell
  std::shared_ptr<insertSystem::insertCylinder> eCutDisk;
  /// bellow infront of collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// box for collimator
  std::shared_ptr<constructSystem::PipeTube> collTubeA;
  /// collimator A
  std::shared_ptr<xraySystem::SqrCollimator> collA;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  maxpeemFrontEnd(const std::string&);
  maxpeemFrontEnd(const maxpeemFrontEnd&);
  maxpeemFrontEnd& operator=(const maxpeemFrontEnd&);
  ~maxpeemFrontEnd();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
