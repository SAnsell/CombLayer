/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/DefPhysics.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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

class Simulation;
namespace mainSystem
{
  class inputParam;
}

namespace physicsSystem
{
  class LSwitchCard;
}

namespace ModelSupport
{
  void setPhysicsModel(physicsSystem::LSwitchCard&,const std::string&);
  void setDefaultPhysics(Simulation&,const mainSystem::inputParam&); 
  void setReactorPhysics(Simulation&,const mainSystem::inputParam&); 
  void setUFissionPhysics(Simulation&,const mainSystem::inputParam&); 
  std::string setDefRotation(const mainSystem::inputParam&);
  void setItemRotate(const attachSystem::FixedComp&,const std::string&);
}


#endif
 
