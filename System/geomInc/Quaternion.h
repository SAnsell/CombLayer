/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Quaternion.h
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
#ifndef Geometry_Quaternion_h
#define Geometry_Quaternion_h

namespace Geometry
{
  template<typename T> class M3;
  
/*!
  \class Quaternion
  \version 1.0
  \author S. Ansell
  \brief Quaternion object
  \date July 2008 

  This is a quaterion system, it allows 
  rotations and other basic rotations. 
*/

class Quaternion
{
 private:
  
  double q0;       ///< Q0 value
  Vec3D Qvec;      ///< QVec (q1,q2,q3)
  
 public:
  
  Quaternion();
  Quaternion(const double,const double,const double,const double);
  Quaternion(const double,Geometry::Vec3D);

  Quaternion(const Quaternion&);
  Quaternion& operator=(const Quaternion&);
  ~Quaternion();

  // ----------------------------------------
  // static
  // ----------------------------------------
  static double calcQ0(const double);
  static double calcQ0deg(const double);
  static Quaternion calcQRot(const double,const double,const double,const double);
  static Quaternion calcQRot(const double,Geometry::Vec3D);
  static Quaternion calcQRotDeg(const double,const double,const double,const double);
  static Quaternion calcQRotDeg(const double,Geometry::Vec3D);
  static Quaternion calcQVRot(const Geometry::Vec3D&,const Geometry::Vec3D&,
			      const Geometry::Vec3D&);
  static Quaternion calcQRotMatrix(const Geometry::M3<double>&);
  
  static Quaternion basisRotate
    (const Geometry::Vec3D&,const Geometry::Vec3D&,const Geometry::Vec3D&,
     const Geometry::Vec3D&,const Geometry::Vec3D&,const Geometry::Vec3D&);
  // ----------------------------------------

  
  
  bool operator==(const Quaternion&) const;
  bool operator!=(const Quaternion&) const;

  double operator[](const size_t) const; 
  double& operator[](const size_t); 
  
  Quaternion& operator+=(const Quaternion&);
  Quaternion operator+(const Quaternion&) const ;
  Quaternion& operator-=(const Quaternion&);
  Quaternion operator-(const Quaternion&) const ;
  Quaternion& operator*=(const Vec3D&);
  Quaternion operator*(const Vec3D&) const;
  Quaternion& operator*=(const Quaternion&);
  Quaternion operator*(const Quaternion&) const;
  Quaternion& operator*=(const double&);
  Quaternion operator*(const double&) const;
  Quaternion& operator/=(const double&);
  Quaternion operator/(const double&) const;

  
  Quaternion& complement();
  Quaternion& inverse();
  
  double modulus() const;
  double makeUnit();
  

  /// Access Q0 value
  double getQ0() const { return q0; }
  /// Access the vector
  const Vec3D& getVec() const { return Qvec; }
  Vec3D getAxis() const;
  double getTheta() const; 
  
  Matrix<double> qMatrix() const;
  M3<double> rMatrix() const;
  Vec3D& rotate(Vec3D&) const;
  Vec3D& invRotate(Vec3D&) const;

  Vec3D makeRotate(const Vec3D&) const;
  Vec3D makeInvRotate(const Vec3D&) const;

  void rotateBasis(Vec3D&,Vec3D&,Vec3D&) const;
  void invRotateBasis(Vec3D&,Vec3D&,Vec3D&) const;
  
  bool zeroAngle(const double&) const;
  
  void read(std::istream&);
  void write(std::ostream&) const;
};


std::ostream&
operator<<(std::ostream&,const Geometry::Quaternion&);

std::istream&
operator>>(std::istream&,Geometry::Quaternion&);

} // NAMESPACE Geometry


#endif
