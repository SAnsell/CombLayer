/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/M3.h
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
#ifndef Geometry_M3_h 
#define Geometry_M3_h

template<typename T> class Matrix;

namespace Geometry
{

  class Vec3D;
  template<typename T> class M3vec;

/*!
  \class M3
  \brief Numerical M3 class
  \version 1.0
  \author S. Ansell
  \date  January 2024

  Holds a matrix of 3x3 for calculatoins [e.g. not useing matrix]
*/

template<typename T>
class M3
{
 private:

  T AA[3][3];     ///< original data 

  void copyAll(const M3<T>&);
  M3<T> calcEigenVectors(const std::vector<T>&) const;
  M3vec<T> eigenVector(const size_t,const T&) const;

 public:

  M3();
  explicit M3(const bool);
  M3(const T&,const T&,const T&,
     const T&,const T&,const T&,
     const T&,const T&,const T&);
  M3(const T&,const T&,const T&);
  M3(const T[3][3]);
  M3(const Matrix<T>&);
  //  M3(const std::vector<T>&,const std::vector<T>&,const std::vector<T>&);
  M3(const std::vector<std::vector<T>>&); 
  M3(const M3<T>&);
  M3<T>& operator=(const M3<T>&); 
  ~M3() =default;

  //< Ptr accessor
  const T* operator[](const size_t I) const { return AA[I]; }
  //< Ptr accessor
  T* operator[](const size_t I) { return AA[I]; }             

  void nameAssign(T&,T&,T&,T&,T&,T&,T&,T&,T&) const;

  T& get(const size_t,const size_t);
  const T& get(const size_t,const size_t) const;
  void setRow(const size_t,const Geometry::M3vec<T>&);
  void setColumn(const size_t,const Geometry::M3vec<T>&);
  
  M3<T>& operator+=(const M3<T>&);  
  M3<T> operator+(const M3<T>&) const;    

  M3<T>& operator-=(const M3<T>&);  
  M3<T> operator-(const M3<T>&) const;
  M3<T> operator-() const;    

  M3<T> operator*(const M3<T>&) const; 
  M3<T> operator*(const T&) const;
  M3<T> operator/(const T&) const;
  Geometry::Vec3D operator*(const Geometry::Vec3D&) const; 
  Geometry::M3vec<T> operator*(const Geometry::M3vec<T>&) const;
  
  M3<T>& operator*=(const M3<T>&);
  M3<T>& operator*=(const T&);

  M3<T>& operator/=(const T&);             

  virtual bool operator==(const M3<T>&) const; 
  virtual bool operator!=(const M3<T>&) const;
  
  M3<T>& zero();
  M3<T>& identity();
  M3<T>& invert();
  M3<T>& transpose();
  M3<T>& adjoint();
  M3<T> Tprime() const;
  M3<T> inverse() const;

  void rowEchelon();
		  
  size_t getEigenValues(T&,T&,T&) const;
  bool QR(M3<T>&,M3<T>&) const;
  T determinate() const;
  bool diagonalize(M3<T>&,M3<T>&,M3<T>&) const;
  
  void write(std::ostream&) const;
  
}; 

template<typename T>
std::ostream&
operator<<(std::ostream&,const M3<T>&);

}

#endif














