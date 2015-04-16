/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/VolSum.h
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
#ifndef VolSum_h
#define VolSum_h

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
  double radius;                            ///< Radius
  double fullVol;                           ///< Full volume
  
  double totalDist;                         ///< Total distance
  int nTracks;                              ///< Number of full tracks
   
  tvTYPE tallyVols;                         ///< TallyNum:Volumes
    
 public:
  
  VolSum(const Geometry::Vec3D&,const double);
  VolSum(const VolSum&);
  VolSum& operator=(const VolSum&);
  ~VolSum();

  void reset();
  void addDistance(const int,const double);
  void addFlux(const int,const double&,const double&);
  //  void populate(const Simulation&);
  void addTally(const int,const int,const std::string&,
		const std::vector<int>&);
  void addTallyCell(const int,const int);

  void run(const Simulation&,const size_t);
  void pointRun(const Simulation&,const size_t);
  double calcVolume(const int) const;
  void populateTally(const Simulation&);
  void write(const std::string&) const;

};

} 

#endif
