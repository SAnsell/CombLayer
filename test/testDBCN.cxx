/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testDBCN.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "dbcnCard.h"

#include "testFunc.h"
#include "testDBCN.h"


testDBCN::testDBCN() 
  /*!
    Constructor
  */
{}

testDBCN::~testDBCN() 
  /*!
    Destructor
  */
{}

int 
testDBCN::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testDBCN","applyTest");
  TestFunc::regSector("testDBCN");

  typedef int (testDBCN::*testPtr)();
  testPtr TPtr[]=
    {
      &testDBCN::testSimpleWrite
    };
  const std::string TestName[]=
    {
      "simpleWrite"
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
testDBCN::testSimpleWrite()
  /*!
    Test a simple write out sot that with  
    a couple of active points we get the correct 
    number of j's
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testDBCN","testSimpleWrite");

  physicsSystem::dbcnCard DC;

  // result : active : set :: [type : double/int ]
  typedef std::tuple<std::string,std::string,int,double,long int> TTYPE;
  std::vector<TTYPE> Tests({
      TTYPE("","",0,0.0,0)
    });

  for(const TTYPE& A : Tests)
    {
      const std::string ActStr=std::get<1>(A);
      const int flag=std::get<2>(A);
      const double valueD=std::get<3>(A);
      const long int valueI=std::get<4>(A);

      if (!ActStr.empty())
	{
	  if (!flag)
	    DC.setActive(ActStr,1);
	  else if (flag==1)
	    DC.setComp(ActStr,valueI);
	  else
	    DC.setComp(ActStr,valueD);	  
	}
	
      std::ostringstream cx;
      DC.write(cx);
      const std::string result=cx.str();
      if (result!=std::get<0>(A))
	{
	  ELog::EM<<"Expected :"<<std::get<0>(A)<<ELog::endDiag;
	  ELog::EM<<"Obtained :"<<result<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}



