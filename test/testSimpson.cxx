/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSimpson.cxx
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
#include <string>
#include <vector>
#include <complex>
#include <map>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Simpson.h"

#include "testFunc.h"
#include "testSimpson.h"

using namespace Simpson;

/*!
  \struct Exp
  \brief Simple exp operator 
  \version 1.0
  \author S. Ansell
  \date June 2010 						
*/
struct Exp
{
  /// Simple exp function
  double operator()(const double& x) const
    { return exp(x); }
};

/*!
  \struct etaFunc
  \brief Simple Eta function from Nucl Inst Method
  \version 1.0
  \author S. Ansell
  \date June 2010 						
*/

struct etaFunc
{
  /// Eta function [Eqn 9: Nucl Inst Method. 213 (1983) 495]
  double operator()(const double& eta)
    {
      return (std::abs(eta)>1e-7) ? eta/(exp(-eta)-1.0) : 1.0;
    }
};


/*!
  \struct Weibull
  \brief Weibull function used by S. Platt 
  \version 1.0
  \author S. Ansell
  \date February 2011 						
*/

struct Weibull
{
  double s;   ///< Power value
  double W;   ///< Weight value
  double eT;  ///< Threshold

  /// Constructor
  Weibull(const double t,const double w,const double ps) :
    s(ps),W(w),eT(t) {}

  /// Weibull function
  double operator()(const double& E)
    {
      return (E>eT) ? 1.0-exp(-pow((E-eT)/W,s)) : 0.0;
    }
};

testSimpson::testSimpson() 
  /*!
    Constructor
  */
{}

testSimpson::~testSimpson() 
  /*!
    Destructor
  */
{}

int 
testSimpson::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testSimpson","applyTest");
  TestFunc::regSector("testSimpson");
  
  typedef int (testSimpson::*testPtr)();
  testPtr TPtr[]=
    {
      &testSimpson::testEta,
      &testSimpson::testExp,
      &testSimpson::testWeibull
    };
  const std::string TestName[]=
    {
      "Eta",
      "Exp",
      "Weibull"
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
testSimpson::testExp()
  /*!
    Test the integral of exp
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testSimpson","testExp");

  typedef std::tuple<double,double,double> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE(0.0,1.0,exp(1)-1.0),
      TTYPE(0.0,10.0,exp(10)-1.0),
      TTYPE(-5.0,10.0,exp(10)-exp(-5.0))
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const double A=
	Simpson::integrate<Exp>(20,std::get<0>(tc),std::get<1>(tc));
      if (std::abs((A-std::get<2>(tc))/std::get<2>(tc))>1e-3)
	{
	  ELog::EM<<"Failed on item "<<cnt<<ELog::endCrit;
	  ELog::EM<<"A="<<A<<" "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"Diff="<<(A-std::get<2>(tc))<<ELog::endCrit;
	  ELog::EM<<"Ratio="<<(A-std::get<2>(tc))/std::get<2>(tc)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testSimpson::testEta()
  /*!
    Test the integral of exp
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testSimpson","testEta");

  typedef std::tuple<double,double,double> TTYPE;
  std::vector<TTYPE> Items;

  Items.push_back(TTYPE(0.0,20.0,-201.644934023564));

  int cnt(1);
  for(const TTYPE& tc : Items)
    {
      const double A=
	Simpson::integrate<etaFunc>(40,std::get<0>(tc),std::get<1>(tc));
      if (std::abs((A-std::get<2>(tc))/std::get<2>(tc))>1e-3)
	{
	  ELog::EM<<"Failed on item "<<cnt<<ELog::endCrit;
	  ELog::EM<<"A="<<A<<" "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"Diff="<<(A-std::get<2>(tc))<<ELog::endCrit;
	  ELog::EM<<"Ratio="<<(A-std::get<2>(tc))/std::get<2>(tc)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testSimpson::testWeibull()
  /*!
    Test the integral of Weibull function
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testSimpson","testWeibull");

  // Integrat from XMaxmima : quad_qags(func,1.5,20)
  typedef std::tuple<double,double,double> TTYPE;
  std::vector<TTYPE> Items;

  Items.push_back(TTYPE(1.5,20.0,12.6807423));

  Weibull WB(1.5,6.3,1.2);
  int cnt(1);
  for(const TTYPE& tc : Items)
    {
      const double A=Simpson::integrate<Weibull>(20,std::get<0>(tc),
						 std::get<1>(tc),WB);
      if (std::abs((A-std::get<2>(tc))/std::get<2>(tc))>1e-3)
	{
	  ELog::EM<<"Failed on item "<<cnt<<ELog::endCrit;
	  ELog::EM<<"A="<<A<<" "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"Diff="<<(A-std::get<2>(tc))<<ELog::endCrit;
	  ELog::EM<<"Ratio="<<(A-std::get<2>(tc))/std::get<2>(tc)<<ELog::endCrit;
	  return -1;
	}
    }
  cnt++;
  return 0;
}
