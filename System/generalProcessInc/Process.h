/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ProcessInc/Process.h
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
#ifndef System_Process_h
#define System_Process_h

class objectGroups;
class Simulation;
class FuncDataBase;

class SimMCNP;
class SimFLUKA;
class SimPHITS;

namespace mainSystem
{
  class inputParam;
}

namespace ModelSupport
{
  void setDefaultPhysics(Simulation&,const mainSystem::inputParam&);

  std::set<int> getActiveMaterial(const Simulation&,std::string);
  std::set<int> getActiveCell(const objectGroups&,const std::string&);

  void setWImp(Simulation&,const mainSystem::inputParam&);
  void procOffset(const objectGroups&,const mainSystem::inputParam&,
		  const std::string&,const size_t);
  void procAngle(const objectGroups&,const mainSystem::inputParam&,
		 const size_t);

  void setDefRotation(const objectGroups&,const mainSystem::inputParam&);
    


}


#endif
 
