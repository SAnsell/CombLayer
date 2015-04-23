/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weightsInc/BasicWWE.h
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
#ifndef WeightSystem_BasicWWE_h
#define WeightSystem_BasicWWE_h

///\file 

class Simulation;


/*!
  \namespace WeightSystem
  \brief Adds a layer of shutters to the Target/Reflect/Moderatr
  \author S. Ansell
  \version 1.0
  \date April 2009
*/

namespace WeightSystem
{ 
  void simulationWeights(Simulation&,const mainSystem::inputParam&);
			
  void setWeights(Simulation&,const std::vector<double>&,
		  const std::vector<double>&,
		  const std::set<std::string>&);

  void setWeights(Simulation&,const std::string&);
  void setWeightsBasic(Simulation&);
  void setWeightsMidE(Simulation&);
  void setWeightsHighE(Simulation&);

}


#endif
 
