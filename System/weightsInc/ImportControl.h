/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/ImportControl.h
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
#ifndef WeightSystem_ImportControl_h
#define WeightSystem_ImportControl_h

///\file 

class Simulation;
namespace mainSystem
{
  class inputParam;
}

namespace physicsSystem
{
  class PhysicsCards;
}

namespace WeightSystem
{
  void setWImp(physicsSystem::PhysicsCards&,const std::string&);
  void clearWImp(physicsSystem::PhysicsCards&,const std::string&);
  void removePhysImp(physicsSystem::PhysicsCards&,const std::string&);
  void zeroImp(physicsSystem::PhysicsCards&,Simulation&,const int,const int);
  void simulationImp(SimMCNP&,const mainSystem::inputParam&);
  void EnergyCellCut(SimMCNP&,const mainSystem::inputParam&);
  void ExtField(const objectGroups&,physicsSystem::PhysicsCards&,
		const mainSystem::inputParam&);
  void FCL(const objectGroups&,physicsSystem::PhysicsCards&,
	   const mainSystem::inputParam&);
  
  void IMP(SimMCNP&,const mainSystem::inputParam&);
  void SBias(const objectGroups&,physicsSystem::PhysicsCards&,
	     const mainSystem::inputParam&);

  void DXT(const objectGroups&,physicsSystem::PhysicsCards&,
	   const mainSystem::inputParam&);
  void PWT(const objectGroups&,physicsSystem::PhysicsCards&,
	   const mainSystem::inputParam&);

}


#endif
 
