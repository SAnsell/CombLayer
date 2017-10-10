/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/TwinFlatGenerator.h
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
#ifndef essSystem_TwinFlatGenerator_h
#define essSystem_TwinFlatGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class TwinFlatGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief TwinFlatGenerator for variables
*/

class TwinFlatGenerator :
  public TwinBaseGenerator
{
 private:

  double portIW;           ///< Main Port inner width
  double portIH;           ///< Main Port inner height
  double portOW;           ///< Main Port outer width
  double portOH;           ///< Main Port outer height
  size_t portNBolt;        ///< Number of bolts
  double portBoltRadius;   ///< Port bolt size [M8 currently]
  
 public:

  TwinFlatGenerator();
  TwinFlatGenerator(const TwinFlatGenerator&);
  TwinFlatGenerator& operator=(const TwinFlatGenerator&);
  ~TwinFlatGenerator();


  void setMaterial(const std::string&,const std::string&);
  void setMainRadius(const double);


  void generateChopper(FuncDataBase&,
		       const std::string&,
		       const double,const double,
		       const double);
};

}

#endif
 
