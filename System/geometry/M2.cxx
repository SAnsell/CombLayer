/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/M2.cxx
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
#include "M2.h"


namespace Geometry
{
  
template<typename T>
std::ostream& 
operator<<(std::ostream& OX,const M2<T>& A)
  /*!
    External outputs point to a stream 
    \param A :: M2 to write out
    \param OX :: output stream
    \returns The output stream (of)
  */
{
  OX<<std::endl;
  A.write(OX);
  return OX;
}

template<typename T>
M2<T>::M2() 
  /*!
    Constructor with pre-set sizes. M2 is zeroed
  */
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j]=0.0;
}

template<typename T>
M2<T>::M2(const T A[2][2])

  /*!
    Constructor with pre-set sizes. M2 is zeroed
    \param nrow :: number of rows
    \param ncol :: number of columns
  */
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j]=A[i][j];
}

template<typename T>
M2<T>::M2(const T& a,const T& b,
	  const T& c,const T& d) 
  /*!
    Constructor with pre-set sizes. M2 is zeroed
    \param nrow :: number of rows
    \param ncol :: number of columns
  */
{
  AData[0][0]=a;
  AData[0][1]=b;
  AData[1][0]=c;
  AData[1][1]=d;
}

template<typename T>
M2<T>::M2(const M2<T>& A)
  /*! 
    Simple copy constructor
    \param A :: Object to copy
  */
{
  copyAll(A);
}
  
template<typename T>
M2<T>&
M2<T>::operator=(const M2<T>& A)
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
M2<T>::copyAll(const M2<T>& A)
{
  for(size_t i=0;i<2;i++)
    {
      for(size_t j=0;j<2;j++)
	{
	  AData[i][j]=A.AData[i][j];
	  U[i][j]=A.U[i][j];
	  V[i][j]=A.V[i][j];
	  R[i][j]=A.R[i][j];
	  lambda[i][j]=A.lambda[i][j];
	}
      Sigma[i]=A.Sigma[i];
    }
  return;
}

template<typename T>
bool
M2<T>::operator!=(const M2<T>& M) const
  /*!
    Check if two M2 items are not identical
    \param M :: Matrix to compare
  */
{
  return (operator==(M)) ? 0 : 1;
}

template<typename T>
bool
M2<T>::operator==(const M2<T>& M) const
  /*!
    Check if two M2 items are identical
    \param M :: Matrix to compare
  */
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      if (AData[i][j] != M.AData[i][j])
	return 0;
  
  return 1;
}

template<typename T>
M2<T>
M2<T>::operator+(const M2<T>& M) const
{
  M2<T> A(*this);
  A+=M;
  return A;
}

template<typename T>
M2<T>
M2<T>::operator-(const M2<T>& M) const
{
  M2<T> A(*this);
  A-=M;
  return A;
}


template<typename T>
M2<T>&
M2<T>::operator+=(const M2<T>& M)
  /*!
    Add M2 together
   */
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j] += M.AData[i][j];
  
  return *this;
}

template<typename T>
M2<T>&
M2<T>::operator-=(const M2<T>& M)
  /*!
    Subtract M2 together
   */
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j] -= M.AData[i][j];
  
  return *this;
}

template<typename T>
M2<T>
M2<T>::operator-() const
  /*!
    Scale by -1 e.g -M
  */
{
  M2<T> M(*this);
  M*=-1.0;
  return M;
}
  
template<typename T>
M2<T>&
M2<T>::operator*=(const M2<T>& M)
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
M2<T>
M2<T>::operator*(const T& V) const
{
  M2<T> A(*this);
  A*=V;
  return A;
  
}

template<typename T>
M2<T>&
M2<T>::operator*=(const T& V) 
{
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j]*=V;

  return *this;
}

template<typename T>
M2<T>
M2<T>::operator*(const M2<T>& M) const
  /*!
    Simple matrix multiplication
    \param M :: Matrix to multiply by
   */
{
  M2<T> Out;
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      for(size_t k=0;k<2;k++)
	Out.AData[i][j]+=AData[i][k]*M.AData[k][j];
  

  return Out;
}

  
template<typename T>
Geometry::Vec2D
M2<T>::operator*(const Geometry::Vec2D& V) const
  /*!
    Multiply out M*V to give a Vec2D
  */
{
  Geometry::Vec2D out;
  out[0]=static_cast<double>(AData[0][0])*V[0]+
    static_cast<double>(AData[0][1])*V[1];
  out[1]=static_cast<double>(AData[1][0])*V[0]+
    static_cast<double>(AData[1][1])*V[1];

  return out;
}

template<typename T>
const T&
M2<T>::get(const size_t i,const size_t j) const
/*!
  Get an indexed variable [not check]
 */
{
  if (i>1 || j>1)
    throw ColErr::IndexError<size_t>(i,j,"i/j out of range (2)");
  return AData[i][j];
}

  
template<typename T>
void
M2<T>::nameAssign(T& a,T& b,T& c,T& d) const
 /*!
   Assign the matrix to values for convience laster
   \param a :: [0][0] values
   \param b :: [0][1] values
   \param c :: [1][0] values
   \param d :: [1][1] values
 */
{
  a=AData[0][0];
  b=AData[0][1];
  c=AData[1][0];
  d=AData[1][1];
  return;
}

template<typename T>
T&
M2<T>::get(const size_t i,const size_t j) 
/*!
  Get an indexed variable [not check]
 */
{
  if (i>1 || j>1)
    throw ColErr::IndexError<size_t>(i,j,"i/j out of range (2)");
  return AData[i][j];
}

template<typename T>
void
M2<T>::constructSVD()
  /*!
    Constructs the SVD system
    Method is to find angle (as it is a rotation)
    and Sigma and then V^T
    This is taken from:
    https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=486688
    Jim Blinn's corner [2x2 matrix]
  */
{
  const T& a(AData[0][0]);
  const T& b(AData[0][1]);
  const T& c(AData[1][0]);
  const T& d(AData[1][1]);

  const T E=(a+d)/2.0;
  const T F=(a-d)/2.0;
  const T G=(b+c)/2.0;
  const T H=(b-c)/2.0;

  const T Q=std::sqrt(E*E+H*H);
  const T R=std::sqrt(F*F+G*G);
  // Check F/E does not equal zero
  const T a1=std::atan2(G,F);
  const T a2=std::atan2(H,E);

  const T beta=(a2-a1)/2.0;
  const T gamma=(a2+a1)/2.0;
  
  Sigma[0]=Q+R;
  Sigma[1]=std::abs(Q-R);
  
  if (Q-R<0.0)
    {
      V[0][0] = -std::cos(gamma);
      V[1][1] = V[0][0];
      V[0][1] = -std::sin(gamma);
      V[1][0] = -V[0][1];
      
      U[0][0] = -std::cos(beta);
      U[1][1] = -U[0][0];
      U[0][1] = std::sin(beta);
      U[1][0] = U[0][1];
    }
  else
    {
      V[0][0] = -std::cos(gamma);
      V[1][1] = std::cos(gamma);
      V[0][1] = -std::sin(gamma);
      V[1][0] = -std::sin(gamma);
      
      U[0][0] = -std::cos(beta);
      U[1][1] = -U[0][0];
      U[0][1] = std::sin(beta);
      U[1][0] = U[0][1];
    }
  return;
}
  
template<typename T>
void
M2<T>::invertSigma() 
  /*!
    Invert Sigma
  */
{
  for(size_t i=0;i<2;i++)
    Sigma[i]=T(1.0)/Sigma[i];
  return;
}

template<typename T>
void
M2<T>::setMinimumSigma(const T& value) 
  /*!
    Ensure that all of sigmas are >= value 
  */
{
  for(size_t i=0;i<2;i++)
    Sigma[i]=std::max<T>(Sigma[i],value);
  return;
}

template<typename T>
T
M2<T>::determinate() const
/*!
  Calcuate the determinate of the original matrix
 */
{
  return AData[0][0]*AData[1][1] - AData[1][0]*AData[0][1];
}

template<typename T>
M2<T>&
M2<T>::invert() 
/*!
  Calcuate the inverted of the original matrix
 */
{
  const T det=determinate();
  if (det<1e-30)
    throw ColErr::NumericalAbort("Matrix not invertable Det=0"); 

  adjoint();
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j]/=det;

  return *this;
}
  
template<typename T>
M2<T>&
M2<T>::transpose() 
/*!
  Calcuate the transpose
 */
{
  std::swap(AData[1][0],AData[0][1]);
  return *this;
}

template<typename T>
M2<T>
M2<T>::Tprime() const
/*!
  Calcuate the transpose
 */
{
  M2<T> Out(*this);
  Out.transpose();
  return Out;
}

template<typename T>
M2<T>&
M2<T>::adjoint() 
/*!
  Convert the matrix to the  the adjoint form
*/
{
  
  std::swap(AData[0][0],AData[1][1]);  
  std::swap(AData[1][0],AData[0][1]);
  AData[1][0]*=-1.0;
  AData[0][1]*=-1.0;
  return *this;
}

template<typename T> 
void
M2<T>::sortEigen()
{

  if (lambda[0][0]>lambda[1][1])
    {
      std::swap(R[0][0],R[0][1]);
      std::swap(R[1][0],R[1][1]);
      std::swap(lambda[0][0],lambda[1][1]);
    }
  return;
}

  
template<typename T> 
void
M2<T>::reCalcSVD()
  /*!
    Calculate AData from the SVD composition (assuming that
    we have changed sigma etc...)
  */
{
  T D[2][2];
  for(size_t i=0;i<2;i++)
   for(size_t j=0;j<2;j++)
     D[i][j]=Sigma[i]*V[i][j];

  // note transpose
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      {
	AData[i][j]=T(0);
	for(size_t kk=0;kk<2;kk++)
	  AData[i][j]+=U[i][kk]*D[kk][j];
      }

  std::swap(AData[0][1],AData[1][0]);

  return;    
}

template<typename T>
void
M2<T>::constructEigen()
  /*!
    Construct eigen vectors and values
    Note if symmetric both SVD and Eigen give same results
  */
{
  const T trace=(AData[0][0]+AData[1][1])/2.0;  // mean of trace
  const T det=(AData[0][0]*AData[1][1]-AData[1][0]*AData[0][1]);
  const T diff=trace*trace-det;
  // this could be complex but so need to check
  if (diff<0.0)
    throw ColErr::SizeError<T>(diff,0.0,"Determinate negative");

  const T sQ=std::sqrt(diff);
  lambda[0][0]=trace+sQ;
  lambda[1][1]=trace-sQ;
  lambda[0][1]=0.0;
  lambda[1][0]=0.0;
  
  const T b=std::abs<T>(AData[1][0]);
  const T c=std::abs<T>(AData[0][1]);

  Geometry::Vec2D AEig,BEig;
  if (b>0.0 && c>0.0)
    {
      if (b>c)
	{
	  AEig=Geometry::Vec2D(static_cast<double>(lambda[0][0]-AData[1][1]),
			       static_cast<double>(AData[1][0]));
	  BEig=Geometry::Vec2D(static_cast<double>(lambda[1][1]-AData[1][1]),
			       static_cast<double>(AData[1][0]));
	}
      else
	{
	  AEig=Geometry::Vec2D(static_cast<double>(AData[0][1]),
			       static_cast<double>(lambda[0][0]-AData[0][0]));
	  BEig=Geometry::Vec2D(static_cast<double>(AData[0][1]),
			       static_cast<double>(lambda[1][1]-AData[0][0]));
	}
      AEig.makeUnit();
      BEig.makeUnit();
    }
  else
    {
      AEig=Geometry::Vec2D(1,0);
      BEig=Geometry::Vec2D(0,1);
    }
  R[0][0]=AEig.X();
  R[1][0]=AEig.Y();
  R[0][1]=BEig.X();
  R[1][1]=BEig.Y();
  sortEigen();
  return;
}

template<typename T>
std::pair<T,T>
M2<T>::getEigPair() const
  /*!
    Get the eigen values
   */
{
  return std::pair<T,T>(lambda[0][0],lambda[1][1]);
}

template<typename T>
Geometry::Vec2D
M2<T>::getEigVec(const size_t index) const
{
  return (!index) ?
    Geometry::Vec2D(static_cast<double>(R[0][0]),
		    static_cast<double>(R[1][0])) :
    Geometry::Vec2D(static_cast<double>(R[0][1]),
		    static_cast<double>(R[1][1]));
}

template<typename T>
T
M2<T>::magVector(const T* vec2) const
  /*!
    Calculate the magnitude of AData * vec2
  */
{
  T out(0);
  for(size_t i=0;i<2;i++)
    {
      T item(0);
      for(size_t j=0;j<2;j++)
	item+=AData[i][j]*vec2[j];
      out+=item*item;
    }
  return std::sqrt(out);
}

template<typename T>
void
M2<T>::write(std::ostream& OX) const
  /*!
    Display M2
   */
{
  OX<<fmt::format
    ("[ {:8.5f}  {:8.5f} ] ",AData[0][0],AData[0][1])<<std::endl;
  OX<<fmt::format
    ("[ {:8.5f}  {:8.5f} ] ",AData[1][0],AData[1][1])<<"\n"
  <<std::endl;
  /*
  OX<<fmt::format
    ("[ {:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] ",
     U[0][0],U[0][1],Sigma[0],0.0,V[0][0],V[0][1])
    <<std::endl;

  OX<<fmt::format
    ("[ {:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] ",
     U[1][0],U[1][1],0.0,Sigma[1],V[1][0],V[1][1])
    <<std::endl;
  */
  return;
}


template<typename T>
M2<T>
M2<T>::getEigValues() const
  /*!
    Accessor to Eigen Value matrix
   */
{
  return M2<T>(lambda);
}

template<typename T>
M2<T>
M2<T>::getEigVectors() const
  /*!
    Accessor to Eigen Value matrix
   */
{
  return M2<T>(R);
}
  
/// \cond TEMPLATE 

template class M2<long double>;
template class M2<double>;

template std::ostream& operator<<(std::ostream&,const M2<long double>&);
template std::ostream& operator<<(std::ostream&,const M2<double>&);

/// \endcond TEMPLATE

}  // NAMESPACE Geometry
