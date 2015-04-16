/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Convex2D.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef Geometry_Convex2D_h
#define Geometry_Convex2D_h

namespace Geometry 
{

/*!
  \class Convex2D
  \brief Build a convex hull from a set of points
  \version 1.0
  \date July 2009
  \author S. Ansell

  Simple Convex2D function based on x,y,z and providing
  simple dot and cross products
 */

class Convex2D
{
 private:
  
  typedef std::vector<Vert2D> VTYPE;     ///< Vertex Type

  std::vector<Vec3D> Pts;           ///< Points
  VTYPE VList;                      ///< 

  Geometry::Vec3D centroid;    ///< Centroid to points
  Geometry::Vec3D normal;      ///< Normal to points
  size_t distIndex;            ///< Most distant from centre

  void createVertex();

 public:
  
  Convex2D();
  Convex2D(const std::vector<Vec3D>&);
  Convex2D(const Convex2D&);
  Convex2D& operator=(const Convex2D&);
  ~Convex2D();

  void constructHull();

  void setPoints(const std::vector<Vec3D>&);
  void addPoint(const Vec3D&);
  double calcNormal();
  double calcArea() const;
  size_t calcMaxIndex();

  int inHull(const Geometry::Vec3D&) const;
  
  /// Access points
  const std::vector<Geometry::Vec3D>& getPoints() const
    { return Pts; }
  /// Access centroid
  const Geometry::Vec3D& getCentroid() { return centroid; }
  /// Access normal
  const Geometry::Vec3D& getNormal() { return normal; }
  /// Index of most distance point
  size_t getDistPoint() const { return distIndex; }
  std::vector<Geometry::Vec3D> getSequence() const;

  //  int isConvex() const;
  void write(std::ostream&) const;
};

}   // NAMESPACE Geometry

#endif

