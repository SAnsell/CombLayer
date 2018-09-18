/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testGroupRange.cxx
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "groupRange.h"

#include "testFunc.h"
#include "testGroupRange.h"

using namespace Geometry;

testGroupRange::testGroupRange() 
  /*!
    Constructor
  */
{}

testGroupRange::~testGroupRange() 
  /*!
    Destructor
  */
{}

int 
testGroupRange::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testGroupRange","applyTest");
  TestFunc::regSector("testGroupRange");

  typedef int (testGroupRange::*testPtr)();
  testPtr TPtr[]=
    {
      &testGroupRange::testMerge
    };
  const std::vector<std::string> TestName=
    {
      "Merge"
    };
  
  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra) 
    {
      TestFunc::writeTests(TestName);
      return 0;
    }
  for(size_t i=0;i<TSize;i++)
    {
      if (extra<0 || static_cast<size_t>(extra)==i+1)
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
testGroupRange::testMerge()
  /*!
    Tests the merge units
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testMerge");

  groupRange A;
  A.addItem(3);
  A.addItem(4);
  A.addItem(5);

  
  
  
  return 0;
}

  

