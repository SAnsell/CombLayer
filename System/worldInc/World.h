/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   worldInc/World.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef WorldSystem_h
#define WorldSystem_h

class Simulation;

/*!
  \namespace World
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Global objects/surfaces
*/

namespace World
{

void createOuterObjects(Simulation&);

const attachSystem::FixedComp& masterOrigin();
const attachSystem::FixedComp& masterTS2Origin();

}

#endif
 
