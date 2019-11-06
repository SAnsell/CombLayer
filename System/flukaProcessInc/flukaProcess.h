/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/flukaProcess.h
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
#ifndef flukaSystem_flukaProcess_h
#define flukaSystem_flukaProcess_h

class objectGroups;
class Simulation;
class SimFLUKA;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace flukaSystem
{
  std::set<int> getActiveUnit(const Simulation&,const int,const std::string&);
  std::set<int> getActiveMaterial(const Simulation&,std::string);
  std::set<int> getActiveCell(const objectGroups&,const std::string&);
  std::set<int> getActiveParticle(const std::string&);
  void setDefaultPhysics(SimFLUKA&,const mainSystem::inputParam&);
}


#endif
 
