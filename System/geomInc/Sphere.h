/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Sphere.h
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
#ifndef Geometry_Sphere_h
#define Geometry_Sphere_h

namespace Geometry
{

  class Circle;
 
/*!
  \class Sphere
  \brief Holds a Sphere as vector form
  \author S. Ansell
  \date April 2004
  \version 1.0

  Defines a sphere as a centre point and a radius.
*/

class Sphere : public Quadratic
{
 private:
  
  Geometry::Vec3D Centre;        ///< Point for centre
  double Radius;                 ///< Radius of sphere
  
  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);
  void mirror(const Geometry::Plane&);

 public:

  Sphere();
  Sphere(const int,const int);
  Sphere(const Sphere&);
  Sphere* clone() const;
  Sphere& operator=(const Sphere&);
  bool operator==(const Sphere&) const;
  bool operator!=(const Sphere&) const;
  virtual ~Sphere();
  /// Effective TYPENAME 
  static std::string classType() { return "Sphere"; }
  /// Effective typeid
  virtual std::string className() const 
    { return "Sphere"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  int setSurface(const std::string&);
  int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;
  double distance(const Geometry::Vec3D&) const;

  int setSphere(const Geometry::Vec3D&,const double);
  void setCentre(const Geometry::Vec3D&);              
  void setRadius(double const);

  /// Get Centre
  const Geometry::Vec3D& getCentre() const { return Centre; } 
  double getRadius() const { return Radius; }          ///< Get Radius
  void setBaseEqn();

  virtual void writeFLUKA(std::ostream&) const;
  virtual void writePOVRay(std::ostream&) const;
  virtual void write(std::ostream&) const; 

};

}  // NAMESPACE Geometry
 
#endif
