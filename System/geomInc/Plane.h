/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Plane.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

  class Quaternion;
  
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

  void setBaseEqn() override;         

 public:

  Plane();
  explicit Plane(const int);
  Plane(const int,const Geometry::Vec3D&,const Geometry::Vec3D&);
  Plane(const Plane&);
  Plane* clone() const override;
  Plane& operator=(const Plane&);
  bool operator==(const Plane&) const;
  bool operator!=(const Plane&) const;
  ~Plane() override;

  /// Effective TYPENAME 
  static std::string classType() { return "Plane"; }
  /// Effective typeid
  std::string className() const override { return "Plane"; }
  /// fast index accessor
  SurfKey classIndex() const override { return SurfKey::Plane; }

  /// Visitor acceptance
  void acceptVisitor(Global::BaseVisit& A) const override 
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  void setNormal(const Vec3D&);
  void setDistance(const double);
  /// Distance from origin
  double getDistance() const { return Dist; }  
  /// Normal to plane (+ve surface)
  const Geometry::Vec3D& getNormal() const { return NormV; } 
  

  int setSurface(const std::string&) override;
  
  int setPlane(const Plane&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&,const Geometry::Vec3D&);
  int setPlane(const Geometry::Vec3D&,const double);

  int side(const Geometry::Vec3D&) const override;
  int onSurface(const Geometry::Vec3D&) const override;
  // stuff for finding intersections etc.
  double dotProd(const Plane&) const;   
  Geometry::Vec3D crossProd(const Plane&) const;      
  double distance(const Geometry::Vec3D&) const override;
  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const override;
  Geometry::Vec3D closestPt(const Geometry::Vec3D&) const;
  
  int isEqual(const Plane&) const;
  int planeType() const;         ///< are we aligned on an axis

  void mirrorPt(Geometry::Vec3D&) const;
  void mirrorAxis(Geometry::Vec3D&) const;

  void rotate(const Geometry::M3<double>&) override;
  void rotate(const Geometry::Quaternion&) override;
  void mirror(const Geometry::Plane&) override;
  void mirrorSelf();

  int reversedPlane() const; 
  void reversePtValid(const int,const Geometry::Vec3D&);

  void displace(const Geometry::Vec3D&) override;

  void print() const override;
  void write(std::ostream&) const override;  
  void writeFLUKA(std::ostream&) const override;
  void writePOVRay(std::ostream&) const override;
};

std::ostream&
operator<<(std::ostream&,const Plane&);

} // NAMESPACE Geometry

#endif
