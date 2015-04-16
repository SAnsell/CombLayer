/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/SVD.cxx
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
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <limits>
#include <vector>
#include <map>
#include <string>

#ifndef NO_GSL
#include <gsl/gsl_linalg.h>
#endif

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "SVD.h"

namespace Geometry
{

SVD::SVD() :
  mRows(0),nCols(0)
  /*!
    Constructor
  */
{}

SVD::SVD(const SVD& SX) : 
  mRows(SX.mRows),nCols(SX.nCols),
  A(SX.A),S(SX.S),U(SX.U),V(SX.V)
  /*!
    Copy Constructor
    \param SX :: SVD object to copy
  */
{ }

SVD&
SVD::operator=(const SVD& SX)
  /*!
    Assignment operator
    \param SX :: SVD object to copy
    \return *this
  */
{
  if (&SX!=this)
    {
      mRows=SX.mRows;
      nCols=SX.nCols;
      A=SX.A;
      S=SX.S;
      U=SX.U;
      V=SX.V;
    }
  return *this;
}

SVD::~SVD()
  /*!
    Destructor
  */
{ }

void
SVD::setMatrix(const Matrix<double>& M)
  /*!
    Given a design matrix M, set the columns and rows
    and matrix.
    \param M :: New design matrix
  */
{
  std::pair<size_t,size_t> Mpair=M.size();
  A=M;
  mRows=Mpair.first;
  nCols=Mpair.second;
  return;
}

int
SVD::removeSingular(const double D)
  /*!
    Remove eigenvectors that have an eigenvalue of magnitude below D
    \param D :: Test value
    \return number zeroed						
  */
{
  int cnt(0);
  for(size_t i=0;i<nCols;i++)
    if (fabs(S[i][i])<D)
      {
	S[i][i]=0.0;
	cnt++;
      }
  return cnt;
}

void
SVD::calcDecomp()
  /*!
    Computes the decomposition
   */
{
  ELog::RegMethod RegItem("SVD","calcDecomp");
  if (A.itemCnt()<1) 
    {
      ELog::EM.error("No values in matrix A");
      return;
    }
  // copy assign
  
  U=A;
  V.setMem(nCols,nCols);
  S.setMem(nCols,nCols);
  S.zeroMatrix();

#ifndef NO_GSL
  gsl_matrix_view gAview=gsl_matrix_view_array(U[0],mRows,nCols);
  gsl_matrix_view gVview=gsl_matrix_view_array(V[0],nCols,nCols);
  gsl_vector* gS=gsl_vector_alloc(nCols);
  gsl_vector* gWork=gsl_vector_alloc(nCols);
  
  
  gsl_linalg_SV_decomp(&gAview.matrix,&gVview.matrix,gS,gWork);
  for(size_t i=0;i<nCols;i++)
    S[i][i]=gsl_vector_get(gS,i);
  
  gsl_vector_free(gS);
  gsl_vector_free(gWork);
#else
  ELog::EM<<"NO GSL available : Recompile with GSL"<<ELog::endErr;
  throw std::runtime_error("NO GSL "+RegItem.getBase());
#endif

  return;
}


std::vector<double>
SVD::linearEQ(const std::vector<double>& B) const
  /*!
    Given a vector B of the same length at the nRows
    find the a vector
    \param B :: Results vector
    \return result of A*R=B
  */
{
  if (B.size()!=mRows)
    throw ColErr::MisMatch<size_t>(B.size(),mRows,"SVD::linearEQ");

  std::vector<double> Out(nCols);
  for(size_t i=0;i<nCols;i++)
    {
      if (fabs(S[i][i])>1e-20)
        {
	  double dotValue(0.0);
	  for(size_t ii=0;ii<mRows;ii++)
	    dotValue+=U[ii][i]*B[ii];
	  
	  dotValue/=S[i][i];
	  
	  for(size_t jj=0;jj<nCols;jj++)
	    Out[jj]+=dotValue*V[jj][i];
	}
    }
  return Out;
}



		
}  // NAMESPACE Geometry



