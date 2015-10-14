/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/BasicWWG.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef WeightSystem_BasicWWG_h
#define WeightSystem_BasicWWG_h

///\file 

class Simulation;


/*!
  \namespace WeightSystem
  \brief Controls basic WWG system
  \author S. Ansell
  \version 1.0
  \date April 2009
*/

namespace WeightSystem
{ 
  void createWWG(Simulation&,const mainSystem::inputParam&);

}


#endif
 
