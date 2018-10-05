/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsProcessInc/phitsProcess.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *

 ****************************************************************************/
#ifndef phitsSystem_phitsProcess_h
#define phitsSystem_phitsProcess_h

class Simulation;
class SimPHITS;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;
}

namespace phitsSystem
{
  void setDefaultPhysics(SimPHITS&,const mainSystem::inputParam&);
}


#endif
 
