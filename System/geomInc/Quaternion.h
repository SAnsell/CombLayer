/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Quaternion.h
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
#ifndef Geometry_Quaternion_h
#define Geometry_Quaternion_h

namespace Geometry
{

/*!
  \class Quaternion
  \version 1.0
  \author S. Ansell
  \brief Quaternion object
  \date July 2008 

  This is a quaterion system, it allows 
  it to be transported into 
*/

class Quaternion
{
 private:
  
  double q0;       ///< Q0 value
  Vec3D Qvec;      ///< QVec (q1,q2,q3)
  
 public:
  
  Quaternion();
  Quaternion(const double,const double,const double,const double);
  Quaternion(const double,const Geometry::Vec3D&);
  
  Quaternion(const Quaternion&);
  Quaternion& operator=(const Quaternion&);
  bool operator==(const Quaternion&) const;
  bool operator!=(const Quaternion&) const;
  ~Quaternion();
  
  double operator[](const size_t) const; 
  double& operator[](const size_t); 
  
  static double calcQ0(const double);
  static double calcQ0deg(const double);
  static Quaternion calcQRot(const double,const double,const double,const double);
  static Quaternion calcQRot(const double,Geometry::Vec3D);
  static Quaternion calcQRotDeg(const double,const double,const double,const double);
  static Quaternion calcQRotDeg(const double,Geometry::Vec3D);
  static Quaternion calcQVRot(const Geometry::Vec3D&,const Geometry::Vec3D&,
			      const Geometry::Vec3D&);
  
  static Quaternion basisRotate
    (const Geometry::Vec3D&,const Geometry::Vec3D&,const Geometry::Vec3D&,
     const Geometry::Vec3D&,const Geometry::Vec3D&,const Geometry::Vec3D&);
  
  Quaternion& complement();
  Quaternion& inverse();
  
  double modulus() const;
  double makeUnit();
  
  Quaternion& operator+=(const Quaternion&);
  Quaternion operator+(const Quaternion&) const ;
  Quaternion& operator-=(const Quaternion&);
  Quaternion operator-(const Quaternion&) const ;
  Quaternion& operator*=(const Vec3D&);
  Quaternion operator*(const Vec3D&) const;
  Quaternion& operator*=(const Quaternion&);
  Quaternion operator*(const Quaternion&) const;
  
  /// Access the vector
  const Vec3D& getVec() const { return Qvec; }
  Vec3D getAxis() const;
  double getTheta() const; 
  
  Matrix<double> qMatrix() const;
  Matrix<double> rMatrix() const;
  Vec3D& rotate(Vec3D&) const;
  Vec3D& invRotate(Vec3D&) const;
  
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
