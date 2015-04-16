/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Convex.h
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
#ifndef Geometry_Convex_h
#define Geometry_Convex_h

namespace Geometry 
{
  class Vertex;
  class Edge;
  class Face;

/*!
  \class Convex
  \brief Build a convex hull from a set of points
  \version 1.0
  \date July 2009
  \author S. Ansell

  Simple Convex function based on x,y,z and providing
  simple dot and cross products
 */

class Convex
{
 private:
  
  typedef std::vector<Vertex*> VTYPE;     ///< Vertex Type
  typedef std::vector<Edge*> ETYPE;       ///< Edge Type
  typedef std::vector<Face*> FTYPE;       ///< Face Type

  std::vector<Vec3D> Pts;           ///< Points
  VTYPE VList;         ///< VList
  ETYPE EList;         ///< Edge List 
  FTYPE FList;         ///< Face List
  std::vector<Plane> SurfList; ///< List of surfaces [if calculated]

  void deleteAll();

  void cleanUp();
  void cleanEdges();
  void cleanFaces();
  void cleanVertex();
  void reduceSurfaces();
  int addOne(Vertex*);
  Face* makeCone(Vertex*,Edge*);
  
 public:
  
  Convex();
  Convex(const std::vector<Vec3D>&);
  Convex(const Convex&);
  Convex& operator=(const Convex&);
  ~Convex();

  int calcDTriangle();
  void constructHull();
  void createSurfaces(int);
  void createAll(const int);

  void setPoints(const std::vector<Vec3D>&);
  void addPoints(const std::vector<Vec3D>&);
  void addPoint(const Vec3D&);
  
  /// Access points
  const std::vector<Geometry::Vec3D>& getPoints() const
    { return Pts; }

  /// Access Planes
  const std::vector<Geometry::Plane>& getPlanes() const
    { return SurfList; }
  /// Access Faces
  const std::vector<Face*>& getFaces() const
    { return FList; }

  int inHull(const Geometry::Vec3D&) const;
  int inSurfHull(const Geometry::Vec3D&) const;
  int validEdge() const;
  int isConvex() const;
  int intersectHull(const Convex&) const;
  int intersectHull(const Convex&,
		    std::vector<const Face*>&) const;
  int getNonMatch(std::vector<const Face*>&) const;

  void writeFaces(std::ostream&) const;
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const Convex&);

}   // NAMESPACE Geometry

#endif

