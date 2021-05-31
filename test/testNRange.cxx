/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testNRange.cxx
*
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <functional>
#include <iterator>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "NGroup.h"

#include "testFunc.h"
#include "testNRange.h"

testNRange::testNRange() 
  /*!
    Constructor
  */
{}

testNRange::~testNRange() 
  /*!
    Destructor
  */
{}

int 
testNRange::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index parameter to test
    \retval -1 Range failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testNRange","applyTest");
  TestFunc::regSector("testNRange");

  typedef int (testNRange::*testPtr)();
  testPtr TPtr[]=
    {
      &testNRange::testCondense,
      &testNRange::testInteger,
      &testNRange::testOperator,
      &testNRange::testOutput,
      &testNRange::testRange
    };
  const std::string TestName[]=
    {
      "Condense",
      "Integer",
      "Operator",
      "Output",
      "Range"
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
testNRange::testCondense()
  /*!
    Test the processsing with intervals 
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testCondense");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests
    ({
      {"1 8log 1e+09 3r ","1 8log 1e+09 3r"},
      {"1 8log 1e+09 3i 1e10","1 8log 1e+09 3i 1e+10"},

      {"1 8log 1e+09 3r 1e9 1e12 1e13 1.2e13",
       "1 8log 1e+09 4r 1e+12 1e+13 1.2e+13"},
      
      {"1 8log 1e+09 3r 1e9","1 8log 1e+09 4r"},
      {"0 1 1 1 1 1 1 ","0 1 3r"}
    });

  for(const TTYPE& tc : Tests)
    {
      const std::string input=std::get<0>(tc);
      const std::string output=std::get<1>(tc);
      RangeUnit::NGroup<double> NE;
      if (NE.processString(input))
	{
	  ELog::EM<<"Error with processing:"<<input<<ELog::endCrit;
	  return -1;
	}

      std::ostringstream px;
      px<<NE;
      const std::string PreC=px.str();
      std::ostringstream cx;
      NE.condense(1e-6);

      cx<<NE;

      if (StrFunc::removeOuterSpace(cx.str())!=output)
	{
	  ELog::EM<<"Test string == "<<input<<ELog::endWarn;
	  ELog::EM<<"Pre string  == "<<PreC<<ELog::endWarn;
	  ELog::EM<<"Cons string =="<<cx.str()<<ELog::endWarn;
	  ELog::EM<<"Out  string == "<<output<<ELog::endWarn;
	  return -1;
	}
    }
  return 0;
}

int
testNRange::testInteger()
  /*!
    Test the processsing with intervals 
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testCondense");

  typedef std::tuple<std::vector<int>,std::string> TTYPE;
  const std::vector<TTYPE> Tests
    ({
      {{0,0,1,1,1,1,1,1},"0 0 1 5r"}
    });

  for(const TTYPE& tc : Tests)
    {
      const std::vector<int>& input=std::get<0>(tc);
      const std::string output=std::get<1>(tc);

      RangeUnit::NGroup<int> NE;
      NE.condense(1e-6,input);

      std::ostringstream cx;
      cx<<NE;

      if (StrFunc::removeOuterSpace(cx.str())!=output)
	{
	  std::ostringstream vx;
	  for(const int CN : input)
	    vx<<CN<<" ";
	  ELog::EM<<"Test string == "<<vx.str()<<ELog::endWarn;
	  ELog::EM<<"Cons string =="<<cx.str()<<ELog::endWarn;
	  ELog::EM<<"Out  string == "<<output<<ELog::endWarn;
	  return -1;
	}
    }
  return 0;
}

int
testNRange::testOperator() 
  /*!
    Test the processsing with intervals 
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testOperator");



  double results[]={0.001,0.01,0.1,1.0,10,100,
                  100,100,100,
		  1000,1001,1002,1003,1004,1005,
		  1006,1007,1008,1009,1010,1011  };

  const std::string testString("0.001 4log 100 3r 1000 49i 1050");
  RangeUnit::NGroup<double> NE;
  
  if (NE.processString(testString))
    {
      ELog::EM<<"Error with processing:"<<testString<<ELog::endCrit;
      return -1;
    }
  // Basic string
  for(int i=0;i<20;i++)
    {
      if (std::abs(results[i]-NE[i])/results[i]>1e-5)
	{
	  ELog::EM<<NE<<ELog::endBasic;
	  ELog::EM<<"NE["<<i<<"]=="<<NE[i]<<ELog::endTrace;
	  return -2;
	}
    }
  return 0;
}

int
testNRange::testOutput() 
  /*!
    Test the processsing with intervals 
    (does not actually check the intervals
    other than to write it to the screen)
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testOutput");

  typedef std::tuple<std::string,double,double> TTYPE;
  std::vector<TTYPE> tests
    ({
      { "1.0 8log 1e9 3r",4.11111111e9,1e3 },
      { "1000.0 49i 1050.0",52275.0,1e-3}
    });

  for(const TTYPE& tc : tests)
    {
      RangeUnit::NGroup<double> NE;
      const std::string testString=std::get<0>(tc);
      if (NE.processString(testString))
	{
	  ELog::EM<<"Error with processing:"<<testString<<ELog::endCrit;
	  return -1;
	}
      // Basic string
      std::vector<double> values;
      NE.writeVector(values);
      const double sum=accumulate(values.begin(),values.end(),0.0,
				   std::plus<double>());
      if (std::abs(sum-std::get<1>(tc))>std::get<2>(tc))
	{
	  ELog::EM<<"Sum == "<<sum<<" from:"<<testString<<ELog::endWarn;
	  ELog::EM<<"Data == ";
	  copy(values.begin(),values.end(),
	       std::ostream_iterator<double>(ELog::EM.Estream(),","));
	  ELog::EM<<ELog::endCrit;
	  return -2;
	}
    }
  return 0;
}

int
testNRange::testRange() 
  /*!
    Test the processsing with intervals 
    (does not actually check the intervals
    other than to write it to the screen)
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testRange");


  typedef std::tuple<std::string,std::string> TTYPE;

  const std::vector<TTYPE> Tests
    ({
      {"1.3e2 100i 1.6e6 5r 0.8e9 12e8 10log 3.7e9",
	  "130 100i 1.6e+06 5r 8e+08 1.2e+09 10log 3.7e+09"}
    });
	

  for(const TTYPE& tc : Tests)
    {
      RangeUnit::NGroup<double> NE;
      const std::string testString=std::get<0>(tc);
      const std::string expectString=std::get<0>(tc);

      if (NE.processString(testString))
	{
	  ELog::EM<<"Error with NRange "<<ELog::endCrit;
	  return -1;
	}

      std::ostringstream cx;
      NE.write(cx);
      if (StrFunc::removeOuterSpace(cx.str())!=expectString)
	{
	  ELog::EM<<"Failed on string match"<<ELog::endDiag;
	  ELog::EM<<"Result == "<<cx.str()<<ELog::endDiag;
	  ELog::EM<<"Expect == "<<expectString<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

