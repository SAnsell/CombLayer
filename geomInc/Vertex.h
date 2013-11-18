/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Vertex.h
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
#ifndef Geometry_Vertex_h
#define Geometry_Vertex_h

namespace Geometry 
{

class Edge;
class Face;

/*!
  \class Vertex
  \brief Build a convex hull from a set of points
  \version 1.0
  \date July 2009
  \author S. Ansell

  Simple Vertex function based on x,y,z and providing
  simple dot and cross products
 */

class Vertex
{
 private:

  int done;            ///< Processed
  int onHull;          ///< On Hull

  int vIndex;          ///< index number   
  Vec3D V;             ///< Current point 

  Edge* coneEdge;      ///< Edge for a cone construction
  
 public:
  
  Vertex();
  Vertex(const int,const Vec3D&);
  Vertex(const Vertex&);
  Vertex& operator=(const Vertex&);
  ~Vertex() {}  ///< Destructor

  /// Get the point
  const Geometry::Vec3D& getV() const { return V; }
  /// Get the idx
  int getID() const { return vIndex; }         
  Edge* getConeEdge() { return coneEdge; }          ///< coneEdge 
 
  int isOnHull() const { return (done && onHull); } ///< is on hull (and done)
  int isNotOnHull() const { return (done && !onHull); } ///< is not on hull (and done)
  int isDone() const { return done; }               ///< is Done
  
  void Done() { done=1; }                           ///< set done flag
  void setOnHull(const int F) { onHull=F; }         ///< set on-hull flag
  void setConeEdge(Edge* EPtr) { coneEdge=EPtr; }   ///< set Cone-Edge

  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Vertex&);
 
}   // NAMESPACE Geometry

#endif
