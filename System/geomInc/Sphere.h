/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Sphere.h
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
  
  void rotate(const Geometry::M3<double>&) override;
  void displace(const Geometry::Vec3D&) override;
  void mirror(const Geometry::Plane&) override;

 public:

  Sphere();
  explicit Sphere(const int);
  Sphere(const Sphere&);
  Sphere* clone() const override;
  Sphere& operator=(const Sphere&);
  bool operator==(const Sphere&) const;
  bool operator!=(const Sphere&) const;
  ~Sphere() override;
  /// Effective TYPENAME 
  static std::string classType() { return "Sphere"; }
  /// Effective typeid
  std::string className() const override { return "Sphere"; }
  /// fast index accessor
  SurfKey classIndex() const override { return SurfKey::Sphere; }
  /// Visitor acceptance
  void acceptVisitor(Global::BaseVisit& A) const override
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  int setSurface(const std::string&) override;
  int side(const Geometry::Vec3D&) const override;
  int onSurface(const Geometry::Vec3D&) const override;
  double distance(const Geometry::Vec3D&) const override;

  int setSphere(const Geometry::Vec3D&,const double);
  void setCentre(const Geometry::Vec3D&);              
  void setRadius(double const);

  /// Get Centre
  const Geometry::Vec3D& getCentre() const { return Centre; } 
  double getRadius() const { return Radius; }          ///< Get Radius
  void setBaseEqn() override;

  void writeFLUKA(std::ostream&) const override;
  void writePOVRay(std::ostream&) const override;
  void write(std::ostream&) const override; 

};

}  // NAMESPACE Geometry
 
#endif
