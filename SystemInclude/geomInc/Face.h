/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Face.h
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
#ifndef Geometry_Face_h
#define Geometry_Face_h

namespace Geometry 
{

class TriGrp;
class Edge;
class Vertex;

/*!
  \class Face
  \brief Three point face
  \author S. Ansell
  \date July 2009
  \version 1.0
*/

class Face
{
 private:
 
  int visible;          ///< Is visible
  Vertex* FPts[3];      ///< Face Points
  Edge* ELines[3];      ///< Edges of face

  int calcTriGrp(const Face&,TriGrp&) const;
  int calcTriGrpCase(const Face&,TriGrp&) const;
  int colinearIntersect(const Face&) const;
  int planeIntersect(const Face&) const;

 public:
  
  Face();
  Face(Vertex*,Vertex*,Vertex*,
       Edge*,Edge*,Edge*);

  Face(const Face&);
  Face& operator=(const Face&);
  ~Face();

  static bool checkAcuteAngle(const Vec3D&,const Vec3D&,const Vec3D&);
  static bool checkNormAngle(const Vec3D&,const Vec3D&,
			      const Vec3D&,const Vec3D&);
  static int orientation(const Vec3D&,const Vec3D&,
			 const Vec3D&,const Vec3D&);
  static int orientation(const Vec3D&,const Vec3D&,
			 const Vec3D&);
  static int testEdge(const Vec3D&,const Vec3D&,const Vec3D&,
		      const Vec3D&,const Vec3D&,const Vec3D&);
  static int testVertex(const Vec3D&,const Vec3D&,const Vec3D&,
		      const Vec3D&,const Vec3D&,const Vec3D&);


  /// Set visible flag
  void setVisible(const int F) { visible=F; }
  /// Get visibility
  bool isVisible() const { return visible; }
  /// Get visibility
  bool notVisible() const { return !visible; }

  int intersect(const Face&) const;
  int Intersect(const Face&) const;

  int volumeSign(const Vertex&) const;
  int volumeSign(const Geometry::Vec3D&) const;
  int flatValid(const Geometry::Vec3D&) const;
  Vec3D getNormal() const;

  void setVertex(Vertex*,Vertex*,Vertex*,
		 Edge*,Edge*,Edge*);
  Edge* getEdge(const size_t);
  Vertex* getVertex(const size_t);
  const Vertex* getVertex(const size_t) const;

  void makeCCW(Edge*,Vertex*);

  void write(std::ostream&) const;
};

/*!
  \struct TriGrp
  \brief Structure to hold clusters of line inter sections
  \version 1.0
  \author S. Ansell
  \date March 2011
*/
class TriGrp
{
 public:

  const Vec3D* sMin;   ///< Min point tri A
  const Vec3D* sMax;   ///< Max Point tri A
  const Vec3D* tMin;   ///< Min Point tri B
  const Vec3D* tMax;   ///< Max Point tri B

  /// Constructor
  TriGrp() : sMin(0),sMax(0),tMin(0),tMax(0) {}
  /// simple assignment operator
  TriGrp& operator()(const Vec3D* a,const Vec3D* b,
		     const Vec3D* c,const Vec3D* d)
    {
      sMin=a; tMin=b; sMax=c; tMax=d; return *this;
    }
  /// Simple assignment operator
  TriGrp& operator()(const Vec3D& a,const Vec3D& b,
		     const Vec3D& c,const Vec3D& d)
    {
      sMin= &a; tMin= &b; sMax= &c; tMax= &d; return *this;
    }
};

std::ostream&
operator<<(std::ostream&,const Vertex&);

std::ostream&
operator<<(std::ostream&,const Edge&);

std::ostream&
operator<<(std::ostream&,const Face&);


 
}   // NAMESPACE Geometry

#endif
