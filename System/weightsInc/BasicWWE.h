/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/BasicWWE.h
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

/*!
  \class baseWeightState 
  \verison 1.0
  \author S. Ansell
  \date October 2015
  \brief Tempory state for weight building
 */
  
class basicWeightState
{
 private:

  std::vector<double> energyBand;      ///< Energy band
  std::vector<double> WT;              ///< Weights

  void setLowEBand();
  void setMidEBand();
  void setHighEBand();
  void setFlatWeight();

  void setWeights(Simulation&,const std::vector<double>&,
		  const std::vector<double>&);

  void setWeightType(Simulation&,const mainSystem::inputParam&);  

  void setWeightsObject(Simulation&,const std::string&,
			const Geometry::Vec3D&);
  
 public:
  
  void simulationWeights(Simulation&,const mainSystem::inputParam&);
  
};
  

			


}


#endif
 
