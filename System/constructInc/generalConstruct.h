/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/generalConstruct.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef constructSystem_generalConstruct_h
#define constructSystem_generalConstruct_h

namespace attachSystem
{
  class ContainerComp;
  class ContainedGroup;
  class BlockZone;
}

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


namespace constructSystem
{
//  --------------------------------------------------------
//                BLOCKZONE
//  --------------------------------------------------------

int
internalUnit(Simulation&,
	     attachSystem::BlockZone&,
	     const attachSystem::FixedComp&,
	     const std::string&,
	     attachSystem::FixedComp&,
	     const std::string&,
	     attachSystem::ExternalCut&,
	     attachSystem::ContainedComp&);

int
internalGroup(Simulation&,
	      attachSystem::BlockZone&,
	      const attachSystem::FixedComp&,
	      const std::string&,
	      attachSystem::FixedComp&,
	      const std::string&,
	      attachSystem::ExternalCut&,
	      attachSystem::ContainedGroup&,
	      const std::set<std::string>&);
  

template<typename T>
int constructUnit(Simulation& System,
		  attachSystem::BlockZone& buildZone,
		  const attachSystem::FixedComp& linkUnit,
		  const std::string& sideName,
		  T& buildUnit,
		  const std::string& backName)
{
  if constexpr (std::is_base_of<attachSystem::ContainedComp,T>::value)

    return internalUnit(System,buildZone,linkUnit,sideName,
			buildUnit,backName,buildUnit,buildUnit);

  else
    
    return internalGroup(System,buildZone,linkUnit,sideName,
			 buildUnit,backName,buildUnit,buildUnit,{"All"});

}

template<typename T>
int constructUnit(Simulation& System,
		  attachSystem::BlockZone& buildZone,
		  const attachSystem::FixedComp& linkUnit,
		  const std::string& sideName,
		  T& buildUnit)
{
  return constructUnit
    (System,buildZone,linkUnit,sideName,buildUnit,"back");
}

template<typename T>
int constructGroup(Simulation& System,
		   attachSystem::BlockZone& buildZone,
		   const attachSystem::FixedComp& linkUnit,
		   const std::string& sideName,
		   T& buildUnit,
		   const std::string& backName,
		   const std::set<std::string>& CGunits)
{
  return internalGroup(System,buildZone,linkUnit,sideName,
		       buildUnit,backName,buildUnit,buildUnit,CGunits);
}
  
template<typename T>
int constructGroup(Simulation& System,
		   attachSystem::BlockZone& buildZone,
		   const attachSystem::FixedComp& linkUnit,
		   const std::string& sideName,
		   T& buildUnit,
		   const std::set<std::string>& CGunits)
{
  return internalGroup(System,buildZone,linkUnit,sideName,
		       buildUnit,"back",buildUnit,buildUnit,CGunits);
}
  
}  // NAMEPSACE constructSystem

#endif
