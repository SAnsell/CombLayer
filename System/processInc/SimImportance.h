/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/SimImportance.h
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
#ifndef SimProcess_SimImportance_h
#define SimProcess_SimImportance_h

class Simulation;
class SimMCNP;
class SimPHITS;
class SimFLUKA;

/*!
  \namespace SimProcess
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Process functions to act on a Simuation instance
 */

namespace SimProcess
{
  void importanceSim(Simulation&,const mainSystem::inputParam&);
  void importanceSim(SimMCNP&,const mainSystem::inputParam&);

}  // namespace SimImportance

#endif
