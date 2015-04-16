/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Triangle.h
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
#ifndef Geometry_Triangle_h
#define Geometry_Triangle_h

namespace Geometry 
{

/*!
  \class Triangle
  \brief Triangle boundary a 3D Plane
  \version 1.0
  \date November 2010
  \author S. Ansell
 */

class Triangle
{
 private:

  int index;              ///< Creation index
  Vec3D V[3];             ///< Current points 
  Vec3D NormV;            ///< Normal
  
 public:

  
  Triangle(const Vec3D&,const Vec3D&,const Vec3D&);
  Triangle(const int,const Vec3D&,const Vec3D&,const Vec3D&);
  Triangle(const Triangle&);
  Triangle& operator=(const Triangle&);
  ~Triangle() {}  ///< Destructor

  /// Get the point
  const Geometry::Vec3D& operator[](const int) const;
  Geometry::Vec3D& operator[](const int);

  void setPoints(const Geometry::Vec3D&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&);

  /// Debug function to get index
  int getIndex() const { return index; }
  double area() const;
  Vec3D centre() const;
  int valid(const Vec3D&) const;

  void write(std::ostream&) const;
};


std::ostream&
operator<<(std::ostream&,const Triangle&);

}   // NAMESPACE Geometry

#endif
