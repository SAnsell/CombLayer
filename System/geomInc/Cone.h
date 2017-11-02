/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Cone.h
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
#ifndef Geometry_Cone_h
#define Geometry_Cone_h

namespace Geometry
{

/*!
  \class Cone
  \brief Holds a cone in vector form
  \author S. Ansell
  \date April 2004
  \version 1.0

  Defines a cone as a centre point (on main axis)
  a vector from that point (unit) and a radius.
  and an angle.
*/

class Cone : public Quadratic
{
 private:
  
  Geometry::Vec3D Centre;        ///< Geometry::Vec3D for centre
  Geometry::Vec3D Normal;        ///< Normal
  double alpha;                  ///< Angle (degrees)
  double cangle;                 ///< Cos(angle)
  int cutFlag;                   ///< Cut flag [+/-1 or zero]
  
 public:

  Cone();
  Cone(const int,const int);
  Cone(const Cone&);
  Cone* clone() const;
  Cone& operator=(const Cone&);
  bool operator==(const Cone&) const;
  bool operator!=(const Cone&) const;
  ~Cone();

  /// Effective TYPENAME 
  static std::string classType() { return "Cone"; }
  /// Public identifier
  virtual std::string className() const { return "Cone"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }
  
  int side(const Geometry::Vec3D&) const;
  int onSurface(const Geometry::Vec3D&) const;
  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const;    

  /// Return centre point
  Geometry::Vec3D getCentre() const { return Centre; }              
  /// Central normal
  Geometry::Vec3D getNormal() const { return Normal; }       
  /// Edge Angle
  double getCosAngle() const { return cangle; } 
  /// Edge Angle
  double getAlpha() const { return alpha; } 

  double distance(const Geometry::Vec3D&) const;   
  void rotate(const Geometry::Matrix<double>&);
  void rotate(const Geometry::Quaternion&);
  void displace(const Geometry::Vec3D&);

  int setSurface(const std::string&);
  void setCone(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  void setCentre(const Geometry::Vec3D&);              
  void setNormal(const Geometry::Vec3D&);       
  void setAngle(double const);  
  void setTanAngle(double const);
  void setBaseEqn();
  /// Set the flag
  void setCutFlag(const int F) { cutFlag=F; }
  int getCutFlag() const { return cutFlag; }

  void write(std::ostream&) const;  
};

}  // NAMESPACE Geometry

#endif
