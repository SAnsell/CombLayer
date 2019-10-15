/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/generalConstruct.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef xrayConstruct_generalConstruct_h
#define xrayConstruct_generalConstruct_h


namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class portItem;
  class PortTube;
  class PipeTube;
  class GateValveCube;
  class GateValveCylinder;
  class JawValveCube;
  class JawFlange;

}

namespace xraySystem
{
  class OpticsHutch;
  class Mirror;
  class MonoBlockXstals;
  class DCMTank;
}

/*!
  \namespace xrayConstruct
  \brief General xray optics helper system
  \version 1.0
  \date October 2019
  \author S. Ansell
*/

namespace xrayConstruct
{

int
internalUnit(Simulation&,
	     attachSystem::InnerZone&,
	     MonteCarlo::Object*,
	     const attachSystem::FixedComp&,
	     const std::string&,
	     attachSystem::FixedComp&,
	     attachSystem::FrontBackCut&,
	     attachSystem::ContainedComp&);
  
template<typename T>
int constructUnit(Simulation& System,
		  attachSystem::InnerZone& buildZone,
		  MonteCarlo::Object* masterCell,
		  const attachSystem::FixedComp& linkUnit,
		  const std::string& sideName,
		  T& buildUnit)
{
  return internalUnit(System,buildZone,masterCell,linkUnit,sideName,
		 buildUnit,buildUnit,buildUnit);
}

}  // NAMEPSACE xrayConstruct

#endif
