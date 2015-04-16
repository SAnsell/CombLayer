/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Edge.h
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
#ifndef Geometry_Edge_h
#define Geometry_Edge_h

namespace Geometry 
{

class TriGrp;
class Vertex;
class Face;

/*!
  \class Edge
  \brief Edge between two points
  \version 1.0
  \date July 2009
  \author S. Ansell

  Does not hold any memory resposibility
*/

class Edge
{
 private:
  
  Vertex* endPts[2];      ///< Endpoints
  Face* adjFace[2];       ///< Adjacent faces  
  Face* newFace;          ///< New dividor face
  
  int deleteRequired;     ///< Deletion pending
  
 public:

  Edge();
  Edge(const Edge&);
  Edge& operator=(const Edge&);
  ~Edge();
 
  int bothVisible() const;
  int oneVisible() const;
  /// Require delete
  int reqDel() const { return deleteRequired; }
  /// new face to use
  int hasNewFace() const { return (newFace) ? 1 : 0; }
  /// mark for deletion
  void markForDeletion() { deleteRequired=1; }
  
  Vertex* getEndPt(const int);
  const Vertex* getEndPt(const int) const;
  Face* getVisibleFace(); 
  Face* getAdjFace(const int); 
  
  void setOnHull();
  void setVisibleFace();
  void setNewFace(Face*);
  void setEndPts(Vertex*,Vertex*);
  void setAdjacentFaces(Face*,Face*); 
  void setAdjFace(const int,Face*); 

  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Edge&);
 
}   // NAMESPACE Geometry

#endif
