/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testVarNameOrder.cxx
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
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "varNameOrder.h"

#include "testFunc.h"
#include "testVarNameOrder.h"

using namespace ModelSupport;

testVarNameOrder::testVarNameOrder()
  /*!
    Constructor
  */
{}

testVarNameOrder::~testVarNameOrder() 
  /*!
    Destructor
  */
{}



int 
testVarNameOrder::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testVarNameOrder","applyTest");
  TestFunc::regSector("testVarNameOrder");

  typedef int (testVarNameOrder::*testPtr)();
  testPtr TPtr[]=
    {
      &testVarNameOrder::testBasic,
      &testVarNameOrder::testCompound
    };

  const std::string TestName[]=
    {
      "Basic",
      "Compound"
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
testVarNameOrder::testBasic()
  /*!
    Test to see if an isolated pipe can exist
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testVarNameOrder","testBasic");


  std::vector<std::string> Names;
  Names.push_back("fred2alpha");
  Names.push_back("fred121alpha");
  Names.push_back("A");
  Names.push_back("B");
  Names.push_back("fred123beta");
  Names.push_back("fred7beta");
  Names.push_back("freds6alpha");
  Names.push_back("fred4alpha");

  std::vector<std::string> Out;
  Out.push_back("A");
  Out.push_back("B");
  Out.push_back("fred2alpha");
  Out.push_back("fred4alpha");
  Out.push_back("fred121alpha");
  Out.push_back("fred7beta");
  Out.push_back("fred123beta");
  Out.push_back("freds6alpha");


  sort(Names.begin(),Names.end(),
       varNameOrder());
  
  size_t i;
  for(i=0;i<Names.size() && 
	Out[i]==Names[i]; i++) ;

  if (i!=Names.size())
    {
      for(i=0;i<Names.size();i++)
	{
	  ELog::EM<<"["<<i<<"]=="<<Names[i]<<"   expect:   "<<Out[i];
	  if (Out[i]!=Names[i])
	    ELog::EM<<ELog::endWarn;
	  else
	    ELog::EM<<ELog::endTrace;
	}
      return -1;
    }

  return 0;
}

int
testVarNameOrder::testCompound()
  /*!
    Test to see a compound list ordering
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testVarNameOrder","testBasic");

  typedef std::pair<std::string,int> PTYPE;
  std::vector<PTYPE> Names;
  Names.push_back(PTYPE("fred2alpha",1));
  Names.push_back(PTYPE("fred121alpha",2));
  Names.push_back(PTYPE("A",3));
  Names.push_back(PTYPE("B",4));
  Names.push_back(PTYPE("fred123beta",5));
  Names.push_back(PTYPE("fred7beta",6));
  Names.push_back(PTYPE("freds6alpha",7));
  Names.push_back(PTYPE("fred4alpha",8));

  std::vector<PTYPE> Out;
  Out.push_back(PTYPE("A",3));
  Out.push_back(PTYPE("B",4));
  Out.push_back(PTYPE("fred2alpha",1));
  Out.push_back(PTYPE("fred4alpha",8));
  Out.push_back(PTYPE("fred121alpha",2));
  Out.push_back(PTYPE("fred7beta",6));
  Out.push_back(PTYPE("fred123beta",5));
  Out.push_back(PTYPE("freds6alpha",7));


  sort(Names.begin(),Names.end(),
       varNameOrder());
  
  size_t i;
  for(i=0;i<Names.size() && 
	Out[i].first==Names[i].first &&
	Out[i].second==Names[i].second;
      i++) ;

  if (i!=Names.size())
    {
      for(i=0;i<Names.size();i++)
	{
	  ELog::EM<<"["<<i<<"]=="<<Names[i].first<<":"<<Out[i].first;
	  if (Out[i].first!=Names[i].first)
	    ELog::EM<<ELog::endWarn;
	  else
	    ELog::EM<<ELog::endTrace;
	}
      return -1;
    }

  return 0;
}

