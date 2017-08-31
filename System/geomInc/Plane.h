/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Plane.h
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
#ifndef Geometry_Plane_h
#define Geometry_Plane_h

namespace Geometry
{

/*!
  \class Plane
  \brief Holds a simple Plane
  \author S. Ansell
  \date April 2004
  \version 1.0

  Defines a plane and a normal and a distance from
  the origin
*/

class Plane : public Quadratic
{
 private:

  Geometry::Vec3D NormV;         ///< Normal vector
  double Dist;                   ///< Distance 

  void setBaseEqn();         

 public:

  Plane();
  Plane(const int,const int);
  Plane(const int,const int,const Geometry::Vec3D&,const Geometry::Vec3D&);
  Plane(const Plane&);
  Plane* clone() const;
  Plane& operator=(const Plane&);
  bool operator==(const Plane&) const;
  bool operator!=(const Plane&) const;
  virtual ~Plane();

  /// Effective TYPENAME 
  static std::string classType() { return "Plane"; }
  /// Effective typeid
  virtual std::string className() const { return "Plane"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const 
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  void setNormal(const Vec3D&);
  void setDistance(const double);
  /// Distance from origin
  double getDistance() const { return Dist; }  
  /// Normal to plane (+ve surface)
  const Geometry::Vec3D& getNormal() const { return NormV; } 


  int setSurface(const std::string&);
  
  int setPlane(const Plane&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&,const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const double);

  int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;
  // stuff for finding intersections etc.
  double dotProd(const Plane&) const;   
  Geometry::Vec3D crossProd(const Plane&) const;      
  double distance(const Geometry::Vec3D&) const;
  Geometry::Vec3D closestPt(const Geometry::Vec3D&) const;
  
  int isEqual(const Plane&) const;
  int planeType() const;         ///< are we aligned on an axis

  void mirrorPt(Geometry::Vec3D&) const;
  void mirrorAxis(Geometry::Vec3D&) const;

  void rotate(const Geometry::Matrix<double>&);
  void rotate(const Geometry::Quaternion&);
  void mirror(const Geometry::Plane&);
  void mirrorSelf();

  int reversedPlane() const; 
  void reversePtValid(const int,const Geometry::Vec3D&);

  void displace(const Geometry::Vec3D&);

  void print() const;
  void write(std::ostream&) const;  
  virtual void writeFLUKA(std::ostream&) const;
  virtual void writePOVRay(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Plane&);

} // NAMESPACE Geometry

#endif
