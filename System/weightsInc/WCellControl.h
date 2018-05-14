/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WCellControl.h
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
#ifndef WeightSystem_WCellControl_h
#define WeightSystem_WCellControl_h

///\file 

class Simulation;
namespace Geometry
{
  class Plane;
}

/*!
  \namespace WeightSystem
  \brief Main variance reduction ssytem
  \author S. Ansell
  \version 1.0
  \date April 2015
*/

namespace WeightSystem
{
  class ItemWeight;
  class CellWeight;
  class WWGWeight;
  
  /*!
    \class WCellControl
    \version 1.0
    \author S. Ansell
    \date October 2015
    \brief Input to Weigths controller
  */
  
class WCellControl : public WeightControl
{
 private:
  
  std::set<std::string> objectList;  ///< Object list to this cut [local]
  
  void setHighEBand();
  void setMidEBand();
  void setLowEBand();

  void scaleObject(const Simulation&,const std::string&,
		   const double,const double);
  void scaleAllObjects(const Simulation&,const double,const double);
  double findMax(const Simulation&,const std::string&,
		 const size_t,const double) const;
  
  void procRebase(const Simulation&,
		  const mainSystem::inputParam&);

  void procTrackLine(const mainSystem::inputParam&);
  void procObject(const Simulation&,
		  const mainSystem::inputParam&);
  void procTrack(const Simulation&,
		 const mainSystem::inputParam&);
  void procWWGWeights(Simulation&,
		      const mainSystem::inputParam&);
  
  void setWeights(Simulation&,const std::string&);
  void cTrack(const Simulation&,const Geometry::Vec3D&,
	      const std::vector<Geometry::Vec3D>&,
	      const std::vector<long int>&,
	      CellWeight&);
  void cTrack(const Simulation&,const Geometry::Plane&,
	      const std::vector<Geometry::Vec3D>&,
	      const std::vector<long int>&,
	      CellWeight&);

  void wTrack(const Simulation&,const Geometry::Vec3D&,
	      WWGWeight&) const;
  void wTrack(const Simulation&,const Geometry::Plane&,
	      WWGWeight&) const;
		  
  void calcCellTrack(const Simulation&,const Geometry::Vec3D&,
		     const std::vector<int>&,CellWeight&);
  void calcCellTrack(const Simulation&,const Geometry::Plane&,
		     const std::vector<int>&,CellWeight&);
  void calcCellTrack(const Simulation&,const Geometry::Cone&,
		     CellWeight&);


 public:

  WCellControl();
  WCellControl(const WCellControl&);
  WCellControl& operator=(const WCellControl&);
  ~WCellControl();

  
  void processWeights(Simulation&,const mainSystem::inputParam&);
  void normWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
