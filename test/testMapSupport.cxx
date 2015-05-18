/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMapSupport.cxx
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
#include "testMapSupport.h"

testMapSupport::testMapSupport() 
  /*!
    Constructor 
  */
{}

testMapSupport::~testMapSupport() 
  /*!
    Destructor
  */
{}

int 
testMapSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: test number to use
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapSupport","applyTest");
  TestFunc::regSector("testMapSupport");

  typedef int (testMapSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testMapSupport::testIterator,
      &testMapSupport::testMapDelete,
      &testMapSupport::testMapRange,
      &testMapSupport::testMapWrite,
      &testMapSupport::testSndValue,
      &testMapSupport::testValEqual
    };

  const std::string TestName[]=
    {
      "Iterator",
      "MapDelete",
      "MapRange",
      "MapWrite",
      "SndValue",
      "ValEqual"
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
testMapSupport::testIterator()
  /*!
    Test of the valEqual functor.
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapSupport","testIterator");

  typedef std::tuple<std::string> TTYPE;  
  
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1,2,3,4"));

  for(const TTYPE& tc : Tests)
    {
      std::set<int> SI;
      std::string line=std::get<0>(tc);
      int value;
      while(StrFunc::section(line,value))
	{
	  SI.insert(value);
	}
      // Run through iterator and sum the values
      MapSupport::mapIterator A(SI);
      std::vector<int> sum(SI.size());
      do
	{
	  std::set<int>::const_iterator sc;
	  size_t index(0);
	  for(sc=SI.begin();sc!=SI.end();sc++,index++)
	    sum[index]+=A.getItem(*sc);
	} while(++A);

      for(size_t i=0;i<sum.size();i++)
	if (sum[i])
	  {
	    ELog::EM<<"Failed on point "<<i<<" "<<sum[i]<<ELog::endCrit;
	    return -1;
	  }
    }

  return 0;
}

int
testMapSupport::testSndValue()
  /*!
    Test of the sndValue functor.
    Given a list of keynames finds the second value
    for depositon 
    \returns -ve on error 0 on success.
   */
{
  std::map<std::string,int> MX;
  MX["a"]=1;
  MX["b"]=2;
  MX["c"]=3;
  std::vector<std::string> Kvec;
  Kvec.push_back("a");
  Kvec.push_back("c");
  Kvec.push_back("b");
  Kvec.push_back("a");

  std::vector<int> Out(4);
  
  transform(Kvec.begin(),Kvec.end(),Out.begin(),
	    MapSupport::sndValue<std::map<std::string,int> >(MX));

  if (Out[0]!=1 || Out[1]!=3 || Out[2]!=2 || Out[3]!=1)
    return -1;
  
  return 0;
}

int
testMapSupport::testMapDelete()
  /*!
    Simple test of map delete 
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testMapSupport","testMapDelete");
  
  std::map<std::string,int*> MX;
  MX["a"]=new int[5];
  MX["b"]=new int[2];
  MX["c"]=new int[3];
  MX["d"]=new int[4];
  MX["e"]=new int[2];
  
  for_each(MX.begin(),MX.end(),
	   MapSupport::mapDelete<std::map<std::string,int*> >());

  return 0;
}


int
testMapSupport::testMapRange()
  /*!
    Test of the range<int> class
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapSupport","testRange");

  using namespace MapSupport;
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

int
testMapSupport::testMapWrite()
  /*!
    Simple test of map write
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testMapSupport","testMapWrite");
  
  std::map<std::string,int> MX;
  MX["a"]=1;
  MX["b"]=2;
  MX["c"]=3;
  MX["d"]=4;
  MX["e"]=5;

  std::ostringstream CX;
  for_each(MX.begin(),MX.end(),
	   MapSupport::mapWrite<std::map<std::string,int> >(CX));
  if (StrFunc::removeSpace(CX.str())!="a1b2c3d4e5")
    {
      ELog::EM<<"Out == "<<ELog::endDiag;
      ELog::EM<<StrFunc::removeSpace(CX.str())<<ELog::endDiag;      
      return -1;
    }
  return 0;
}

int
testMapSupport::testValEqual()
  /*!
    Test of the valEqual functor.
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testMapSupport","testValEqual");

  std::map<std::string,int> MX;
  MX["a"]=1;
  MX["b"]=2;
  MX["c"]=3;
  MX["d"]=4;
  MX["e"]=5;
  
  int a(3);
  std::map<std::string,int>::const_iterator vc;
  vc=find_if(MX.begin(),MX.end(),
	     MapSupport::valEqual<std::string,int>(a));
  if (vc==MX.end() || vc->second!=3)
    {
      ELog::EM<<"Did not find number"<<ELog::endErr;
      return -1;
    }
  return 0;
}
  
