/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testIndexCounter.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "IndexCounter.h"

#include "testFunc.h"
#include "testIndexCounter.h"


testIndexCounter::testIndexCounter() 
///< Constructor
{}

testIndexCounter::~testIndexCounter() 
  ///< Destructor
{}

int 
testIndexCounter::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to apply
    \return 0 on success/ -ve on error
  */
{
  ELog::RegMethod RegA("testIndexCounter","applyTest");
  TestFunc::regSector("testIndexCounter");

  typedef int (testIndexCounter::*testPtr)();
  testPtr TPtr[]=
    {
      &testIndexCounter::testAddition
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
testIndexCounter::testAddition()
  /*!
    Tests the addition to the counter
    to check that it loops back
    \return 0 (always succeeds)
  */
{
  ELog::RegMethod RegA("testIndexCounter","testAddition");


  // max : max : max : NPlus/Minus results
  typedef std::tuple<size_t,size_t,size_t,
		     long int,size_t,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests({
      {3,4,5,40,2,0,0},
      {3,4,5,80,1,0,0},
	{3,4,5,-40,1,0,0}
    });
  //  Tests.push_back(TTYPE(3,4,5,80,1,0,0));
  //  Tests.push_back(TTYPE

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      IndexCounter<size_t> RX(std::get<0>(tc),std::get<1>(tc),std::get<2>(tc));
      const bool plusFlag(std::get<3>(tc)>0);
      const size_t maxPlus(std::abs(std::get<3>(tc)));
      for(size_t i=0;i<maxPlus;i++)
	{
	  if (plusFlag)
	    RX++;
	  else
	    RX--;
	  //	  ELog::EM<<"RX["<<i<<"] == "<<RX<<ELog::endDiag;
	}
      ELog::EM<<"------------------"<<ELog::endDiag;
      if (RX[0]!=std::get<4>(tc) ||
	  RX[1]!=std::get<5>(tc) ||
	  RX[2]!=std::get<6>(tc))
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"RX    = "<<RX<<ELog::endDiag;
	  ELog::EM<<"Expect= "<<std::get<4>(tc)<<" "<<std::get<5>(tc)
		  <<" "<<std::get<6>(tc)<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }

      

  return 0;
}
