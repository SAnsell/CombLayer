/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Quadrilateral.h
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
#ifndef Geometry_Quadrilateral_h
#define Geometry_Quadrilateral_h

namespace Geometry 
{

/*!
  \class Quadrilateral
  \brief Quadrilateral boundary a 3D Plane
  \version 1.0
  \date November 2010
  \author S. Ansell
 */

class Quadrilateral 
{
 private:

  int index;              ///< Index of the shape
  Vec3D V[4];             ///< Current points 
  Vec3D NormV;            ///< Normal
  
 public:

  Quadrilateral(const Vec3D&,const Vec3D&,
		const Vec3D&,const Vec3D&);
  Quadrilateral(const int,const Vec3D&,const Vec3D&,
		const Vec3D&,const Vec3D&);
  Quadrilateral(const Quadrilateral&);
  Quadrilateral& operator=(const Quadrilateral&);
  ~Quadrilateral() {}  ///< Destructor

  /// Get the point
  const Geometry::Vec3D& operator[](const int) const;
  Geometry::Vec3D& operator[](const int);

  void setPoints(const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&);

  /// Access the index
  int getIndex() const { return index; }
  double area() const;
  Vec3D centre() const;

  void write(std::ostream&) const;
};


std::ostream&
operator<<(std::ostream&,const Quadrilateral&);

}   // NAMESPACE Geometry

#endif
