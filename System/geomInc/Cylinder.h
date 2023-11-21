/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Cylinder.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef Geometry_Cylinder_h
#define Geometry_Cylinder_h

namespace Geometry
{

/*!
  \class  Cylinder
  \brief Holds a cylinder as a vector form
  \author S. Ansell
  \date April 2004
  \version 1.2

  Defines a cylinder as a centre point (on main axis)
  a vector from that point (unit) and a radius.

  Updated to use the Quadratic base class
*/

class Cylinder : public Quadratic
{
 private:
  
  Geometry::Vec3D Centre;        ///< Geometry::Vec3D for centre
  Geometry::Vec3D Normal;        ///< Direction of centre line
  int Nvec;            ///< Normal vector is x,y or z :: (1-3) (0 if general)
  double Radius;       ///< Radius of cylinder
  
  void setNvec();      ///< check to obtain orientation

 public:

  Cylinder();
  Cylinder(const int,const int);
  Cylinder(const int,Geometry::Vec3D ,
	   const Geometry::Vec3D&,const double);
  Cylinder(const Cylinder&);
  Cylinder* clone() const override;
  Cylinder& operator=(const Cylinder&);
  ~Cylinder() override;


  bool operator==(const Cylinder&) const;
  bool operator!=(const Cylinder&) const;

  /// Effective TYPENAME 
  static std::string classType() { return "Cylinder"; }
  /// Public identifer
  std::string className() const override { return "Cylinder"; }
  /// fast index accessor
  SurfKey classIndex() const override { return SurfKey::Cylinder; }
  /// Visitor acceptance
  void acceptVisitor(Global::BaseVisit& A) const override
    {  A.Accept(*this); }
  /// Accept visitor for input
  void acceptVisitor(Global::BaseModVisit& A) override
    { A.Accept(*this); }

  int side(const Geometry::Vec3D&) const override;
  int onSurface(const Geometry::Vec3D&) const override;
  double distance(const Geometry::Vec3D&) const override;

  int setSurface(const std::string&) override;
  int setCylinder(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  void setCentre(const Geometry::Vec3D&);              
  void setNorm(const Geometry::Vec3D&);       
  void setRadius(double const);

  /// Return centre point
  const Geometry::Vec3D& getCentre() const { return Centre; }
  /// Return Central line
  const Geometry::Vec3D& getNormal() const { return Normal; }  

  double getRadius() const { return Radius; }  ///< Get Radius      
  void setBaseEqn() override;

  void mirror(const Geometry::Plane&) override;
  void rotate(const Geometry::Matrix<double>&) override;
  void rotate(const Geometry::Quaternion&) override;
  void displace(const Geometry::Vec3D&) override;

  void print() const override;
  
  void writeFLUKA(std::ostream&) const override;
  void write(std::ostream&) const override;


};


}   // NAMESPACE Geometry

#endif

