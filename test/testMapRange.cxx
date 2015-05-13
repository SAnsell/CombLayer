/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMapRange.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MapSupport.h"
#include "MapRange.h"
#include "mapIterator.h"

#include "testFunc.h"
#include "testMapRange.h"

using namespace MapSupport;

testMapRange::testMapRange() 
  /*!
    Constructor 
  */
{}

testMapRange::~testMapRange() 
  /*!
    Destructor
  */
{}

int 
testMapRange::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: test number to use
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapRange","applyTest");
  TestFunc::regSector("testMapRange");

  typedef int (testMapRange::*testPtr)();
  testPtr TPtr[]=
    {
      &testMapRange::testFind
    };

  const std::string TestName[]=
    {
      "Find"
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
testMapRange::testFind()
  /*!
    Test of the range<int> class
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapRange","testFind");

  typedef std::map<Range<int>,std::string> MapTYPE;
  
  MapTYPE MUnit;
  MUnit.insert(MapTYPE::value_type(Range<int>(1,3),"a"));
  MUnit.insert(MapTYPE::value_type(Range<int>(17,18),"b"));
  MUnit.insert(MapTYPE::value_type(Range<int>(14,14),"c"));

  // test number : Found : key
  typedef std::tuple<int,bool,std::string> TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE(1,1,"a"),
      TTYPE(0,0,""),
      TTYPE(3,1,"a"),
      TTYPE(4,0,""),
      TTYPE(14,1,"c"),
      TTYPE(18,1,"b"),
      TTYPE(19,0,""),
      TTYPE(13,0,""),
      TTYPE(15,0,""),
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int testNum(std::get<0>(tc));
      MapTYPE::const_iterator mc=MUnit.find(Range<int>(testNum));
      const bool resultFind(mc!=MUnit.end());

      if (resultFind!=std::get<1>(tc) ||
	  (resultFind && mc->second!=std::get<2>(tc)))
	{
	  ELog::EM<<"Test   == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Search == "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Result["<<std::get<1>(tc)<<
	    "] == "<<resultFind<<ELog::endDiag;
	  if (resultFind)
	    ELog::EM<<"Found["<<std::get<2>(tc)
		    <<"]== "<<mc->second<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }
  return 0;
}
