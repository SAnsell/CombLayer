/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/MatrixBase.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/multi_array.hpp> 

#include "Exception.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "PolyFunction.h"
#include "PolyVar.h"

template<typename T>
std::ostream& 
Geometry::operator<<(std::ostream& OX,const Geometry::MatrixBase<T>& A)
  /*!
    External outputs point to a stream 
    \param A :: MatrixBase to write out
    \param OX :: output stream
    \returns The output stream (of)
  */
{
  OX<<std::endl;
  A.write(OX,5);
  return OX;
}

template<int index>
std::ostream& 
Geometry::operator<<(std::ostream& OX,
     const Geometry::MatrixBase<mathLevel::PolyVar<index> >& A)
  /*!
    External outputs point to a stream 
    \param A :: MatrixBase to write out
    \param OX :: output stream
    \returns The output stream (OX)
  */
{
  OX<<std::endl;
  A.writeGrid(OX);
  return OX;
}

namespace Geometry
{

template<typename T>
MatrixBase<T>::MatrixBase(const size_t nrow,const size_t ncol)
  : nx(0),ny(0),V(0)
  /*!
    Constructor with pre-set sizes. MatrixBase is zeroed
    \param nrow :: number of rows
    \param ncol :: number of columns
  */
{
  // Note:: nx,ny zeroed so setMem always works
  setMem(nrow,ncol);
  zeroMatrix();
}

template<typename T>
MatrixBase<T>::MatrixBase(const std::vector<T>& A,const std::vector<T>& B)
  : nx(0),ny(0),V(0)
  /*!
    Constructor to take two vectors and multiply them to 
    construct a matrix. (assuming that we have columns x row
    vector.)
    \param A :: Column vector to multiply
    \param B :: Row vector to multiply
  */
{
  // Note:: nx,ny zeroed so setMem always works
  setMem(A.size(),B.size());
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      V[i][j]=A[i]*B[j];  
}

template<typename T>
MatrixBase<T>::MatrixBase(const MatrixBase<T>& A,const size_t nrow,
			  const size_t ncol)
  : nx(0),ny(0),V(0)
  /*!
    Constructor with for a missing row/column.
    \param A :: MatrixBase to use as master
    \param nrow :: number of row to miss
    \param ncol :: number of column to miss
  */
{
  // Note:: nx,ny zeroed so setMem always works
  if (nrow>=A.nx)
    throw ColErr::IndexError<size_t>(nrow,A.nx,
			     "MatrixBase::Constructor without row");
  if (ncol>=A.ny)
    throw ColErr::IndexError<size_t>(ncol,A.ny,
			     "MatrixBase::Constructor without col");
  
  if (A.nx<2 || A.ny<2) return;

  // A.nx is >= 2
  setMem(A.nx-1,A.ny-1);
  size_t iR(0);
  for(size_t i=0;i<A.nx;i++)
    {
      if (i!=nrow)
        {
	  size_t jR(0);
	  for(size_t j=0;j<A.ny;j++)
	    if (j!=ncol)
	      {
		V[iR][jR]=A.V[i][j];
		jR++;
	      }
	  iR++;
	}
    }
 }

template<typename T>
MatrixBase<T>::MatrixBase(const MatrixBase<T>& A)
  : nx(0),ny(0),V(0)
  /*! 
    Simple copy constructor
    \param A :: Object to copy
  */
{
  // Note:: nx,ny zeroed so setMem always works
  setMem(A.nx,A.ny);
  if (nx*ny)
    {
      for(size_t i=0;i<nx;i++)
	for(size_t j=0;j<ny;j++)
	  V[i][j]=A.V[i][j];
    }
}


template<typename T>
MatrixBase<T>&
MatrixBase<T>::operator=(const MatrixBase<T>& A)
  /*! 
    Simple assignment operator 
    \param A :: Object to copy
    \return *this
  */
{
  if (&A!=this)
    {
      setMem(A.nx,A.ny);
      if (nx*ny)
	for(size_t i=0;i<nx;i++)
	  for(size_t j=0;j<ny;j++)
	    V[i][j]=A.V[i][j];
    }
  return *this;
}

template<typename T>
MatrixBase<T>::~MatrixBase()
  /*!
    Delete operator :: removes memory for 
    matrix
  */
{
  deleteMem();
}


template<typename T>
MatrixBase<T>&
MatrixBase<T>::operator+=(const MatrixBase<T>& A)
   /*! 
     MatrixBase addition THIS + A  
     If the size is different then 0 is added where appropiate
     MatrixBase A is not expanded.
     \param A :: MatrixBase to add
     \returns MatrixBase(this + A)
   */
{
  const size_t Xpt((nx>A.nx) ? A.nx : nx);
  const size_t Ypt((ny>A.ny) ? A.ny : ny);
  for(size_t i=0;i<Xpt;i++)
    for(size_t j=0;j<Ypt;j++)
      V[i][j]+=A.V[i][j];

  return *this;
}

template<typename T>
MatrixBase<T>&
MatrixBase<T>::operator-=(const MatrixBase<T>& A)
   /*! 
     MatrixBase subtractoin THIS - A  
     If the size is different then 0 is added where appropiate
     MatrixBase A is not expanded.
     \param A :: MatrixBase to add
     \returns Matrix - A
   */
{
  const size_t Xpt((nx>A.nx) ? A.nx : nx);
  const size_t Ypt((ny>A.ny) ? A.ny : ny);
  for(size_t i=0;i<Xpt;i++)
    for(size_t j=0;j<Ypt;j++)
      V[i][j]-=A.V[i][j];

  return *this;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::operator+(const MatrixBase<T>& A)
   /*! 
     MatrixBase addition THIS + A  
     If the size is different then 0 is added where appropiate
     MatrixBase A is not expanded.
     \param A :: MatrixBase to add
     \returns MatrixBase(this + A)
   */
{
  MatrixBase<T> X(*this);
  return X+=A;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::operator-(const MatrixBase<T>& A)
   /*! 
     MatrixBase subtraction THIS - A  
     If the size is different then 0 is subtracted where 
     appropiate. This matrix determines the size 
     \param A :: MatrixBase to add
     \returns MatrixBase(this + A)
   */
{
  MatrixBase<T> X(*this);
  return X-=A;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::operator*(const MatrixBase<T>& A) const
  /*! 
    MatrixBase multiplication THIS * A  
    \param A :: MatrixBase to multiply by  (this->row must == A->columns)
    \throw MisMatch<int> if there is a size mismatch.
    \return MatrixBase(This * A) 
 */
{
  if (ny!=A.nx)
    throw ColErr::MisMatch<size_t>(ny,A.nx,
				     "MatrixBase::operator*(MatrixBase)");
  MatrixBase<T> X(nx,A.ny);
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<A.ny;j++)
      for(size_t kk=0;kk<ny;kk++)
	X.V[i][j]+=V[i][kk]*A.V[kk][j];

  return X;
}

template<typename T>
std::vector<T>
MatrixBase<T>::operator*(const std::vector<T>& Vec) const
  /*! 
    MatrixBase multiplication THIS * Vec to produce a vec  
    \param Vec :: size of vector > this->nrows
    \throw MisMatch<int> if there is a size mismatch.
    \return MatrixBase(This * Vec)
  */
{
  std::vector<T> Out;
  if (ny>Vec.size())
    throw ColErr::MisMatch<size_t>(ny,Vec.size(),
				     "MatrixBase::operator*(Vec)");

  Out.resize(nx);
  for(size_t i=0;i<nx;i++)
    {
      Out[i]=0;
      for(size_t j=0;j<ny;j++)
	Out[i]+=V[i][j]*Vec[j];
    }
  return Out;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::operator*(const T& Value) const
   /*! 
     MatrixBase multiplication THIS * Value  
     \param Value :: Scalar to multiply by
     \return V * (this)
   */
{
  MatrixBase<T> X(*this);
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      X.V[i][j]*=Value;

  return X;
}

template<typename T>
MatrixBase<T>&
MatrixBase<T>::operator*=(const MatrixBase<T>& A)
   /*! 
     MatrixBase multiplication THIS *= A  
     Note that we call operator* to avoid the problem
     of changing matrix size.
    \param A :: MatrixBase to multiply by  (this->row must == A->columns)
    \return This *= A 
   */
{
  if (ny!=A.nx)
    throw ColErr::MisMatch<size_t>(ny,A.nx,"MatrixBase*=(MatrixBase<T>)");
  // This construct to avoid the problem of changing size
  *this = this->operator*(A);
  return *this;
}

template<typename T>
MatrixBase<T>&
MatrixBase<T>::operator*=(const T& Value)
   /*! 
     MatrixBase multiplication THIS * Value  
     \param Value :: Scalar to multiply matrix by
     \return *this
   */
{
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      V[i][j]*=Value;
  return *this;
}

template<typename T>
bool
MatrixBase<T>::operator!=(const MatrixBase<T>& A) const
  /*!
    Element by Element comparison
    \param A :: MatrixBase to check
    \return 1 :: on sucees
    \return 0 :: failoure
  */
{
  return (this->operator==(A)) ? 0 : 1;
}

template<typename T>
bool
MatrixBase<T>::operator==(const MatrixBase<T>& A) const
  /*! 
    Element by element comparison within tolerance.
    Tolerance means that the value must be > tolerance
    and less than (diff/max)>tolerance 

    Always returns 0 if the MatrixBase have different sizes
    \param A :: matrix to check.
    \return 1 on success 
  */
{
  if (&A!=this)       // this == A == always true
    {
      if(A.nx!=nx || A.ny!=ny)
	return 0;

      for(size_t i=0;i<nx;i++)
	for(size_t j=0;j<ny;j++)
	  {
	    if (V[i][j]!=A.V[i][j])
	      return 0;
	  }
    }
  //this == this is true
  return 1;
}

template<typename T>
void
MatrixBase<T>::deleteMem()
  /*!
    Deletes the memory held in matrix 
  */
{
  if (V)
    {
      delete [] *V;
      delete [] V;
      V=0;
    }
  nx=0;
  ny=0;
  return;
}

template<typename T>
void
MatrixBase<T>::setMem(const size_t a,const size_t b)
  /*! 
    Sets the memory held in matrix 
    \param a :: number of rows
    \param b :: number of columns
  */
{
  if (a==nx && b==ny) 
    return;

  deleteMem();
  if (a<=0 || b<=0)
    return;

  nx=a;
  ny=b;  
  if (nx*ny)
    {
      T* tmpX=new T[nx*ny];
      V=new T*[nx];
      for (size_t i=0;i<nx;i++)
	V[i]=tmpX + (i*ny);
    }
  return;
}  


template<typename T> 
std::vector<T>
MatrixBase<T>::Row(const size_t RowI) const
  /*!
    Returns a specific row
    \param RowI :: Row index 
    \return Vector of row
   */
{
  if (RowI>=nx)
    throw ColErr::IndexError<size_t>(RowI,nx,"MatrixBase::Row");
  std::vector<T> Out(V[RowI],V[RowI]+ny);
  return Out;
}

template<typename T> 
std::vector<T>
MatrixBase<T>::Column(const size_t ColI) const
  /*!
    Returns a specific column
    \param ColI :: Column index 
    \return Vector of Column
   */
{
  if (ColI>=ny)
    throw ColErr::IndexError<size_t>(ColI,ny,"MatrixBase::Column");
  std::vector<T> Out(nx);
  for(size_t i=0;i<nx;i++)
    Out[i]=V[i][ColI];
  return Out;
}

template<typename T> 
void
MatrixBase<T>::swapRows(const size_t RowI,const size_t RowJ)
  /*! 
    Swap rows I and J
    \param RowI :: row I to swap
    \param RowJ :: row J to swap
  */
{
  if (nx*ny && RowI<nx && RowJ<nx &&
      RowI!=RowJ) 
    {
      for(size_t k=0;k<ny;k++)
        {
	  T tmp=V[RowI][k];
	  V[RowI][k]=V[RowJ][k];
	  V[RowJ][k]=tmp;
	}
    }
  return;
}

template<typename T> 
void
MatrixBase<T>::swapCols(const size_t colI,const size_t colJ)
  /*! 
    Swap columns I and J 
    \param colI :: col I to swap
    \param colJ :: col J to swap
  */
{
  if (nx*ny && colI<ny && colJ<ny &&
      colI!=colJ) 
    {
      for(size_t k=0;k<nx;k++)
        {
	  T tmp=V[k][colI];
	  V[k][colI]=V[k][colJ];
	  V[k][colJ]=tmp;
	}
    }
  return;
}

template<typename T> 
void
MatrixBase<T>::zeroMatrix()
  /*! 
    Zeros all elements of the matrix 
  */
{
  if (nx*ny)
    for(size_t i=0;i<nx;i++)
      for(size_t j=0;j<ny;j++)
	V[i][j]=static_cast<T>(0);
  return;
}

template<typename T>
T
MatrixBase<T>::laplaceDeterminate() const
  /*!
    This function calculates the determinate of a matrix.
    It uses the Cauche method of Det.
    There are some careful constructs to avoid that 
    problem of matrix template object T not having a well
    defined 1.0 and an well defined 0.0.

    \return Determinate 
  */
{
  if (nx*ny<=0 || nx!=ny) 
    return T(0);
  if (nx==1)
    return V[0][0];
  if (nx==2)
    return (V[0][0]*V[1][1])-(V[0][1]*V[1][0]);

  T Sum(V[0][0]);            // Avoids the problem of incomplete zero
  MatrixBase<T> M(*this,0,0);
  Sum*=M.laplaceDeterminate();

  for(size_t j=1;j<ny;j++)          // Loop over all columns:
    {
      MatrixBase<T> MX(*this,0,j);
      if (j % 2)
	Sum-=V[0][j]*MX.laplaceDeterminate();
      else 
	Sum+=V[0][j]*MX.laplaceDeterminate();
    }
  return Sum;
}


template<typename T> 
void
MatrixBase<T>::identityMatrix()
  /*!
    Makes the matrix an idenity matrix.
    Zeros all the terms outside of the square
  */
{
  if (nx*ny)
    for(size_t i=0;i<nx;i++)
      for(size_t j=0;j<ny;j++)
	V[i][j]=(j==i) ? 1 : 0;

  return;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::fDiagonal(const std::vector<T>& Dvec) const
/*!
  Construct a matrix based on 
  A * This, where A is made into a diagonal 
  matrix.
  \param Dvec :: diagonal matrix (just centre points)
  \returns a matrix multiplication
*/
{
  // Note:: nx,ny zeroed so setMem always works
  if (Dvec.size()!=nx)
    throw ColErr::MisMatch<size_t>(nx,Dvec.size(),
				     "MatrixBase::fDiagonal");
  
  MatrixBase<T> OutM(Dvec.size(),ny);
  for(size_t i=0;i<Dvec.size();i++)
    for(size_t j=0;j<ny;j++)
      OutM.V[i][j]=Dvec[i]*V[i][j];
  return OutM;
}
  
template<typename T>
MatrixBase<T>
MatrixBase<T>::bDiagonal(const std::vector<T>& Dvec) const
/*!
  Construct a matrix based on 
  This * A, where A is made into a diagonal 
  matrix.
  \param Dvec :: diagonal matrix (just centre points)
  \return This * Diag
*/
{
  if (Dvec.size()!=ny)
    throw ColErr::MisMatch<size_t>(ny,Dvec.size(),"Matrix::bDiagonal");
  
  // Note:: nx,ny zeroed so setMem always works
  MatrixBase<T> X(nx,Dvec.size());
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<Dvec.size();j++)
      X.V[i][j]=Dvec[j]*V[i][j];
  return X;
}

template<typename T>
MatrixBase<T>
MatrixBase<T>::Tprime() const
/*! 
  Transpose the matrix : 
  Has transpose for a square matrix case.
  \return M^T
*/
{
  if (!(nx*ny))
    return *this;
  
  if (nx==ny)   // inplace transpose
    {
      MatrixBase<T> MT(*this);
      MT.Transpose();
      return MT;
    }
  
  // irregular matrix
  MatrixBase<T> MT(ny,nx);
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      MT.V[j][i]=V[i][j];
  
  return MT;
}
  
template<typename T>
MatrixBase<T>&
MatrixBase<T>::Transpose()
  /*! 
    Transpose the matrix : 
    Has a inplace transpose for a square matrix case.
    \return this
  */
{
  if (!(nx*ny))
    return *this;
  if (nx==ny)   // inplace transpose
    {
      for(size_t i=0;i<nx;i++)
	for(size_t j=i+1;j<ny;j++)
	  {
	    T tmp=V[i][j];
	    V[i][j]=V[j][i];
	    V[j][i]=tmp;
	  }
      return *this;
    }
  // irregular matrix
  // get some memory
  T* tmpX=new T[ny*nx];
  T** Vt=new T*[ny];
  for (size_t i=0;i<ny;i++)
    Vt[i]=tmpX + (i*nx);
  
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      Vt[j][i]=V[i][j];
  // remove old memory
  const size_t tx=nx;
  const size_t ty=ny;
  deleteMem();  // resets nx,ny
  // replace memory
  V=Vt;
  nx=ty;
  ny=tx;

  return *this;
}

template<typename T>
T
MatrixBase<T>::compSum() const
  /*!
    Add up each component sums for the matrix
    \return \f$ \sum_i \sum_j V_{ij}^2 \f$
  */
{
  T sum(0);
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      sum+=V[i][j]*V[i][j];
  return sum;
}


template<typename T> 
std::vector<T>
MatrixBase<T>::Diagonal() const
  /*!
    Returns the diagonal form as a vector
    \returns Diagonal elements
  */
{
  const size_t Msize=(ny>nx) ? nx : ny;
  std::vector<T> Diag(Msize);
  for(size_t i=0;i<Msize;i++)
    Diag[i]=V[i][i];
  return Diag;
}

template<typename T> 
T
MatrixBase<T>::Trace() const
  /*!
    Calculates the trace of the matrix
    \returns Trace of matrix 
  */
{
  const size_t Msize=(ny>nx) ? nx : ny;
  T Trx(0);
  for(size_t i=0;i<Msize;i++)
    Trx+=V[i][i];
  return Trx;
}


template<typename T>
void
MatrixBase<T>::print() const
  /*! 
    Simple print out routine 
   */
{
  write(std::cout,10);
  return;
}

template<typename T>
void
MatrixBase<T>::write(std::ostream& OX,const int blockCnt) const
  /*!
    Write out function for blocks of 10 Columns 
    \param Fh :: file stream for output
    \param blockCnt :: number of columns per line (0 == full)
  */
{
  std::ios::fmtflags oldFlags=OX.flags();
  OX.setf(std::ios::floatfield,std::ios::scientific);
  const size_t blockNumber((blockCnt>0) ? 
			   static_cast<size_t>(blockCnt) : ny);  
  size_t BCnt(0);

  do
    {
      const size_t ACnt=BCnt;
      BCnt+=blockNumber;
      if (BCnt>ny)
	BCnt=ny;

      if (ACnt)
	OX<<" ----- "<<ACnt<<" "<<BCnt<<" ------ "<<std::endl;
      for(size_t i=0;i<nx;i++)
        {
	  for(size_t j=ACnt;j<BCnt;j++)
	    OX<<std::setw(10)<<V[i][j]<<"  ";
	  OX<<std::endl;
	}
    } while(BCnt<ny);

  OX.flags(oldFlags);
  return;
}

template<typename T>
void
MatrixBase<T>::writeGrid(std::ostream& FX) const
  /*!
    Write out function for blocks:
    \param FX :: file stream for output
  */
{
  if (nx*ny<1) return;
  // need a list of longest strings [for each column]:
  std::vector<size_t> LStr(ny);
  fill(LStr.begin(),LStr.end(),0);
  
  // a matrix of strings:
  boost::multi_array<std::string,2> 
    MStr(boost::extents[static_cast<long int>(nx)]
	 [static_cast<long int>(ny)]);
  
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      {
	std::stringstream cx;
	cx<<V[i][j];
	const size_t len(cx.str().length());
	if (LStr[j]<len)
	  LStr[j]=len;
	const long int li(static_cast<long int>(i));
	const long int lj(static_cast<long int>(j));
	MStr[li][lj]=cx.str();
      }
  
  // WRITE PART:
  for(size_t i=0;i<nx;i++)
    {
      for(size_t j=0;j<ny;j++)
        {
	  const long int li(static_cast<long int>(i));
	  const long int lj(static_cast<long int>(j));
	  const size_t Fpad(LStr[j]-MStr[li][lj].length());
	  const size_t Lpad(2+Fpad/2);
	  const size_t Rpad(2+Lpad + (Fpad % 2));
	  FX<<std::string(Lpad,' ')<<MStr[li][lj]<<std::string(Rpad,' ');
	}
      FX<<std::endl;
    }
  return;
}

template<typename T>
std::string
MatrixBase<T>::str(const int spx) const
  /*!
    Convert the matrix into a simple linear string expression 
    \param spx :: precision [0 to use default] 
    \returns String value of output
  */
{
  std::ostringstream cx;
  for(size_t i=0;i<nx;i++)
    for(size_t j=0;j<ny;j++)
      {
	if (spx)
	  cx<<std::setprecision(spx)<<V[i][j]<<" ";
	else
	  cx<<V[i][j]<<" ";
      }
  return cx.str();
}

/// \cond TEMPLATE 

template class MatrixBase<double>;
template class MatrixBase<int>;
template class MatrixBase<mathLevel::PolyVar<1> >;
template class MatrixBase<mathLevel::PolyVar<2> >;
template class MatrixBase<mathLevel::PolyVar<3> >;

template std::ostream& operator<<(std::ostream&,const MatrixBase<double>&);
template std::ostream& operator<<(std::ostream&,const MatrixBase<int>&);
template std::ostream& operator<<(std::ostream&,
				  const MatrixBase<mathLevel::PolyVar<1> >&);
template std::ostream& operator<<(std::ostream&,
				  const MatrixBase<mathLevel::PolyVar<2> >&);
template std::ostream& operator<<(std::ostream&,
				  const MatrixBase<mathLevel::PolyVar<3> >&);

/// \endcond TEMPLATE

}  // NAMESPACE Geometry
