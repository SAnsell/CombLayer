/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testRotCounter.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "RotCounter.h"

#include "testFunc.h"
#include "testRotCounter.h"


testRotCounter::testRotCounter() 
///< Constructor
{}

testRotCounter::~testRotCounter() 
  ///< Destructor
{}

int 
testRotCounter::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to apply
    \return 0 on success/ -ve on error
  */
{
  ELog::RegMethod RegA("testRotCounter","applyTest");
  TestFunc::regSector("testRotCounter");

  typedef int (testRotCounter::*testPtr)();
  testPtr TPtr[]=
    {
      &testRotCounter::testAddition
    };

  std::string TestName[]=
    {
      "Addition"
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
testRotCounter::testAddition()
  /*!
    Tests the addition to the counter
    to check that it loops back
    \return 0 (always succeeds)
  */
{
  ELog::RegMethod RegA("testRotCounter","testAddition");


  typedef std::tuple<size_t,bool,int,int,int> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(1,1,3,4,5));
  Tests.push_back(TTYPE(7,1,1,3,4));
  Tests.push_back(TTYPE(20,1,0,1,2));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      RotaryCounter<int> RX(3,6);
      for(size_t i=0;i<std::get<0>(tc);i++)
	{
	  if (std::get<1>(tc))
	    RX--;
	  else
	    RX++;
	}

      if (RX[0]!=std::get<2>(tc) ||
	  RX[1]!=std::get<3>(tc) ||
	  RX[2]!=std::get<4>(tc))
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"RX    = "<<RX<<ELog::endTrace;
	  ELog::EM<<"Expect= "<<std::get<2>(tc)<<" "<<std::get<3>(tc)
		  <<" "<<std::get<4>(tc)<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }

      

  return 0;
}
