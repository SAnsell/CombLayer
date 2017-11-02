/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSVD.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <functional>
#include <numeric>
#include <boost/format.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "mathSupport.h"
#include "SVD.h"

#include "testFunc.h"
#include "testSVD.h"

extern MTRand RNG;
using namespace Geometry;


testSVD::testSVD() 
  /*!
    Constructor
  */
{}

testSVD::~testSVD() 
  /*!
    Destructor
  */
{}

int 
testSVD::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval 0 :: Success
    \retval -ve :: Failure number
  */
{
  ELog::RegMethod RegItem("testSVD","applyTest");
  TestFunc::regSector("testSVD");

  typedef int (testSVD::*testPtr)();
  testPtr TPtr[]=
    { 
      &testSVD::testInit,
      &testSVD::testMakeSolution,
      &testSVD::testMakeSolLong
    };

  const std::string TestName[] = 
    {
      "init",
      "makeSolution",
      "makeSolLong"
    };
  const int TSize(sizeof(TPtr)/sizeof(testPtr));

  int retValue(0);
  boost::format FmtStr("test%1$s%|30t|(%2$d)\n");
  if (!extra)
    {
      for(int i=0;i<TSize;i++)
	std::cout<<FmtStr % TestName[i] % (i+1);
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
 
  return retValue;
}

void
testSVD::init()
  /*!
    Setup and build an initial matrix
    and decomponse
  */
{
  ELog::RegMethod RegItem("testSVD","init");

  Matrix<double> M(10,5);
  for(size_t i=0;i<10;i++)
    for(size_t j=0;j<5;j++)
      {
	M[i][j]=static_cast<double>(50-((i*3+(j+4)*2) % 70));
      }
  A.setMatrix(M);
  A.calcDecomp();
  return;
}


int
testSVD::testInit()
  /*!
    setup and build an initial matrix
    \return 0 on success 
  */
{
  ELog::RegMethod RegA("testSVD","testInit");

  init();
  Matrix<double> M=A.getU()*A.getS()*(A.getV().Tprime());
  if (M!=A.getDesign())
    {
      ELog::EM<<"U == "<<A.getU()<<ELog::endTrace;
      ELog::EM<<"S == "<<A.getS()<<ELog::endTrace;
      ELog::EM<<"V == "<<A.getV()<<ELog::endTrace;
      
      ELog::EM<<"M == "<<M<<ELog::endTrace;
      ELog::EM<<"A == "<<A.getDesign()<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testSVD::testMakeSolution() 
  /*!
    Set up a matrix to solve a simple polynominal
    \retval -ve on error
    \retval 0 :: success
  */
{
  ELog::RegMethod RegItem("testSVD","testMakeSolution");

  // Design matrix:
  const double Param[]={3.0,2.3,0,4.5};
  std::vector<double> B;
  Matrix<double> M(5,4);
  for(size_t i=0;i<5;i++)
    {
      const double x(static_cast<double>(8+6*i));
      // Calc B
      double bValue(0);
      double xV=1.0;
      for(size_t j=0;j<4;j++)
	{
	  bValue+=xV*Param[j];
	  xV*=x;
	}
      B.push_back(bValue);
      // Calc matrix terms:
      M[i][0]=1.0;
      for(size_t j=1;j<4;j++)
	M[i][j]=x*M[i][j-1];
    }
  A.setMatrix(M);
  A.calcDecomp();
    
  std::vector<double> Bout=A.linearEQ(B);
 
  int flag(0);
  for(size_t i=0;i<4;i++)
    if (fabs(Bout[i]-Param[i])>1e-6)
      flag++;
  if (flag)
    {
      ELog::EM<<"P == ";
      copy(Param,Param+4,
	   std::ostream_iterator<double>(ELog::EM.Estream()," "));
      ELog::EM<<ELog::endTrace;

      ELog::EM<<"Bout == ";
      copy(Bout.begin(),Bout.end(),
	   std::ostream_iterator<double>(ELog::EM.Estream()," "));
      ELog::EM<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testSVD::testMakeSolLong() 
  /*!
    Set up a matrix to solve a simple polynominal.
    It adds some random noise to the data set and
    the fit using SVD. If the fit is worse than the 
    original polynominal the clearly things have gone wrong.
    
    \retval -ve on error
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSVD","testMakeSolLong");

  const size_t vSize(80);
  const size_t polyN(5);

  // Design matrix:
  const double Param[]={3.0,2.3,12.3,6.2,0.0};
  std::vector<double> B;
  std::vector<double> X;
  Matrix<double> M(vSize,polyN);

  for(size_t i=0;i<vSize;i++)
    {
      const double x(static_cast<double>(8+2*i));
      X.push_back(x);
      // Calc B
      double bValue(0);
      double xV=1.0;
      for(size_t j=0;j<polyN;j++)
	{
	  bValue+=xV*Param[j];
	  xV*=x;
	}
      bValue+=RNG.randNorm(0.0,1.0);
      B.push_back(bValue);
      // Calc matrix terms:
      M[i][0]=1.0;
      for(size_t j=1;j<polyN;j++)
	M[i][j]=x*M[i][j-1];
    }
  A.setMatrix(M);
  A.calcDecomp();
    
  std::vector<double> Bout=A.linearEQ(B);
  
  // Check Chi^2
  double chi1(0.0),chi2(0.0);
  for(size_t i=0;i<vSize;i++)
   {
     double Y1(0.0);
     double Y2(0.0);
     double xPower(1.0);
     for(size_t j=0;j<polyN;j++)
       {
	 Y1+=Param[j]*xPower;
	 Y2+=Bout[j]*xPower;
	 xPower*=X[i];
       }
     chi1+=(B[i]-Y1)*(B[i]-Y1);
     chi2+=(B[i]-Y2)*(B[i]-Y2);
   }

  if (chi1<chi2)
    {
      ELog::EM<<"Chi1 == "<<chi1<<" chi2="<<chi2<<ELog::endTrace;
      return -2;
    }
		 
  return 0;
}
 
