/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/M2.h
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
#ifndef M2_h 
#define M2_h

/*!
   \namespace Geometry
   \brief Classes for geometric handlers
   \version 1.0
   \author S. Ansell
   \date February 2006

*/

namespace Geometry
{

  class Vec2D;
  class Vec3D;
  
/*!
  \class M2
  \brief Numerical M2 class
  \version 1.0
  \author S. Ansell
  \date August 2023

  Holds a matrix of 2x2 for simple SVD.
  Such that A=U.S.V'
*/

template<typename T>
class M2
{
 private:

  T AData[2][2];     ///< original data 

  T U[2][2];          ///< U data
  T Sigma[2];         ///< sigma (diagonal matrix)

  T V[2][2];          ///< V data
  T R[2][2];          ///< R eigen rotation
  T lambda[2][2];     ///< Eigen values [diagonal]

  void copyAll(const M2<T>&);
  
 public:

  M2();
  M2(const T&,const T&,const T&,const T&);
  M2(const T[2][2]);
  M2(const std::vector<T>&,const std::vector<T>&);
  M2(const std::vector<T>&); 
  M2(const M2<T>&);
  M2<T>& operator=(const M2<T>&); 
  ~M2() =default;

  //< Ptr accessor
  const T* operator[](const size_t I) const { return AData[I]; }
  //< Ptr accessor
  T* operator[](const size_t I) { return AData[I]; }             
  void nameAssign(T&,T&,T&,T&) const;
  
  M2<T>& operator+=(const M2<T>&);  
  M2<T> operator+(const M2<T>&) const;    

  M2<T>& operator-=(const M2<T>&);  
  M2<T> operator-(const M2<T>&) const;
  M2<T> operator-() const;    

  M2<T> operator*(const M2<T>&) const; 
  M2<T> operator*(const T&) const;
  Geometry::Vec2D operator*(const Geometry::Vec2D&) const; 
  

  M2<T>& operator*=(const M2<T>&);
  M2<T>& operator*=(const T&);             

  bool operator==(const M2<T>&) const; 
  bool operator!=(const M2<T>&) const;

  T& get(const size_t,const size_t);
  const T& get(const size_t,const size_t) const;
  
  void constructSVD();
  void constructEigen();
  void reCalcSVD();

  M2<T>& invert();
  M2<T>& transpose();
  M2<T>& adjoint();
  M2<T> prime() const;
  void sortEigen();
  
  const T& getU(const size_t,const size_t) const;
  std::pair<T,T> getS() const;

  void setMinimumSigma(const T&);
  void invertSigma();

  T maxSigma() const;
  T minSigma() const;
  T determinate() const;
  T magVector(const T*) const;
  std::pair<T,T> getEigPair() const;
  Geometry::Vec2D getEigVec(const size_t) const;

  M2<T> getEigValues() const;
  M2<T> getEigVectors() const;
  bool check() const;
  void write(std::ostream&) const;
  
}; 

template<typename T>
std::ostream&
operator<<(std::ostream&,const M2<T>&);

}

#endif














