/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testFortranWrite.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>

#ifndef NO_REGEX
#include <boost/regex.hpp>
#endif

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "regexSupport.h"
#include "fortranWrite.h"

#include "testFunc.h"
#include "testFortranWrite.h"

using namespace StrFunc;

testFortranWrite::testFortranWrite()
  /*!
    Constructor
   */
{}

testFortranWrite::~testFortranWrite()
  /*!
    Destructor
  */
{}

int 
testFortranWrite::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testFortranWrite","applyTest");

  typedef int (testFortranWrite::*testPtr)();
  testPtr TPtr[]=
    {
      &testFortranWrite::testParse
    };

  const std::string TestName[]=
    {
      "Parse"
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
testFortranWrite::testParse()
  /*!
    Applies a test to convert
    \retval -1 :: failed to convert a good double
    \retval -2 :: converted a number with leading stuff
    \retval -3 :: converted a number with trailing stuff
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testFortranWrite","testConvert");

  // type : Init string : final : results : (outputs)
  typedef std::tuple<std::string,size_t,size_t,
		     size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests;

  int resultFlag;       // Section return
  bool checkFlag;       // Output matches

  // Test of the split
  Tests.push_back(TTYPE("I3,I4 ",2,1,1,4));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      std::string outS;
      fortranWrite(std::get<0>(tc));
      cnt++;
    }
  return 0;
}
