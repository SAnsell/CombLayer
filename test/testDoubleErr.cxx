/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testDoubleErr.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "doubleErr.h"

#include "testFunc.h"
#include "testDoubleErr.h"

testDoubleErr::testDoubleErr()  
  /*!
    Constructor 
  */
{}

testDoubleErr::~testDoubleErr() 
  /*!
    Destructor
   */
{}

int 
testDoubleErr::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: control variable
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testDoubleErr","applyTest");
  TestFunc::regSector("testDoubleErr");

  typedef int (testDoubleErr::*testPtr)();
  testPtr TPtr[]=
    {
      &testDoubleErr::testPower,
      &testDoubleErr::testRead
    };
  const std::string TestName[]=
    {
      "Power",
      "Read"
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
testDoubleErr::testPower()
  /*!
    Test of the sndValue functor.
    \returns -ve on error 0 on success.
   */
{
  ELog::RegMethod RegA("testDoubleErr","testPower");

  typedef std::tuple<double,DError::doubleErr,DError::doubleErr> TTYPE;  
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(2,DError::doubleErr(2,3),
			DError::doubleErr(4,8.31777)));

  for(const TTYPE& tc : Tests)
    {
      DError::doubleErr X=DError::pow(std::get<0>(tc),std::get<1>(tc));
      if (fabs(std::get<2>(tc).getVal()-X.getVal())>1e-5 ||
	  fabs(std::get<2>(tc).getErr()-X.getErr())>1e-5)
	{
	  ELog::EM<<"Failed on "<<X<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testDoubleErr::testRead()
  /*!
    Test of the read in function
    \returns -ve on error 0 on success.
   */
{
  ELog::RegMethod("testDoubleErr","testRead");

  typedef std::tuple<std::string,DError::doubleErr> TTYPE;  
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("-78e-3 -1.2",DError::doubleErr(-7.8e-2,1.2)));
  Tests.push_back(TTYPE("7.8e-3 1.0",DError::doubleErr(7.8e-3,1.0)));

  DError::doubleErr A;  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      std::istringstream cx;
      cx.str(std::get<0>(tc));
      A.read(cx);
      if (fabs(A.getVal()-std::get<1>(tc).getVal())>1e-10 ||
	  fabs(A.getErr()-std::get<1>(tc).getErr())>1e-10 )
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Read   =="<<A<<ELog::endTrace;
	  ELog::EM<<"expect =="<<std::get<1>(tc)<<ELog::endTrace;
	  ELog::EM<<"input  =="<<std::get<0>(tc)<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

//  637232  50
