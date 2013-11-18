/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/SurfVertex.h
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
#ifndef MonteCarlo_SurfVertex_h
#define MonteCarlo_SurfVertex_h

class Intersect;
namespace Geometry 
{
  class Surface;
}

namespace MonteCarlo
{
  
/*!
  \class SurfVertex
  \brief Holds the intersections and surfaces to a particular
  vertex
  \version 1.0
  \date August 2005
  \author S. Ansell
*/
    
class SurfVertex 
{
 private:

  std::vector<const Geometry::Surface*> Surf;    ///< List of surfaces
  Geometry::Vec3D Pt;                            ///< Point at SurfVertex
  
 public:
  
  SurfVertex();
  SurfVertex(const SurfVertex&);
  SurfVertex& operator=(const SurfVertex&);
  ~SurfVertex();

  /// Set the value 
  void setPoint(const Geometry::Vec3D& A) { Pt=A; }  
  /// Return vector point
  const Geometry::Vec3D& getPoint() const { return Pt; }   

  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);
  void mirror(const Geometry::Plane&);

  void addSurface(const Geometry::Surface*);

  void print(std::ostream&,const Geometry::Vec3D&,
	     const Geometry::Matrix<double>& ) const;
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const SurfVertex&);

}

#endif
