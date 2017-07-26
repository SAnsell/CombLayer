/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMathSupport.cxx
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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <utility>
#include <string>
#include <vector>
#include <complex>
#include <map>
#include <algorithm>
#include <functional>
#include <tuple>

#include "MersenneTwister.h"
#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "polySupport.h"
#include "ClebschGordan.h"
#include "MapRange.h"
#include "permSort.h"

#include "testFunc.h"
#include "testMathSupport.h"



testMathSupport::testMathSupport() 
  /*!
    Constructor
  */
{}

testMathSupport::~testMathSupport() 
  /*!
    Destructor
  */
{}

int 
testMathSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMathSupport","applyTest");
  TestFunc::regSector("testMathSupport");

  typedef int (testMathSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testMathSupport::testBinSearch,
      &testMathSupport::testClebschGordan,
      &testMathSupport::testCountBits,
      &testMathSupport::testCubic,      
      &testMathSupport::testFibinacci,
      &testMathSupport::testIndexSort,
      &testMathSupport::testLowBitIndex,
      &testMathSupport::testMinDifference,
      &testMathSupport::testNormalDist,
      &testMathSupport::testOrder,
      &testMathSupport::testPairCombine,
      &testMathSupport::testPairSort,
      &testMathSupport::testPermSort,
      &testMathSupport::testPolInterp
    };
  const std::string TestName[]=
    {
      "BinSearch",
      "ClebschGordan",
      "CountBits",
      "Cubic",
      "Fibinacci",
      "IndexSort",
      "LowBitIndex",
      "MinDifference",
      "NormalDist",
      "Order",
      "PairCombine",
      "PairSort",
      "PermSort",
      "PolInterp"
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
testMathSupport::validCubic(const double* Coef,
			    const std::complex<double>& Ans) const
  /*!
    Tests the validity of a cubic equation
    \param Coef :: Coefficients
    \param Ans :: Answer 
    \retval 0 :: failure
    \retval 1 :: success
  */
{
  std::complex<double> Res;
  std::complex<double> X(1.0,0.0);
  for(int i=3;i>=0;i--)
    {
      Res+=Coef[i]*X;
      X*=Ans;
    }
  if (fabs(Res.real())>1e-8 || fabs(Res.imag())>1e-8)
    return 0;

  return 1;
}

int
testMathSupport::testBinSearch()
  /*!
    Test the binary search function
    \retval -1 :: failed to find end
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA ("testMathSupport","testBinSearch");

  std::vector<double> XVal;
  for(int i=0;i<10;i++)
    XVal.push_back(i+3.3);
  
  // Returns 0 at end of range:
  long int ox=mathFunc::binSearch(XVal.begin(),XVal.end(),-1.2);
  if (ox!=0)
    {
      ELog::EM<<"ox(-1.2) ="<<ox;
      ELog::EM.error();
      return -1;
    }

  // Gets the bin in the middle
  ox=mathFunc::binSearch(XVal.begin(),XVal.end(),30.0);
  if (ox!=(static_cast<long int>(XVal.size()))-1)
    {
      ELog::EM<<"ox(30.0) ="<<ox;
      ELog::EM.error();
      return -1;
    }

  // Gets the bin in the middle
  ox=mathFunc::binSearch(XVal.begin(),XVal.end(),80.0);
  if (ox!=(static_cast<long int>(XVal.size())-1))
    {
      ELog::EM<<"ox(80.0) ="<<ox;
      ELog::EM.error();
      return -1;
    }

  return 0;  
}

int
testMathSupport::testCountBits()
  /*!
    Applies a simple test to check the bit counting.
    Applies to both the short and the long int version.
    \retval -1 on failure
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testMathSupport","testCountBits");
  
  size_t A[]={7,16,140,160};
  size_t Res[]={3,1,3,2};
  for(size_t i=0;i<sizeof(A)/sizeof(size_t);i++)
    {
      if (countBits(A[i])!=Res[i])
	{
	  ELog::EM<<"Failed on [size_t]"<<A[i]<<" == "<<countBits(A[i])
		  <<" expect:"<<Res[i]<<ELog::endCrit;
	  return -static_cast<int>(i+1);
	}
      unsigned int usA=static_cast<unsigned int>(A[i]);
      if (countBits(usA)!=static_cast<unsigned int>(Res[i]))
	{
	  ELog::EM<<"Failed on [unsigned int]"<<usA<<" == "
		  <<countBits(usA)
		  <<" expect:"<<Res[i]<<ELog::endCrit;
	  return -static_cast<int>(i+1);
	}
    }
  return 0;
}

int
testMathSupport::testLowBitIndex()
  /*!
    Applies a simple test to check the bit counting 
    \retval -1 on failure
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testMathSupport","testCountBits");
  
  unsigned int A[]={7,16,140,160};
  unsigned int Res[]={1,5,3,6};
  for(unsigned int i=0;i<sizeof(A)/sizeof(unsigned int);i++)
    {
      if (lowBitIndex(A[i])!=Res[i])
	{
	  ELog::EM<<"Failed on "<<A[i]<<" == "<<lowBitIndex(A[i])
		  <<" expect:"<<Res[i]<<ELog::endCrit;
	  return -static_cast<int>(i+1);
	}
    }
  return 0;
}

int
testMathSupport::testQuadratic()
  /*!
    Applies a simple test to check the quadratic
    function (in particular with an imaginary quadratic)
    \retval -1 on failure
    \retval 0 on success
  */
{
  typedef std::complex<double> CP ;
  std::pair<CP,CP > Ans;
  // (x-2)(2x+3)
  double CoefA[3]={2,-1,-6};
  // Note: this should order solutions :
  size_t flag=solveQuadratic(CoefA,Ans); 
  if (flag!=2 || Ans.first!= CP(2,0) ||
      Ans.second!= CP(-1.5,0))
    {
      ELog::EM<<"First Polynominal (x-2)(2x+3) :"<<
	Ans.first<<" "<<Ans.second<<ELog::endCrit;
      return -1;
    }

  double Coef[3]={3,1,1};
  flag=solveQuadratic(Coef,Ans);
  if (flag!=2)
    return -1;

  CP X;
  X=3.0*Ans.first*Ans.first;
  X+=Ans.first;
  X+=1.0;

  CP Y=3.0*Ans.second*Ans.second;
  Y+=Ans.second;
  Y+=1.0;

  if (fabs(X.real())>1e-10 ||
      fabs(X.imag())>1e-10 ||
      fabs(Y.real())>1e-10 ||
      fabs(Y.imag())>1e-10)
    {
      ELog::EM<<"Point == "<<Ans.first<<" "<<Ans.second<<ELog::endCrit;
      ELog::EM<<"Factor == "<<X<<" "<<Y<<ELog::endCrit;
      return -2;
    }
  return 0;
}

int
testMathSupport::testCubic()
  /*!
    Applies a simple test to check the cubic
    function
    \retval -1 on failure
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testMathSupport","testCubic");

  typedef std::complex<double> CP ;
  CP A;
  CP B;
  CP C;
  // THREE REAL:
  // (x-2)(x+3)(x-3)
  const double CoefA[4]={1,-2,-9,18};
  size_t flag=solveCubic(CoefA,A,B,C);
  if (flag!=3 ||
      std::abs(A.real()+3.0)>1e-10  ||
      std::abs(B.real()-3.0)>1e-10  ||
      std::abs(C.real()-2.0)>1e-10  ||
      std::abs(A.imag())>1e-10  ||
      std::abs(B.imag())>1e-10  ||
      std::abs(C.imag())>1e-10)
    {
      ELog::EM<<"Equation (x-2)(x+3)(x-3)"<<ELog::endCrit;
      ELog::EM<<"Answers == "<<A<<ELog::endCrit;
      ELog::EM<<"Answers == "<<B<<ELog::endCrit;
      ELog::EM<<"Answers == "<<C<<ELog::endCrit;
      return -1;
    }

  // THREE REAL: [two identical] 
  // (x-2)(x+3)(x+3)
  const double CoefB[4]={1,0,-1,2/sqrt(27)};
  flag=solveCubic(CoefB,A,B,C);
  if (flag!=2 ||
      !validCubic(CoefB,A) ||
      !validCubic(CoefB,B) ||
      !validCubic(CoefB,C) )
    {
      ELog::EM<<"Equation x^3-x+2/sqrt(27)=0"<<ELog::endCrit;
      ELog::EM<<"Flag "<<flag<<ELog::endCrit;
      ELog::EM<<"Answers == "<<A<<ELog::endCrit;
      ELog::EM<<"Answers == "<<B<<ELog::endCrit;
      ELog::EM<<"Answers == "<<C<<ELog::endCrit;
      return -1;
    }

  // ONE REAL: TWO COMPLEX
  const double CoefC[4]={1,2,-1,6};
  flag=solveCubic(CoefC,A,B,C);
  if (flag!=3 ||
      !validCubic(CoefC,A) ||
      !validCubic(CoefC,B) ||
      !validCubic(CoefC,C))
    {
      ELog::EM<<"Equation x^3+2x^2-x+6=0"<<ELog::endCrit;
      ELog::EM<<"Answers == "<<A<<ELog::endCrit;
      ELog::EM<<"Answers == "<<B<<ELog::endCrit;
      ELog::EM<<"Answers == "<<C<<ELog::endCrit;
      return -1;
    }

  return 0;
}

int
testMathSupport::testIndexSort()
  /*!
    Simple test of the indexSort system
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testMathSupport","testIndexSort");
  
  MTRand Rand(123456L);

  std::vector<double> V(10);
  std::generate(V.begin(),V.end(),
		std::bind<double(MTRand::*)()>(&MTRand::rand,Rand));


  std::vector<size_t> Index(10);
  std::generate(Index.begin(),Index.end(),IncSeq());

  indexSort(V,Index);
  size_t sumI=0;
  for(size_t i=1;i<10;i++)
    {
      if ( V[Index[i-1]] > V[Index[i]])
	{
	  ELog::EM<<"Error between values ["<<i-1<<" to "
	  <<i<<"] "<<V[Index[i-1]]<<":"<<V[Index[i]]<<ELog::endDebug;
	  return -1;
	}
      sumI+=Index[i];
    }     
  if (sumI!=45)
    return -2;

  return 0;
}

int
testMathSupport::testPermSort()
  /*!
    Simple test of the indexSort system
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testMathSupport","testPermSort");
  
  MTRand Rand(123456L);

  std::vector<double> V(30);
  std::generate(V.begin(),V.end(),
  		std::bind<double(MTRand::*)()>(&MTRand::rand,Rand));

  std::vector<size_t> Index=
    mathSupport::sortPermutation(V,[](const double& a,
                                      const double& b)
                                 { return (a<b); } );

  mathSupport::applyPermutation(V,Index);
  for(size_t i=1;i<V.size();i++)
    {
      if (V[i-1]>V[i])
        {
          ELog::EM<<"Item["<<i<<"] "<<V[i-1]<<" "<<V[i]<<ELog::endDebug;
          return -1;
        }
    }
  return 0;
}

int
testMathSupport::testOrder()
  /*!
    Test of the mathFunc::order function
    \returns -ve on error 0 on success.
   */
{
  double a(-4.0);
  double b(3.2);
  mathFunc::Order(a,b);
  if (a!=-4.0)
    return -1;
  mathFunc::Order(b,a);
  if (a!=3.2)
    return -2;
  
  return 0;
}

int
testMathSupport::testPairSort()
  /*!
    Test of the mathsupport PSep component to do sorted
    and to do finding.
    \returns -ve on error 0 on success.
   */
{
  ELog::RegMethod RegA("testMathSupport","testPairSort");
  std::vector<std::pair<int,int> > GList;

  GList.push_back(std::pair<int,int>(3,4));
  GList.push_back(std::pair<int,int>(6,2));
  GList.push_back(std::pair<int,int>(7,8));
  GList.push_back(std::pair<int,int>(2,6));
  // using second term
  sort(GList.begin(),GList.end());  
  std::vector<std::pair<int,int> >::const_iterator vc;    
  int a(-10);
  int flag(0);
  for(vc=GList.begin();vc!=GList.end();vc++)
    {
      if (vc->first<a)
	flag=-1;
      a=vc->first;
    }
  if (flag)
    return -1;

  a=-10;
  sort(GList.begin(),GList.end(),mathSupport::PairSndLess<int,int>());  
  for(vc=GList.begin();vc!=GList.end();vc++)
    {
      if (vc->second<a)
	flag=-1;
      a=vc->second;
    }
  if (flag)  return -2;


  std::vector<std::pair<int,int> >::iterator fc;
  fc=find_if(GList.begin(),GList.end(),
	  std::bind2nd(mathSupport::PairSndEq<int,int>(),8));

  if (fc==GList.end())
    return -3;
  if (fc->first!=7)
    return -4;

  // Now check a bound sorting
  fc=lower_bound(GList.begin(),GList.end(),4,
		 mathSupport::PairSndLess<int,int>());
  if (fc->first!=3)
    return -5;

  // Check inserting
  fc=lower_bound(GList.begin(),GList.end(),5,
		 mathSupport::PairSndLess<int,int>());
  GList.insert(fc,std::pair<int,int>(9,5));
  a=-10;
  for(vc=GList.begin();vc!=GList.end();vc++)
    {
      if (vc->second<a)
	flag=-1;
      a=vc->second;
    }
  if (flag)
    return -6;

  return 0; 
}

int
testMathSupport::testPairCombine()
  /*!
    Objective is to test the PCombine class
    and the method crossSort
    \retval -1 on failure
    \retval 0 on success
  */ 
{
  std::vector<int> A;
  std::vector<double> B;
  for(int i=0;i<10;i++)
    {
      B.push_back(5.0+i);
      A.push_back(10-i);
    }
  mathFunc::crossSort(A,B);
  double bpart(100);
  int apart(-1);
  int flag(0);
  for(size_t i=0;i<A.size() && !flag;i++)
    {
      if (apart>A[i] || bpart<B[i])
	flag=1;
      apart=A[i];
      bpart=B[i];
    }

  if (flag)     // Error occured
    {
      for(size_t i=0;i<A.size();i++)
	ELog::EM<<"A == "<<A[i]<<" "<<B[i]<<ELog::endErr;
      return -1;
    }

  return 0;
}

int
testMathSupport::testPolInterp()
  /*!
    Test the polynominal function
    \retval 0 :: Success
  */
{
  ELog::RegMethod RegA("testMathSupport","testPolInterp");
  
  std::vector<double> X;
  std::vector<std::complex<double> > Y;
  for(int i=0;i<100;i++)
    {
      X.push_back(i*1.0);
      Y.push_back(std::complex<double>(2.0*i,-i*i*3.0-2.0));
    }
  if (polInterp(0.5,4,X,Y)!=std::complex<double>(1,-2.75))
    {
      ELog::EM<<"A == "<<polInterp(0.5,4,X,Y)
	      <<" Expect (1,"<<-0.25*3.0-2.0<<")"<<ELog::endTrace;
      for(size_t i=0;i<8;i++)
	ELog::EM<<"Array == "<<X[i]<<" "<<Y[i]<<ELog::endTrace;
      return -1;
    }
  return 0;

}

int
testMathSupport::testMinDifference()
  /*!
    Test the minimum difference in an vector/list
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testMathSupport","testMinDifference");

  const double tValues[]={1,3.4,3.5,8.4,9.0,49.0,49.05,60,90,100.0};
  std::vector<double> TV(10);
  copy(tValues,tValues+10,TV.begin());

  const double D=mathFunc::minDifference(TV,1e-6);
  if (fabs(D-0.05)>1e-6)
    {
      ELog::EM<<"Difference == "<<D<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testMathSupport::testNormalDist()
  /*!
    Test the gaussian normal distribution function
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testMathSupport","testNormalDist");

  double sumND(0.0);
  double sum2ND(0.0);
  double errND(0.0);
  double err2ND(0.0);

  for (int i=0;i<100;i++)
    {
      sumND+=normalDist(i/100.0);
      errND+=invErf(i/100.0);
      sum2ND+=normalDist(i/100.0)*normalDist(i/100.0);
      err2ND+=invErf(i/100.0)*invErf(i/100.0);
    }
  if (fabs(sumND)>1e-6 || fabs(sum2ND-91.2942)>1e-4 ||
      fabs(errND-55.3039)>1e-4 || fabs(err2ND-47.5063)>1e-4)  
    {
      ELog::EM<<"norm Sum  == "<<sumND<<ELog::endTrace;
      ELog::EM<<"norm Sum2 == "<<sum2ND<<ELog::endTrace;
      ELog::EM<<"error sum == "<<errND<<ELog::endTrace;
      ELog::EM<<"error sum2== "<<err2ND<<ELog::endTrace;
      return -1;
    }

  return 0;
}

int
testMathSupport::testClebschGordan()
  /*!
    Simple test of know values
    [Example valus taken from WolframAlpha calculator]
    \return -ve on error
  */
{ 
  ELog::RegMethod RegA("testMathSupport","testClebschGordan");

  //                   j   j1   j2  m1 m2   res
  typedef std::tuple<int,int,int,int,int,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(2,2,2,0,0,-sqrt(2.0)/sqrt(7.0) ));
  Tests.push_back(TTYPE(1,5,4,0,0,sqrt(5.0/33.0) ));
  Tests.push_back(TTYPE(3,5,4,2,1,-5*sqrt(7.0/858.0) ));
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int j(std::get<0>(tc));
      const int j1(std::get<1>(tc));
      const int j2(std::get<2>(tc));

      const int m1(std::get<3>(tc));
      const int m2(std::get<4>(tc));

      const double R=mathSupport::ClebschGordan::calc(j,j1,j2,m1,m2);
      if (fabs(R-std::get<5>(tc))>1e-5 )
	{
	  ELog::EM<<"Failed on item "<<cnt<<ELog::endDebug;
	  ELog::EM<<"C["<<j<<","<<j1<<","<<j2<<","
		  <<m1<<","<<m2<<"]=="<<R<<" ("<<std::get<5>(tc)<<")"
		  <<ELog::endDebug;
	  ELog::EM<<"Error term == "<<fabs(R-std::get<5>(tc))<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}


int
testMathSupport::testFibinacci()
  /*!
    Test a fibinacci series [first 50 terms]
    \return -1 :: failed to find end
  */
{
  ELog::RegMethod RegA ("testMathSupport","testFibinacci");

  double FNA(0);
  double FNB(1);
  for(int i=2;i<50;i++)
    {
      if (fabs(fibonacci(i)-(FNA+FNB))>1e-3)
        {
	  ELog::EM<<"Fib["<<i<<"]=="<<fibonacci(i)<<" "<<FNA+FNB<<ELog::endErr;
	}
      const double tmp=FNB;
      FNB+=FNA;
      FNA=tmp;
    }
  return 0;  
}
