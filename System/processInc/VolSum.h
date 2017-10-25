/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/VolSum.h
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
#ifndef ModelSupport_VolSum_h
#define ModelSupport_VolSum_h

class Simulation;
namespace MonteCarlo
{
  class Object;
}

namespace ModelSupport
{

/*!
  \class VolSum
  \brief Hold an official model number
  \date August 2010
  \author S. Ansell
  \version 1.0
*/
						
class VolSum
{
 private:
  
  /// tally volume type
  typedef std::map<int,volUnit> tvTYPE; 
  // Input data
  Geometry::Vec3D Origin;                   ///< Origin
  Geometry::Vec3D X;                        ///< Axis of box (X)
  Geometry::Vec3D Y;                        ///< Axis of box (Y)
  Geometry::Vec3D Z;                        ///< Axis of box (Z)

  double fracX;                             ///< area xy-plane to total surface
  double fracY;                             ///< area-xyplane / total surface
  
  double fullVol;                           ///< Full volume  
  double totalDist;                         ///< Total distance
  size_t nTracks;                           ///< Number of full tracks
   
  tvTYPE tallyVols;                         ///< TallyNum:Volumes

  Geometry::Vec3D getCubePoint() const;
  
 public:
  
  VolSum(const Geometry::Vec3D&,const Geometry::Vec3D&);
  VolSum(const VolSum&);
  VolSum& operator=(const VolSum&);
  ~VolSum();

  void reset();
  void addDistance(const int,const double);
  void addFlux(const int,const double&,const double&);
  
  void addTally(const int,const int,const std::string&,
		const std::vector<int>&);
  void addTallyCell(const int,const int);

  void trackRun(const Simulation&,const size_t);
  void pointRun(const Simulation&,const size_t);
  double calcVolume(const int) const;
  void populateTally(const Simulation&);
  void populateAll(const Simulation&);
  void populateVSet(const Simulation&,const std::vector<int>&);
  
  void write(const std::string&) const;

};

} 

#endif
