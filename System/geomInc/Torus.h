/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Torus.h
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
#ifndef Geometry_Torus_h
#define Geometry_Torus_h


namespace Geometry
{

/*!
  \class Torus
  \brief Holds a torus in vector form
  \author S. Ansell
  \date December 2006
  \version 1.0

  Defines a Torus as a centre, normal and
  three parameters:
  - Iradius :: inner radius of the torus
  - Dradius :: displaced radius (the radius away from the plane)
  These are c,b,a in that order.
*/


class Torus : public Surface
{
 private:

  Vec3D Centre;                  ///< Geometry::Vec3D for centre
  Quaternion RotPhase;           ///< Current rotoation
  Vec3D Normal;                  ///< Normal

  double Iradius;                ///< Inner radius
  double Oradius;                ///< Outer radius

  void rotate(const Geometry::Matrix<double>&);
  void displace(const Geometry::Vec3D&);

 public:



  Torus();
  Torus(const Torus&);
  Torus* clone() const;
  Torus& operator=(const Torus&);
  bool operator==(const Torus&) const;
  bool operator!=(const Torus&) const;
  virtual ~Torus();

  /// Effective TYPENAME 
  static std::string classType() { return "Torus"; }
  /// Public identifier
  virtual std::string className() const { return "Torus"; }
    /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  void setCentre(const Vec3D&);
  void setNormal(const Vec3D&);
  void setIRad(const double);
  void setORad(const double);
  //  void setDisplace(const double);

  /// Return centre point
  Geometry::Vec3D getCentre() const { return Centre; }              
  /// Central normal
  Geometry::Vec3D getNormal() const { return Normal; }       
  double getIRad() const { return Iradius; }     ///< IRad accessor
  double getORad() const { return Oradius; }     ///< ORad accessor
  
  int setSurface(const std::string&);
  int side(const Geometry::Vec3D&) const;  
  int onSurface(const Geometry::Vec3D&) const;  
  double distance(const Geometry::Vec3D&) const;   
  void mirror(const Geometry::Plane&);


  Geometry::Vec3D surfaceNormal(const Geometry::Vec3D&) const;

  virtual void writeFLUKA(std::ostream&) const;
  virtual void writePOVRay(std::ostream&) const;
  virtual void write(std::ostream&) const;
  
};

} // NAMESPACE Geometry

#endif
