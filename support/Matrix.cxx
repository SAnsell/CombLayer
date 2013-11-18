/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/Matrix.cxx
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
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

namespace Geometry
{

template<typename T>
Matrix<T>::Matrix(const size_t nrow,const size_t ncol) :
  MatrixBase<T>(nrow,ncol)
  /*!
    Constructor with pre-set sizes. Matrix is zeroed
    \param nrow :: number of rows
    \param ncol :: number of columns
  */
{}
 

template<typename T>
Matrix<T>::Matrix(const std::vector<T>& A,const std::vector<T>& B) :
  MatrixBase<T>(A,B)
  /*!
    Constructor to take two vectors and multiply them to 
    construct a matrix. (assuming that we have columns x row
    vector.
    \param A :: Column vector to multiply
    \param B :: Row vector to multiply
  */
{}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& A,
		  const size_t nrow,const size_t ncol)
  : MatrixBase<T>(A,nrow,ncol)
  /*!
    Constructor with for a missing row/column.
    \param A :: MatrixBase to use as master
    \param nrow :: number of row to cut
    \param ncol :: number of column to cut
  */
{}

template<typename T>
Matrix<T>::Matrix(const MatrixBase<T>& A)
  : MatrixBase<T>(A)
  /*! 
    Simple copy constructor
    \param A :: Object to copy
  */
{}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& A)
  : MatrixBase<T>(A)
  /*! 
    Simple copy constructor
    \param A :: Object to copy
  */
{}

template<typename T>
Matrix<T>&
Matrix<T>::operator=(const Matrix<T>& A)
  /*! 
    Simple assignment operator 
    \param A :: Object to copy
		\return *this
  */
{
  if (&A!=this)
    MatrixBase<T>::operator=(A);
  return *this;
}

template<typename T>
Matrix<T>::~Matrix()
  /*!
    Delete operator :: removes memory for 
    matrix
  */
{}

template<typename T>
Matrix<T>&
Matrix<T>::operator+=(const MatrixBase<T>& A)
  /*!
    Addition operator
    \param A :: Matrix to add
    \return this+A;
  */
{
  MatrixBase<T>::operator+=(A);
  return *this;
}


template<typename T>
Matrix<T>&
Matrix<T>::operator-=(const MatrixBase<T>& A) 
  /*!
    Subtraction operator
    \param A :: Matrix to subtract
    \return this - A
  */
{
  MatrixBase<T>::operator-=(A);
  return *this;
}


template<typename T>
Matrix<T>&
Matrix<T>::operator*=(const MatrixBase<T>& A)
  /*!
    Multiplication operator
    \param A :: Matrix to multiply
    \return this * A
  */
{
  MatrixBase<T>::operator*=(A);
  return *this;
}

template<typename T>
Matrix<T>&
Matrix<T>::operator*=(const T& A)
  /*!
    Multiplication operator
    \param A :: Value to multiply
    \return this * A
  */
{
  MatrixBase<T>::operator*=(A);
  return *this;
}

template<typename T>
Matrix<T>
Matrix<T>::operator+(const MatrixBase<T>& A) const
  /*!
    Addition operator
    \param A :: Matrix to add
    \return (this + A)
  */
{
  Matrix<T> Out(*this);
  return Out+=A;
}

template<typename T>
Matrix<T>
Matrix<T>::operator-(const MatrixBase<T>& A) const
  /*!
    Subtraction operator
    \param A :: Matrix to subtract
    \return (this - A)
  */
{
  Matrix<T> Out(*this);
  return Out-=A;
}

template<typename T>
Matrix<T>
Matrix<T>::operator*(const MatrixBase<T>& A) const
  /*!
    Multiplication operator
    \param A :: Matrix to multiply
    \return (this * A)
  */
{
  Matrix<T> Out(*this);
  return Out*=A;
}

template<typename T>
Matrix<T>
Matrix<T>::operator*(const T& A) const
  /*!
    Multiplication operator
    \param A :: Value to multiply
    \return (this * A)
  */
{
  Matrix<T> Out(*this);
  return Out*=A;
}

template<typename T>
std::vector<T>
Matrix<T>::operator*(const std::vector<T>& Vec) const
  /*! 
    MatrixBase multiplication THIS * Vec to produce a vec  
    \param Vec :: size of vector > this->nrows
    \throw MisMatch<int> if there is a size mismatch.
    \return vector<T> (This * Vec)
  */
{
  return MatrixBase<T>::operator*(Vec);
}

template<typename T>
Vec3D
Matrix<T>::operator*(const Vec3D& Vx) const
  /*! 
    Matrix multiplication THIS * V
    \param Vx :: Colunm vector to multiply by
    \throw MisMatch<long int> if there is a size mismatch.
    \return Matrix(This * A) 
 */
{
  if (this->ny!=3)
    throw ColErr::MisMatch<size_t>(this->ny,3,"Matrix::operator*(Vec3D)");
  Vec3D X;
  for(size_t i=0;i<this->nx;i++)
    for(size_t kk=0;kk<this->ny;kk++)
      X[i]+=this->V[i][kk]*Vx[kk];
  return X;
}

template<typename T>
Matrix<T>&
Matrix<T>::operator/=(const T& Value)
   /*! 
     Matrix division THIS / Value  
     \param Value :: Scalar to multiply matrix by
     \return *this
   */
{
  for(size_t i=0;i<this->nx;i++)
    for(size_t j=0;j<this->ny;j++)
      this->V[i][j]/=Value;
  return *this;
}

template<typename T>
bool
Matrix<T>::operator!=(const Matrix<T>& A) const
  /*!
    Element by Element comparison
    \param A :: Matrix to check
    \return 1 :: on sucees
    \return 0 :: failoure
  */
{
  return (this->operator==(A)) ? 0 : 1;
}

template<typename T>
bool
Matrix<T>::operator==(const Matrix<T>& A) const
  /*! 
    Element by element comparison within tolerance.
    Tolerance means that the value must be > tolerance
    and less than (diff/max)>tolerance 

    Always returns 0 if the Matrix have different sizes
    \param A :: matrix to check.
    \return 1 on success 
  */
{
  ELog::RegMethod RegA("Matrix","operator==");

  const T Tolerance(1e-6);
  if (&A!=this)       // this == A == always true
    {
      if(A.nx!=this->nx || A.ny!=this->ny)
	return 0;

      T maxS(0.0);
      T maxDiff(0.0);   // max di
      for(size_t i=0;i<this->nx;i++)
	for(size_t j=0;j<this->ny;j++)
	  {
	    const T diff=this->V[i][j]-A.V[i][j];
	    if (fabs(diff)>maxDiff)
	      maxDiff=fabs(diff);
	    if (fabs(this->V[i][j])>maxS)
	      maxS=fabs(this->V[i][j]);
	  }
      if (maxDiff<Tolerance)
	return 1;
      if (maxS>1.0 && (maxDiff/maxS)<Tolerance)
	return 1;
      return 0;
    }
  //this == this is true
  return 1;
}

template<typename T>
void
Matrix<T>::rotate(const T tau,const T s,const size_t i,const size_t j,
		  const size_t k,const size_t m)
/*!
  Applies a rotation to a particular point of tan(theta)=tau
  \param tau :: tan(theta) 
  \param s :: sin(theta)
  \param i ::  first index (xpos) 
  \param j ::  first index (ypos) 
  \param k ::  second index (xpos) 
  \param m ::  second index (ypos) 
*/
{
  const T gg=this->V[i][j];
  const T hh=this->V[k][m]; 
  this->V[i][j]=static_cast<T>(gg-s*(hh+gg*tau));
  this->V[k][m]=static_cast<T>(hh+s*(gg-hh*tau));
  return;
}

template<>
int
Matrix<int>::GaussJordan(Geometry::Matrix<int>&)
 /*!
   Not valid for Integer
   \return 0
*/
{
  return 0;
}

template<typename T>
int
Matrix<T>::GaussJordan(Matrix<T>& B)
/*!
  Invert this Matrix and solve the 
  form such that if A.x=B then  solve to generate x.  
  This requires that B is B[A.nx][A.ny]
  The result is placed back in B
  \param B :: Matrix B 
  \return 0 :: success / -ve on error
*/
{
  ELog::RegMethod RegA("Matrix<T>","GaussJordan");
  // check for input errors
  if (this->nx!=this->ny || B.nx!=this->nx || !this->nx)
    return -1;
  
  // pivoted rows 
  std::vector<size_t> pivoted(this->nx);
  fill(pivoted.begin(),pivoted.end(),0);

  std::vector<size_t> indxcol(this->nx);   // Colunm index
  std::vector<size_t> indxrow(this->nx);   // row index

  size_t irow(0),icol(0);
  for(size_t i=0;i<this->nx;i++)
    {
      // Get Biggest non-pivoted item
      T bigItem=0.0;         // get point to pivot over
      for(size_t j=0;j<this->nx;j++)
	{
	  if (pivoted[j]!= 1)  //check only non-pivots
	    {
	      for(size_t k=0;k<this->nx;k++)
		if (!pivoted[k])
		  {
		    if (fabs(this->V[j][k]) >=bigItem)
		      {
			bigItem=fabs(this->V[j][k]);
			irow=j;
			icol=k;
		      }
		  }
	    }
	  else if (pivoted[j]>1)
	    throw ColErr::ExBase
	      (static_cast<int>(j),
	       "Error doing G-J elem on a singular matrix");
	}


      pivoted[icol]++;
      // Swap in row/col to make a diagonal item 
      if (irow != icol)
	{
	  this->swapRows(irow,icol);
	  B.swapRows(irow,icol);
	}
      indxrow[i]=irow;
      indxcol[i]=icol;

      if (fabs(this->V[icol][icol])>1e-80)
	{
	  ELog::EM<<"Singular matrix"<<ELog::endCrit;
	  return 1;
	}

      const T pivDiv= 1.0/this->V[icol][icol];
      this->V[icol][icol]=1;
      for(size_t l=0;l<this->nx;l++) 
	this->V[icol][l] *= pivDiv;
      for(size_t l=0;l<B.ny;l++)
	B.V[icol][l] *=pivDiv;

      for(size_t ll=0;ll<this->nx;ll++)
	if (ll!=icol)
	  {
	    const T div_num=this->V[ll][icol];
	    this->V[ll][icol]=0.0;
	    for(size_t l=0;l<this->nx;l++) 
	      this->V[ll][l] -= this->V[icol][l]*div_num;
	    for(size_t l=0;l<B.ny;l++)
	      B.V[ll][l] -= B.V[icol][l]*div_num;
	  }
    }
  // Un-roll interchanges
  for(size_t ll=this->nx;ll>0;ll--)
    {
      const size_t l(ll-1);
      if (indxrow[l] !=indxcol[l])
	this->swapCols(indxrow[l],indxcol[l]);
    }
  return 0;  
}

template<typename T>
std::vector<T>
Matrix<T>::Faddeev(Matrix<T>& InvOut)
/*!
  Return the polynominal for the matrix
  and the inverse. 
  The polynomial is such that
  \f[
  det(sI-A)=s^n+a_{n-1}s^{n-1} \dots +a_0
  \f]
  \param InvOut ::: output 
  \return vector of polynominal
*/
{
  if (this->nx!=this->ny)
    throw ColErr::MisMatch<size_t>
      (this->nx,this->ny,"Matrix::Faddev(Matrix)");

  Matrix<T>& A(*this);
  Matrix<T> B(A);
  Matrix<T> Ident(this->nx,this->ny);
  Ident.identityMatrix();

  T tVal=B.Trace();                     // Trace of the matrix
  std::vector<T> Poly;
  Poly.push_back(tVal);

  // skip first (just copy) and last (to keep B-1)
  for(size_t i=0;(i+2)<this->nx;i++)   
    {
      B=A* (B - Ident*tVal);
      tVal=B.Trace()/static_cast<T>(i+2);
      Poly.push_back(tVal);
    }
  // Last on need to keep B;
  InvOut=B;
  B=A*(B - Ident*tVal);

  InvOut -= Ident* Poly.back();
  tVal=B.Trace()/static_cast<T>(this->nx);
  Poly.push_back(tVal);

  InvOut/= Poly.back();
  Poly.push_back(1);
 
  return Poly;
}

template<typename T>
T
Matrix<T>::Invert()
/*!
  If the Matrix is square then invert the matrix
  using LU decomposition
  \returns Determinate (0 if the matrix is singular)
*/
{
  if (this->nx!=this->ny && this->nx<1)
    return 0;
  
  size_t* indx=new size_t[this->nx];   // Set in lubcmp

  T* col=new T[this->nx];
  int d;
  Matrix<T> Lcomp(*this);
  Lcomp.lubcmp(indx,d);
	
  T det=static_cast<T>(d);
  for(size_t j=0;j<this->nx;j++)
    det *= Lcomp.V[j][j];
  
  for(size_t j=0;j<this->nx;j++)
    {
      for(size_t i=0;i<this->nx;i++)
	col[i]=0;
      
      col[j]=1;
      Lcomp.lubksb(indx,col);
      for(size_t i=0;i<this->nx;i++)
	this->V[i][j]=static_cast<T>(col[i]);
    } 
  delete [] indx;
  delete [] col;
  return det;
}

template<typename T>
T
Matrix<T>::determinant() const
  /*!
    Calculate the derminant of the matrix
    \return Determinant of matrix.
  */
{
  if (this->nx!=this->ny)
    throw ColErr::MisMatch<size_t>(this->nx,this->ny,
				   "Determinant error :: Matrix is not NxN");

  Matrix<T> Mt(*this);         //temp copy
  T D=Mt.factor();
  return D;
}

template<typename T>
T
Matrix<T>::factor()
/*! 
  Gauss jordan diagonal factorisation 
  The diagonal is left as the values, 
  the lower part is zero.
  \return Factor
*/
{
  ELog::RegMethod RegA("Matrix<T>","factor");
  if (this->nx!=this->ny || this->nx<1)
    throw ColErr::MisMatch<size_t>(this->nx,this->ny,
				     " Matrix is not NxN");

  T Pmax;
  T deter=1.0;
  for(size_t i=0;i<this->nx-1;i++)       //loop over each row
    {
      size_t jmax=i;
      Pmax=fabs(this->V[i][i]);
      for(size_t j=i+1;j<this->nx;j++)     // find max in Row i
        {
	  if (fabs(this->V[i][j])>Pmax)
            {
	      Pmax=fabs(this->V[i][j]);
	      jmax=j;
	    }
	}
      if (Pmax<1e-8)         // maxtrix signular 
        {
	  ELog::EM<<"Singular matrix"<<ELog::endCrit;
	  return 0;
	}
      // Swap Columns
      if (i!=jmax) 
        {
	  this->swapCols(i,jmax);
	  deter*=-1;            //change sign.
	}
      // zero all rows below diagonal
      Pmax=this->V[i][i];
      deter*=Pmax;
      for(size_t k=i+1;k<this->nx;k++)  // row index
        {
	  const T scale=this->V[k][i]/Pmax;
	  this->V[k][i]=static_cast<T>(0);
	  for(size_t q=i+1;q<this->nx;q++)   //column index
	    this->V[k][q]-=static_cast<T>(scale*this->V[i][q]);
	}
    }
  deter*=this->V[this->nx-1][this->nx-1];
  return static_cast<T>(deter);
}

template<typename T>
void 
Matrix<T>::normVert()
/*!
  Normalise EigenVectors
  Assumes that they have already been calculated
*/
{
  for(size_t i=0;i<this->nx;i++)
    {
      T sum=0;
      for(size_t j=0;j<this->ny;j++)
	sum+=this->V[i][j]*this->V[i][j];
      sum=static_cast<T>(sqrt(sum));
      for(size_t j=0;j<this->ny;j++)
	this->V[i][j]/=sum;
    }
  return;
}

template<typename T>
void 
Matrix<T>::lubcmp(size_t* rowperm,int& interchange)
/*!
  Find biggest pivit and move to top row. Then
  divide by pivit.
  \param rowperm :: row permuations [array of nx]
  \param interchange :: odd/even interchange (+/-1)
*/
{
  ELog::RegMethod RegA("Matrix<T>","lubcmp");
  size_t imax(0);
  T sum,dum,big;
  
  if (this->nx!=this->ny || this->nx<2)
    {
      ELog::EM<<"Requires square matrix"<<ELog::endErr;
      return;
    }

  T *vv=new T[this->nx];
  interchange=1;
  for(size_t i=0;i<this->nx;i++)
    {
      big=0.0;
      for(size_t j=0;j<this->nx;j++)
	{
	  const T temp=fabs(this->V[i][j]);
	  if (temp > big) 
	    big=temp;
	}
      // care with memory leak
      if (big <1e-80) 
        {
	  delete [] vv;
	  return;
	}
      vv[i]=1.0/big;
    }

  for (size_t j=0;j<this->nx;j++)
    {
      for(size_t i=0;i<j;i++)
        {
          sum=this->V[i][j];
          for(size_t k=0;k<i;k++)
            sum-= this->V[i][k] * this->V[k][j];
          this->V[i][j]=sum;
        }
      big=0.0;
      imax=j;
      for (size_t i=j;i<this->nx;i++)
        {
          sum=this->V[i][j];
          for (size_t k=0;k<j;k++)
            sum -= this->V[i][k] * this->V[k][j];
          this->V[i][j]=sum;
	  dum=vv[i] * fabs(sum);
          if (dum >= big)
            {
              big=dum;
              imax=i;
            }
        }
      if (j!=imax)
        {
          for(size_t k=0;k<this->nx;k++)
            {                     //Interchange rows
              dum=this->V[imax][k];
              this->V[imax][k]=this->V[j][k];
              this->V[j][k]=static_cast<T>(dum);
            }
          interchange *= -1;
          vv[imax]=vv[j];
        }

      rowperm[j]=imax;
      if (fabs(this->V[j][j]) < 1e-28) 
        this->V[j][j]=1e-14;
      if (j != this->nx-1)
        {
          dum=1.0/(this->V[j][j]);
          for(size_t i=j+1;i<this->nx;i++)
            this->V[i][j] *= dum;
        }
    }
  delete [] vv;
  return;
}

template<typename T>
void 
Matrix<T>::lubksb(const size_t* rowperm,T* b)
/*!
  Impliments a separation of the Matrix
  into a triangluar matrix
  \param rowperm :: array of perumutate array
  \param b :: b vector
*/
{
  size_t ii(this->nx);

  for(size_t i=0;i<this->nx;i++)
    {
      size_t ip=rowperm[i];
      T sum=b[ip];
      b[ip]=b[i];
      if (ii != this->nx) 
        for (size_t j=ii;j<i;j++) 
          sum -= this->V[i][j] * b[j];
      else if (sum!=0.0) 
        ii=i;
      b[i]=sum;
    }

  size_t i=this->nx;
  do
    {
      i--;
      T sum=b[i];
      for (size_t j=i+1;j<this->nx;j++)
        sum -= this->V[i][j] * static_cast<T>(b[j]);
      b[i]=sum/this->V[i][i];
    } while(i>0);
  return;
}

template<typename T>
void
Matrix<T>::averSymmetric()
/*!
  Simple function to take an average symmetric matrix
  out of the Matrix
*/
{
  const size_t minSize=(this->nx>this->ny) ? this->ny : this->nx;
  for(size_t i=0;i<minSize;i++)
    for(size_t j=i+1;j<minSize;j++)
      {
	this->V[i][j]=(this->V[i][j]+this->V[j][i])/2;
	this->V[j][i]=this->V[i][j];
      }
  return;
}

template<typename T>
Matrix<T>
Matrix<T>::Tprime() const
/*! 
  Transpose the matrix : 
  Has transpose for a square matrix case.
  \return M^T
*/
{
  Matrix<T> MT(MatrixBase<T>::Tprime());
  return MT;
}

template<typename T> 
void
Matrix<T>::sortEigen(Matrix<T>& DiagMatrix) 
/*!
  Sorts the eigenvalues into increasing
  size. Moves the EigenVectors correspondingly
  \param DiagMatrix :: matrix of the EigenValues
*/
{
  ELog::RegMethod RegA("Matrix<T>","sortEigen");
  if (this->ny!=this->nx || 
      this->nx!=DiagMatrix.nx || this->nx!=DiagMatrix.ny)
    {
      ELog::EM<<"Matrixes not matched"<<ELog::endErr;
      return;
    }
  std::vector<size_t> index;
  std::vector<T> X=DiagMatrix.Diagonal();
  indexSort(X,index);
  Matrix<T> EigenVec(*this);
  for(size_t Icol=0;Icol<this->nx;Icol++)
    {
      for(size_t j=0;j<this->nx;j++)
	this->V[j][Icol]=EigenVec[j][index[Icol]];
      DiagMatrix[Icol][Icol]=X[index[Icol]];
    }

  return;
}

template<typename T>
int 
Matrix<T>::Diagonalise(Matrix<T>& EigenVec,Matrix<T>& DiagMatrix) const
/*!
  Attempt to diagonalise the matrix IF symmetric
  \param EigenVec :: (output) the Eigenvectors matrix 
  \param DiagMatrix  :: the diagonal matrix of eigenvalues 
  \returns :: 1  on success 0 on failure
*/
{
  ELog::RegMethod RegA("Matrix<T>","Diagnalise");
  T theta,tresh,tanAngle,cosAngle,sinAngle;
  if(this->nx!=this->ny || this->nx<1)
    {
      ELog::EM.error("Matrix not square");
      return 0;
    }
  for(size_t i=0;i<this->nx;i++)
    for(size_t j=i+1;j<this->nx;j++)
      if (fabs(this->V[i][j]-this->V[j][i])>1e-6)
        {
	  ELog::EM<<"Matrix not Symmetric"<<ELog::endCrit;
	  ELog::EM<<(*this)<<ELog::endErr;
	  return 0;
	}
    
  Matrix<T> A(*this);
  //Make V an identity matrix
  EigenVec.setMem(this->nx,this->nx);
  EigenVec.identityMatrix();
  DiagMatrix.setMem(this->nx,this->nx);
  DiagMatrix.zeroMatrix();

  std::vector<T> Diag(this->nx);
  std::vector<T> B(this->nx);
  std::vector<T> ZeroComp(this->nx);
  //set b and d to the diagonal elements o A
  for(size_t i=0;i<this->nx;i++)
    {
      Diag[i]=B[i]= A.V[i][i];
      ZeroComp[i]=0;
    }

  int iteration=0;
  T sm; 
  for(size_t i=0;i<100;i++)        //max 50 iterations 
    {
      sm=0.0;           // sum of off-diagonal terms
      for(size_t ip=0;ip<this->nx-1;ip++)
	for(size_t iq=ip+1;iq<this->nx;iq++)
	  sm+=fabs(A.V[ip][iq]);

      if (fabs(sm)<1e-31)           // Nothing to do return...
	{
	  // Make OUTPUT -- D + A
	  for(size_t ix=0;ix<this->nx;ix++)
	    DiagMatrix.V[ix][ix]=static_cast<T>(Diag[ix]);
	  return 1;
	}

      // Threshold large for first 5 sweeps
      tresh= (i<6) ? 0.2*sm/static_cast<T>(this->nx*this->nx) : 0;

      for(size_t ip=0;ip<this->nx-1;ip++)
	{
	  for(size_t iq=ip+1;iq<this->nx;iq++)
	    {
	      T g=100.0*fabs(A.V[ip][iq]);
	      // After 4 sweeps skip if off diagonal small
	      if (i>6 && 
		  static_cast<float>(fabs(Diag[ip]+g))==
		  static_cast<float>(fabs(Diag[ip])) &&
		  static_cast<float>(fabs(Diag[iq]+g))==
		  static_cast<float>(fabs(Diag[iq])))
		A.V[ip][iq]=0;

	      else if (fabs(A.V[ip][iq])>tresh)
		{
		  T h=Diag[iq]-Diag[ip];
		  if (static_cast<float>((fabs(h)+g)) == 
		      static_cast<float>(fabs(h)))
		    tanAngle=A.V[ip][iq]/h;      // tanAngle=1/(2theta)
		  else
		    {
		      theta=0.5*h/A.V[ip][iq];
		      tanAngle=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
		      if (theta<0.0)
			tanAngle = -tanAngle;
		    }
		  cosAngle=1.0/sqrt(1+tanAngle*tanAngle);
		  sinAngle=tanAngle*cosAngle;
		  T tau=sinAngle/(1.0+cosAngle);
		  h=tanAngle*A.V[ip][iq];
		  ZeroComp[ip] -= h;
		  ZeroComp[iq] += h;
		  Diag[ip] -= h;
		  Diag[iq] += h;
		  A.V[ip][iq]=0;
		  // Rotations 0<j<p
		  for(size_t j=0;j<ip;j++)
		    A.rotate(tau,sinAngle,j,ip,j,iq);
		  for(size_t j=ip+1;j<iq;j++)
		    A.rotate(tau,sinAngle,ip,j,j,iq);
		  for(size_t j=iq+1;j<this->nx;j++)
		    A.rotate(tau,sinAngle,ip,j,iq,j);
		  for(size_t j=0;j<this->nx;j++)
		    EigenVec.rotate(tau,sinAngle,j,ip,j,iq);
		  iteration++;
		}
	    }
	}   
      for(size_t j=0;j<this->nx;j++)
	{
	  B[j]+=ZeroComp[j];
	  Diag[j]=B[j];
	  ZeroComp[j]=0.0;
	}
    }

  ELog::EM<<"Iterations are a problem"<<ELog::endCrit;
  return 0;
}

/// \cond TEMPLATE

template class Matrix<double>;
  // template class Matrix<int>;

/// \endcond TEMPLATE

}
