/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/EllipticCyl.h
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
#ifndef Geometry_EllipticCyl_h
#define Geometry_EllipticCyl_h

namespace Geometry
{

/*!
  \class  EllipticCyl
  \brief Holds a cylinder as a vector form
  \author S. Ansell
  \date August 2012
  \version 1.2

  Defines a cylinder as a centre point (on main axis)
  a vector from that point (unit) and a radius.

  Updated to use the Quadratic base class
*/

class EllipticCyl : public Quadratic
{
 private:
  
  Geometry::Vec3D Centre;        ///< Geometry::Vec3D for centre
  Geometry::Vec3D Normal;        ///< Direction of centre line
  Geometry::Vec3D LAxis;         ///< Direction of Long axis
  Geometry::Vec3D CAxis;         ///< Direction of Short axis

  double ARadius;       ///< Radius of Long 
  double BRadius;       ///< Radius of Short

  static double firstQuadrant(const double[2],const double[2]);
  
 public:

  EllipticCyl();
  EllipticCyl(const int,const int);
  EllipticCyl(const EllipticCyl&);
  EllipticCyl* clone() const;
  EllipticCyl& operator=(const EllipticCyl&);
  virtual ~EllipticCyl();

  bool operator==(const EllipticCyl&) const;
  bool operator!=(const EllipticCyl&) const;

  /// Effective TYPENAME 
  static std::string classType() { return "EllipticCyl"; }
  /// Public identifer
  virtual std::string className() const { return "EllipticCyl"; }  
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;
  double distance(const Geometry::Vec3D&) const;

  int setSurface(const std::string&);
  int setEllipticCyl(const Geometry::Vec3D&,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const double,const double);
  void setCentre(const Geometry::Vec3D&);              
  int setNorm(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setRadius(const double,const double);

  /// Return centre point       
  const Geometry::Vec3D& getCentre() const { return Centre; }   
  /// Return Normal
  const Geometry::Vec3D& getNormal() const { return Normal; }   
  double getARadius() const { return ARadius; }  ///< Get A radius
  double getBRadius() const { return BRadius; }  ///< Get B radius
  ///< Long axis
  const Geometry::Vec3D& getAAxis() const 
    { return LAxis; }       
  ///< Short axis
  const Geometry::Vec3D& getBAxis() const 
    { return CAxis; }       


  void setBaseEqn();

  void mirror(const Geometry::Plane&);
  void rotate(const Geometry::Matrix<double>&);
  void rotate(const Geometry::Quaternion&);
  void displace(const Geometry::Vec3D&);

  virtual void print() const;

  virtual void writePOVRay(std::ostream&) const;
  virtual void write(std::ostream&) const;


};

std::ostream&
operator<<(std::ostream&,const EllipticCyl&);


}   // NAMESPACE Geometry

#endif

