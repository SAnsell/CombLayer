/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WeightControl.h
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
#ifndef WeightSystem_WeightControl_h
#define WeightSystem_WeightControl_h

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
    \class WegthControl
    \version 1.0
    \author S. Ansell
    \date October 2015
    \brief Input to Weights controler
  */
  
class WeightControl
{
 private:
  
  std::vector<double> EBand;     ///< Energy bandk
  std::vector<double> WT;        ///< Weight scalar

  bool sourceFlag;               ///< Set point/tally flags
  bool tallyFlag;                ///< Set point/tally flags
  Geometry::Vec3D sourcePt;      ///< Source Point
  Geometry::Vec3D tallyPt;       ///< Tally Point

  void setHighEBand();
  void setMidEBand();
  void setLowEBand();

  void help() const;
  
  void procType(const mainSystem::inputParam&);
  void procSource(const mainSystem::inputParam&);
  void procTallyPoint(const mainSystem::inputParam&);
  void procObject(const Simulation&,
		  const mainSystem::inputParam&);
  void procRebase(const Simulation&,
		  const mainSystem::inputParam&);
  
			
  void setWeights(Simulation&);
  void calcTrack(const Simulation&,const Geometry::Vec3D&,
		 const int,const int,
		 const double,const double,const double);

 public:

  WeightControl();
  WeightControl(const WeightControl&);
  WeightControl& operator=(const WeightControl&);
  ~WeightControl();
  
  void processWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
