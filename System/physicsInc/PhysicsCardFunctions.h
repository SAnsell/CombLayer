/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PhysicsCardsFunctions.h
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
#ifndef PhysicsSystem_PhysicsCardsFunctions_h
#define PhysicsSystem_PhysicsCardsFunctions_h



namespace physicsSystem
{
  // funtions
  std::string getMCNPparticleList(const std::set<int>&);
  std::string getMCNPparticleList(const std::set<int>&,const std::set<int>&);
  std::string getMCNPparticleList(const std::set<int>&,
				  const std::set<std::string>&);

}

#endif
