/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/M2.cxx
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
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      AData[i][j]=A.AData[i][j];
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
      for(size_t i=0;i<2;i++)
	for(size_t j=0;j<2;j++)
	  AData[i][j]=A.AData[i][j];

    }
  return *this;
}

template<typename T>
bool
M2<T>::operator!=(const M2<T>& M) const
  /*!
    Checkf if two M2 items are not itentical
  */
{
  return (operator==(M)) ? 0 : 1;
}

template<typename T>
bool
M2<T>::operator==(const M2<T>& M) const
  /*!
    Checkf if two M2 items are itentical
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
	Out.AData[i][j]+AData[i][k]*M.AData[k][j];
  

  return Out;
}

  
template<typename T>
Geometry::Vec2D
M2<T>::operator*=(const Geometry::Vec2D& V) const
{
  Geometry::Vec2D out;
  out[0]=static_cast<double>(AData[0][0])*V[0]+
    static_cast<double>(AData[0][1])*V[1];
  out[1]=static_cast<double>(AData[1][0])*V[0]+
    static_cast<double>(AData[1][1])*V[1];

  return out;
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

  T Sigma[2];
  T U[2][2];
  T V[2][2];
  
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
T
M2<T>::determinate() const
/*!
  Calcuate the determinate of the original matrix
 */
{
  return AData[0][0]*AData[1][1] - AData[1][0]*AData[0][1];
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
  */
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
  const T diff=tr*tr-det;
  // this could be complex but so need to check
  if (diff<0.0)
    throw ColErr::SizeError<T>(diff,0.0,"Determinate negative");

  const T sQ=std::sqrt(diff);
  lambda[0][0]=tr+sQ;
  lambda[1][1]=tr-sQ;

  
  const T b=std::abs<T>(AData[1][0]);
  const T c=std::abs<T>(AData[0][1]);
      
  Geometry:::Vec2D AEig,BEig;
  if (b>0.0 && c>0.0)
    {
      if (b>a)
	{
	  AEig=Geometry::Vec2D(lambda[0][0]-AData[1][1],AData[1][0]);
	  BEig=Geometry::Vec2D(lambda[1][1]-AData[1][1],AData[1][0]);
	}
      else
	{
	  AEig=Geometry::Vec2D(AData[0][1],lambda[0][0]-AData[0][0]);
	  BEig=Geometry::Vec2D(AData[0][1],lambda[1][1]-AData[0][0]);
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
  R[0][1]=AEig.Y();
  R[1][0]=BEig.X();
  R[1][1]=BEig.Y();
  return;
}

template<typename T>
std::pair<T,T>
M2<T>::getEigValues() const
  /*!
    Get the eigen values
   */
{
  const T tr=(AData[0][0]+AData[1][1])/2.0;  // mean of trac
  const T det=(AData[0][0]*AData[1][1]-AData[1][0]*AData[0][1]);
  const T diff=tr*tr-det;
  // this could be complex but so need to check
  if (diff<0.0)
    throw ColErr::SizeError<T>(diff,0.0,"Determinate negative");

  const T sQ=std::sqrt(diff);
  return std::pair<T,T>(tr+sQ,tr-sQ);
}

template<typename T>
Geometry::Vec2D
M2<T>::getEigVec(const size_t index) const
{
  const T b=std::abs<T>(AData[1][0]);
  const T c=std::abs<T>(AData[0][1]);
  if (b>0.0 && c>0.0)
    {
      const T eValue=(!index) ? getEigValues().first :
	getEigValues().second;
      
      
      return (AData[1][0]>AData[0][1]) ?

	Geometry::Vec2D(static_cast<double>(eValue-AData[1][1]),
			static_cast<double>(AData[1][0])) :
	
    }

  return (index) ?
    Geometry::Vec2D(1.0,0.0) :
    Geometry::Vec2D(0.0,1.0);
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
bool
M2<T>::check() const
  /*!
    Ensures that the SVD system is correct
    \return -1 on error
  */
{
  /*
  T Part[2][2];

  // Multiply S*V
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      Part[i][j] = Sigma[i] * V[i][j];
  
  // Multiply U * Part ==> U*S*V
  T BData[2][2];
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      {
	BData[i][j]=0.0;
	for(size_t k=0;k<2;k++)
	  BData[i][j] += U[i][k] * Part[k][j];
      }


  T missMatch(0);
  T normFactor(0);
  for(size_t i=0;i<2;i++)
    {
      missMatch+=(AData[i][0]-BData[i][0])*(AData[i][0]-BData[i][0]);
      missMatch+=(AData[i][1]-BData[i][1])*(AData[i][1]-BData[i][1]);
      normFactor+=Sigma[i]*Sigma[i];

    }
  if (normFactor<1e-20 || missMatch/normFactor>1e-6)
    {
      for(size_t i=0;i<2;i++)
	{
	  const std::string Out=fmt::format
	    ("M : [ {:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] ",
	     AData[i][0],AData[i][1],
	     BData[i][0],BData[i][1]);
	  ELog::EM<<Out<<ELog::endDiag;
	}
      ELog::EM<<"----------"<<ELog::endDiag;
      for(size_t i=0;i<2;i++)
	{
	  const std::string OutB=fmt::format
	    ("M : [ {:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ] [{:8.5f}  {:8.5f} ]",
	     U[i][0],U[i][1],
	     ((i==0) ? Sigma[i] : 0.0),
	     ((i==0) ? 0.0 : Sigma[i]),
	     V[i][0],V[i][1]);
	  ELog::EM<<OutB<<ELog::endDiag;;
	}
      ELog::EM<<"mis == "<<missMatch<<" "<<normFactor<<" \n "<<ELog::endDiag;
      return 1 ;
    }
  */
  return 0;
}
  
/// \cond TEMPLATE 

template class M2<long double>;
template class M2<double>;

template std::ostream& operator<<(std::ostream&,const M2<long double>&);
template std::ostream& operator<<(std::ostream&,const M2<double>&);

/// \endcond TEMPLATE

}  // NAMESPACE Geometry
