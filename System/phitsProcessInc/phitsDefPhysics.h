/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsProcessInc/phitsDefPhysics.h
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
#ifndef phitsSystem_PhitsDefPhysics_h
#define phitsSystem_PhitsDefPhysics_h

class Simulation;
class SimPHITS;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace phitsSystem
{
  class phitsPhysics;

  void setModelPhysics(SimPHITS&, const mainSystem::inputParam&);
    
  void setXrayPhysics(phitsPhysics&,const mainSystem::inputParam&);
}


#endif
 
