/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Vert2D.h
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
#ifndef Geometry_Vert2D_h
#define Geometry_Vert2D_h

namespace Geometry 
{

/*!
  \class Vert2D
  \brief Single point on a 2D plane approximate
  \version 1.0
  \date August 2009
  \author S. Ansell

  Contains a simple point on a 2D approximation 
  for a convex hull applied to 3D points.
 */

class Vert2D
{
 private:

  int done;            ///< Processed
  int onHull;          ///< On Hull
  size_t vIndex;     ///< index number   
  double cAngle;       ///< Cos of Angle

  Vec3D V;             ///< Current point 

 public:
  
  Vert2D();
  Vert2D(const size_t,const Vec3D&);
  Vert2D(const Vert2D&);
  Vert2D& operator=(const Vert2D&);
  ~Vert2D() {}  ///< Destructor

  /// Get the point
  const Geometry::Vec3D& getV() const { return V; }
  /// Get the index
  size_t getID() const { return vIndex; }         
 
  int isOnHull() const { return (done && onHull); } ///< is on hull (and done)
  int isNotOnHull() const { return (done && !onHull); } ///< is not on hull (and done)
  int isDone() const { return done; }               ///< is Done
  double getAngle() const { return cAngle; }        ///< Get angle
  

  void Done() { done=1; }                           ///< set done flag
  void setOnHull(const int F) { onHull=F; }         ///< set on-hull flag
  double calcAngle(const Vec3D&,const Vec3D&);        
  
  void write(std::ostream&) const;
};


std::ostream&
operator<<(std::ostream&,const Vert2D&);

}   // NAMESPACE Geometry

#endif
