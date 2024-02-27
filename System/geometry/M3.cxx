/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/M3.cxx
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <fmt/core.h>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "M3.h"


namespace Geometry
{
  
template<typename T>
std::ostream& 
operator<<(std::ostream& OX,const M3<T>& A)
  /*!
    External outputs point to a stream 
    \param A :: M3 to write out
    \param OX :: output stream
    \returns The output stream (of)
  */
{
  OX<<std::endl;
  A.write(OX);
  return OX;
}

template<typename T>
M3<T>::M3() 
  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j]=0.0;
}

template<typename T>
M3<T>::M3(const T A[3][3])

  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j]=A[i][j];
}

template<typename T>
M3<T>::M3(const std::vector<std::vector<T>>& V)
  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j]=V[i][j];
}

template<typename T>
M3<T>::M3(const T& a,const T& b,const T& c,
	  const T& d,const T& e,const T& f,
	  const T& g,const T& h,const T& i)

  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  AData[0][0]=a;  AData[0][1]=b;  AData[0][2]=c;
  AData[1][0]=d;  AData[1][1]=e;  AData[1][2]=f;
  AData[2][0]=g;  AData[2][1]=h;  AData[2][2]=i;
}

template<typename T>
M3<T>::M3(const M3<T>& A)
  /*! 
    Simple copy constructor
    \param A :: Object to copy
  */
{
  copyAll(A);
}
  
template<typename T>
M3<T>&
M3<T>::operator=(const M3<T>& A)
  /*! 
    Simple assignment operator 
    \param A :: Object to copy
    \return *this
  */
{
  if (&A!=this)
    {
      copyAll(A);
    }
  return *this;
}

template<typename T>  
void
M3<T>::copyAll(const M3<T>& A)
{
  for(size_t i=0;i<3;i++)
    {
      for(size_t j=0;j<3;j++)
	{
	  AData[i][j]=A.AData[i][j];
	}
    }
  return;
}

template<typename T>
bool
M3<T>::operator!=(const M3<T>& M) const
  /*!
    Checkf if two M3 items are not itentical
  */
{
  return (operator==(M)) ? 0 : 1;
}

template<typename T>
bool
M3<T>::operator==(const M3<T>& M) const
  /*!
    Checkf if two M3 items are itentical
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      if (AData[i][j] != M.AData[i][j])
	return 0;
  
  return 1;
}

template<typename T>
M3<T>
M3<T>::operator+(const M3<T>& M) const
{
  M3<T> A(*this);
  A+=M;
  return A;
}

template<typename T>
M3<T>
M3<T>::operator-(const M3<T>& M) const
{
  M3<T> A(*this);
  A-=M;
  return A;
}


template<typename T>
M3<T>&
M3<T>::operator+=(const M3<T>& M)
  /*!
    Add M3 together
   */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j] += M.AData[i][j];
  
  return *this;
}

template<typename T>
M3<T>&
M3<T>::operator-=(const M3<T>& M)
  /*!
    Subtract M3 together
   */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j] -= M.AData[i][j];
  
  return *this;
}

template<typename T>
M3<T>
M3<T>::operator-() const
  /*!
    SCale by -1 e.g -M
  */
{
  M3<T> M(*this);
  M*=-1.0;
  return M;
}
  
template<typename T>
M3<T>&
M3<T>::operator*=(const M3<T>& M)
/*!
  MatrixBase multiplication THIS *= A  
  Note that we call operator* to avoid the problem of overlap
  \param A :: MatrixBase to multiply by  (this->row must == A->columns)
  \return This *= A 
*/
{
  *this = this->operator*(M);
  return *this;
}

template<typename T>
M3<T>
M3<T>::operator*(const T& V) const
{
  M3<T> A(*this);
  A*=V;
  return A;
  
}

template<typename T>
M3<T>&
M3<T>::operator*=(const T& V) 
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AData[i][j]*=V;

  return *this;
}

template<typename T>
M3<T>
M3<T>::operator*(const M3<T>& M) const
  /*!
    Simple matrix multiplication
    \param M :: Matrix to multiply by
   */
{
  M3<T> Out;
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      for(size_t k=0;k<3;k++)
	Out.AData[i][j]+=AData[i][k]*M.AData[k][j];
  

  return Out;
}

  
template<typename T>
Geometry::Vec3D
M3<T>::operator*(const Geometry::Vec3D& V) const
  /*!
    Multiply out M*V to give a Vec3D
  */
{
  Geometry::Vec3D out;
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      out[i]+=static_cast<double>(AData[i][j])*V[j];

  return out;
}
  
template<typename T>
const T&
M3<T>::get(const size_t i,const size_t j) const
/*!
  Get an indexed variable [not check]
 */
{
  if (i>2 || j>2)
    throw ColErr::IndexError<size_t>(i,j,"i/j out of range (2)");
  return AData[i][j];
}

template<typename T>
T&
M3<T>::get(const size_t i,const size_t j) 
/*!
  Get an indexed variable [not check]
 */
{
  if (i>2 || j>2)
    throw ColErr::IndexError<size_t>(i,j,"i/j out of range (2)");
  return AData[i][j];
}
  
template<typename T>
T
M3<T>::determinate() const
/*!
  Calcuate the determinate of the original matrix
  \return determinate
 */
{
  const T detA=
    AData[1][1]*AData[2][2] - AData[1][2]*AData[2][1];
  const T detB=
    AData[1][0]*AData[2][2] - AData[1][2]*AData[2][0];
  const T detC=
    AData[1][0]*AData[2][1] - AData[1][1]*AData[2][0];

  // + - +
  return AData[0][0]*detA-AData[1][1]*detB+AData[2][2]*detC;
}
  
template<typename T>
M3<T>&
M3<T>::transpose() 
/*!
  Calcuate the transpose
 */
{
  std::swap(AData[0][1],AData[1][0]);
  std::swap(AData[0][2],AData[2][0]);
  std::swap(AData[1][2],AData[2][1]);  

  return *this;
}

template<typename T>
M3<T>
M3<T>::prime() const
/*!
  Calcuate the transpose
 */
{
  M3<T> Out(*this);
  Out.transpose();
  return Out;
}

template<typename T>
void
M3<T>::write(std::ostream& OX) const
  /*!
    Display M3
   */
{
  for(size_t i=0;i<3;i++)
    OX<<fmt::format
      ("[ {:8.5f}  {:8.5f} {:8.5f} ] \n",AData[i][0],AData[i][1],AData[i][2]);
  OX<<std::endl;
  return;
}

  
/// \cond TEMPLATE 

template class M3<long double>;
template class M3<double>;

template std::ostream& operator<<(std::ostream&,const M3<long double>&);
template std::ostream& operator<<(std::ostream&,const M3<double>&);

/// \endcond TEMPLATE

}  // NAMESPACE Geometry
