/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/ImportControl.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
class SimMCNP;
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
  void zeroImp(Simulation&,const int,const int);
  void simulationImp(SimMCNP&,const mainSystem::inputParam&);
  void EnergyCellCut(SimMCNP&,const mainSystem::inputParam&);
  void ExtField(SimMCNP&,const mainSystem::inputParam&);
  void FCL(SimMCNP&,const mainSystem::inputParam&);
  
  void IMP(Simulation&,const mainSystem::inputParam&);
  void SBias(SimMCNP&,const mainSystem::inputParam&);

  void DXT(SimMCNP&,const mainSystem::inputParam&);
  void PWT(SimMCNP&,const mainSystem::inputParam&);

}


#endif
 
