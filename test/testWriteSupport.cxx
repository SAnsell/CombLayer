/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testWriteSupport.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "writeSupport.h"

#include "testFunc.h"
#include "testWriteSupport.h"

using namespace StrFunc;

testWriteSupport::testWriteSupport()
  /*!
    Constructor
   */
{}

testWriteSupport::~testWriteSupport()
  /*!
    Destructor
  */
{}

int 
testWriteSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testWriteSupport","applyTest");
  TestFunc::regSector("testWriteSupport");
  
  typedef int (testWriteSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testWriteSupport::testDouble
    };

  const std::string TestName[]=
    {
      "Double"
    };

  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(size_t i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
      return 0;
    }
  for(size_t i=0;i<TSize;i++)
    {
      if (extra<0 || extra==static_cast<int>(i+1))
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
testWriteSupport::testDouble()
  /*!
    Applies a test to convert
    \retval -1 :: failed to convert a good double
    \retval -2 :: converted a number with leading stuff
    \retval -3 :: converted a number with trailing stuff
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testWriteSupport","testConvert");

  // type : Init string : final : results : (outputs)
  typedef std::tuple<double,std::string> TTYPE;

  const std::vector<TTYPE> Tests=
    {
      //         1234567890
      TTYPE(2.0,"         2"),
      TTYPE(2.1,"       2.1"),
      TTYPE(2.0000000001,"         2"),
      TTYPE(-2.1,   "      -2.1"),
      TTYPE(-2.1e12,"  -2.1e+12")
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const double tD=std::get<0>(tc);
      const std::string out=StrFunc::flukaNumber(tD);
      const std::string TLine=std::get<1>(tc);
      if (TLine!=out)
	{
	  ELog::EM<<"TEST :: "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Output D==: "<<tD<<" :=="<<ELog::endDiag;
	  ELog::EM<<"10 count    :1234567890:=="<<ELog::endDiag;
	  ELog::EM<<"Output    ==:"<<out<<":=="<<ELog::endDiag;
	  ELog::EM<<"Expected  ==:"<<TLine<<":=="<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

