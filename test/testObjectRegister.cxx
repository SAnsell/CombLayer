/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testObjectRegister.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"

#include "testFunc.h"
#include "testObjectRegister.h"

using namespace ModelSupport;

testObjectRegister::testObjectRegister() 
  /*!
    Constructor
  */
{}

testObjectRegister::~testObjectRegister() 
  /*!
    Destructor
  */
{
}

int 
testObjectRegister::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testObjectRegister","applyTest");
  TestFunc::regSector("testObjectRegister");

  typedef int (testObjectRegister::*testPtr)();
  testPtr TPtr[]=
    {
      &testObjectRegister::testExcludeItem,
    };
  const std::string TestName[]=
    {
      "ExcludeItem"
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
testObjectRegister::testExcludeItem()
  /*!
    Test the CellSTr method including the 
    complementary of both \#(XXX) and \#Cell.
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testObjectRegister","testExcludeItem");

  

  return 0;
}





