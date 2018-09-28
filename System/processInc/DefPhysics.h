/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/DefPhysics.h
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
#ifndef ModelSupport_DefPhysics_h
#define ModelSupport_DefPhysics_h

class objectGroups;
class Simulation;
class SimMCNP;
class SimFLUKA;
class SimPHITS;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace physicsSystem
{
  class LSwitchCard;
  class PhysicsCards;
}

namespace ModelSupport
{
  void setPhysicsModel(physicsSystem::LSwitchCard&,const std::string&);
  void setGenericPhysics(SimMCNP&,const std::string&);
  
  void procOffset(const objectGroups&,const mainSystem::inputParam&,
		  const size_t);
  void procAngle(const objectGroups&,const mainSystem::inputParam&,
		 const size_t);



  void setNeutronPhysics(physicsSystem::PhysicsCards&,const FuncDataBase&,
			 const double); 
  void setReactorPhysics(physicsSystem::PhysicsCards&,const FuncDataBase&,
			 const mainSystem::inputParam&); 
  void setDefRotation(const objectGroups&,const mainSystem::inputParam&);

  
  void setDefaultPhysics(SimMCNP&,const mainSystem::inputParam&);
  void setDefaultPhysics(SimFLUKA&,const mainSystem::inputParam&);
  void setDefaultPhysics(SimPHITS&,const mainSystem::inputParam&);
  void setDefaultPhysics(Simulation&,const mainSystem::inputParam&);
}


#endif
 
