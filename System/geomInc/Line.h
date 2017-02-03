/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Line.h
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
#ifndef Geometry_Line_h
#define Geometry_Line_h

namespace Geometry
{

/*!
  \class Line
  \brief Impliments a line
  \author S. Ansell
  \date Apr 2005
  \version 0.7
  
  Impliments the line 
  \f[ r=\vec{O} + \lambda \vec{n} \f]
*/

class Surface;
class Quadratic;

class ArbPoly;
class CylCan;
class Cylinder;
class Cone;
class General;
class MBrect;
class Plane;
class Sphere;
class Torus;
  
class Line 
{
  
 private:
  
  Geometry::Vec3D Origin;   ///< Orign point (on plane)
  Geometry::Vec3D Direct;   ///< Direction of outer surface (Unit Vector) 
  
  size_t lambdaPair(const size_t,const std::pair<std::complex<double>,
		    std::complex<double> >&,std::vector<Geometry::Vec3D>&) const;

 public: 

  Line();
  Line(const Geometry::Vec3D&,const Geometry::Vec3D&);
  Line(const Line&);
  Line& operator=(const Line&);
  virtual ~Line();

  /// Effective typeid
  virtual std::string className() const { return "Line"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  void setOrigin(const Geometry::Vec3D& Pt) { Origin=Pt; }  ///< Set Origin
  void setDirect(const Geometry::Vec3D&);

  Geometry::Vec3D getPoint(const double) const;   ///< gets the point O+lam*N
  Geometry::Vec3D getOrigin() const { return Origin; }   ///< returns the origin
  Geometry::Vec3D getDirect() const { return Direct; }   ///< returns the direction
  double distance(const Geometry::Vec3D&) const;  ///< distance from line
  Geometry::Vec3D closestPoint(const Geometry::Vec3D&) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    closestPoints(const Line&) const; 
  int isValid(const Geometry::Vec3D&) const;     ///< Is the point on the line

  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);

  int setLine(const Geometry::Vec3D&,const Geometry::Vec3D&); 
  int setLine(const Geometry::Plane&,const Geometry::Plane&);
  
  template<typename T> Geometry::Vec3D interPoint(const T&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Quadratic&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const ArbPoly&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Cone&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const CylCan&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Cylinder&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const EllipticCyl&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const MBrect&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Plane&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Sphere&) const;
  size_t intersect(std::vector<Geometry::Vec3D>&,const Torus&) const;

  //  int intersect(std::vector<Geometry::Vec3D>&,const Line*) const;
  //  int intersect(std::vector<Geometry::Vec3D>&,const Circle*) const;

  void print() const;
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const Line&);

}  // NAMESPACE Geometry

#endif
