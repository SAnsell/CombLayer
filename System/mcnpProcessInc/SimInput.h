/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/SimInput.h
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
#ifndef SimProcess_SimInput_h
#define SimProcess_SimInput_h

class Simulation;
class SimFLUKA;
class SimMCNP;
class SimPHITS;

namespace physicsSystem
{
  class PhysicsCards;
}

namespace mainSystem
{
  class inputParam;
}

namespace SimProcess
{

  // Generic
  void inputProcessForSim(Simulation&,const mainSystem::inputParam&);
  int processExitChecks(Simulation&,const mainSystem::inputParam&);

  // MCNP only
  void inputProcessForSimMCNP(SimMCNP&,const mainSystem::inputParam&);

  void processPTrack(const mainSystem::inputParam&,
		     physicsSystem::PhysicsCards&);
  
  void processEvent(const std::string&,
		    const mainSystem::inputParam&,
		    physicsSystem::PhysicsCards&);
  
}  // namespace SimProcess

#endif
