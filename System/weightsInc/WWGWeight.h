/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGWeight.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef WeightSystem_WWGWeight_h
#define WeightSystem_WWGWeight_h

namespace WeightSystem
{

/*!
  \class WWGWeight
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Tracks cell weight in WWG mesh
*/
  
class WWGWeight 
{
 private:

  const long int WX;             ///< Weight XIndex size
  const long int WY;             ///< Weight YIndex size
  const long int WZ;             ///< Weight ZIndex size
  const long int WE;             ///< Energy size
  
  /// local storage for data [i,j,j,Energy]
  boost::multi_array<double,4> WGrid; 
  
 public:

  WWGWeight(const size_t,const Geometry::Mesh3D&);
  WWGWeight(const WWGWeight&);
  WWGWeight& operator=(const WWGWeight&);    
  ~WWGWeight() {}          ///< Destructor

  long int getXSize() const { return WX; }
  long int getYSize() const { return WY; }
  long int getZSize() const { return WZ; }
  long int getESize() const { return WE; }

  void zeroWGrid();
  double calcMaxAttn(const long int) const;
  double calcMaxAttn() const;

  void makeSource(const double);
  void makeAdjoint(const double);
  
  //  void updateWM(WWG&,const double,const double,
  //		const double,const double) const;

  /// accessor to Cells
  const boost::multi_array<double,4>& getGrid() const
    { return WGrid; }
  void setPoint(const long int,const long int,const double);

  void wTrack(const Simulation&,const Geometry::Vec3D&,
	      const std::vector<double>&,
	      const std::vector<Geometry::Vec3D>&);
  
  void wTrack(const Simulation&,const Geometry::Plane&,
	      const std::vector<double>&,
	      const std::vector<Geometry::Vec3D>&);

  void write(std::ostream&) const;
};

}

#endif
