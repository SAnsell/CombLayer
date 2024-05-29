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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "polySupport.h"
#include "M3vec.h"
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
      AA[i][j]=0.0;
}

template<typename T>
M3<T>::M3(const bool) 
  /*!
    Constructor with pre-set sizes. M3 is
    constructed to the identiy matrix
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]=(i==j) ? T(1.0) : T(0.0);
}

template<typename T>
M3<T>::M3(const T A[3][3])

  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]=A[i][j];
}

template<typename T>
M3<T>::M3(const std::vector<std::vector<T>>& V)
  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]=V[i][j];
}

template<typename T>
M3<T>::M3(const T& a,const T& b,const T& c)

  /*!
    Constructor a diagonal matrix
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=i+1;j<3;j++)
      {
	AA[i][j]=0.0;
	AA[j][i]=0.0;
      }
  AA[0][0]=a;
  AA[1][1]=b;
  AA[2][2]=c;
}
  
template<typename T>
M3<T>::M3(const Matrix<T>& M) 
  /*!
    Constructor with matrix [MUST be >= (3,3)]
    \param M  :: Matrix unit
  */
{
  if (!M.hasSize(3,3))
    throw ColErr::DimensionError(
      std::vector<size_t>({M.size().first,M.size().second}),
      {3,3},"M3(Matrix) too small");
      
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]=M[i][j];
}

template<typename T>
M3<T>::M3(const T& a,const T& b,const T& c,
	  const T& d,const T& e,const T& f,
	  const T& g,const T& h,const T& i)

  /*!
    Constructor with pre-set sizes. M3 is zeroed
  */
{
  AA[0][0]=a;  AA[0][1]=b;  AA[0][2]=c;
  AA[1][0]=d;  AA[1][1]=e;  AA[1][2]=f;
  AA[2][0]=g;  AA[2][1]=h;  AA[2][2]=i;
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
  /*!
    Copy all the dat in a M3 to another M3
    \param A :: Matrix to copy from
  */
{
  for(size_t i=0;i<3;i++)
    {
      for(size_t j=0;j<3;j++)
	{
	  AA[i][j]=A.AA[i][j];
	}
    }
  return;
}

template<typename T>
bool
M3<T>::operator!=(const M3<T>& M) const
  /*!
    Check if two M3 items are not itentical
    \return true if not equal
  */
{
  return (operator==(M)) ? 0 : 1;
}

template<typename T>
bool
M3<T>::operator==(const M3<T>& M) const
  /*!
    Checkf if two M3 items are itentical
    \return true if equal
  */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      if (std::abs(AA[i][j]-M.AA[i][j])>1e-12)
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
      AA[i][j] += M.AA[i][j];
  
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
      AA[i][j] -= M.AA[i][j];
  
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
  /*!
    Multipy by constant
   */
{
  M3<T> A(*this);
  A*=V;
  return A;
}

template<typename T>
M3<T>
M3<T>::operator/(const T& V) const
  /*!
    Divide by constant
   */
{
  M3<T> A(*this);
  A/=V;
  return A;
}
  
template<typename T>
M3<T>&
M3<T>::operator*=(const T& V) 
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]*=V;

  return *this;
}

template<typename T>
M3<T>&
M3<T>::operator/=(const T& V)
  /*!
    Divide by a constant
    \param V :: Value
   */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]/=V;

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
	Out.AA[i][j]+=AA[i][k]*M.AA[k][j];
  

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
      out[i]+=static_cast<double>(AA[i][j])*V[j];

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
  return AA[i][j];
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
  return AA[i][j];
}


template<typename T>
void
M3<T>::setColumn(const size_t i,const M3vec<T>& V) 
/*!
  Set the column vector
 */
{
  if (i>2)
    throw ColErr::IndexError<size_t>(i,2,"M3::setColumn i out of range (2)");

  V.setM3(i,*this);
  return;
}

template<typename T>
void
M3<T>::setRow(const size_t i,const M3vec<T>& V) 
/*!
  Set the row vector
 */
{
  if (i>2)
    throw ColErr::IndexError<size_t>(i,2,"M3::setRow i out of range (2)");

  V.setM3(*this,i);
  return;
}

template<typename T>
M3vec<T>
M3<T>::getRow(const size_t index) const
/*!
  Get the row vector
 */
{
  if (index>2)
    throw ColErr::IndexError<size_t>
      (index,2,"M3::getRow index out of range (2)");

  return M3vec<T>(*this,index);
}

template<typename T>
M3vec<T>
M3<T>::getColumn(const size_t index) const
/*!
  Get the column vector
 */
{
  if (index>2)
    throw ColErr::IndexError<size_t>
      (index,2,"M3::getColumn index out of range (2)");

  return M3vec<T>(index,*this);
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
    AA[1][1]*AA[2][2] - AA[1][2]*AA[2][1];
  const T detB=
    AA[1][0]*AA[2][2] - AA[1][2]*AA[2][0];
  const T detC=
    AA[1][0]*AA[2][1] - AA[1][1]*AA[2][0];
  // + - +
  return AA[0][0]*detA-AA[0][1]*detB+AA[0][2]*detC;
}
  
template<typename T>
M3<T>&
M3<T>::identity() 
/*!
  Set the matrix to the identity
 */
{
  for(size_t i=0;i<3;i++)
    {
      AA[i][i]=1.0;
      for(size_t j=i+1;j<3;j++)
	{
	  AA[i][j]=0.0;
	  AA[j][i]=0.0;
	}
    }
  return *this;
}

template<typename T>
M3<T>&
M3<T>::zero() 
/*!
  Calcuate the zero matirx
 */
{
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      AA[i][j]=0.0;

  return *this;
}

template<typename T>
M3<T>&
M3<T>::invert() 
/*!
  Fast inverse based on cross product system
  as if M=( a b c ) and M^-1 is (d e f )
  as a-f are vectors we see that the AA-1 is a lot of dot products
  and we can determine d,e,f (columns of A-1) via
    d=bxc/|bxc| * (1/|a| cos(theta) :: cos(theta) = a.(bxc) / |bxc||a|
 */
{
  T a,b,c,d,e,f,g,h,j;
  T A,B,C,D,E,F,G,H,J;
  nameAssign(a,b,c,d,e,f,g,h,j);

  // calc det
  T det=a*(e*j-f*h)-b*(d*j-f*g)+c*(d*h-e*g);
  if (det>1e-20)
    {
      A=  (e*j - f*h)/det;
      B = (c*h - b*j)/det;
      C = (b*f - c*e)/det;
      D = (f*g - d*j)/det;
      E = (a*j - c*g)/det;
      F = (d*c - a*f)/det;
      G = (d*h - g*e)/det;
      H = (g*b - a*h)/det;
      J = (a*e - d*b)/det;
      *this=M3<T>(A,B,C,D,E,F,G,H,J);
    }
  return *this;
}

template<typename T>
M3<T>
M3<T>::inverse() const
/*!
  Fast inverse based on cross product system
  as if M=( a b c ) and M^-1 is (d e f )
  as a-f are vectors we see that the AA-1 is a lot of dot products
  and we can determine d,e,f (columns of A-1) via
  d=bxc/|bxc| * (1/|a| cos(theta) :: cos(theta) = a.(bxc) / |bxc||a|
 */
{
  M3<T> out(*this);
  out.invert();
  return out;
}
  
template<typename T>
M3<T>&
M3<T>::transpose() 
  /*!
    Calcuate the transpose
  */
{
  std::swap(AA[0][1],AA[1][0]);
  std::swap(AA[0][2],AA[2][0]);
  std::swap(AA[1][2],AA[2][1]);  

  return *this;
}

template<typename T>
M3<T>
M3<T>::Tprime() const
/*!
  Calcuate the transpose
 */
{
  M3<T> Out(*this);
  Out.transpose();
  return Out;
}

template<typename T>  
bool
M3<T>::QR(M3<T>& Qtrans,M3<T>& R) const
/*!
  Deconvolve matrix into Q and R
  Q -> such that Q'Q=I and R is upper triangular
  Effectively this
    
  e1=a1/||a1||
  e2=a2-(a2.e1)e1
  e3=a3-(a3.e1)e1-(a3.e2)e2
  and R ==> a1.e1 a1.e2...
  Q == |e1,e2,e3|

  \return 0 on failure (A signular) and 1 on success
*/
{
  // following Gram-Schmidt process

  T cVal[2],dVal[3];

  R= *this;
  for(size_t k=0;k<2;k++)
    {
      T mag=0.0;      
      for(size_t i=k;i<3;i++)
	mag=std::max(mag,std::abs(R[i][k]));
      if (mag!=0.0)
	{
	  for(size_t i=k;k<3;k++)
	    R[i][k]=AA[i][k]/mag;
	  T sum(0.0);
	  for(size_t i=k;i<3;i++)
	    sum+=R[i][k]*R[i][k];
	  const T sigma=(R[k][k]>=0.0) ? std::sqrt(sum) : -std::sqrt(sum);
	  R[k][k]+=sigma;
	  cVal[k]=sigma*R[k][k];
	  dVal[k]= -mag*sigma;
	  for(size_t j=k+1;j<3;j++)
	    {
	      sum=0.0;
	      for(size_t i=k;i<3;i++)
		sum+=R[i][k]*R[i][j];
	      const T tau=sum/cVal[k];
	      for(size_t i=k;i<3;i++)
		R[i][k] -= tau*R[i][k];
	    }
	}
      else
	{
	  cVal[k]=0.0;
	  dVal[k]=0.0;
	}
    }
  dVal[2]=R[2][2];

  Qtrans.identity();
  for(size_t k=0;k<2;k++)
    {
      if (std::abs(cVal[k]) > 1e-30)
	{
	  for(size_t j=0;j<3;j++)
	    {
	      T sum=0.0;
	      for(size_t i=k;i<3;i++)
		sum+=R[i][k]*Qtrans[i][j];
	      sum/=cVal[k];
	      for(size_t i=k;i<3;i++)
		Qtrans[i][j] -= sum*R[i][k];
	    }

	}
    }
  R.zero();
  for(size_t i=0;i<3;i++)
    R[i][i]=dVal[i];
  return 1;
}

template<typename T>
void
M3<T>::nameAssign(T& a,T& b,T& c,
		  T& d,T& e,T& f,
		  T& g,T& h,T& j) const
  /*!
    Stupid assignment to allow stuff like
    double a,b,c..
    M.nameAssign(a,b,c,...)
  */
{
  a=AA[0][0]; b=AA[0][1]; c=AA[0][2];
  d=AA[1][0]; e=AA[1][1]; f=AA[1][2];
  g=AA[2][0]; h=AA[2][1]; j=AA[2][2];
  return;
}

template<typename T>  
size_t
M3<T>::getEigenValues(T& eA,T& eB,T& eC) const
  /*!
    Solve and get the eigen values
    \param eA :: First eigen value
    \param eB :: second eigen value
    \param eC :: third eigen value	
    \return Number of distinct eigenvalues
   */
{
  T a,b,c,d,e,f,g,h,j;
  nameAssign(a,b,c,d,e,f,g,h,j);
  // find characteristic poly : p(x) = det(A-xI)

  const T c0=a*e*j-a*f*h + b*f*g-b*d*j +  c*d*h-c*e*g;
  const T c1= -a*e-a*j+ b*d+c*g-e*j+ f*h;
  const T c2=a+e+j;
  const T c3=-1.0;

  return solveRealCubic(c3,c2,c1,c0,eA,eB,eC);
}

template<typename T>
void
M3<T>::rowEchelon()
  /*!
    Convert the matrix to row escheon form
   */
{
  const T aA=std::abs(AA[0][0]);
  const T aB=std::abs(AA[1][0]);
  const T aC=std::abs(AA[2][0]);
  const size_t iMax=(aA>=aB) ?
    ((aA>=aC) ? 0 : 2) : ((aB>=aC) ? 1 : 2);
  if (std::abs(AA[iMax][0])<1e-30) return;
  
  if (iMax)
    {
      for(size_t i=0;i<3;i++)
	std::swap(AA[0][i],AA[iMax][i]);
    }
  // row one:
  const T& dV(AA[0][0]);
  for(size_t i=1;i<3;i++)
    AA[0][i]/=dV;
  AA[0][0]=1.0;

  AA[1][1]-= AA[1][0]*AA[0][1];
  AA[1][2]-= AA[1][0]*AA[0][2];
  AA[2][1]-= AA[2][0]*AA[0][1];
  AA[2][2]-= AA[2][0]*AA[0][2];
  AA[1][0]=0.0;
  AA[2][0]=0.0;

  //row 2:
  const T bA=std::abs(AA[1][1]);
  const T bB=std::abs(AA[2][1]);
  if (bA<bB)
    {
      std::swap(AA[1][1],AA[2][1]);
      std::swap(AA[1][2],AA[2][2]);
    }
  const T& dW(AA[1][1]);
  if (std::abs(dW)<1e-30) return;
  AA[1][2]/=dW;
  AA[1][1]=1.0;
  AA[2][2]-=AA[2][1]*AA[1][2];
  AA[2][1]=0.0;
  
  return;
}
  

template<typename T>
bool
M3<T>::diagonalize(M3<T>& Pinvert,
		   M3<T>& DiagMatrix,
		   M3<T>& P) const
  /* !
  Attempt to diagonalise the matrix IF symmetric
  \param Pmatrex :: (output) the Eigenvectors matrix [vertical columns]
  \param DiagMatrix  :: the diagonal matrix of eigenvalues 
  \returns :: 1  on success 0 on failure
 */
{
  const T& a=AA[0][0]; const T& b=AA[0][1];
  const T& c=AA[0][2]; const T& d=AA[1][0];
  const T& e=AA[1][1]; const T& f=AA[1][2];
  const T& g=AA[2][0]; const T& h=AA[2][1];
  const T& j=AA[2][2];
    
  // find characteristic poly : p(x) = det(A-xI)

  const T c0=a*e*j-a*f*h + b*f*g-b*d*j +  c*d*h-c*e*g;
  const T c1= -a*e-a*j+ b*d+c*g-e*j+ f*h;
  const T c2=a+e+j;
  const T c3=-1.0;

  // determine the 3 eigenvalues from 
  T value[3];
  const size_t N=
    solveRealCubic(c3,c2,c1,c0,value[0],value[1],value[2]);
  if (N!=3)
    return 0;

  DiagMatrix=M3<T>(value[0],value[1],value[2]);
  P=calcEigenVectors({value[0],value[1],value[2]});
  Pinvert=P.inverse();
  
  return 1;
}

template<typename T>
Geometry::M3vec<T>
M3<T>::eigenVector(const size_t degen,const T& eValue) const
  /*!
    Assume that the matrix is of the form
    M | 000 (resolve with minimal form after
    row operation [+ normalize]
  */
{
  // construct M-lamba(I):
  M3<T> M(1);  // identiy matrix
  M *= eValue;
  M -= *this;
  M3vec<T> A(M,0);
  M3vec<T> B(M,1);
  M3vec<T> C(M,2);

  M3vec<T> abCross=A*B;
  if (abCross.abs()>1e-12)
    return abCross;
  abCross=A*C;
  if (abCross.abs()>1e-12)
    return abCross;

  M.rowEchelon();
  M3vec AA(M,0);
  if (!degen)
    return AA.crossNormal();

  return AA*AA.crossNormal();
}

template<typename T>
M3<T>
M3<T>::calcEigenVectors(const std::vector<T>& eigenValues) const
  /*!
    Calcuate the eigenvectors based on the eigen values:
    Observe that (M - lambda I) x = 0 for an x != 0.
    Thus : if M is row vectors a,b,c
    a.x =0 b.x=0 and c.x=0. and only if a,b and c are
    coplane can we have eigenvectors
   */
{
  M3<T> out;
  for(size_t i=0;i<3;i++)
    {
      const size_t degen=
	(i && std::abs(eigenValues[i]-eigenValues[i-1])<1e-5) ? 1 : 0;
      M3vec<T> eV=eigenVector(degen,eigenValues[i]);
      out.setColumn(i,eV);
    }
  return out;
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
      ("[ {:8.5f}  {:8.5f} {:8.5f} ] \n",AA[i][0],AA[i][1],AA[i][2]);
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
