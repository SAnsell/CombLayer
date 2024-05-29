/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/M3vec.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

#include "Exception.h"
#include "M3.h"
#include "M3vec.h"


namespace Geometry
{

template<typename T>
std::ostream&
operator<<(std::ostream& OX,const M3vec<T>& A)
{
  A.write(OX);
  return OX;
}
  
template<typename T>
M3vec<T>::M3vec() :
  x(0),y(0),z(0)
  /*!
    Constructor from base on row index
    \param M :: M3 matrix
    \param rowIndex :: row lind
  */
{}


template<typename T>
M3vec<T>::M3vec(const M3<T>& M,const size_t rowIndex) :
  x(M[rowIndex][0]),
  y(M[rowIndex][1]),
  z(M[rowIndex][2])
  /*!
    Constructor from base on row index
    \param M :: M3 matrix
    \param rowIndex :: row lind
  */
{}

template<typename T>
M3vec<T>::M3vec(const size_t colIndex,const M3<T>& M) :
  x(M[0][colIndex]),
  y(M[1][colIndex]),
  z(M[2][colIndex])
  /*!
    Constructor from base on row index
    \param M :: M3 matrix
    \param rowIndex :: row lind
  */
{}

template<typename T>
M3vec<T>::M3vec(const T& a,const T& b,const T& c) :
  x(a),y(b),z(c)
  /*!
    Constructor from base values
    \param a :: x paramter
    \param b :: x paramter
    \param c :: x paramter
  */
{}
  
template<typename T>
void
M3vec<T>::setM3(const size_t colIndex,M3<T>& M) const
/*!
    Put M3vec into M3 (columns)
    \param M :: M3 matrix
    \param colIndex :: column index
  */
{
  M[0][colIndex]=x;
  M[1][colIndex]=y;
  M[2][colIndex]=z;
  return;
}

template<typename T>
void
M3vec<T>::setM3(M3<T>& M,const size_t rowIndex) const
/*!
    Put M3vec into M3 (row)
    \param M :: M3 matrix
    \param rowIndex :: column index
  */
{
  M[rowIndex][0]=x;
  M[rowIndex][1]=y;
  M[rowIndex][2]=z;
  return;
}

template<typename T>  
M3vec<T>&
M3vec<T>::operator*=(const T& V) 
  /*!
    Simple multiplication of this * Value 
    \param V :: scalar to multiply each component 
    \return M3vec scaled by value
  */
{
  x*=V;
  y*=V;
  z*=V;
  return *this;
}

template<typename T>  
M3vec<T>&
M3vec<T>::operator/=(const T& V) 
  /*!
    Simple divition of this/Value 
    \param V :: scalar to divide each component [no zero check]
    \return M3vec scaled by value
  */
{
  x/=V;
  y/=V;
  z/=V;
  return *this;
}

template<typename T>  
M3vec<T>&
M3vec<T>::operator*=(const M3vec<T>& V) 
  /*!
    Simple multiplication of this/Value 
    \param V :: scalar to multiply each component 
    \return M3vec scaled by value
  */
{
  *this = operator*(V);
  return *this;
}

template<typename T>  
M3vec<T>
M3vec<T>::operator*(const T& V) const
  /*!
    Simple multiplication of this/Value 
    \param V :: scalar to multiply each component 
    \return M3vec scaled by value
  */
{
  M3vec<T> X(*this);
  X*=V;
  return X;
}


template<typename T>    
M3vec<T>
M3vec<T>::operator*(const M3vec<T>& V) const
  /*!
    Cross produce of this*A
    \param A :: M3vec 
    \return this X A
  */
{
  return M3vec<T>
    ( y*V.z - z*V.y,
      z*V.x - x*V.z,
      x*V.y - y*V.x );
}

template<typename T>  
M3vec<T>
M3vec<T>::operator/(const T& V) const
  /*!
    Simple multiplication of this/Value 
    \param V :: scalar to multiply each component 
    \return M3vec scaled by value
  */
{
  M3vec<T> X(*this);
  X/=V;
  return X;
}
  
template<typename T>
T
M3vec<T>::dotProd(const M3vec<T>& A) const
  /*!
    Calculate the dot product.
    \param A :: vector to take product from
    \returns this.A
  */
{
  return A.x*x+A.y*y+A.z*z;
}

template<typename T>
M3vec<T>
M3vec<T>::crossNormal() const
  /*!
    Calculate the normal to the vector without reference to another
    \return Normal vector
   */
{
  M3vec<T> N(y,z,x);

  return (*this * N);
}

template<typename T>
T
M3vec<T>::abs() const
  /*!
    Calculate the magnatude of the point
    \returns \f$ | this | \f$ 
  */
{
  return std::sqrt(x*x+y*y+z*z);
}

template<typename T>
T
M3vec<T>::makeUnit()
  /*!
    Make the vector a unit vector 
    \return :: the old magnitude 
  */
{
  const T Sz(abs());
  if (Sz>1e-30)
    {
      x/=Sz;
      y/=Sz;
      z/=Sz;
    }
  return Sz;
}

template<typename T>
void
M3vec<T>::write(std::ostream& OX) const
  /*!
    Write out the point values
    \param OX :: Output stream
  */
{
  OX<<x<<" "<<y<<" "<<z;
  return;
}

/// \cond TEMPLATE 

template class M3vec<long double>;
template class M3vec<double>;

template std::ostream& operator<<(std::ostream&,const M3vec<long double>&);
template std::ostream& operator<<(std::ostream&,const M3vec<double>&);

/// \endcond TEMPLATE   

}  // NAMESPACE Geometry
