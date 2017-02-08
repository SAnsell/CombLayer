/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Ellipsoid.h
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
#ifndef Geometry_Ellipsoid_h
#define Geometry_Ellipsoid_h

namespace Geometry
{

/*!
  \class  Ellipsoid
  \brief Holds a cylinder as a vector form
  \author S. Ansell
  \date October 2015
  \version 1.0

  Defines a cylinder as a centre point (on main axis)
  a vector from that point (unit) and a radius.

  Updated to use the Quadratic base class
*/

class Ellipsoid : public Quadratic
{
 private:
  
  Geometry::Vec3D Centre;        ///< Geometry::Vec3D for centre
  Geometry::Vec3D AAxis;         ///< Direction of First tri-axis
  Geometry::Vec3D BAxis;         ///< Direction of Second tri-axis
  Geometry::Vec3D CAxis;         ///< Direction of Third triaxis

  double ARadius;       ///< Radius of Long   (a>b>c)
  double BRadius;       ///< Radius of Mid
  double CRadius;       ///< Radius of Short

  static void orderRadii(Geometry::Vec3D*,double*);
  static double firstOctant(const double[3],const double[3]);
  static double solveTBar(const double[3],const double[3]);
  
 public:

  Ellipsoid();
  Ellipsoid(const int,const int);
  Ellipsoid(const Ellipsoid&);
  Ellipsoid* clone() const;
  Ellipsoid& operator=(const Ellipsoid&);
  virtual ~Ellipsoid();

  bool operator==(const Ellipsoid&) const;
  bool operator!=(const Ellipsoid&) const;

  /// Effective TYPENAME 
  static std::string classType() { return "Ellipsoid"; }
  /// Public identifer
  virtual std::string className() const { return "Ellipsoid"; }  
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
  int setEllipsoid(const Geometry::Vec3D&,const Geometry::Vec3D&,
		   const Geometry::Vec3D&,const double,
		   const double,const double);
  void setCentre(const Geometry::Vec3D&);              		 
  void setRadii(const double,const double,const double);

  
  /// Return centre point       
  const Geometry::Vec3D& getCentre() const { return Centre; }   
  double getARadius() const { return ARadius; }  ///< Get A radius
  double getBRadius() const { return BRadius; }  ///< Get B radius
  double getCRadius() const { return CRadius; }  ///< Get C radius
  ///< Long axis
  const Geometry::Vec3D& getAAxis() const 
    { return AAxis; }       
  ///< Short axis
  const Geometry::Vec3D& getBAxis() const 
    { return BAxis; }       
  ///< Short axis
  const Geometry::Vec3D& getCAxis() const 
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
operator<<(std::ostream&,const Ellipsoid&);


}   // NAMESPACE Geometry

#endif

