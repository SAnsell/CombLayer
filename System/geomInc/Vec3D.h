/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Vec3D.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef Geometry_Vec3D_h
#define Geometry_Vec3D_h

#ifndef M_PI
#define M_PI 3.14159265358979323844
#endif

namespace Geometry 
{

template<typename T> class Matrix;
template<typename T> class M3;

const double zeroTol(1e-8);       ///< Zero occured
const double shiftTol(1e-5);      ///< Shift a point to change state
const double parallelTol(1e-10);  ///< parallel occored
const int Nprecision(10);         ///< Precision of the output

/*!
  \class Vec3D
  \brief Basic 3D point class
  \version 1.1
  \date August 2004
  \author S. Ansell

  Simple Vec3D function based on x,y,z and providing
  simple dot and cross products
 */

class Vec3D
{
 protected:
  
  double x;        ///< X-Coordinates
  double y;        ///< Y-Coordinates
  double z;        ///< Z-Coordinates
  
 public:
  
  Vec3D();
  Vec3D(const double,const double,const double);
  explicit Vec3D(const double*);
  Vec3D(const Vec3D&);
  Vec3D(Vec3D&&) =default;
  virtual ~Vec3D();

  const double& X() const { return x; }   ///< Accessor function (X)
  const double& Y() const { return y; }   ///< Accessor function (Y)
  const double& Z() const { return z; }   ///< Accessor function (Z)

  Vec3D& operator=(const Vec3D&);
  Vec3D& operator=(Vec3D&&) =default;
  Vec3D& operator()(const double,const double,const double);

  template<typename IT> double& operator[](const IT);
  template<typename IT> double operator[](const IT) const;

  /// Convert matrix to a point (3x1 or 1x3)
  template<typename T> Vec3D operator()
     (const Matrix<T>&) const;
  
  bool operator<(const Vec3D&) const;
  bool operator>(const Vec3D&) const;
  
  Vec3D& operator*=(const Vec3D&);
  template<typename T> Vec3D& operator*=(const Matrix<T>&);
  template<typename T> Vec3D& operator*=(const M3<T>&);
  Vec3D& operator*=(const double);
  
  Vec3D& operator/=(const double);
  Vec3D& operator+=(const Vec3D&);
  Vec3D& operator-=(const Vec3D&);

  Vec3D operator*(const Vec3D&) const;
  Vec3D operator*(const double) const;     // Scale factor
  template<typename T> Vec3D operator*(const Matrix<T>&) const;
  template<typename T> Vec3D operator*(const M3<T>&) const;

  Vec3D operator/(const double) const;     // Scale factor
  Vec3D operator+(const Vec3D&) const;
  Vec3D operator-(const Vec3D&) const;
  Vec3D operator-() const;
 
  bool operator==(const Vec3D&) const;
  bool operator!=(const Vec3D&) const;
  virtual void rotate(const Vec3D&,const Vec3D&,const double);
  virtual void rotate(const Vec3D&,const double);
  
  double Distance(const Vec3D&) const;    ///< Calculate scale distance
  double makeUnit();                      ///< Convert into unit vector
  Geometry::Vec3D unit() const; 
  Geometry::Vec3D component(const Geometry::Vec3D&) const; 
  Geometry::Vec3D cutComponent(const Geometry::Vec3D&) const;
  double getComponent(const Geometry::Vec3D&) const;
  double getScalant(const Geometry::Vec3D&) const;
  void makePosCos(const Geometry::Vec3D&);
  void makePosPrinciple();
  
  /// Calculate the volmue of a cube X*Y*Z
  double volume() const { return std::abs(x*y*z); }      

  M3<double> outerProd(const Vec3D&) const;
  double dotProd(const Vec3D&) const;
  double abs() const;
  double absFlat(const size_t) const;    
  template<typename T> void rotate(const Matrix<T>&); 
  template<typename T> void rotate(const M3<T>&);
  
  Vec3D& boundaryCube(const Vec3D&,const Vec3D&);

  int forwardBase(const Vec3D&,const Vec3D&,const Vec3D&);
  Vec3D getInBasis(const Vec3D&,const Vec3D&,const Vec3D&) const;
  int reBase(const Vec3D&,const Vec3D&,const Vec3D&);   
  int masterDir(const double= Geometry::zeroTol) const;
  size_t principleDir() const;
  int nullVector(const double= Geometry::zeroTol) const;             
  int coLinear(const Vec3D&,const Vec3D&) const;
  Vec3D crossNormal() const; 

  void read(std::istream&);
  void write(std::ostream&) const;
};

Vec3D angleVec(const double,const double);

std::ostream& operator<<(std::ostream&,const Vec3D&);
std::istream& operator>>(std::istream&,Vec3D&);

}   // NAMESPACE Geometry

#endif

