/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/TMRCreate.h
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
#ifndef TMRCreate_h
#define TMRCreate_h

class Simulation;


/*!
  \namespace TMRSystem
  \brief Adds a layer of shutters to the Target/Reflect/Moderatr
  \author S. Ansell
  \version 1.0
  \date April 2009
*/

namespace TMRSystem
{
  /// Enumeration of the layers
  enum LAYER { moderator=0,reflector=1,torpedo=2,shutter=3,inner=4,outer=5 };
  
  void setWeights(Simulation&,const std::vector<double>&,
		  const std::vector<double>&);
  void setWeights(Simulation&);
  void setWeightsHighE(Simulation&);
  void makeNGroove(Simulation&);
  void makeBGroove(Simulation&);
  void addCorregation(Simulation&);
  void setVirtualContainers(Simulation&);

}


#endif
 
