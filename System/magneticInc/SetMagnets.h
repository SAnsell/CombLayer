/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   magneticInc/SetMagnets.h
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
#ifndef magnetSystem_flukaSetMagnets_h
#define magnetSystem_flukaSetMagnets_h

class Simulation;
class SimFLUKA;
class SimPHITS;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace magnetSystem
{

  template<typename SimTYPE> void setDefMagnets(SimTYPE&);
  template<typename SimTYPE>
    void setMagneticPhysics(SimTYPE&,const mainSystem::inputParam&);
  template<typename SimTYPE>
    void setMagneticExternal(SimTYPE&,const mainSystem::inputParam&);
    
}


#endif
 
