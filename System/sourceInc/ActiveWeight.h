/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/ActiveWeight.h
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
#ifndef SDef_ActiveWeight_h
#define SDef_ActiveWeight_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class ActiveWeight
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief Creates an active projection source
*/

class ActiveWeight 
{
 private:

  size_t nPoints;                 ///< Number of points
  
  Geometry::Vec3D Origin;         ///< Origin 
  Geometry::Vec3D Axis;           ///< Beam Axis
  double distWeight;              ///< Distance weight
  double angleWeight;             ///< Angular weight
  
  Geometry::Vec3D ABoxPt;         ///< Bounding box corner
  Geometry::Vec3D BBoxPt;         ///< Bounding box corner

  std::map<int,WorkData> matFlux; ///< material / flux

 public:

  ActiveWeight();
  ActiveWeight(const ActiveWeight&);
  ActiveWeight& operator=(const ActiveWeight&);
  ~ActiveWeight();

  /// Set number of output points
  void setNPoints(const size_t N) { nPoints=N; }
  void addMaterial(const std::string&,const std::string&);

  void setBiasConst(const Geometry::Vec3D&,const Geometry::Vec3D&,
		    const double,const double);
  void setBox(const Geometry::Vec3D&,const Geometry::Vec3D&);
    
  void createSource(Simulation&,const std::string&);
		 
  
};

}

#endif
 
