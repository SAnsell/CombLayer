/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testMatrix.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"

#include "testFunc.h"
#include "testMatrix.h"

using namespace Geometry;

testMatrix::testMatrix() 
  /// Constructor
{}

testMatrix::~testMatrix() 
  /// Destructor
{}

int 
testMatrix::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : testMatrix failed
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testMatrix","applyTest");
  TestFunc::regSector("testMatrix");

  typedef int (testMatrix::*testPtr)();
  testPtr TPtr[]=
    {
      &testMatrix::testDeterminate,
      &testMatrix::testDiagonalise,
      &testMatrix::testEigenvectors,
      &testMatrix::testInvert,
      &testMatrix::testLaplaceDeterminate,
      &testMatrix::testSwapRows
    };
  const std::string TestName[]=
    {
      "Determinate",
      "Diagonalise",
      "Eigenvectors",
      "Invert",
      "LaplaceDeterminate",
      "SwapRows"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(int i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
      return 0;
    }
  for(int i=0;i<TSize;i++)
    {
      if (extra<0 || extra==i+1)
        {
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

int
testMatrix::testDiagonalise()
  /*!
    Tests the diagonalisation 
    \retval -1 on failure
    \retval 0 :: success with 2x2 matrix
  */
{
  Matrix<double> Eval;
  Matrix<double> Diag;
  Matrix<double> A(2,2);
  A[0][0]=1.0;
  A[1][0]=3.0;
  A[0][1]=3.0;
  A[1][1]=4.0;
  if(!A.Diagonalise(Eval,Diag))
    return -1;
  Matrix<double> EvalT(Eval);
  EvalT.Transpose();
  Eval*=Diag;
  Eval*=EvalT;
  return (Eval==A) ? 0 : -2;
}

int
testMatrix::testEigenvectors()
  /*!
    Tests the eigenvalues and vectors
    \retval 0 on sucess with 3x3 matrix (symmetric)
    \retval 1 on failure
  */
{
  ELog::RegMethod RegA("testMatrix","testEigenvectors");

  Matrix<double> Eval;
  Matrix<double> Diag;
  Matrix<double> A(3,3);
  A[0][0]=1.0;
  A[1][0]=A[0][1]=4.0;
  A[0][2]=A[2][0]=5.0;
  A[1][1]=3.0;
  A[2][1]=A[1][2]=6.0;
  A[2][2]=-7.0;
  if(!A.Diagonalise(Eval,Diag))
    return -1;

  ELog::EM.lock();
  
  Matrix<double> MA=A*Eval;
  Matrix<double> MV=Eval*Diag;

  Eval.sortEigen(Diag);

  std::vector<double> X(3);
  X[0]=Eval[0][1];
  X[1]=Eval[1][1];
  X[2]=Eval[2][1];

  ELog::EM<<"T == ";
  copy(X.begin(),X.end(),std::ostream_iterator<double>(ELog::EM.Estream()," : "));
  ELog::EM<<ELog::endDebug;
  std::vector<double> out=A*X;
  ELog::EM<<"MX == ";
  copy(out.begin(),out.end(),std::ostream_iterator<double>(ELog::EM.Estream()," : "));
  ELog::EM<<ELog::endDebug;

  ELog::EM<<"lam*X == ";
  transform(X.begin(),X.end(),X.begin(),
	    std::bind2nd(std::multiplies<double>(),Diag[1][1]));
  copy(X.begin(),X.end(),
       std::ostream_iterator<double>(ELog::EM.Estream()," : "));
  ELog::EM<<ELog::endDebug;

  if (MA==MV)
    {
      ELog::EM.dispatch(0);
      return 0;
    }
  ELog::EM.dispatch(1);
  return -1;
}
  

int
testMatrix::testEigenvectors2()
  /*!
    Tests the eigenvalues and vectors
    \retval 0 on sucess with 3x3 matrix (symmetric)
    \retval 1 on failure
  */
{
  ELog::RegMethod RegA("testMatrix","testEigenvectors2");
  
  Matrix<double> Eval;
  Matrix<double> Diag;
  Matrix<double> A(2,2);
  A[0][0]=1.0;
  A[1][0]=-3.0;
  A[0][1]=-3.0;
  A[1][1]=2.0;
  if(!A.Diagonalise(Eval,Diag))
    return 1;
  Matrix<double> MA=A*Eval;
  Matrix<double> MV=Diag*Eval;
  std::cout<<"A == "<<A<<std::endl;
  std::cout<<"Diag == "<<Diag<<std::endl;
  std::cout<<"Eval == "<<Eval<<std::endl;
  std::cout<<"MA == "<<MA<<std::endl;
  std::cout<<"MV == "<<MV<<std::endl;
  std::vector<double> X(2);
  X[0]=Eval[0][0];
  X[1]=Eval[1][0];
  copy(X.begin(),X.end(),std::ostream_iterator<double>(std::cout," : "));
  std::vector<double> out=A*X;
  std::cout<<"MX == ";
  copy(out.begin(),out.end(),std::ostream_iterator<double>(std::cout," : "));
  std::cout<<std::endl;

  std::cout<<"lam*X == ";
  transform(X.begin(),X.end(),X.begin(),std::bind2nd(std::multiplies<double>(),Diag[0][0]));
  copy(X.begin(),X.end(),std::ostream_iterator<double>(std::cout," : "));
  std::cout<<std::endl;


  return (MA==MV) ? 0 : -1;
}
  
int
testMatrix::testSwapRows()
  /*!
    Check that we can swap rows and columns
    Always return 0
    \retval 0 :: always
  */
{
  ELog::RegMethod RegA("testMatrix","testSwapRows");

  Matrix<double> A(3,3);
  Matrix<double> Result(3,3);
  double resVal[]={1,3,2,7,9,8,4,6,5};

  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      {
	const size_t cnt=3*i+j;
	A[i][j]=static_cast<double>(cnt)+1.0;    // make 1->9
	Result[i][j]=resVal[cnt];
      }
  A.swapRows(1,2);
  A.swapCols(1,2);
  if (A!=Result)
    {
      ELog::EM<<"Matrix="<<A<<ELog::endDiag;
      ELog::EM<<"Result="<<Result<<ELog::endDiag;
      return -1;
    }

  return 0;
}

int
testMatrix::testInvert()
  /*!
    Test that a matrix can be inverted
    \retval -1 :: failure
    \retval 0 :: Succes with a 3x3 matrix
  */
{
  ELog::RegMethod RegA("testMatrix","Invert");

  Matrix<double> A(3,3);

  A[0][0]=1.0; A[1][0]=3.0; A[0][1]=4.0;
  A[0][2]=6.0; A[2][0]=5.0; A[1][1]=3.0;
  A[2][1]=1.0; A[1][2]=6.0; A[2][2]=-7.0;

  Matrix<double> Iv=A;
  const double Det=Iv.Invert();
  Matrix<double> Ident(3,3);
  Ident.identityMatrix();

  if (fabs(Det-105.0)>1e-5 || 
      (Ident!= A*Iv) || (Ident!=Iv*A))
    {
      ELog::EM<<A<<ELog::endTrace;
      ELog::EM<<"Failed Invert"<<ELog::endTrace;
      ELog::EM<<"Det (105) == "<<Det<<ELog::endTrace;
      ELog::EM<<"IV == "<<Iv<<ELog::endTrace;
      ELog::EM<<A*Iv<<ELog::endTrace;
      ELog::EM<<"Flags =="<<fabs(Det-105.0)<<":"
	      <<(Ident!=A*Iv)<<ELog::endTrace;
      return -1;
    }

  return 0;
}

int
testMatrix::testDeterminate()
  /*!
    Test that a matrix can be inverted using 
    Faddeev method. 
    \retval -1 :: failure
    \retval 0 :: Success with a 3x3 matrix
  */
{
  ELog::RegMethod RegA("testMatrix","testDeterminate");

  // EXAMPLE MATRIX : detemiate is 18.
  Matrix<double> A(3,3);

  A[0][0]=2.0;
  A[0][1]=-1.0;
  A[0][2]=1.0;
  
  A[1][0]=-1.0;
  A[1][1]=2.0;
  A[1][2]=1.0;

  A[2][0]=1.0;
  A[2][1]=-1.0;
  A[2][2]=2.0;

  Matrix<double> IA(3,3);
  Matrix<double> Ident(3,3);
  Ident.identityMatrix();
  std::vector<double> Poly=A.Faddeev(IA);
  
  if (IA*A!=Ident || fabs(Poly.front()-6.0)>1e-5)
    {  
      ELog::EM<<"Matrix ="<<ELog::endCrit;
      ELog::EM<<A<<ELog::endCrit;
      ELog::EM<<"Inverse ="<<ELog::endCrit;
      ELog::EM<<IA<<ELog::endCrit;
      
      ELog::EM<<"Ident ="<<ELog::endCrit;
      ELog::EM<<IA*A<<ELog::endCrit;

      // Not sure what this is for
      ELog::EM<<"Poly == "<<ELog::endCrit;
      copy(Poly.begin(),Poly.end(),
	   std::ostream_iterator<double>(ELog::EM.Estream()," : "));
      ELog::EM<<ELog::endCrit;
      return -1;
    }
   
  return 0;
}

int
testMatrix::testLaplaceDeterminate()
  /*!
    Test that a matrix determinate 
    \retval -1 :: failure
    \retval 0 :: Success with a 3x3/4x4 matrix
  */
{
  ELog::RegMethod RegA("testMAtrix","testLaplaceDeterminate");
  // EXAMPLE MATRIX : detemiate is 6.
  Matrix<double> A(3,3);

  A[0][0]=2.0;
  A[0][1]=-1.0;
  A[0][2]=1.0;
  
  A[1][0]=-1.0;
  A[1][1]=2.0;
  A[1][2]=1.0;

  A[2][0]=1.0;
  A[2][1]=-1.0;
  A[2][2]=2.0;

  const double DetA=A.laplaceDeterminate();
  if (fabs(DetA-6.0)>1e-6)
    {
      ELog::EM<<"Matrix == "<<A<<ELog::endDebug;
      ELog::EM<<"Determinate(3x3) == "<<A.laplaceDeterminate()<<ELog::endDebug;
      return -2;
    }

  Matrix<double> B(4,4);

  B[0][0]=2.0;
  B[0][1]=-1.0;
  B[0][2]=1.0;
  B[0][3]=4.0;
  
  B[1][0]=-1.0;
  B[1][1]=2.0;
  B[1][2]=1.0;
  B[1][3]=-3.0;

  B[2][0]=1.0;
  B[2][1]=-1.0;
  B[2][2]=2.0;
  B[2][3]=8;

  B[3][0]=7.0;
  B[3][1]=4.0;
  B[3][2]=3.0;
  B[3][3]=2.0;

  const double DetB=B.laplaceDeterminate();
  if (fabs(DetB-94.0)>1e-6)
    {
      ELog::EM<<"Matrix == "<<B<<ELog::endDebug;
      ELog::EM<<"Determinate(4x4) == "<<B.laplaceDeterminate()<<ELog::endDebug;
      return -2;
    }
  return 0;
}
  
